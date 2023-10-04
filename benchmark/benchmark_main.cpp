#include <benchmark/benchmark.h>

#include "MeshTriangulation.hpp"
#include "TriangulationFlipGraph.hpp"
#include "../tests/Factory.hpp"
#include <memory>


static void BM_TriangulationFlipGraph_generateGraph(benchmark::State& state)
{
    for(auto _ : state)
    {
        state.PauseTiming();
        auto coords = PointFactory::randomSample(state.range(0));
        state.ResumeTiming();
        TriangulationFlipGraph mesh(*coords);
        mesh.generateGraph();
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_TriangulationFlipGraph_generateGraph)
    ->Iterations(40)
    ->DenseRange(4, 13, 1)
    ->Complexity([](benchmark::IterationCount n) -> double
                 { return std::exp(n); });

BENCHMARK_MAIN();
