#include "MeshTriangulation.hpp"

#include <boost/assert.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/polygon/voronoi.hpp>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <iomanip>

namespace boost::polygon
{
    template<>
    struct geometry_concept<Point>
    {
        typedef point_concept type;
    };

    template<>
    struct point_traits<Point>
    {
        typedef double coordinate_type;

        static inline coordinate_type
        get(const Point& point, const orientation_2d& orient)
        {
            return (orient == HORIZONTAL) ? point.x : point.y;
        }
    };
}

MeshTriangulation::MeshTriangulation() = default;

MeshTriangulation::MeshTriangulation(const std::vector<Point>& pt)
    : coord_(pt)
{
    init_();
}

MeshTriangulation::MeshTriangulation(std::vector<Point>&& pt)
    : coord_(std::move(pt))
{
    init_();
}

MeshTriangulation::~MeshTriangulation() = default;

void MeshTriangulation::init_()
{
    std::set<Point> v(coord_.begin(), coord_.end());
    coord_.assign(v.begin(), v.end());

    triangulate_();

    boost::hash<Point> hasher{};
    this->coordMeshHash_ = 0;
    for(const auto& p : this->coordinates())
        boost::hash_combine(this->coordMeshHash_, hasher(p));
}

void MeshTriangulation::setCoordinates(const std::vector<Point>& pt)
{
    coord_ = pt;
    init_();
}

void MeshTriangulation::setCoordinates(std::vector<Point>&& pt)
{
    coord_ = std::move(pt);
    init_();
}

void MeshTriangulation::triangulate()
{
    lines_.clear();
    triangles_.clear();

    boost::polygon::voronoi_diagram<double> vd;
    boost::polygon::construct_voronoi(coordinates().begin(), coordinates().end(), &vd);

    for(const auto& vertex : vd.vertices())
    {
        const boost::polygon::voronoi_edge<double>* base_edge = vertex.incident_edge();

        std::queue<const boost::polygon::voronoi_edge<double>*> bfs;
        bfs.push(base_edge->rot_next());
        std::size_t base_idx = base_edge->cell()->source_index();

        while(bfs.back() != base_edge && !bfs.empty())
        {
            const boost::polygon::voronoi_edge<double>* edge = bfs.back();
            std::size_t idx = edge->cell()->source_index();

            BOOST_ASSERT_MSG(edge->cell()->contains_point(),
                "LineCell cell does not contain point, in MeshTriangulation::triangulate");

            lines_.emplace(base_idx, idx);

            if(bfs.size() == 2)
            {
                std::size_t next_idx = bfs.front()->cell()->source_index();
                lines_.emplace(idx, next_idx);
                triangles_.emplace(base_idx, idx, next_idx);
                bfs.pop();
            }

            bfs.push(edge->rot_next());
        }
    }

    computeConnectivity_();
}

void MeshTriangulation::flipLine(const LineCell& l)
{
    if(!edgeTrigAdj_.count(l))
        return;

    if(!flippable_.count(l))
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

const std::unordered_map<LineCell, std::set<TriangleCell>>& MeshTriangulation::edgeTriangleAdjacency() const
{
    return this->edgeTrigAdj_;
}

std::string MeshTriangulation::wkt() const
{
    std::ostringstream oss;
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

void MeshTriangulation::triangulate_()
{
    std::vector<std::size_t> idx(coord_.size());
    std::iota(idx.begin(), idx.end(), 0);

    sweepHullSort_(idx, 2 * coord_.size() / 5);

    for(auto j : idx)
    {
        auto [ccc, rr] = Point::circumcircle(coord_[idx[0]], coord_[idx[1]], coord_[j]);
        std::cout << "[" << j << "]"
                  << std::setw(25) << coord_[j].toString()
                  << std::setw(12) << coord_[j].distance(coord_[idx[0]])
                  << std::setw(25) << ccc.toString()
                  << std::setw(12) << rr
                  << "\n";
    }
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
        double min_r = Point::circumcircle(coord_[idx[0]], coord_[idx[1]],
            coord_[idx[2]]).second;
        if(std::isnan(min_r))
        {
            ++i0;
            continue;
        }
        for(std::size_t k = 3; k < n; k++)
        {
            double curr_r = Point::circumcircle(coord_[idx[0]], coord_[idx[1]],
                coord_[idx[k]]).second;
            if(!std::isnan(curr_r) && min_r > curr_r)
            {
                min_r = curr_r;
                i2 = k;
            }
        }
        std::swap(idx[2], idx[i2]);

        double cross = Point::cross(
            coord_[idx[0]], coord_[idx[1]], coord_[idx[2]]);
        if(cross > 0)
            std::swap(idx[1], idx[2]);
        else if(cross == 0)
        {
            ++i0;
            continue;
        }

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


