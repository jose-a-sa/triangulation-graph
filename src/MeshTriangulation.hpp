#ifndef CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
#define CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP

#include <boost/functional/hash.hpp>
#include <functional>
#include <vector>
#include <memory>
#include <set>
#include <initializer_list>
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

    //void triangulateDelaunay();
    void triangulate();
    void flipLine(const LineCell& l);

    const std::vector<Point>& coordinates() const;
    const std::set<LineCell>& lines() const;
    const std::set<TriangleCell>& triangles() const;
    const std::unordered_map<LineCell, LineCell>& flippableLines() const;
    const std::unordered_map<LineCell, std::set<TriangleCell>>& edgeTriangleAdjacency() const;

    bool operator==(const MeshTriangulation& other) const;

    std::string wkt() const;
    friend std::size_t hash_value(const MeshTriangulation& mesh);
    const std::set<TriangleCell>& GetTriangles() const;

private:
    std::vector<Point> coord_;
    std::set<LineCell> lines_;
    std::set<TriangleCell> triangles_;
    std::unordered_map<LineCell, std::set<TriangleCell>> edgeTrigAdj_;
    std::unordered_map<LineCell, LineCell> flippable_;
    std::size_t coordMeshHash_;

    void init_();
    void sweepHullSort_(std::vector<std::size_t>& idx, std::size_t pivot);
    void sweepHullAdd_(std::list<std::size_t>& hull, std::size_t i);
    void updateAdjacency_(const TriangleCell& t);
    void updateAdjacency_(const TriangleCell& t, const LineCell& l1, const LineCell& l2, const LineCell& l3);
    void computeConnectivity_();
    void computeConnectivity_(const LineCell& l);
    void updateFlippable_(const LineCell& l);

    double triangleArea_(std::size_t a, std::size_t b, std::size_t c) const;
    bool convexPolygon_(std::size_t i, std::size_t j, std::size_t k, std::size_t l) const;
};

template<>
struct std::hash<MeshTriangulation>
{
    std::size_t operator()(const MeshTriangulation& t) const
    {
        return boost::hash<MeshTriangulation>()(t);
    }
};

class MeshFactory
{
public:
    static std::unique_ptr<MeshTriangulation> makeMesh(const std::vector<Point>& coord)
    {
        std::unique_ptr<MeshTriangulation> mesh =
            std::make_unique<MeshTriangulation>(coord);
        mesh->triangulate();
        return mesh;
    }

    static std::unique_ptr<MeshTriangulation> makeMeshSingle(double x)
    {
        return MeshFactory::makeMesh(
            {Point(0, 0), Point(x, 0), Point(0, x)}
        );
    }
};

#endif //CONVEX_TRIANGULATIONS_MESHTRIANGULATION_HPP
