#include "MeshTriangulation.hpp"

#include <boost/math/special_functions/relative_difference.hpp>
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <list>
#include <set>
#include <utility>

template<typename T>
class ListUtil
{
public:
    using list_iterator = typename std::list<T>::iterator;
    static list_iterator circularErase(std::list<T>& lst, list_iterator b, list_iterator e)
    {
        if(b == e)
            return e;

        auto b_c = b;
        while(b_c != e)
        {
            if(b_c++ == lst.end())
            {
                auto dist = std::distance(b, e) - 1;
                lst.splice(lst.end(), lst, e, b);
                return lst.erase(lst.begin(), std::next(lst.begin(), dist));
            }
        }

        return lst.erase(b, e);
    }
};

MeshTriangulation::MeshTriangulation(std::weak_ptr<std::vector<Point>> pts)
    : mp_coords(std::move(pts))
{
}

void MeshTriangulation::flipLine(const EdgeCell& l)
{
    if(!m_edgeTrigAdj.count(l) || !m_flippable.count(l))
        return;

    std::size_t la = l.a, lb = l.b;
    EdgeCell lFlip = m_flippable.at(l);
    m_edgeTrigAdj.erase(l);
    m_flippable.erase(l);

    TriangleCell trig1(la, lb, lFlip.a), trig2(la, lb, lFlip.b);
    TriangleCell trigFlip1(lFlip.a, lFlip.b, la), trigFlip2(lFlip.a, lFlip.b, lb);

    m_edgeTrigAdj[lFlip] = EdgeAdjacency{true, trigFlip1, trigFlip2};
    m_flippable[lFlip] = l;

    updateAdjacency(EdgeCell(l.a, lFlip.a), trig1, trigFlip1);
    updateAdjacency(EdgeCell(l.a, lFlip.b), trig2, trigFlip1);
    updateAdjacency(EdgeCell(lFlip.a, l.b), trig1, trigFlip2);
    updateAdjacency(EdgeCell(l.b, lFlip.b), trig2, trigFlip2);
}

inline std::double_t
MeshTriangulation::triangleArea(const std::vector<Point>& coords, std::size_t a, std::size_t b, std::size_t c)
{
    std::double_t s1, s2, s3;
    s1 = coords[a].distance(coords[b]);
    s2 = coords[a].distance(coords[c]);
    s3 = coords[b].distance(coords[c]);
    return 0.25 * sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3));
}

inline bool MeshTriangulation::convexPolygon(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const
{
    BOOST_ASSERT_MSG(!mp_coords.expired(), "vector<Point> pointer has expired");
    auto& coords = *mp_coords.lock();

    std::size_t n = coords.size();
    if(i >= n || j >= n || k >= n || l >= n)
        return false;

    std::double_t t1, t2, t1_f, t2_f;
    t1 = triangleArea(coords, i, j, k), t2 = triangleArea(coords, k, l, i);
    t1_f = triangleArea(coords, j, k, l), t2_f = triangleArea(coords, l, i, j);

    return boost::math::epsilon_difference(t1 + t2, t1_f + t2_f) < 3.0 &&
        boost::math::epsilon_difference(t1_f, 0) > 3.0 &&
        boost::math::epsilon_difference(t2_f, 0) > 3.0;
}

const std::vector<Point>& MeshTriangulation::coordinates() const
{
    BOOST_ASSERT_MSG(!mp_coords.expired(), "vector<Point> pointer has expired");
    auto& coords = *mp_coords.lock();

    return coords;
}

std::vector<EdgeCell> MeshTriangulation::lines() const
{
    std::vector<EdgeCell> res;
    res.reserve(m_edgeTrigAdj.size());
    for(auto& kv : m_edgeTrigAdj)
        res.push_back(kv.first);
    return res;
}

std::vector<TriangleCell> MeshTriangulation::triangles() const
{
    std::set < TriangleCell > res;
    for(auto& kv : m_edgeTrigAdj)
    {
        auto& adj = kv.second;
        res.insert(adj.t1);
        if(adj.internal)
            res.insert(adj.t2);
    }
    return {res.begin(), res.end()};
}

const std::unordered_map<EdgeCell, EdgeCell>& MeshTriangulation::flippableLines() const
{
    return this->m_flippable;
}

const std::map<EdgeCell, EdgeAdjacency>& MeshTriangulation::edgeTriangleAdjacency() const
{
    return this->m_edgeTrigAdj;
}

std::string MeshTriangulation::toString() const
{
    BOOST_ASSERT_MSG(!mp_coords.expired(), "vector<Point> pointer has expired");
    auto& coords = *mp_coords.lock();

    std::ostringstream oss;
    oss.precision(std::numeric_limits<std::double_t>::max_digits10);
    oss << "(";
    for(const auto& t : triangles())
    {
        oss << "(";
        oss << coords[t.a].x << " " << coords[t.a].y << ",";
        oss << coords[t.b].x << " " << coords[t.b].y << ",";
        oss << coords[t.c].x << " " << coords[t.c].y;
        oss << (t != *std::prev(triangles().end()) ? "), " : ")");
    }
    oss << ")";
    return oss.str();
}

std::size_t hash_value(const MeshTriangulation& mesh)
{
    boost::hash<EdgeCell> hasher{};

    std::size_t seed = 0;
    for(const auto& [l, _] : mesh.edgeTriangleAdjacency())
        boost::hash_combine(seed, hasher(l));
    return seed;
}

bool MeshTriangulation::operator==(const MeshTriangulation& other) const
{
    return this->coordinates() == other.coordinates() && this->lines() == other.lines();
}

void MeshTriangulation::triangulate()
{
    BOOST_ASSERT_MSG(!mp_coords.expired(), "vector<Point> pointer has expired");
    auto& coords = *mp_coords.lock();
    std::size_t n = coords.size();

    m_flippable.clear();
    m_edgeTrigAdj.clear();

    if(n < 3)
        return;

    std::vector<std::size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);
    sweepHullSort(coords, idx, 0);

    std::list<std::size_t> hull(idx.begin(), std::next(idx.begin(), 3));
    insertAdjacency(TriangleCell(idx[0], idx[1], idx[2]));

    for(std::size_t i = 3; i < n; i++)
        sweepHullAdd(coords, hull, idx[i]);
}

