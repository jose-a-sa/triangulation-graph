#include <iostream>

#include "Factory.hpp"
#include "TriangulationFlipGraph.hpp"
#include "Utils.hpp"

int main(int argc, const char* argv[])
{
    for(std::size_t sampleSize = 4; sampleSize <= 14; sampleSize++)
    {
        TimerAverage<20> timeit(true);
        for(std::size_t sample = 0; sample < 20; sample++)
        {
            TriangulationFlipGraph gr(PointFactory::randomSample(sampleSize));
            timeit.StartMeasure();
            gr.generateGraph();
            timeit.StopMeasure();
        }
    }
}
