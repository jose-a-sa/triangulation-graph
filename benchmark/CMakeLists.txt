# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.24) # or higher if applicable
project(triangulation-graph-tests)

include(../cmake/FetchBenchmark.cmake)

add_executable(bm_TriangulationFlipGraph bm_triangulation_flip_graph.cpp)
target_link_libraries(bm_TriangulationFlipGraph fmt::fmt benchmark::benchmark tbb absl::flat_hash_map absl::flat_hash_set)
