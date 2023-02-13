#pragma once

#include <boost/assert.hpp>
#include <boost/math/special_functions/relative_difference.hpp>
#include <boost/polygon/voronoi.hpp>
#include <vector>
#include <set>
#include <initializer_list>
#include <unordered_map>

#include "MeshCells.hpp"
#include "Point.hpp"

namespace boost::polygon
{
    template <>
    struct geometry_concept<Point<int, 2>>
    {
        typedef point_concept type;
    };

    template <>
    struct point_traits<Point<int, 2>>
    {
        typedef int coordinate_type;

        static inline coordinate_type get(const Point<int, 2> &point, orientation_2d orient)
        {
            return (orient == HORIZONTAL) ? point[0] : point[1];
        }
    };
}

class MeshTriangulation
{
public:
    MeshTriangulation(const MeshTriangulation &mesh) = default;
    MeshTriangulation(const std::vector<Point<int, 2>> &pt);
    MeshTriangulation(std::vector<Point<int, 2>> &&pt);
    MeshTriangulation(std::initializer_list<Point<int, 2>> &&pt);
    ~MeshTriangulation();
    void triangulateDelaunay();
    void flipEdge(const LineCell &l);
    const double triangleArea(const TriangleCell &t) const;
    const bool collinear(std::size_t i1, std::size_t i2, std::size_t i3) const;
    const std::vector<Point<int, 2>> &points() const;
    const std::set<LineCell> &lines() const;
    const std::set<TriangleCell> &triangles() const;
    const std::unordered_map<LineCell, LineCell> &flippableLines() const;
    const std::unordered_map<LineCell, std::set<TriangleCell>> &edgeTriangleAdjacency() const;
    const std::string wkt() const;

private:
    std::vector<Point<int, 2>> points_;
    std::set<LineCell> lines_;
    std::set<TriangleCell> triangles_;
    std::unordered_map<LineCell, std::set<TriangleCell>> edgeTrigAdj_;
    std::unordered_map<LineCell, LineCell> flippable_;
    void computeConnectivity_();
    void computeConnectivity_(const LineCell &l);
};

inline MeshTriangulation::MeshTriangulation(const std::vector<Point<int, 2>> &pt)
    : points_(pt)
{
}

inline MeshTriangulation::MeshTriangulation(std::vector<Point<int, 2>> &&pt)
    : points_(std::move(pt))
{
}

inline MeshTriangulation::MeshTriangulation(std::initializer_list<Point<int, 2>> &&pt)
    : points_(std::move(pt))
{
}

inline MeshTriangulation::~MeshTriangulation()
{
}

inline void MeshTriangulation::triangulateDelaunay()
{
    lines_.clear();
    triangles_.clear();

    boost::polygon::voronoi_diagram<double> vd;
    boost::polygon::construct_voronoi(points_.begin(), points_.end(), &vd);

    for (const auto &vertex : vd.vertices())
    {
        const boost::polygon::voronoi_edge<double> *base_edge = vertex.incident_edge();

        std::queue<const boost::polygon::voronoi_edge<double> *> bfs;
        bfs.push(base_edge->rot_next());
        std::size_t base_idx = base_edge->cell()->source_index();

        while (bfs.back() != base_edge && !bfs.empty())
        {
            const boost::polygon::voronoi_edge<double> *edge = bfs.back();
            std::size_t idx = edge->cell()->source_index();

            BOOST_ASSERT_MSG(edge->cell()->contains_point(),
                             "LineCell cell does not contain point, in MeshTriangulation::triangulateDelaunay");

            lines_.insert(LineCell(base_idx, idx));

            if (bfs.size() == 2)
            {
                std::size_t next_idx = bfs.front()->cell()->source_index();
                lines_.insert(LineCell(idx, next_idx));
                triangles_.insert(TriangleCell(base_idx, idx, next_idx));
                bfs.pop();
            }

            bfs.push(edge->rot_next());
        }
    }

    computeConnectivity_();
}

