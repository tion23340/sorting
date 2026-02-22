#include <stddef.h>
#include <string.h>
#include <stdlib.h>
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
void bubble_sort(void *arr, size_t len, size_t size, cmp_func compar) {
    if (len < 2) return;

    char *base = (char *)arr;

    void *temp = malloc(size);
    if (!temp) return;
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            void *elem1 = base + (j * size);
            void *elem2 = base + ((j + 1) * size);

            if (compar(elem1, elem2) > 0) {
                memcpy(temp, elem1, size);
                memcpy(elem1, elem2, size);
                memcpy(elem2, temp, size);
            }
        }
    }

    free(temp);
}
