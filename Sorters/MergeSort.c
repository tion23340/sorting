#include "Base.h"
#include <stdlib.h>
#include <string.h>
#include "memory_tracker.h"

void merge(void* arr, size_t l, size_t r, cmp_func comp, size_t el_size) {
    size_t mid = l + (r - l) / 2;
    size_t i1 = l, i2 = mid, t = 0;
    char* base = (char*)arr;

    size_t total_bytes = (r - l) * el_size;
    void* sorted = tracked_malloc(total_bytes);
    if (!sorted) return; 

    char* sorted_ptr = (char*)sorted;

    while (i1 < mid && i2 < r) {
        if (comp(base + i1 * el_size, base + i2 * el_size) < 0) {
            memcpy(sorted_ptr + t * el_size, base + i1 * el_size, el_size);   
            i1++;
        } else {
            memcpy(sorted_ptr + t * el_size, base + i2 * el_size, el_size);
            i2++;
        }
        t++;
    }

    while (i1 < mid) {
        memcpy(sorted_ptr + t * el_size, base + i1 * el_size, el_size);
        i1++;
        t++;
    }
    while (i2 < r) {
        memcpy(sorted_ptr + t * el_size, base + i2 * el_size, el_size);
        i2++;
        t++;
    }

    memcpy(base + l * el_size, sorted_ptr, total_bytes);

    tracked_free(sorted);
}

void sort_recursive(void* arr, size_t l, size_t r, cmp_func comp, size_t size) {
    if (r - l <= 1) return;
    
    size_t mid = l + (r - l) / 2;

    sort_recursive(arr, l, mid, comp, size);
    sort_recursive(arr, mid, r, comp, size);
    merge(arr, l, r, comp, size);
}

void merge_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (!arr || len < 2 || size == 0) return;
    sort_recursive(arr, 0, len, compar, size);
}