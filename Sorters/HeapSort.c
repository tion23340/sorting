#include "Base.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "memory_tracker.h"

void heapify(void* arr, size_t n, size_t i, cmp_func comp, size_t el_size, void* temp) {
    char* base = (char*)arr;
    while (1) {
        size_t best = i;
        size_t l = 2 * i + 1;
        size_t r = 2 * i + 2;

        if (l < n && comp(base + l * el_size, base + best * el_size) > 0)
            best = l;

        if (r < n && comp(base + r * el_size, base + best * el_size) > 0)
            best = r;

        if (best == i) break;

        // In-place swap using the provided temp buffer
        memcpy(temp, base + i * el_size, el_size);
        memcpy(base + i * el_size, base + best * el_size, el_size);
        memcpy(base + best * el_size, temp, el_size);

        i = best;
    }
}

void heap_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (!arr || len < 2 || size == 0) return;

    // 1. Allocate a single temp buffer for swaps (O(1) auxiliary space)
    void* temp = tracked_malloc(size);
    if (!temp) return;

    /* NOTE: If your benchmark requires the sort to own its memory 
       (like Radix or Merge), you'd malloc a full copy here. 
       But Heap Sort's "spirit" is in-place. 
    */

    // 2. Build Max Heap
    for (size_t i = len / 2; i > 0; i--) {
        heapify(arr, len, i - 1, compar, size, temp);
    }

    // 3. Extract elements from heap one by one
    char* base = (char*)arr;
    for (size_t i = len; i > 1; i--) {
        // Swap root (max) with the last element
        memcpy(temp, base, size);
        memcpy(base, base + (i - 1) * size, size);
        memcpy(base + (i - 1) * size, temp, size);

        // Heapify the reduced heap
        heapify(arr, i - 1, 0, compar, size, temp);
    }

    // 4. Clean up the swap buffer
    tracked_free(temp);
}