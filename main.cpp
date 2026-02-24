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

// -------------------------------
// Memory tracking via malloc/free interposition (safe)
// -------------------------------
static size_t total_allocated   = 0;
static size_t current_allocated = 0;
static size_t peak_allocated    = 0;

static void* (*real_malloc)(size_t) = nullptr;
static void  (*real_free)(void*)    = nullptr;

// Large fallback for very-early init (dlsym recursion, OpenMP runtime init, etc.)
// This memory is a bump allocator; we do not reclaim it.
static char   fallback_buf[16 << 20]; // 16 MiB
static size_t fallback_used = 0;
static bool   initializing  = false;

static void init_real_allocs() {
    if (!real_malloc) real_malloc = (void*(*)(size_t))dlsym(RTLD_NEXT, "malloc");
    if (!real_free)   real_free   = (void(*)(void*))  dlsym(RTLD_NEXT, "free");
}

// Tag allocations we own so free() can safely distinguish "ours" vs "not ours".
static constexpr std::uint64_t MAGIC = 0xC0FFEEBADC0FFEEULL;

struct Header {
    std::uint64_t magic;
    std::size_t   size;
};

static_assert(alignof(Header) <= alignof(std::max_align_t),
              "Header alignment too strict for malloc() return alignment");

extern "C" {

void* malloc(size_t size) {
    if (!real_malloc) {
        if (initializing) {
            // Serve early allocations while resolving real malloc/free via dlsym.
            // Bounds-checked bump allocator with 16-byte alignment.
            size_t aligned = (size + 15) & ~((size_t)15);
            if (fallback_used + aligned > sizeof(fallback_buf)) return nullptr;
            void* ptr = fallback_buf + fallback_used;
            fallback_used += aligned;
            return ptr;
        }

        initializing = true;
        init_real_allocs();
        initializing = false;

        // If dlsym failed, best-effort fallback (avoid calling nullptr).
        if (!real_malloc || !real_free) {
            size_t aligned = (size + 15) & ~((size_t)15);
            if (fallback_used + aligned > sizeof(fallback_buf)) return nullptr;
            void* ptr = fallback_buf + fallback_used;
            fallback_used += aligned;
            return ptr;
        }
    }

    // Normal path: allocate extra bytes for our header.
    std::size_t total = size + sizeof(Header);
    void* raw = real_malloc(total);
    if (!raw) return nullptr;

    auto* h = reinterpret_cast<Header*>(raw);
    h->magic = MAGIC;
    h->size  = size;

    current_allocated += size;
    total_allocated   += size;
    if (current_allocated > peak_allocated)
        peak_allocated = current_allocated;

    return reinterpret_cast<char*>(raw) + sizeof(Header);
}

void free(void* ptr) {
    if (!ptr) return;

    // Ignore frees of fallback memory (bump allocator)
    if ((char*)ptr >= fallback_buf &&
        (char*)ptr <  fallback_buf + sizeof(fallback_buf)) {
        return;
    }

    init_real_allocs();
    if (!real_free) return;

    // Check whether this pointer came from OUR malloc().
    // If not, pass through unchanged (prevents "free(): invalid pointer").
    char* raw = reinterpret_cast<char*>(ptr) - (std::ptrdiff_t)sizeof(Header);
    auto* h   = reinterpret_cast<Header*>(raw);

    if (h->magic == MAGIC) {
        current_allocated -= h->size;
        h->magic = 0; // poison (helps catch double-free of our blocks)
        real_free(h);
    } else {
        // Not ours (e.g., allocated by aligned_alloc/posix_memalign/etc. or other runtime paths)
        real_free(ptr);
    }
}

} // extern "C"

// -------------------------------
// Benchmark wrapper
// -------------------------------
static void BM_Sort_Advanced(
    benchmark::State& state,
    sort_func_t sort_fn,
    input_fill_generator fill_generator,
    size_t element_size,
    comparator comp_fn,
    const char* name)
{
    const size_t N = state.range(0);

    std::vector<char> source(N * element_size);
    fill_generator(source.data(), N);

    std::vector<char> working_copy;
    working_copy.reserve(N * element_size);

    state.SetLabel(name);

    for (auto _ : state) {
        state.PauseTiming();
        current_allocated = total_allocated = peak_allocated = 0;

        working_copy.assign(source.begin(), source.end());

        state.ResumeTiming();

        sort_fn(working_copy.data(), N, element_size, comp_fn);

        benchmark::DoNotOptimize(working_copy.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * N);
    state.counters["peak_kb"]        = peak_allocated  / 1024.0;
    state.counters["total_alloc_kb"] = total_allocated / 1024.0;
}

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
BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_IntSigned,
                  radix_sort, gen_int_signed, sizeof(int), compare_int_asc,
                  "signed int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_IntUnsigned,
                  radix_sort, gen_int_unsigned, sizeof(int), compare_int_asc,
                  "unsigned int")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_FloatSigned,
                  radix_sort, gen_float_signed, sizeof(float), compare_float_asc,
                  "signed float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_FloatUnsigned,
                  radix_sort, gen_float_unsigned, sizeof(float), compare_float_asc,
                  "unsigned float")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_DoubleSigned,
                  radix_sort, gen_double_signed, sizeof(double), compare_double_asc,
                  "signed double")
    ->Args({100})->Args({1000})->Args({10000})->Args({1000000})->Args({10000000});

BENCHMARK_CAPTURE(BM_Sort_Advanced, RadixSort_DoubleUnsigned,
                  radix_sort, gen_double_unsigned, sizeof(double), compare_double_asc,
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
