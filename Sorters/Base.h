#ifndef BASE_H
#define BASE_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*cmp_func)(const void*, const void*);
typedef void (*sort_func_t)(void* arr,
                            size_t len,
                            size_t size,
                            cmp_func compar);

void bubble_sort(void* arr, size_t len, size_t size, cmp_func compar);
void quick_sort(void* arr, size_t len, size_t size, cmp_func compar);
void merge_sort(void* arr, size_t len, size_t size, cmp_func compar);
void radix_sort(void* arr, size_t len, size_t size, cmp_func compar);

/**
 * Sorts an array using the Bitonic sort algorithm.
 *
 * This function modifies the array in place. It works with generic data types
 * by using a void pointer for the array and a user-provided comparison
 * function.
 *
 * @param arr    Pointer to the first element of the array to be sorted.
 * @param len    Number of elements in the array.
 * @param size   Size (in bytes) of each element in the array.
 * @param compar Pointer to a comparison function.
 *               The function must return:
 *               < 0  if first element is less than second
 *                 0  if elements are equal
 *               > 0  if first element is greater than second
 *
 *               Example signature:
 *               int compar(const void* a, const void* b);
 */
void bitonic_sort(void* arr, size_t len, size_t size, cmp_func compar);

#ifdef __cplusplus
}
#endif

#endif