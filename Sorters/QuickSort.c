#include "Base.h"
#include "../MemoryTracker/memory_tracker.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <omp.h>

#define INSERTION_SORT_CUTOFF 8
#define TASK_CUTOFF           64
#define MAX_TASK_DEPTH        10
#define SCRATCH_SIZE          256

static inline void swap_bytes(void* a, void* b, size_t size, unsigned char* scratch) {
    if (a == b) return;
    if (size == 4) {
        uint32_t t = *(uint32_t*)a;
        *(uint32_t*)a = *(uint32_t*)b;
        *(uint32_t*)b = t;
        return;
    }
    memcpy(scratch, a, size);
    memcpy(a, b, size);
    memcpy(b, scratch, size);
}

static size_t partition(char* base, size_t len, size_t size, cmp_func compar) {
    size_t mid = len / 2;
    // Median of three
    char* A = base;
    char* B = base + mid * size;
    char* C = base + (len - 1) * size;
    char* p_ptr = (compar(A, B) < 0) ? ((compar(B, C) < 0) ? B : (compar(A, C) < 0 ? C : A))
                                    : ((compar(B, C) > 0) ? B : (compar(A, C) > 0 ? C : A));

    // memory allocation for the pivot
    unsigned char* pivot = (unsigned char*)tracked_malloc(size);
    memcpy(pivot, p_ptr, size);

    // memory allocated for the swap buffer
    unsigned char* swap_buf = (unsigned char*)tracked_malloc(size);

    size_t i = 0;
    size_t j = len - 1;
    while (1) {
        while (compar(base + i * size, pivot) < 0) i++;
        while (compar(base + j * size, pivot) > 0) j--;
        if (i >= j) break;

        swap_bytes(base + i * size, base + j * size, size, swap_buf);
        i++; if (j > 0) j--;
    }

    tracked_free(pivot);
    tracked_free(swap_buf);
    return j + 1;
}

static void insertion_sort(char* base, size_t len, size_t size, cmp_func compar) {
    // memory allocation for the key
    unsigned char* key = (unsigned char*)tracked_malloc(size);
    for (size_t i = 1; i < len; i++) {
        memcpy(key, base + i * size, size);
        size_t j = i;
        while (j > 0 && compar(base + (j - 1) * size, key) > 0) {
            memcpy(base + j * size, base + (j - 1) * size, size);
            j--;
        }
        memcpy(base + j * size, key, size);
    }
    tracked_free(key);
}

static void quick_sort_seq(char* base, size_t len, size_t size, cmp_func compar) {
    if (len <= 1) return;
    if (len <= INSERTION_SORT_CUTOFF) {
        insertion_sort(base, len, size, compar);
        return;
    }

    size_t split = partition(base, len, size, compar);
    quick_sort_seq(base, split, size, compar);
    quick_sort_seq(base + split * size, len - split, size, compar);
}

static void quick_sort_par(char* base, size_t len, size_t size, cmp_func compar, int depth) {
    if (len <= 1) return;

    if (len < TASK_CUTOFF || depth >= MAX_TASK_DEPTH) {
        quick_sort_seq(base, len, size, compar);
        return;
    }

    size_t split = partition(base, len, size, compar);

    #pragma omp task default(none) firstprivate(base, split, size, compar, depth)
    quick_sort_par(base, split, size, compar, depth + 1);

    #pragma omp task default(none) firstprivate(base, split, len, size, compar, depth)
    quick_sort_par(base + split * size, len - split, size, compar, depth + 1);

    #pragma omp taskwait
}

void quick_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (!arr || len < 2 || size == 0 || !compar) return;

    MemTracker* t = mem_get_tracker();

    #pragma omp parallel
    {
        // Crucial: Every thread in the pool must know about the tracker
        if (t) mem_set_tracker(t);

        #pragma omp single nowait
        {
            quick_sort_par((char*)arr, len, size, compar, 0);
        }
    }
}
