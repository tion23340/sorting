#include "Base.h"

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

void bitonic_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    //TODO
}
