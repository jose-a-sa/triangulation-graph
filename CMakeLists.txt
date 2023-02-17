# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.5.0)
project(convex-triangulations)

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../bin")

include_directories("/opt/homebrew/include")
link_directories("${CMAKE_BINARY_DIR}/../lib")

find_package(Boost)

# Source related variables
set(SRC_FILES
        src/MeshCells.cpp
        src/MeshTriangulation.cpp
        src/TriangulationFlipGraph.cpp
        src/main.cpp)

add_executable(triangulation-graph ${SRC_FILES})
set_property(TARGET triangulation-graph PROPERTY CXX_STANDARD 17)