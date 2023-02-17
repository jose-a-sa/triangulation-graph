#include <iostream>

#include "Utils.hpp"
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"

int main(int argc, const char* argv[])
{
    const std::size_t sampleSize = 20;
    RandomPoint randomPoint(-1.0, 1.0);
    std::vector<Point> pts = randomPoint.generate(10);

    for(const auto& pt : pts)
        std::cout << pt.toString() << " ";
    std::cout << "\n\n";

    MeshTriangulation mesh(pts);
    mesh.triangulate();
    std::cout << mesh.wkt() << std::endl;

//    std::cout << std::endl;
//    TriangulationFlipGraph gr({{0, 0}, {1, 0}, {0, 1}});
//    gr.generateGraph();
//
//    for(const auto& m : gr.vertices())
//        std::cout << m.wkt() << std::endl;
//    for(const auto& [h, t] : gr.edges())
//        std::cout << "{" << h << "," << t << "} ";
//    std::cout << std::endl;

    return 0;
}