inline void MeshTriangulation::flipEdge(const LineCell &l)
{
    if (!edgeTrigAdj_.count(l))
        return;

    if (!flippable_.count(l))
        return;

    const LineCell &l_flip = flippable_.at(l);

    lines_.insert(std::move(l_flip));
    TriangleCell t1_flip(l.a, l_flip.a, l_flip.b);
    TriangleCell t2_flip(l.b, l_flip.a, l_flip.b);
    triangles_.insert(std::move(t1_flip));
    triangles_.insert(std::move(t2_flip));

    std::set<TriangleCell> t_pair{std::move(t1_flip), std::move(t2_flip)};
    edgeTrigAdj_.insert({std::move(l_flip), std::move(t_pair)});

    flippable_.insert({std::move(l_flip), std::move(l)});

    std::vector<LineCell> bd_edges{
        LineCell(l.a, l_flip.a), LineCell(l.a, l_flip.b),
        LineCell(l.b, l_flip.a), LineCell(l.b, l_flip.b)};

    for (const auto &t : edgeTrigAdj_.at(l))
        triangles_.erase(t);
    edgeTrigAdj_.erase(l);
    lines_.erase(l);
    flippable_.erase(l);

    for (const auto &e : bd_edges)
        computeConnectivity_(e);
}

inline const double MeshTriangulation::triangleArea(const TriangleCell &t) const
{
    double s1, s2, s3;

    s1 = points_[t.a].distance(points_[t.b]);
    s2 = points_[t.a].distance(points_[t.c]);
    s3 = points_[t.b].distance(points_[t.c]);

    return sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3)) / 4;
}

inline const bool MeshTriangulation::collinear(std::size_t i1, std::size_t i2, std::size_t i3) const
{
    int dx1, dy1, dx2, dy2;
    dx1 = points_[i2][0] - points_[i1][0];
    dy1 = points_[i2][1] - points_[i1][1];
    dx2 = points_[i3][0] - points_[i1][0];
    dy2 = points_[i3][1] - points_[i1][1];
    return dx1 * dy2 == dx2 * dy1;
}

inline const std::vector<Point<int, 2>> &
MeshTriangulation::points() const
{
    return this->points_;
}

inline const std::set<LineCell> &
MeshTriangulation::lines() const
{
    return this->lines_;
}

inline const std::set<TriangleCell> &
MeshTriangulation::triangles() const
{
    return this->triangles_;
}

const std::string MeshTriangulation::wkt() const
{
    std::ostringstream oss;
    oss << "MULTIPOLYGON(";
    for (const auto &t : triangles())
    {
        oss << "(";
        oss << points_[t.a].wkt() << ",";
        oss << points_[t.b].wkt() << ",";
        oss << points_[t.c].wkt();
        oss << (t != *std::prev(triangles().end()) ? "), " : ")");
    }
    oss << ")";
    return oss.str();
}

const std::unordered_map<LineCell, std::set<TriangleCell>> &
MeshTriangulation::edgeTriangleAdjacency() const
{
    return edgeTrigAdj_;
}

const std::unordered_map<LineCell, LineCell> &MeshTriangulation::flippableLines() const
{
    return flippable_;
}

void MeshTriangulation::computeConnectivity_(const LineCell &l)
{
    if (edgeTrigAdj_.count(l))
        edgeTrigAdj_.at(l).clear();
    flippable_.erase(l);

    std::set<std::size_t> eg_flip{};
    for (const TriangleCell &t : triangles())
    {
        if (t.contains(l))
        {
            edgeTrigAdj_[l].insert(std::move(t));
            eg_flip.merge(t.difference(l));
        }
    }

    if (eg_flip.size() != 2)
        return;

    LineCell l_flip(eg_flip);

    if (boost::math::epsilon_difference(
            triangleArea(TriangleCell(l.a, l_flip.a, l_flip.b)),
            triangleArea(TriangleCell(l.b, l_flip.a, l_flip.b))) > 3.0)
        return;

    if (collinear(l_flip.a, l.a, l.b) || collinear(l_flip.b, l.a, l.b))
        return;

    flippable_.insert({std::move(l), std::move(l_flip)});
}

void MeshTriangulation::computeConnectivity_()
{
    for (const LineCell &l : lines())
        computeConnectivity_(l);
}
