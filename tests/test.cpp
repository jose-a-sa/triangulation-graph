#include <iostream>

#include "Factory.hpp"
#include "TriangulationFlipGraph.hpp"
#include "Utils.hpp"
#include <fstream>
#include <algorithm>
#include <vector>
#include <array>

int main(int argc, const char* argv[])
{
    const std::size_t RUNS = 100;
    std::vector<double> vertNumAvg;
    std::vector<double> edgeNumAvg;
    for(std::size_t sampleSize = 4; sampleSize <= 14; sampleSize++)
    {
        std::array<std::size_t, RUNS> vertNum{};
        std::array<std::size_t, RUNS> edgeNum{};
        TimerAverage<20> timeit(true);
        for(std::size_t run = 0; run < RUNS; run++)
        {
            TriangulationFlipGraph gr(PointFactory::randomSample(sampleSize));
            timeit.StartMeasure();
            gr.generateGraph();
            timeit.StopMeasure();
            vertNum[run] = gr.vertices().size();
            edgeNum[run] = gr.edges().size() / 2;
        }

        vertNumAvg.push_back(std::reduce(vertNum.begin(), vertNum.end(), 0.0) / RUNS);
        edgeNumAvg.push_back(std::reduce(edgeNum.begin(), edgeNum.end(), 0.0) / RUNS);
    }

    for(auto v : vertNumAvg)
        std::cout << v << " ";
    std::cout << "\n";
    for(auto v : edgeNumAvg)
        std::cout << v << " ";
    std::cout << "\n";
}
