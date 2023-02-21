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
    : coord_(pt), coordMeshHash_(0)
{
    init_();
}

MeshTriangulation::MeshTriangulation(std::vector<Point>&& pt)
    : coord_(std::move(pt)), coordMeshHash_(0)
{
    init_();
}

MeshTriangulation::~MeshTriangulation() = default;

void MeshTriangulation::init_()
{
    std::set<Point> v(coord_.begin(), coord_.end());
    coord_.assign(v.begin(), v.end());

    boost::hash<Point> hasher{};
    this->coordMeshHash_ = 0;
    for(const auto& p : this->coordinates())
        boost::hash_combine(this->coordMeshHash_, hasher(p));
}

void MeshTriangulation::flipLine(const LineCell& l)
{
    if(!edgeTrigAdj_.count(l) || !flippable_.count(l))
        return;

    LineCell l_flip = flippable_.at(l);
    TriangleCell t1_flip(l_flip.a, l_flip.b, l.a);
    TriangleCell t2_flip(l_flip.a, l_flip.b, l.b);

    lines_.insert(l_flip);
    triangles_.insert(t1_flip);
    triangles_.insert(t2_flip);

    edgeTrigAdj_[l_flip] = {t1_flip, t2_flip};
    flippable_.insert({l_flip, l});

    for(const auto& t : edgeTrigAdj_.at(l))
        triangles_.erase(t);
    edgeTrigAdj_.erase(l);
    lines_.erase(l);
    flippable_.erase(l);

    computeConnectivity_(LineCell(l.a, l_flip.a));
    computeConnectivity_(LineCell(l.a, l_flip.b));
    computeConnectivity_(LineCell(l_flip.a, l.b));
    computeConnectivity_(LineCell(l.b, l_flip.b));
}

inline double MeshTriangulation::triangleArea_(std::size_t a, std::size_t b, std::size_t c) const
{
    double s1, s2, s3;
    s1 = coord_[a].distance(coord_[b]);
    s2 = coord_[a].distance(coord_[c]);
    s3 = coord_[b].distance(coord_[c]);
    return 0.25 * sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3));
}

inline bool MeshTriangulation::convexPolygon_(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const
{
    double t1, t2, t1_f, t2_f;
    t1 = triangleArea_(i, j, k), t2 = triangleArea_(k, l, i);
    t1_f = triangleArea_(j, k, l), t2_f = triangleArea_(l, i, j);

    return boost::math::epsilon_difference(t1 + t2, t1_f + t2_f) < 3.0 &&
        boost::math::epsilon_difference(t1, 0) > 3.0 &&
        boost::math::epsilon_difference(t2, 0) > 3.0 &&
        boost::math::epsilon_difference(t1_f, 0) > 3.0 &&
        boost::math::epsilon_difference(t2_f, 0) > 3.0;
}

const std::vector<Point>& MeshTriangulation::coordinates() const
{
    return this->coord_;
}

const std::set<LineCell>& MeshTriangulation::lines() const
{
    return this->lines_;
}

const std::set<TriangleCell>& MeshTriangulation::triangles() const
{
    return this->triangles_;
}

const std::unordered_map<LineCell, LineCell>& MeshTriangulation::flippableLines() const
{
    return this->flippable_;
}

std::string MeshTriangulation::wkt() const
{
    std::ostringstream oss;
    oss.precision(std::numeric_limits<double>::max_digits10);
    oss << "MULTIPOLYGON(";
    for(const auto& t : triangles())
    {
        oss << "(";
        oss << coord_[t.a].x << " " << coord_[t.a].y << ",";
        oss << coord_[t.b].x << " " << coord_[t.b].y << ",";
        oss << coord_[t.c].x << " " << coord_[t.c].y;
        oss << (t != *std::prev(triangles().end()) ? "), " : ")");
    }
    oss << ")";
    return oss.str();
}

void MeshTriangulation::computeConnectivity_(const LineCell& l)
{
    if(edgeTrigAdj_.count(l))
        edgeTrigAdj_.at(l).clear();
    flippable_.erase(l);

    LineCell l_flip;
    for(const TriangleCell& t : triangles())
    {
        std::size_t o = t.oppositePoint(l);
        if(o != -1)
        {
            edgeTrigAdj_[l].emplace(t);
            l_flip.insert(o);
        }
    }

    if(l_flip.size() != 2)
        return;
    if(!convexPolygon_(l.a, l_flip.a, l.b, l_flip.b))
        return;

    flippable_.insert({l, l_flip});
}

void MeshTriangulation::computeConnectivity_()
{
    for(const LineCell& l : lines())
        computeConnectivity_(l);
}

std::size_t hash_value(const MeshTriangulation& mesh)
{
    std::size_t seed = mesh.coordMeshHash_;

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
    lines_.clear();
    triangles_.clear();

    std::size_t n = coord_.size();
    std::vector<std::size_t> idx(n);
    std::iota(idx.begin(), idx.end(), 0);

    sweepHullSort_(idx, 2 * n / 5);

    std::list<std::size_t> hull(idx.begin(), std::next(idx.begin(), 3));
    auto trig_it = triangles_.emplace(idx[0], idx[1], idx[2]).first;
    auto l1_it = lines_.emplace(idx[0], idx[1]).first;
    auto l2_it = lines_.emplace(idx[1], idx[2]).first;
    auto l3_it = lines_.emplace(idx[2], idx[0]).first;
    updateAdjacency_(*trig_it, *l1_it, *l2_it, *l3_it);

    for(std::size_t i = 3; i < n; i++)
    {
        sweepHullAdd_(hull, idx[i]);
    }

    computeConnectivity_();
}

void MeshTriangulation::sweepHullSort_(std::vector<std::size_t>& idx, std::size_t pivot = 0)
{
    size_t n = idx.size();

    std::size_t i0 = pivot;
    while(i0 < n + pivot)
    {
        std::swap(idx[0], idx[i0 % n]);

        auto it1 = std::min_element(
            std::next(idx.begin(), 1), idx.end(),
            [&](std::size_t i, std::size_t j) -> bool
            {
                return coord_[idx[0]].distance(coord_[i]) < coord_[idx[0]].distance(coord_[j]);
            });
        std::swap(idx[1], *it1);

        std::size_t i2 = 2;
        double min_r = Point::circumcircle(coord_[idx[0]], coord_[idx[1]], coord_[idx[2]]).second;
        if(std::isnan(min_r))
        {
            ++i0;
            continue;
        }
        for(std::size_t k = 3; k < n; k++)
        {
            double curr_r = Point::circumcircle(coord_[idx[0]], coord_[idx[1]], coord_[idx[k]]).second;
            if(!std::isnan(curr_r) && min_r > curr_r)
            {
                min_r = curr_r;
                i2 = k;
            }
        }
        std::swap(idx[2], idx[i2]);

        double cross = Point::cross(coord_[idx[0]], coord_[idx[1]], coord_[idx[2]]);
        if(boost::math::epsilon_difference(cross, 0) < 3.0)
        {
            ++i0;
            continue;
        }
        if(cross > 0)
            std::swap(idx[1], idx[2]);

        const auto& [cc, radius] = Point::circumcircle(
            coord_[idx[0]], coord_[idx[1]], coord_[idx[2]]);
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
                          return CC.distance(coord_[i]) < CC.distance(coord_[j]);
                      });
        }

        break;
    }
}

