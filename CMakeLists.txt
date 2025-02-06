# Specify the minimum CMAKE version required
cmake_minimum_required(VERSION 3.24) # or higher if applicable
project(triangulation-graph)

# Declare additional CMake options
option(BUILD_TESTING "" OFF)
option(BUILD_BENCHMARK "" OFF)

# Main options
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# If fmtlib installed in the system use find_package
# find_package(fmt REQUIRED)
# Otherwise use FetchContent:
set(FMTLIB_VERSION 11.0.2)
include(FetchContent)
FetchContent_Declare(fmtlib
    GIT_REPOSITORY https://github.com/fmtlib/fmt
    GIT_TAG ${FMTLIB_VERSION}
)
FetchContent_MakeAvailable(fmtlib)

# If spdlog installed in the system use find_package
# find_package(spdlog REQUIRED)
# Otherwise use FetchContent:
set(SPDLOG_VERSION v1.15.1)
include(FetchContent)
set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "Use external fmt library" FORCE)
FetchContent_Declare(spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog
    GIT_TAG ${SPDLOG_VERSION}
)
FetchContent_MakeAvailable(spdlog)

# If abseil installed in the system use find_package
# find_package(abseil REQUIRED)
# Otherwise use FetchContent:
set(ABSEIL_VERSION 20240722.1)
include(FetchContent)
set(ABSL_PROPAGATE_CXX_STD ON)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "") # up to you
set(BUILD_TESTING OFF CACHE BOOL "") # to disable abseil test
set(ABSL_ENABLE_INSTALL ON CACHE BOOL "") # now you can enable install rules even in subproject...
add_compile_options(-Wno-error=gcc-compat)
FetchContent_Declare(abseil-cpp
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp/
    GIT_TAG ${ABSEIL_VERSION}
)
FetchContent_MakeAvailable(abseil-cpp)



# If CLI11 installed in the system use find_package
# find_package(cli11 REQUIRED)
# Otherwise use FetchContent:
set(CLI11_VERSION v2.4.2)
include(FetchContent)
FetchContent_Declare(cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
    GIT_TAG ${CLI11_VERSION}
)
FetchContent_MakeAvailable(cli11)

# If oneTBB installed in the system use find_package
# find_package(cli11 REQUIRED)
# Otherwise use FetchContent:
set(TBB_VERSION v2022.0.0)
include(FetchContent)
set(BUILD_SHARED_LIBS ON)
FetchContent_Declare(tbb
    GIT_REPOSITORY https://github.com/uxlfoundation/oneTBB
    GIT_TAG ${TBB_VERSION}
)
FetchContent_MakeAvailable(tbb)

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
file(GLOB_RECURSE SRC_FILES "src/*.cpp" "src/*.h" "src/**/*.cpp" "src/**/*.h" "src/**/*.inl")
add_executable(triangulation-graph ${SRC_FILES})
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
