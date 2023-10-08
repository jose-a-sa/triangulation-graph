#include <benchmark/benchmark.h>

#include "MeshTriangulation.hpp"
#include "TriangulationFlipGraph.hpp"
#include "Factory.hpp"
#include <memory>

double RollingMean(double currentMean, double entry, std::size_t iteration)
{
    double n;
    n = static_cast<double>(iteration);
    return (currentMean * n + entry) / (n + 1);
}

static void BM_TriangulationFlipGraph_generateGraph(benchmark::State& state)
{
    for(auto _ : state)
    {
        state.PauseTiming();
        auto coords = PointFactory::randomSample(state.range(0));

        state.ResumeTiming();
        TriangulationFlipGraph mesh(*coords);
        mesh.generateGraph();

        state.PauseTiming();
        state.counters["Edges"] += static_cast<double>(mesh.edges().size());
        state.counters["Nodes"] += static_cast<double>(mesh.vertices().size());
    }

    state.SetComplexityN(state.range(0));
}

BENCHMARK(BM_TriangulationFlipGraph_generateGraph)
    ->DenseRange(4, 13, 1)
    ->Iterations(1)
    ->Repetitions(100)
    ->Complexity([](benchmark::IterationCount N) -> double
                 { return std::exp(1.2 * N); })
    ->ReportAggregatesOnly();

BENCHMARK_MAIN();