void MeshTriangulation::sweepHullAdd_(std::list<std::size_t>& hull, std::size_t idx)
{
    auto circularNext = [&](std::list<std::size_t>::iterator it)
    { return std::next(it) == hull.end() ? std::next(it, 2) : std::next(it); };

    bool visible_prev, visible = false;
    std::list<std::size_t>::iterator it, it_next, cut_b = hull.end(), cut_e = hull.end();
    for(it = hull.begin(), it_next = circularNext(hull.begin()); it != hull.end(); it++, it_next = circularNext(it))
    {
        visible_prev = visible;
        visible = Point::cross(coord_[idx], coord_[*it], coord_[*it_next]) > 0;
        if(visible)
        {
            cut_b = visible_prev ^ visible ? it : cut_b;
            auto trig_it = triangles_.emplace(idx, *it, *it_next).first;
            auto l1_it = lines_.emplace(idx, *it).first;
            auto l2_it = lines_.emplace(idx, *it_next).first;
            updateAdjacency_(*trig_it, *l1_it, *l2_it, LineCell(*it, *it_next));
        }
        else
            cut_e = visible_prev ^ visible ? it : cut_e;
    }

    hull.insert(ListUtil<std::size_t>::circularErase(hull, std::next(cut_b), cut_e), idx);
}

void
MeshTriangulation::updateAdjacency_(const TriangleCell& t)
{
    updateAdjacency_(t, LineCell(t.a, t.b), LineCell(t.a, t.c), LineCell(t.b, t.c));
}

void
MeshTriangulation::updateAdjacency_(const TriangleCell& t, const LineCell& l1, const LineCell& l2, const LineCell& l3)
{
    edgeTrigAdj_[l1].insert(t);
    edgeTrigAdj_[l2].insert(t);
    edgeTrigAdj_[l3].insert(t);
}

void MeshTriangulation::updateFlippable_(const LineCell& l)
{
    if(!edgeTrigAdj_.count(l))
        return;
    if(edgeTrigAdj_.at(l).size() != 2)
        return;

    std::size_t lf_a, lf_b;
    auto trig_it = edgeTrigAdj_.at(l).begin();
    lf_a = trig_it->oppositePoint(l);
    std::advance(trig_it, 1);
    lf_b = trig_it->oppositePoint(l);

    if(lf_a == -1 || lf_b == -1)
        return;

    if(!convexPolygon_(l.a, lf_a, l.b, lf_b))
        return;

    flippable_[l] = LineCell(lf_a, lf_b);
}

