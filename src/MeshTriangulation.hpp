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
    template<>
    struct geometry_concept<Point<int, 2>>
    {
        typedef point_concept type;
    };

    template<>
    struct point_traits<Point<int, 2>>
    {
        typedef int coordinate_type;

        static inline coordinate_type get(const Point<int, 2> &point, const orientation_2d &orient)
        {
            return (orient == HORIZONTAL) ? point[0] : point[1];
        }
    };
}

class MeshTriangulation
{
public:
    MeshTriangulation(const MeshTriangulation &mesh) = default;

    explicit MeshTriangulation(std::vector<Point<int, 2>> &&pt);

    MeshTriangulation(std::initializer_list<Point<int, 2>> &&lst);

    ~MeshTriangulation() = default;

    void triangulateDelaunay();

    void flipEdge(const LineCell &l);

    const std::set<LineCell> &lines() const;

    const std::set<TriangleCell> &triangles() const;

    const std::unordered_map<LineCell, LineCell> &flippableLines() const;

    const std::unordered_map<LineCell, std::set<TriangleCell>> &edgeTriangleAdjacency() const;

    std::string wkt() const;

private:
    std::vector<Point<int, 2>> points_;
    std::set<LineCell> lines_;
    std::set<TriangleCell> triangles_;
    std::unordered_map<LineCell, std::set<TriangleCell>> edgeTrigAdj_;
    std::unordered_map<LineCell, LineCell> flippable_;

    void computeConnectivity_();

    void computeConnectivity_(const LineCell &l);

    double triangleArea_(const TriangleCell &t) const;

    bool collinear_(std::size_t i1, std::size_t i2, std::size_t i3) const;
};

inline MeshTriangulation::MeshTriangulation(std::vector<Point<int, 2>> &&pt)
        : points_(std::move(pt))
{
}

MeshTriangulation::MeshTriangulation(std::initializer_list<Point<int, 2>> &&lst)
        : points_(lst)
{
}

inline void MeshTriangulation::triangulateDelaunay()
{
    lines_.clear();
    triangles_.clear();

    boost::polygon::voronoi_diagram<double> vd;
    boost::polygon::construct_voronoi(points_.begin(), points_.end(), &vd);

    for (const auto &vertex: vd.vertices())
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

    lines_.insert(l_flip);
    TriangleCell t1_flip(l_flip.a, l_flip.b, l.a);
    TriangleCell t2_flip(l_flip.a, l_flip.b, l.b);
    triangles_.insert(t1_flip);
    triangles_.insert(t2_flip);

    std::set < TriangleCell > t_pair{t1_flip, t2_flip};
    edgeTrigAdj_.insert({l_flip, t_pair});

    std::vector<LineCell> bd_edges{
            LineCell(l.a, l_flip.a), LineCell(l.a, l_flip.b),
            LineCell(l_flip.a, l.b), LineCell(l.b, l_flip.b)};

    flippable_.insert({l_flip, l});

    for (const auto &t: edgeTrigAdj_.at(l))
        triangles_.erase(t);
    edgeTrigAdj_.erase(l);
    lines_.erase(l);
    flippable_.erase(l);

    for (const auto &e: bd_edges)
        computeConnectivity_(e);
}

inline double MeshTriangulation::triangleArea_(const TriangleCell &t) const
{
    double s1, s2, s3;

    s1 = points_[t.a].distance(points_[t.b]);
    s2 = points_[t.a].distance(points_[t.c]);
    s3 = points_[t.b].distance(points_[t.c]);

    return sqrt((s1 + s2 + s3) * (-s1 + s2 + s3) * (s1 - s2 + s3) * (s1 + s2 - s3)) / 4;
}

inline bool MeshTriangulation::collinear_(std::size_t i1, std::size_t i2, std::size_t i3) const
{
    int dx1, dy1, dx2, dy2;
    dx1 = points_[i2][0] - points_[i1][0];
    dy1 = points_[i2][1] - points_[i1][1];
    dx2 = points_[i3][0] - points_[i1][0];
    dy2 = points_[i3][1] - points_[i1][1];
    return dx1 * dy2 == dx2 * dy1;
}

inline const std::set<LineCell> & MeshTriangulation::lines() const
{
    return this->lines_;
}

inline const std::set<TriangleCell> & MeshTriangulation::triangles() const
{
    return this->triangles_;
}

std::string MeshTriangulation::wkt() const
{
    std::ostringstream oss;
    oss << "MULTIPOLYGON(";
    for (const auto &t: triangles())
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

    std::set < std::size_t > eg_flip{};
    for (const TriangleCell &t: triangles())
    {
        if (t.contains(l))
        {
            edgeTrigAdj_[l].insert(t);
            eg_flip.merge(t.difference(l));
        }
    }

    if (eg_flip.size() != 2)
        return;

    LineCell l_flip(eg_flip);

    if (boost::math::epsilon_difference(
            triangleArea_(TriangleCell(l_flip.a, l_flip.b, l.a)),
            triangleArea_(TriangleCell(l_flip.a, l_flip.b, l.b))) > 3.0)
        return;

    if (collinear_(l_flip.a, l.a, l.b) || collinear_(l_flip.b, l.a, l.b))
        return;

    flippable_.insert({l, l_flip});
}

void MeshTriangulation::computeConnectivity_()
{
    for (const LineCell &l: lines())
        computeConnectivity_(l);
}
