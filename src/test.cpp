#include <iostream>

#include "Utils.hpp"
#include "Point.hpp"
#include "TriangulationFlipGraph.hpp"


int main(int argc, const char* argv[])
{
    TriangulationFlipGraph gr(PointFactory::randomSample(4));
    gr.generateGraph();

    return 0;
}
