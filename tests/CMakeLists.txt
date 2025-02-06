set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_executable(triangulation-graph-test ${MAIN_SRC} ../benchmark/timer_TriangulationFlipGraph.cpp)
set_property(TARGET triangulation-graph-test PROPERTY CXX_STANDARD 17)
#target_compile_options(triangulation-graph-test PRIVATE -O0)

find_package(GTest REQUIRED)

enable_testing()

# set(TEST_SRC
#         unit_Point.cpp
#         unit_MeshTriangulation.cpp)

# add_executable(unittest_triangulation-graph ${MAIN_SRC} ${TEST_SRC})
# set_property(TARGET unittest_triangulation-graph PROPERTY CXX_STANDARD 17)
# target_link_libraries(unittest_triangulation-graph GTest::gtest_main)

include(GoogleTest)
# gtest_discover_tests(unittest_triangulation-graph)
