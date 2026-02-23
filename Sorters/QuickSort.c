#include "Base.h"
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

static void swap(void* a, void* b, size_t size)
{
    if (a == b) return;
   
    char* tmp = (char*)malloc(size);
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    free(tmp);
}

// Divide and Conquer implementation

static void quick_sort_recursive(char* base, size_t len, size_t size, cmp_func compar)
{
    if (len < 2)
        return;

    size_t pivot_index = len / 2;
    char* pivot = base + pivot_index * size;

    size_t i = 0;
    size_t j = len - 1;

    while (i <= j)
    {
        while (compar(base + i * size, pivot) < 0)
            i++;

        while (compar(base + j * size, pivot) > 0)
            j--;

        if (i <= j)
        {
            swap(base + i * size, base + j * size, size);
            i++;
            if (j > 0) j--;
        }
    }

    if (j + 1 > 0)
        quick_sort_recursive(base, j + 1, size, compar);

    if (i < len)
        quick_sort_recursive(base + i * size, len - i, size, compar);
}

void quick_sort(void* arr, size_t len, size_t size, cmp_func compar) 
{
    if (!arr || len < 2 || size == 0 || !compar)
        return;

    quick_sort_recursive((char*)arr, len, size, compar);
}
