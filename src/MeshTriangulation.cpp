#include "MeshTriangulation.hpp"

#include "Utils.hpp"

#include <boost/assert.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <list>

MeshTriangulation::MeshTriangulation() = default;

MeshTriangulation::MeshTriangulation(const std::vector<Point>& pt)
    : m_coords(pt), m_coordMeshHash(0)
{
    init();
}

MeshTriangulation::MeshTriangulation(std::vector<Point>&& pt)
    : m_coords(std::move(pt)), m_coordMeshHash(0)
{
    init();
}

MeshTriangulation::~MeshTriangulation() = default;

void MeshTriangulation::init()
{
    std::set<Point> st(m_coords.begin(), m_coords.end());
    m_coords.assign(st.begin(), st.end());

    boost::hash<Point> hasher{};
    this->m_coordMeshHash = 0;
    for(const auto& p : this->coordinates())
        boost::hash_combine(this->m_coordMeshHash, hasher(p));
}

void MeshTriangulation::flipLine(const LineCell& l)
{
    if(!m_edgeTrigAdj.count(l) || !m_flippable.count(l))
        return;

    LineCell lFlip = m_flippable.at(l);
    auto& trigFlip1 = *m_triangles.emplace(lFlip.a, lFlip.b, l.a).first;
    auto& trigFlip2 = *m_triangles.emplace(lFlip.a, lFlip.b, l.b).first;
    TriangleCell trig1(l.a, l.b, lFlip.a), trig2(l.a, l.b, lFlip.b);
    LineCell bdLine1 = LineCell(l.a, lFlip.a), bdLine2 = LineCell(l.a, lFlip.b);
    LineCell bdLine3 = LineCell(lFlip.a, l.b), bdLine4 = LineCell(l.b, lFlip.b);

    m_lines.erase(l);
    m_triangles.erase(trig1);
    m_triangles.erase(trig2);
    m_flippable.erase(l);
    m_edgeTrigAdj.erase(l);
    m_edgeTrigAdj[bdLine1].erase(trig1);
    m_edgeTrigAdj[bdLine2].erase(trig2);
    m_edgeTrigAdj[bdLine3].erase(trig1);
    m_edgeTrigAdj[bdLine4].erase(trig2);

    m_lines.insert(lFlip);
    m_triangles.insert(trigFlip1);
    m_triangles.insert(trigFlip2);
    m_flippable[lFlip] = l;
    updateAdjacency(trigFlip1, bdLine1, bdLine2, lFlip);
    updateAdjacency(trigFlip2, bdLine3, bdLine4, lFlip);
    updateFlippable(bdLine1);
    updateFlippable(bdLine2);
    updateFlippable(bdLine3);
    updateFlippable(bdLine4);
}

inline double MeshTriangulation::triangleArea(std::size_t a, std::size_t b, std::size_t c) const
{
    double s1, s2, s3;
    s1 = m_coords[a].distance(m_coords[b]);
    s2 = m_coords[a].distance(m_coords[c]);
    s3 = m_coords[b].distance(m_coords[c]);
    return 0.25 * sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3));
}

inline bool MeshTriangulation::convexPolygon(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const
{
    double t1, t2, t1_f, t2_f;
    t1 = triangleArea(i, j, k), t2 = triangleArea(k, l, i);
    t1_f = triangleArea(j, k, l), t2_f = triangleArea(l, i, j);

    return boost::math::epsilon_difference(t1 + t2, t1_f + t2_f) < 3.0 &&
        boost::math::epsilon_difference(t1, 0) > 3.0 &&
        boost::math::epsilon_difference(t2, 0) > 3.0 &&
        boost::math::epsilon_difference(t1_f, 0) > 3.0 &&
        boost::math::epsilon_difference(t2_f, 0) > 3.0;
}

const std::vector<Point>& MeshTriangulation::coordinates() const
{
    return this->m_coords;
}

const std::set<LineCell>& MeshTriangulation::lines() const
{
    return this->m_lines;
}

const std::set<TriangleCell>& MeshTriangulation::triangles() const
{
    return this->m_triangles;
}

const std::unordered_map<LineCell, LineCell>& MeshTriangulation::flippableLines() const
{
    return this->m_flippable;
}

std::string MeshTriangulation::wkt() const
{
    std::ostringstream oss;
    oss.precision(std::numeric_limits<double>::max_digits10);
    oss << "MULTIPOLYGON(";
    for(const auto& t : triangles())
    {
        oss << "(";
        oss << m_coords[t.a].x << " " << m_coords[t.a].y << ",";
        oss << m_coords[t.b].x << " " << m_coords[t.b].y << ",";
        oss << m_coords[t.c].x << " " << m_coords[t.c].y;
        oss << (t != *std::prev(triangles().end()) ? "), " : ")");
    }
    oss << ")";
    return oss.str();
}

std::size_t hash_value(const MeshTriangulation& mesh)
{
    std::size_t seed = mesh.m_coordMeshHash;

    for(const auto& t : mesh.triangles())
        boost::hash_combine(seed, boost::hash<TriangleCell>()(t));
    return seed;
}

bool MeshTriangulation::operator==(const MeshTriangulation& other) const
{
    return hash_value(*this) == hash_value(other);
}

void MeshTriangulation::triangulate()
{
    m_lines.clear();
    m_triangles.clear();

    std::size_t n = m_coords.size();
    std::vector<std::size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);

    sweepHullSort(idx, 0);

    std::list<std::size_t> hull(idx.begin(), std::next(idx.begin(), 3));
    auto trig_it = m_triangles.emplace(idx[0], idx[1], idx[2]).first;
    auto l1_it = m_lines.emplace(idx[0], idx[1]).first;
    auto l2_it = m_lines.emplace(idx[1], idx[2]).first;
    auto l3_it = m_lines.emplace(idx[2], idx[0]).first;
    updateAdjacency(*trig_it, *l1_it, *l2_it, *l3_it);

    for(std::size_t i = 3; i < n; i++)
        sweepHullAdd(hull, idx[i]);

    for(const LineCell& l : m_lines)
        updateFlippable(l);
}

