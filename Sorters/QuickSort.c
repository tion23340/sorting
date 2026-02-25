#include "Base.h"
#include "../MemoryTracker/memory_tracker.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <omp.h>

#define INSERTION_SORT_CUTOFF 32
#define SWAP_STACK_BUF 256

// Spawn tasks only for large partitions to avoid overhead/task explosion.
// Tune: try 1<<16, 1<<17, 1<<18
#define TASK_CUTOFF (1u << 18)   // 262144 elements

// Limit recursion depth at which we spawn tasks (optional safety)
#define MAX_TASK_DEPTH 6

static inline void swap_bytes(void* a, void* b, size_t size) {
    if (a == b) return;

    if (size == 4) {
        uint32_t t = *(uint32_t*)a;
        *(uint32_t*)a = *(uint32_t*)b;
        *(uint32_t*)b = t;
        return;
    }
    if (size == 8) {
        uint64_t t = *(uint64_t*)a;
        *(uint64_t*)a = *(uint64_t*)b;
        *(uint64_t*)b = t;
        return;
    }

    unsigned char tmp[SWAP_STACK_BUF];
    if (size <= SWAP_STACK_BUF) {
        memcpy(tmp, a, size);
        memcpy(a, b, size);
        memcpy(b, tmp, size);
    } else {
        unsigned char* pa = (unsigned char*)a;
        unsigned char* pb = (unsigned char*)b;
        for (size_t i = 0; i < size; i++) {
            unsigned char t = pa[i];
            pa[i] = pb[i];
            pb[i] = t;
        }
    }
}

static void insertion_sort(char* base, size_t len, size_t size, cmp_func compar) {
    for (size_t i = 1; i < len; i++) {
        if (size > SWAP_STACK_BUF) {
            // fallback: swap-based insertion (rare in your bench)
            size_t j = i;
            while (j > 0 && compar(base + (j - 1) * size, base + j * size) > 0) {
                swap_bytes(base + (j - 1) * size, base + j * size, size);
                j--;
            }
            continue;
        }

        unsigned char key[SWAP_STACK_BUF];
        memcpy(key, base + i * size, size);

        size_t j = i;
        while (j > 0 && compar(base + (j - 1) * size, key) > 0) {
            memcpy(base + j * size, base + (j - 1) * size, size);
            j--;
        }
        memcpy(base + j * size, key, size);
    }
}

static inline size_t median3(char* base, size_t a, size_t b, size_t c, size_t size, cmp_func compar) {
    char* A = base + a * size;
    char* B = base + b * size;
    char* C = base + c * size;

    int ab = compar(A, B);
    int ac = compar(A, C);
    int bc = compar(B, C);

    if (ab < 0) { // A < B
        if (bc < 0) return b;      // A < B < C
        return (ac < 0) ? c : a;   // A < C <= B ? C : A
    } else { // A >= B
        if (bc > 0) return b;      // C < B <= A
        return (ac > 0) ? c : a;   // B <= C < A ? C : A
    }
}

// Partition using a copied pivot value (so pivot doesn't move)
static inline size_t partition(char* base, size_t len, size_t size, cmp_func compar) {
    size_t mid = len / 2;
    size_t pidx = median3(base, 0, mid, len - 1, size, compar);

    unsigned char pivotbuf[SWAP_STACK_BUF];
    unsigned char* pivot = pivotbuf;

    if (size <= SWAP_STACK_BUF) {
        memcpy(pivot, base + pidx * size, size);
    } else {
        // Rare path; pivot points into array (may move); still works but not ideal.
        pivot = (unsigned char*)(base + pidx * size);
    }

    size_t i = 0;
    size_t j = len - 1;

    while (1) {
        while (compar(base + i * size, pivot) < 0) i++;
        while (compar(base + j * size, pivot) > 0) j--;

        if (i >= j) break;

        swap_bytes(base + i * size, base + j * size, size);
        i++;
        if (j > 0) j--;
    }

    return j + 1; // split point: left [0..split-1], right [split..len-1]
}

static void quick_sort_seq(char* base, size_t len, size_t size, cmp_func compar) {
    while (len > 1) {
        if (len <= INSERTION_SORT_CUTOFF) {
            insertion_sort(base, len, size, compar);
            return;
        }

        size_t split = partition(base, len, size, compar);
        size_t left_len = split;
        size_t right_len = len - split;

        // recurse smaller side first, loop on larger
        if (left_len < right_len) {
            if (left_len > 1) quick_sort_seq(base, left_len, size, compar);
            base += left_len * size;
            len = right_len;
        } else {
            if (right_len > 1) quick_sort_seq(base + left_len * size, right_len, size, compar);
            len = left_len;
        }
    }
}

static void quick_sort_par(char* base, size_t len, size_t size, cmp_func compar, int depth) {
    if (len <= 1) return;

    if (len <= INSERTION_SORT_CUTOFF) {
        insertion_sort(base, len, size, compar);
        return;
    }

    // If partition is small enough or we've spawned enough depth, do sequential
    if (len < TASK_CUTOFF || depth >= MAX_TASK_DEPTH) {
        quick_sort_seq(base, len, size, compar);
        return;
    }

    size_t split = partition(base, len, size, compar);
    size_t left_len = split;
    size_t right_len = len - split;

    char* right_base = base + left_len * size;

#pragma omp task default(none) firstprivate(base, left_len, size, compar, depth)
    {
        quick_sort_par(base, left_len, size, compar, depth + 1);
    }

#pragma omp task default(none) firstprivate(right_base, right_len, size, compar, depth)
    {
        quick_sort_par(right_base, right_len, size, compar, depth + 1);
    }

#pragma omp taskwait
}

void quick_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (!arr || len < 2 || size == 0 || !compar) return;

#pragma omp parallel
    {
#pragma omp single nowait
        {
            quick_sort_par((char*)arr, len, size, compar, 0);
        }
    }
}
