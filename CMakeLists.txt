# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.5.0)
project(triangulation-graph)

## Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

include_directories("/opt/homebrew/include")
include_directories("${CMAKE_BINARY_DIR}/../include")
link_directories("${CMAKE_BINARY_DIR}/../lib")

find_package(Boost)

# Source related variables
set(MAIN_SRC
        src/Point.cpp
        src/MeshCell.cpp
        src/MeshTriangulation.cpp
        src/TriangulationFlipGraph.cpp)
list(TRANSFORM MAIN_SRC PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

add_executable(triangulation-graph ${MAIN_SRC} src/main.cpp)
set_property(TARGET triangulation-graph PROPERTY CXX_STANDARD 17)

add_executable(triangulation-graph-test ${MAIN_SRC} src/test.cpp)
set_property(TARGET triangulation-graph-test PROPERTY CXX_STANDARD 17)
target_compile_options(triangulation-graph-test PRIVATE -O0)


add_subdirectory(tests)

