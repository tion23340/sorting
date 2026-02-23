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

void merge(void* arr, size_t l, size_t r, cmp_func comp, size_t el_size){
    size_t mid = (l + r) / 2;
    size_t i1 = l, i2 = mid, t = 0;
    

    void* sorted = malloc((r - l) * el_size);
    if(!sorted) return; 


    while(i1 < mid && i2 < r){
        if(comp(arr+i1*el_size, arr+i2*el_size) < 0){
            memcpy(sorted+t*el_size, arr + i1*el_size, el_size);   
            i1++;
        }
        else {
            memcpy(sorted+t*el_size, arr + i2*el_size, el_size);
            i2++;
        }
        t++;
    }

    while (i1 < mid) {
        memcpy(sorted+t*el_size, arr + i1*el_size, el_size);
        i1++;
        t++;
    }
    while (i2 < r) {
        memcpy(sorted+t*el_size, arr+i2*el_size, el_size);
        i2++;
        t++;
    }

    t = 0;
    i1 = l;
    while(i1 < r){
        memcpy(arr+i1*el_size, sorted+t*el_size, el_size);
        i1++;
        t++;
    }

    free(sorted);
}

void sort(void* arr, size_t l, size_t r, cmp_func comp, size_t size){
    if(l == r - 1) 
        return;
    size_t mid = (l + r) / 2;

    sort(arr, l, mid, comp, size);
    sort(arr, mid, r, comp, size);
    merge(arr, l, r, comp, size);
}


int compare_int_asc(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

void merge_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    sort(arr, 0, len, compar, size);
}
