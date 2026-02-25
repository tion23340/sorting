cmake_minimum_required(VERSION 3.27)
project(SortingAlgorithms LANGUAGES C CXX)

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

find_package(Threads REQUIRED)
find_package(OpenMP REQUIRED)

add_library(sorters
  Sorters/BubbleSort.c
  Sorters/QuickSort.c
  Sorters/HeapSort.c
  Sorters/MergeSort.c
  Sorters/RadixSort.c
  Sorters/BitonicSort.c
)

target_include_directories(sorters PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/Sorters
  ${CMAKE_CURRENT_SOURCE_DIR}/InputGenerators
  ${CMAKE_CURRENT_SOURCE_DIR}/MemoryTracker
)

target_link_libraries(sorters PUBLIC OpenMP::OpenMP_C memory_tracker)

target_compile_options(sorters PRIVATE -O3 -march=native)

add_library(generators
  InputGenerators/Generators.cpp
  InputGenerators/Comparators.c
)

target_include_directories(generators PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/InputGenerators
)

add_library(memory_tracker
  MemoryTracker/memory_tracker.c
)

target_include_directories(memory_tracker PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/MemoryTracker
)

add_executable(benchmark_app
  main.cpp
  Benchmarks/bench_utilities.cpp
)

target_link_libraries(benchmark_app PRIVATE
  sorters
  generators
  memory_tracker
  benchmark::benchmark
  Threads::Threads
  OpenMP::OpenMP_CXX
)

target_include_directories(benchmark_app PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/Sorters
  ${CMAKE_CURRENT_SOURCE_DIR}/InputGenerators
  ${CMAKE_CURRENT_SOURCE_DIR}/Benchmarks
  ${CMAKE_CURRENT_SOURCE_DIR}/MemoryTracker
)

target_compile_options(benchmark_app PRIVATE -O3 -march=native)
