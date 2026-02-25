#ifndef BENCH_UTILITIES_H
#define BENCH_UTILITIES_H

#include <benchmark/benchmark.h>

void BM_Sort_Advanced(benchmark::State& state,
                      void (*sort_func)(void*, size_t, size_t, int (*)(const void*, const void*)),
                      void (*gen_func)(void*, size_t),
                      size_t type_size,
                      int (*cmp)(const void*, const void*),
                      const char* type_name);

#endif
