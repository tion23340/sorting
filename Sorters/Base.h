#ifndef BASE_H
#define BASE_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif
    typedef int  (*cmp_func)(const void *, const void *);
    typedef void (*sort_func_t)(void *arr, size_t len, size_t size, cmp_func compar);

    void bubble_sort       (void *arr, size_t len, size_t size, cmp_func compar);
    void quick_sort        (void *arr, size_t len, size_t size, cmp_func compar);
    void merge_sort        (void *arr, size_t len, size_t size, cmp_func compar);
    void radix_sort        (void *arr, size_t len, size_t size, cmp_func compar);
    void bitonic_sort      (void *arr, size_t len, size_t size, cmp_func compar);

#ifdef __cplusplus
}
#endif

#endif