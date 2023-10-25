cmake_minimum_required(VERSION 3.27)

project(
    benchmark
    LANGUAGES CXX
)

if(PROJECT_IS_TOP_LEVEL)
    find_package(para REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(
    nanobench
    GIT_REPOSITORY https://github.com/martinus/nanobench.git
    GIT_TAG        v4.3.11
    GIT_SHALLOW    TRUE
    SYSTEM
    FIND_PACKAGE_ARGS
)
FetchContent_MakeAvailable(nanobench)

add_executable(para_bench
    src/para_bench.cpp
)

target_link_libraries(para_bench PRIVATE para::para nanobench::nanobench)