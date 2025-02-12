# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.24) # or higher if applicable
project(triangulation-graph)

# Declare additional CMake options
option(BUILD_TESTING "" OFF)
option(BUILD_BENCHMARK "" ON)

# Main options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Dependencies
include(cmake/FetchFmtLib.cmake)
include(cmake/FetchSpdLog.cmake)
include(cmake/FetchCLI11.cmake)
include(cmake/FetchAbseil.cmake)
include(cmake/FetchTBB.cmake)


# Add compiler options (more pedantic in errors)
if(MSVC)
    add_compile_options(/W3 /WX)
else()
    add_compile_options(
        -Wall # Enable all warnings
        -Wextra # Enable extra warnings
        -Wpedantic # Enforce standard-compliant code
        -Werror # Treat warnings as errors
        -Wno-gnu-statement-expression
    )
endif()

# Add main folder as include directory
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add source library (this can be updated to add to use add_subdirectory too)
add_executable(triangulation-graph src/main.cpp)
target_link_libraries(triangulation-graph PRIVATE fmt::fmt spdlog::spdlog CLI11::CLI11 tbb absl::flat_hash_map absl::flat_hash_set)

# Build testing binaries
if(BUILD_TESTING)
    # enable_testing() should be called in the top-level CMakeLists.txt
    enable_testing()
    add_subdirectory(tests)
endif()

# Build benchmark binaries
if(BUILD_BENCHMARK)
    add_subdirectory(benchmark)
endif()
