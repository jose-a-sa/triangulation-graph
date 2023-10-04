set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

include_directories("../src/")

find_package(benchmark REQUIRED)

set(BENCH_MESHTRIG_SRC
        benchmark_main.cpp)

add_executable(benchmark_MeshTriangulation ${MAIN_SRC} ${BENCH_MESHTRIG_SRC})
set_property(TARGET benchmark_MeshTriangulation PROPERTY CXX_STANDARD 17)
target_link_libraries(benchmark_MeshTriangulation benchmark::benchmark)
