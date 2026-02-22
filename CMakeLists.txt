cmake_minimum_required(VERSION 3.27)
project(CPP LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.8.3
)

FetchContent_MakeAvailable(benchmark)

add_library(sorters
        Sorters/BubbleSort.c
        Sorters/QuickSort.c
        Sorters/MergeSort.c
        Sorters/RadixSort.c
        Sorters/BitonicSort.c
)

target_include_directories(sorters PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Sorters
)

add_executable(CPP
        main.cpp
        InputGenerators/BaseGenerator.h
        InputGenerators/Generators.cpp
        InputGenerators/BaseComparators.h
        InputGenerators/Comparators.cpp
)

target_link_libraries(CPP
        PRIVATE
        sorters
        benchmark::benchmark
        Threads::Threads
)

find_package(Threads REQUIRED)

target_compile_options(CPP PRIVATE -O3 -march=native)
