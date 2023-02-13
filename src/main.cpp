#include <iostream>

#include "Point.hpp"
#include "MeshTriangulation.hpp"

int main()
{
    std::vector<Point<int, 2>> points{
            {0, 0},
            {1, 0},
            {0, 1},
            {1, 1},
            {1, 2},
            {0, 2}}; // points owned by MeshTriangulation

    std::cout << "Points created !" << std::endl;

    MeshTriangulation reg(std::move(points));
    reg.triangulateDelaunay();

    MeshTriangulation reg2{{0, 0},
                            {1, 0},
                            {0, 1},
                            {1, 1},
                            {1, 2},
                            {0, 2}};
    reg2.triangulateDelaunay();
    std::cout << reg2.wkt() << '\n';
    for (const auto &[k, v]: reg2.flippableLines())
        std::cout << k << "->" << v << " ";
    std::cout << std::endl;

    std::cout << reg.wkt() << '\n';
    for (const auto &[k, v]: reg.flippableLines())
        std::cout << k << "->" << v << " ";
    std::cout << std::endl;

    for (const auto &[k, v]: reg.flippableLines())
    {
        MeshTriangulation reg_f(reg);
        reg_f.flipEdge(k);
        std::cout << reg_f.wkt() << '\n';
        for (const auto &[k2, v2]: reg_f.flippableLines())
            std::cout << k2 << "->" << v2 << " ";
        std::cout << std::endl;
    }

    return 0;
}