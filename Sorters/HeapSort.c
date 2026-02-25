#include "Base.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/*
 * @param arr
 *      Pointer to the first element of the array to be sorted.
 *      The array is modified in place.
 *
 * @param len
 *      Number of elements in the array.
 *
 * @param size
 *      Size (in bytes) of each element in the array.
 *
 * @param compar
 *      Pointer to a comparison function.
 *      The function must return:
 *          < 0  if first element is less than second
 *            0  if elements are equal
 *          > 0  if first element is greater than second
 *
 *      Example signature:
 *          int compar(const void* a, const void* b);
*/

void heapify(void* arr, size_t n, size_t i, cmp_func comp, size_t el_size, void* temp) {
    while (1) {
        size_t best = i;
        size_t l = 2 * i + 1;
        size_t r = 2 * i + 2;

        if (l < n && comp((char*)arr + l * el_size, (char*)arr + best * el_size) > 0)
            best = l;

        if (r < n && comp((char*)arr + r * el_size, (char*)arr + best * el_size) > 0)
            best = r;

        if (best == i) break;

        memcpy(temp, (char*)arr + i * el_size, el_size);
        memcpy((char*)arr + i * el_size, (char*)arr + best * el_size, el_size);
        memcpy((char*)arr + best * el_size, temp, el_size);

        i = best;
    }
}

void heap_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (len < 2) return;

    void* temp = malloc(size);
    if (!temp) return;

    for (size_t i = len / 2; i > 0; i--) {
        heapify(arr, len, i - 1, compar, size, temp);
    }

    for (size_t i = len; i > 1; i--) {
        memcpy(temp, arr, size);
        memcpy(arr, (char*)arr + (i - 1) * size, size);
        memcpy((char*)arr + (i - 1) * size, temp, size);

        heapify(arr, i - 1, 0, compar, size, temp);
    }

    free(temp);
}
