#include <iostream>
#include <benchmark/benchmark.h>
#include <vector>
#include <algorithm>
#include <random>
#include <dlfcn.h>
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <cstdint>
#include <new>

#include "Sorters/Base.h"
#include "InputGenerators/BaseComparators.h"
#include "InputGenerators/BaseGenerator.h"
#include "Benchmarks/bench_utilities.h"

// -------------------------------
// Benchmark wrapper
// -------------------------------

// -------------------------------------------------------
// QUICK SORT
// -------------------------------------------------------
BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_IntSigned,
                  quick_sort, gen_int_signed, sizeof(int), compare_int_asc,
                  "signed int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_IntUnsigned,
                  quick_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_FloatSigned,
                  quick_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_FloatUnsigned,
                  quick_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_DoubleSigned,
                  quick_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, QuickSort_DoubleUnsigned,
                  quick_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
                  "unsigned double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

// -------------------------------------------------------
// BUBBLE SORT
// -------------------------------------------------------
BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_IntSigned,
                  bubble_sort, gen_int_signed, sizeof(int), compare_int_asc,
                  "signed int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_IntUnsigned,
                  bubble_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_FloatSigned,
                  bubble_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_FloatUnsigned,
                  bubble_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_DoubleSigned,
                  bubble_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BubbleSort_DoubleUnsigned,
                  bubble_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
                  "unsigned double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

// -------------------------------------------------------
// MERGE SORT
// -------------------------------------------------------
BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_IntSigned,
                  merge_sort, gen_int_signed, sizeof(int), compare_int_asc,
                  "signed int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_IntUnsigned,
                  merge_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_FloatSigned,
                  merge_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_FloatUnsigned,
                  merge_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_DoubleSigned,
                  merge_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, MergeSort_DoubleUnsigned,
                  merge_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
                  "unsigned double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

// -------------------------------------------------------
// RADIX SORT
// -------------------------------------------------------

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_IntUnsigned,
                  radix_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_IntUnsigned,
            radix_sort, gen_int_signed, sizeof(int), compare_int_asc,
            "signed int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});


    BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_FloatUnsigned,
                  radix_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_IntUnsigned,
            radix_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});


// -------------------------------------------------------
// HeapSort
// -------------------------------------------------------

BENCHMARK_CAPTURE(BM_Sort_Advanced, HeapSort_IntUnsigned,
                  heap_sort, gen_int_unsigned, sizeof(unsigned int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, HeapSort_FloatSigned,
                  heap_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, HeapSort_FloatUnsigned,
                  heap_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, HeapSort_DoubleSigned,
                  heap_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, HeapSort_DoubleUnsigned,
                  heap_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
                  "unsigned double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

// -------------------------------------------------------
// BITONIC SORT
// -------------------------------------------------------
BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_IntSigned,
                  bitonic_sort, gen_int_signed, sizeof(int), compare_int_asc,
                  "signed int")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_IntUnsigned,
                  bitonic_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_FloatSigned,
                  bitonic_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_FloatUnsigned,
                  bitonic_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_DoubleSigned,
                  bitonic_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_CAPTURE(BM_Sort_Advanced, BitonicSort_DoubleUnsigned,
                  bitonic_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
                  "unsigned double")
    ->Args({128})->Args({1024})->Args({16384})->Args({1048576})->Args({16777216});

BENCHMARK_MAIN();
