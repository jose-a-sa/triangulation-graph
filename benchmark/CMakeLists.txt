set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(benchmark REQUIRED)

add_executable(benchmark_TriangulationFlipGraph ${MAIN_SRC} benchmark_TriangulationFlipGraph.cpp)
set_property(TARGET benchmark_TriangulationFlipGraph PROPERTY CXX_STANDARD 17)
target_link_libraries(benchmark_TriangulationFlipGraph benchmark::benchmark)

add_executable(timer_TriangulationFlipGraph ${MAIN_SRC} timer_TriangulationFlipGraph.cpp)
set_property(TARGET timer_TriangulationFlipGraph PROPERTY CXX_STANDARD 17)
