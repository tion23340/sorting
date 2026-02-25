#include <iostream>
#include <vector>
#include <benchmark/benchmark.h>

#include "memory_tracker.h"
#include "Base.h"
#include "BaseComparators.h"
#include "BaseGenerator.h"

void BM_Sort_Advanced(
            benchmark::State& state,
            void (*sort_fn)(void*, size_t, size_t, cmp_func),
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
    MemTracker tracker;
    mem_reset(&tracker);
    
    for (auto _ : state) {
        state.PauseTiming();
        
        mem_reset(&tracker);
        mem_set_tracker(&tracker);

        working_copy.assign(source.begin(), source.end());

        state.ResumeTiming();

        sort_fn(working_copy.data(), N, element_size, comp_fn);

        mem_set_tracker(NULL);

        benchmark::DoNotOptimize(working_copy.data());
        benchmark::ClobberMemory();
    }

    state.SetItemsProcessed(state.iterations() * N);
    state.counters["Total Allocated "] =  benchmark::Counter(static_cast<double>(tracker.total_allocated ), benchmark::Counter::kAvgThreads);
    state.counters["Peak Allocated "] =  benchmark::Counter(static_cast<double>(tracker.peak_allocated ), benchmark::Counter::kAvgThreads);
    state.counters["Number of Allocations "] =  benchmark::Counter(tracker.allocation_count, benchmark::Counter::kAvgThreads);
}
