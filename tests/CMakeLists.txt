set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(GTest)

enable_testing()

set(TEST_SRC
        unittest_Point.cpp
        unittest_MeshTriangulation.cpp)

add_executable(test_triangulation-graph ${MAIN_SRC} ${TEST_SRC})
set_property(TARGET test_triangulation-graph PROPERTY CXX_STANDARD 17)
target_link_libraries(test_triangulation-graph GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(test_triangulation-graph)
