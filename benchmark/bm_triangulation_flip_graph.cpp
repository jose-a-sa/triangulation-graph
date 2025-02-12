#include <benchmark/benchmark.h>

#include <cstddef>

#include <initializer_list>
#include <vector>

#include "mesh_triangulation_2d.h"
#include "triangulation_flip_graph.h"
#include "vec2.h"


struct PointSet1
{
    static constexpr std::initializer_list<Vec2> value = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {0, 2}, {1, 2},
                                                          {2, 0}, {2, 1}, {2, 2}, {0, 3}, {1, 3}, {2, 3},
                                                          {3, 0}, {3, 1}, {3, 2}, {3, 3}, {1, 4}, {2, 4}, {3, 4}};
};

struct PointSet2
{
    static constexpr std::initializer_list<Vec2> value = {{1, 1},   {0, 3},  {2, 0},   {-5, -1}, {-1, -5}, {4, 5},
                                                          {-6, -4}, {2, -2}, {-3, -1}, {2, -5},  {-5, 1},  {-1, 1},
                                                          {-2, -2}, {3, -2}, {-2, 2},  {-3, -3}, {1, -2},  {5, -2}};
};

using AbslC = MeshTriangulationAbslTraits;
using StdC = MeshTriangulationDefaultTraits;
using TbbC = MeshTriangulationTbbTraits;


template<class Traits, class PointSet>
static void BM_TriangulationFlipGraph(benchmark::State& state)
{
    size_t const num_points = state.range(0);
    auto const   points_cref =
        std::make_shared<std::vector<Vec2>>(PointSet::value.begin(), PointSet::value.begin() + num_points);

    TriangulationFlipGraph<Traits> gr(points_cref);
    gr.generate_graph();
    gr.generate_graph();

    for(auto _: state)
    {
        gr.generate_graph();
    }

    state.counters["nodes"] = benchmark::Counter(gr.nodes().size());
    state.counters["edges"] = benchmark::Counter(gr.edges().size());
    state.counters["node_rate"] = benchmark::Counter(gr.nodes().size(), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["edge_rate"] = benchmark::Counter(gr.edges().size(), benchmark::Counter::kIsIterationInvariantRate);
}


template<class Traits, class PointSet>
static void BM_ConcurrentTriangulationFlipGraph(benchmark::State& state)
{
    size_t const num_threads = 8;
    size_t const num_points  = state.range(0);
    auto const   points_cref =
        std::make_shared<std::vector<Vec2>>(PointSet::value.begin(), PointSet::value.begin() + num_points);

    ConcurrentTriangulationFlipGraph<Traits> gr(points_cref);
    gr.generate_graph(num_threads);
    gr.generate_graph(num_threads);

    for(auto _: state)
    {
        gr.generate_graph(num_threads);
    }

    state.counters["nodes"] = benchmark::Counter(gr.nodes().size());
    state.counters["edges"] = benchmark::Counter(gr.edges().size());
    state.counters["node_rate"] = benchmark::Counter(gr.nodes().size(), benchmark::Counter::kIsIterationInvariantRate);
    state.counters["edge_rate"] = benchmark::Counter(gr.edges().size(), benchmark::Counter::kIsIterationInvariantRate);
}

BENCHMARK_TEMPLATE(BM_TriangulationFlipGraph, StdC, PointSet1)
    ->Unit(benchmark::kMicrosecond)
    ->DenseRange(4, 18);

BENCHMARK_TEMPLATE(BM_TriangulationFlipGraph, AbslC, PointSet1)
    ->Unit(benchmark::kMicrosecond)
    ->DenseRange(4, 18);

BENCHMARK_TEMPLATE(BM_ConcurrentTriangulationFlipGraph, StdC, PointSet1)
    ->Unit(benchmark::kMicrosecond)
    ->DenseRange(4, 18);

BENCHMARK_TEMPLATE(BM_ConcurrentTriangulationFlipGraph, AbslC, PointSet1)
    ->Unit(benchmark::kMicrosecond)
    ->DenseRange(4, 18);


BENCHMARK_MAIN();

// int main(int argc, char** argv)
// {
//     // Initialize libcds
//     cds::Initialize();
//     {
//         // Initialize Hazard Pointer singleton
//         cds::gc::HP hpGC;
//         // If main thread uses lock-free containers
//         // the main thread should be attached to libcds infrastructure
//         cds::threading::Manager::attachThread();
//         // Now you can use HP-based containers in the main thread
//         char arg0_default[] = "benchmark";
//         char *args_default = arg0_default;
//         if (!argv) {
//           argc = 1;
//           argv = &args_default;
//         }
//         ::benchmark::Initialize(&argc, argv);
//         if (::benchmark::ReportUnrecognizedArguments(argc, argv))
//           return 1;
//         ::benchmark::RunSpecifiedBenchmarks();
//         ::benchmark::Shutdown();
//     }
//     // Terminate libcds
//     cds::Terminate();
// }