void MeshTriangulation::sweepHullSort(std::vector<std::size_t>& idx, std::size_t pivot = 0)
{
    size_t n = idx.size();

    std::size_t i0 = pivot;
    while(i0 < n + pivot)
    {
        std::swap(idx[0], idx[i0 % n]);

        std::size_t i1 = 1;
        double minDist = m_coords[idx[0]].distance(m_coords[idx[1]]);
        for(std::size_t k = 2; k < n; k++)
        {
            double currDist = m_coords[idx[0]].distance(m_coords[idx[k]]);
            if(minDist > currDist)
            {
                minDist = currDist;
                i1 = k;
            }
        }
        std::swap(idx[1], idx[i1]);

        std::size_t i2 = 2;
        double min_r = Point::circumcircle(m_coords[idx[0]], m_coords[idx[1]], m_coords[idx[2]]).second;
        if(std::isnan(min_r))
        {
            ++i0;
            continue;
        }
        for(std::size_t k = 3; k < n; k++)
        {
            double curr_r = Point::circumcircle(m_coords[idx[0]], m_coords[idx[1]], m_coords[idx[k]]).second;
            if(!std::isnan(curr_r) && min_r > curr_r)
            {
                min_r = curr_r;
                i2 = k;
            }
        }
        std::swap(idx[2], idx[i2]);

        double cross = Point::cross(m_coords[idx[0]], m_coords[idx[1]], m_coords[idx[2]]);
        if(boost::math::epsilon_difference(cross, 0) < 3.0)
        {
            ++i0;
            continue;
        }
        if(cross > 0)
            std::swap(idx[1], idx[2]);

        const auto& [cc, radius] = Point::circumcircle(
            m_coords[idx[0]], m_coords[idx[1]], m_coords[idx[2]]);
        if(std::isnan(radius))
        {
            ++i0;
            continue;
        }

        if(n > 4)
        {
            std::sort(std::next(idx.begin(), 3), idx.end(),
                      [&CC = cc, this](std::size_t i, std::size_t j) -> bool
                      {
                          return CC.distance(m_coords[i]) < CC.distance(m_coords[j]);
                      });
        }

        break;
    }
}

void MeshTriangulation::sweepHullAdd(std::list<std::size_t>& hull, std::size_t i)
{
    auto circularNext = [&](std::list<std::size_t>::iterator it)
    { return std::next(it) == hull.end() ? std::next(it, 2) : std::next(it); };

    bool visible_prev, visible = false;
    std::list<std::size_t>::iterator it, it_next, cut_b = hull.end(), cut_e = hull.end();
    for(it = hull.begin(), it_next = circularNext(hull.begin()); it != hull.end(); it++, it_next = circularNext(it))
    {
        visible_prev = visible;
        visible = Point::cross(m_coords[i], m_coords[*it], m_coords[*it_next]) > 0;
        if(visible)
        {
            cut_b = visible_prev ^ visible ? it : cut_b;
            auto trig_it = m_triangles.emplace(i, *it, *it_next).first;
            auto l1_it = m_lines.emplace(i, *it).first;
            auto l2_it = m_lines.emplace(i, *it_next).first;
            updateAdjacency(*trig_it, *l1_it, *l2_it, LineCell(*it, *it_next));
        }
        else
            cut_e = visible_prev ^ visible ? it : cut_e;
    }

    hull.insert(ListUtil<std::size_t>::circularErase(hull, std::next(cut_b), cut_e), i);
}

void
MeshTriangulation::updateAdjacency(const TriangleCell& t,
                                   const LineCell& l1,
                                   const LineCell& l2,
                                   const LineCell& l3)
{
    m_edgeTrigAdj[l1].insert(t);
    m_edgeTrigAdj[l2].insert(t);
    m_edgeTrigAdj[l3].insert(t);
}

void MeshTriangulation::updateFlippable(const LineCell& l)
{
    m_flippable.erase(l);

    if(!m_edgeTrigAdj.count(l))
        return;
    if(m_edgeTrigAdj.at(l).size() != 2)
        return;

    std::size_t lf_a, lf_b;
    auto trig_it = m_edgeTrigAdj.at(l).begin();
    lf_a = trig_it->oppositePoint(l);
    std::advance(trig_it, 1);
    lf_b = trig_it->oppositePoint(l);

    if(lf_a == -1 || lf_b == -1)
        return;

    if(!convexPolygon(l.a, lf_a, l.b, lf_b))
        return;

    m_flippable[l] = LineCell(lf_a, lf_b);
}

