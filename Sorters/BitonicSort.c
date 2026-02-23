#include <omp.h>
#include "Base.h"

#define PARALLEL_THRESHOLD 4096

/**
 * direction:
 * 1 -> ascending order
 * 0 -> desceding order
 */
void comp_swap(void* arr,
               size_t size,
               int i,
               int j,
               int direction,
               cmp_func comparator) {
  void* elem_i = (char*)arr + (i * size);
  void* elem_j = (char*)arr + (j * size);
  int comp_res = comparator(elem_i, elem_j);

  if ((direction == 1 && comp_res > 0) || (direction == 0 && comp_res < 0)) {
    char temp[size];
    memcpy(temp, elem_i, size);
    memcpy(elem_i, elem_j, size);
    memcpy(elem_j, temp, size);
  }
}

void bitonic_merge(void* arr,
                   size_t size,
                   int start,
                   int count,
                   int direction,
                   cmp_func comparator) {
  if (count > 1) {
    int k = count / 2;

#pragma omp parallel for if (k > PARALLEL_THRESHOLD)
    for (int i = start; i < start + k; i++) {
      comp_swap(arr, size, i, i + k, direction, comparator);
    }

    if (count > PARALLEL_THRESHOLD) {
#pragma omp task
      bitonic_merge(arr, size, start, k, direction, comparator);
#pragma omp task
      bitonic_merge(arr, size, start + k, k, direction, comparator);
#pragma omp taskwait
    } else {
      bitonic_merge(arr, size, start, k, direction, comparator);
      bitonic_merge(arr, size, start + k, k, direction, comparator);
    }
  }
}

void _bitonic_sort(void* arr,
                   size_t size,
                   int start,
                   int count,
                   int direction,
                   cmp_func comparator) {
  if (count > 1) {
    int k = count / 2;

    if (count > PARALLEL_THRESHOLD) {
#pragma omp task
      _bitonic_sort(arr, size, start, k, 1, comparator);
#pragma omp task
      _bitonic_sort(arr, size, start + k, k, 0, comparator);
#pragma omp taskwait
    } else {
      _bitonic_sort(arr, size, start, k, 1, comparator);
      _bitonic_sort(arr, size, start + k, k, 0, comparator);
    }

    bitonic_merge(arr, size, start, count, direction, comparator);
  }
}

void bitonic_sort(void* arr, size_t len, size_t size, cmp_func compar) {
#pragma omp parallel
  {
#pragma omp single
    {
      _bitonic_sort(arr, size, 0, len, 1, compar);
    }
  }
}