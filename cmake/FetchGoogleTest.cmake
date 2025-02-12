if(GTest_FOUND)
    message(STATUS "GoogleTest ${GTest_VERSION} already found in ${GTest_DIR}")
    return()
endif()

if(NOT DEFINED googletest_FETCH_VERSION)
    set(googletest_FETCH_VERSION "v1.15.2")
endif()


include(FetchContent)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest
    GIT_TAG ${googletest_FETCH_VERSION}
)

FetchContent_MakeAvailable(googletest)

message(STATUS "Built GoogleTest ${googletest_FETCH_VERSION} in ${googletest_BINARY_DIR}")
