//
// Created by Jose Sa on 21/02/2023.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "Point.hpp"
#include "MeshTriangulation.hpp"

class PointFactory
{
public:
    static std::vector<Point> randomSample(std::size_t n = 1, std::double_t a = 0.0, std::double_t b = 1.0)
    {
        std::random_device rd;
        std::mt19937 eng(rd());
        std::uniform_real_distribution<> dist(a, b);

        std::vector<Point> res;
        res.reserve(n);
        for(std::size_t i = 0; i < n; i++)
        {
            res.emplace_back(dist(eng), dist(eng));
        }
        return res;
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

    static std::unique_ptr<MeshTriangulation> makeMeshSingle(std::double_t x)
    {
        return MeshFactory::makeMesh(
            {Point(0, 0), Point(x, 0), Point(0, x)}
        );
    }
};

#endif //FACTORY_HPP
