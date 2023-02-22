#include <iostream>

#include "Factory.hpp"
#include "TriangulationFlipGraph.hpp"


int main(int argc, const char* argv[])
{
    MeshTriangulation mesh(PointFactory::randomSample(100));
    mesh.triangulate();
    std::cout << mesh.wkt() << "\n";

    TriangulationFlipGraph gr(PointFactory::randomSample(10));
    gr.generateGraph();

    return 0;
}
