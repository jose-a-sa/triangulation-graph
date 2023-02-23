#ifndef CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
#define CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <vector>
#include <set>
#include <initializer_list>
#include <map>
#include <unordered_map>
#include <string>
#include <list>

#include "Point.hpp"
#include "MeshCell.hpp"


class MeshTriangulation
{
public:
    MeshTriangulation();
    explicit MeshTriangulation(const std::vector<Point>& pt);
    explicit MeshTriangulation(std::vector<Point>&& pt);
    ~MeshTriangulation();

    bool operator==(const MeshTriangulation& other) const;

    const std::vector<Point>& coordinates() const;
    std::vector<LineCell> lines() const;
    std::vector<TriangleCell> triangles() const;
    const std::unordered_map<LineCell, LineCell>& flippableLines() const;
    const std::map<LineCell, std::tuple<bool, TriangleCell, TriangleCell>>& edgeTriangleAdjacency() const;

    void triangulate();
    void flipLine(const LineCell& l);

    std::string wkt() const;
    friend std::size_t hash_value(const MeshTriangulation& mesh);

#ifndef GTEST_NAME

    friend class MeshTriangulationFixture;

#endif

private:
    std::vector<Point> m_coords;
    std::map<LineCell, std::tuple<bool, TriangleCell, TriangleCell>> m_edgeTrigAdj;
    std::unordered_map<LineCell, LineCell> m_flippable;
    std::size_t m_coordMeshHash;

    void init();
    void sweepHullSort(std::vector<std::size_t>& idx, std::size_t pivot);
    void sweepHullAdd(std::list<std::size_t>& hull, std::size_t i);
    void updateAdjacency(const LineCell& l, const TriangleCell& t, const TriangleCell& newT);
    void insertAdjacency(const TriangleCell& t);

    double triangleArea(std::size_t a, std::size_t b, std::size_t c) const;
    bool convexPolygon(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const;
};

template<>
struct std::hash<MeshTriangulation>
{
    std::size_t operator()(const MeshTriangulation& t) const
    {
        return boost::hash<MeshTriangulation>()(t);
    }
};

#endif //CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
