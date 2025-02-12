#define MESH_TRIANGULATION_HAS_STD 1
#define MESH_TRIANGULATION_HAS_TBB 1
#define MESH_TRIANGULATION_HAS_ABSL 1

#include <CLI/CLI.hpp>

#include <fmt/base.h>
#include <fmt/ranges.h>

#include <vector>

#include "mesh_triangulation_2d.h"
#include "triangulation_flip_graph.h"
#include "utils/timer.h"
#include "vec2.h"

template<class Traits>
void run_measure(std::shared_ptr<std::vector<Vec2>> const& vec_ptr, std::string_view name)
{
    // TriangulationFlipGraph<Traits> gr(vec_ptr);
    // gr.generate_graph();
    // gr.generate_graph();
    //
    // Timer<std::chrono::milliseconds>::MeasureRepeated<10>(fmt::format("TriangulationFlipGraph<{}>", name),
    //                                                       [&gr]() { gr.generate_graph(); });

    ConcurrentTriangulationFlipGraph<Traits> gr2(vec_ptr);
    gr2.generate_graph();
    gr2.generate_graph();

    for(size_t n_threads = 1; n_threads <= 2 * std::thread::hardware_concurrency(); ++n_threads)
    {
        Timer<std::chrono::milliseconds>::MeasureRepeated<10>(
            fmt::format("ConcurrentTriangulationFlipGraph<{}>, n_threads={}", name, n_threads),
            [&gr2, n_threads]() { gr2.generate_graph(n_threads); });
    }
}


int main(int argc, char** argv)
{
    CLI::App app{"App description"};
    app.failure_message(CLI::FailureMessage::help);

    std::vector<std::pair<Vec2::float_type, Vec2::float_type>> vec;
    auto* points_opt = app.add_option("point_set", vec, "List of 2d points.")
                           ->required()
                           ->delimiter(',')
                           ->type_name("FLOAT FLOAT, FLOAT FLOAT,");

    argv = app.ensure_utf8(argv);
    try
    {
        app.parse(argc, argv);
        if(points_opt->count() % 2 == 1)
            throw CLI::ParseError("Number of arguments is not even.", 1);
    }
    catch(CLI::ParseError const& e)
    {
        return app.exit(e);
    };

    // vec.resize(18);
    fmt::println("Vec: {}", vec);

    using iter_t         = typename std::vector<std::pair<double, double>>::iterator;
    auto const coord_ptr = std::make_shared<std::vector<Vec2>>(std::move_iterator<iter_t>(vec.begin()),
                                                               std::move_iterator<iter_t>(vec.end()));

    // run_measure<MeshTriangulationDefaultTraits>(coord_ptr, "Std");
    // run_measure<MeshTriangulationAbslTraits>(coord_ptr, "Absl");
    // run_measure<MeshTriangulationTbbTraits>(coord_ptr, "Tbb");

    ConcurrentTriangulationFlipGraph<MeshTriangulationAbslTraits> gr2(coord_ptr);
    gr2.generate_graph();

    return gr2.nodes().size() > 0 ? 0 : 1;
}