void
MeshTriangulation::sweepHullSort(const std::vector<Point>& coords, std::vector<std::size_t>& idx, std::size_t pivot = 0)
{
    size_t n = idx.size();

    std::size_t i0 = pivot;
    while(i0 < n + pivot)
    {
        std::swap(idx[0], idx[i0 % n]);

        std::size_t i1 = 1;
        std::double_t minDist = coords[idx[0]].distance(coords[idx[1]]);
        for(std::size_t k = 2; k < n; k++)
        {
            std::double_t currDist = coords[idx[0]].distance(coords[idx[k]]);
            if(minDist > currDist)
            {
                minDist = currDist;
                i1 = k;
            }
        }
        std::swap(idx[1], idx[i1]);

        std::size_t i2 = 2;
        std::double_t min_r = Point::circumcircle(coords[idx[0]], coords[idx[1]], coords[idx[2]]).second;
        if(std::isnan(min_r))
        {
            ++i0;
            continue;
        }
        for(std::size_t k = 3; k < n; k++)
        {
            std::double_t curr_r = Point::circumcircle(coords[idx[0]], coords[idx[1]], coords[idx[k]]).second;
            if(!std::isnan(curr_r) && min_r > curr_r)
            {
                min_r = curr_r;
                i2 = k;
            }
        }
        std::swap(idx[2], idx[i2]);

        std::double_t cross = Point::cross(coords[idx[0]], coords[idx[1]], coords[idx[2]]);
        if(boost::math::epsilon_difference(cross, 0) < 3.0)
        {
            ++i0;
            continue;
        }
        if(cross > 0)
            std::swap(idx[1], idx[2]);


        const auto& [cc, radius] = Point::circumcircle(
            coords[idx[0]], coords[idx[1]], coords[idx[2]]);
        if(std::isnan(radius))
        {
            ++i0;
            continue;
        }

        if(n > 4)
        {
            std::sort(std::next(idx.begin(), 3), idx.end(),
                      [&CC = cc, &coords = coords](std::size_t i, std::size_t j) -> bool
                      {
                          return CC.distance(coords[i]) < CC.distance(coords[j]);
                      });
        }

        break;
    }
}

void MeshTriangulation::sweepHullAdd(const std::vector<Point>& coords, std::list<std::size_t>& hull, std::size_t i)
{
    auto circularNext = [&](std::list<std::size_t>::iterator it)
    { return std::next(it) == hull.end() ? std::next(it, 2) : std::next(it); };

    bool visible_prev, visible = false;
    std::list<std::size_t>::iterator it, it_next, cut_b = hull.end(), cut_e = hull.end();
    for(it = hull.begin(), it_next = circularNext(hull.begin());
        it != hull.end();
        it++, it_next = circularNext(it))
    {
        visible_prev = visible;
        visible = Point::cross(coords[i], coords[*it], coords[*it_next]) > 0;
        if(visible)
        {
            cut_b = visible_prev ^ visible ? it : cut_b;
            insertAdjacency(TriangleCell(i, *it, *it_next));
        }
        else
            cut_e = visible_prev ^ visible ? it : cut_e;
    }

    // this can be improved by a circular doubly-liked list implementation
    // with end() pointing to begin() value instead of this -> &size()
    hull.insert(ListUtil<std::size_t>::circularErase(hull, std::next(cut_b), cut_e), i);
}

void MeshTriangulation::updateAdjacency(const EdgeCell& l,
                                        const TriangleCell& t,
                                        const TriangleCell& newT)
{
    auto& adj = m_edgeTrigAdj[l];
    if(adj.t1 == t)
        adj.t1 = newT;
    else if(adj.t2 == t)
        adj.t2 = newT;

    m_flippable.erase(l);

    if(!adj.internal)
        return;

    // update flippable
    std::size_t lf_a = adj.t1.oppositePoint(l), lf_b = adj.t2.oppositePoint(l);
    if(convexPolygon(l.a, lf_a, l.b, lf_b))
        m_flippable[l] = EdgeCell(lf_a, lf_b);
}

void MeshTriangulation::insertAdjacency(const TriangleCell& t)
{
    for(auto& l : {EdgeCell{t.a, t.b}, EdgeCell{t.a, t.c}, EdgeCell{t.b, t.c}})
    {
        auto& adj = m_edgeTrigAdj[l];
        if(adj.t1.undefined())
            adj.t1 = t, adj.internal = false;
        else if(adj.t2.undefined())
        {
            adj.t2 = t, adj.internal = true;

            // update flippable
            std::size_t lf_a = adj.t1.oppositePoint(l), lf_b = adj.t2.oppositePoint(l);
            if(convexPolygon(l.a, lf_a, l.b, lf_b))
                m_flippable[l] = EdgeCell(lf_a, lf_b);
        }
    }
}