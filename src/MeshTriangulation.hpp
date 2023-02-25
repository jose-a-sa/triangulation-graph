#ifndef CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
#define CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <list>

#include "Point.hpp"
#include "MeshCell.hpp"

struct EdgeAdjacency
{
    bool internal = false;
    TriangleCell t1, t2;
};

class MeshTriangulation
{
public:
    MeshTriangulation();
    explicit MeshTriangulation(const std::vector<Point>& pt);
    explicit MeshTriangulation(std::vector<Point>&& pt);
    ~MeshTriangulation();

    bool operator==(const MeshTriangulation& other) const;

    [[nodiscard]] const std::vector<Point>& coordinates() const;
    [[nodiscard]] std::vector<EdgeCell> lines() const;
    [[nodiscard]] std::vector<TriangleCell> triangles() const;
    [[nodiscard]] const std::unordered_map<EdgeCell, EdgeCell>& flippableLines() const;
    [[nodiscard]] const std::map<EdgeCell, EdgeAdjacency>& edgeTriangleAdjacency() const;

    void triangulate();
    void flipLine(const EdgeCell& l);

    [[nodiscard]] std::string toString() const;
    friend std::size_t hash_value(const MeshTriangulation& mesh);

#ifndef GTEST_NAME

    friend class MeshTriangulationFixture;

#endif

private:
    std::vector<Point> m_coords;
    std::map<EdgeCell, EdgeAdjacency> m_edgeTrigAdj;
    std::unordered_map<EdgeCell, EdgeCell> m_flippable;
    std::size_t m_coordMeshHash = 0;

    void init();
    void sweepHullSort(std::vector<std::size_t>& idx, std::size_t pivot);
    void sweepHullAdd(std::list<std::size_t>& hull, std::size_t i);
    void updateAdjacency(const EdgeCell& l, const TriangleCell& t, const TriangleCell& newT);
    void insertAdjacency(const TriangleCell& t);

    [[nodiscard]] double triangleArea(std::size_t a, std::size_t b, std::size_t c) const;
    [[nodiscard]] bool convexPolygon(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const;
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
