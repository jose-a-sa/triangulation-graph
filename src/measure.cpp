#include <iostream>

#include "Utils.hpp"
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"
#include <list>


int main(int argc, const char* argv[])
{
    RandomPoint randomPoint(-15.0, 15.0);
    std::vector<Point> pts = randomPoint.generate(10);

    MeshTriangulation mesh(pts);
    mesh.triangulateDelaunay();
    std::cout << mesh.wkt() << std::endl;
    mesh.triangulate();
    std::cout << mesh.wkt() << std::endl;

    auto mesh2 = MeshFactory::makeMeshSingle(0.001);
    for(auto t : mesh2->triangles())
        std::cout << t << "\n";
    for(auto l : mesh2->lines())
        std::cout << l << "\n";

    return 0;
}
