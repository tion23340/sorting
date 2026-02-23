#include "Base.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

// DATA AND BUCKET SIZE FLAGS
// uint32_t, int32_t (and int?), float, double
typedef enum { UINT32, SINT32, FLOAT32, FLOAT64} DataType;
typedef enum { bit8 = 8, bit16 = 16} BitsPerPass;

void radix_sort_32(uint32_t *arr, size_t n, BitsPerPass bits_per_pass){
    uint32_t *output = malloc(n * sizeof(uint32_t));
    if (!output) return;
    
    size_t base = 1ULL << bits_per_pass; // 2^bits_per_pass
    size_t passes = 32 / bits_per_pass;

    for (size_t pass = 0; pass < passes; pass++) {
        size_t *count = malloc(base * sizeof(size_t));
        if (!count) { free(output); return; }
        memset(count, 0, base * sizeof(size_t));

        // Count occurrences
        for (size_t i = 0; i < n; i++) {
            size_t digit = (arr[i] >> (pass * bits_per_pass)) & (base - 1);
            count[digit]++;
        }

        // Create offsets
        for (size_t i = 1; i < base; i++) {
            count[i] += count[i - 1];
        }

        // Sort
        for (size_t i = n; i > 0; i--) {
            uint32_t value = arr[i - 1];
            size_t digit = (value >> (pass * bits_per_pass)) & (base - 1);
            output[--count[digit]] = value;
        }

        // Copy back
        memcpy(arr, output, n * sizeof(uint32_t));

        free(count);
    }
    free(output);
}

void radix_sort_64(uint64_t *arr, size_t n, BitsPerPass bits_per_pass){
    uint64_t *output = malloc(n * sizeof(uint64_t));
    if (!output) return;

    size_t base = 1ULL << bits_per_pass;
    size_t passes = 64 / bits_per_pass;

    for (size_t pass = 0; pass < passes; pass++) {
        size_t *count = malloc(base * sizeof(size_t));
        if (!count) { free(output); return; }
        memset(count, 0, base * sizeof(size_t));

        // Count occurrences
        for (size_t i = 0; i < n; i++) {
            size_t digit = (arr[i] >> (pass * bits_per_pass)) & (base - 1);
            count[digit]++;
        }

        // Create offsets
        for (size_t i = 1; i < base; i++) {
            count[i] += count[i - 1];
        }

        // Sort
        for (size_t i = n; i > 0; i--) {
            uint64_t value = arr[i - 1];
            size_t digit = (value >> (pass * bits_per_pass)) & (base - 1);
            output[--count[digit]] = value;
        }

        // Copy back
        memcpy(arr, output, n * sizeof(uint64_t));

        free(count);
    }
    free(output);
}


void int32_to_uint32(int32_t *arr, size_t n){
    uint32_t *u = (uint32_t *)arr;
    for (size_t i = 0; i < n; i++) {
        u[i] ^= 0x80000000u;
    }
}
void uint32_to_int32(uint32_t *arr, size_t n){
    for (size_t i = 0; i < n; i++) {
        arr[i] ^= 0x80000000u;
    }
}
void float32_to_int32(float *arr, size_t n){
    uint32_t *u = (uint32_t *)arr;
    for (size_t i = 0; i < n; i++) {
        uint32_t bits = u[i];
        if (bits & 0x80000000u)
            u[i] = ~bits;
        else
            u[i] = bits ^ 0x80000000u;
    }
}
void uint32_to_float32(uint32_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        uint32_t bits = arr[i];
        if (bits & 0x80000000u)
            arr[i] = bits ^ 0x80000000u;
        else
            arr[i] = ~bits;
    }
}
void float64_to_int64(double *arr, size_t n){
    uint64_t *u = (uint64_t *)arr;
    for (size_t i = 0; i < n; i++) {
        uint64_t bits = u[i];
        if (bits & 0x8000000000000000ull)
            u[i] = ~bits;
        else
            u[i] = bits ^ 0x8000000000000000ull;
    }
}
void uint64_to_float64(uint64_t *arr, size_t n) {
    for (size_t i = 0; i < n; i++) {
        uint64_t bits = arr[i];
        if (bits & 0x8000000000000000ull)
            arr[i] = bits ^ 0x8000000000000000ull;
        else
            arr[i] = ~bits;
    }
}

//O(p*(n + k)) where p - passes, n - array size, k - bucket size
//Ex. 32 bit integers and 8 bits per pass: O(4*(n + 256))
//Ex. 32 bit integers and 16 bits per pass: O(2*(n + 65536))
void radix_sort(void *arr, size_t len, cmp_func compar, DataType type, BitsPerPass bits_per_pass) {
    if (arr == NULL || len <= 1) return;
    switch(type) {
        case UINT32: {
            uint32_t *u = (uint32_t *)arr;
            radix_sort_32(u, len, bits_per_pass);
            break;
        }
        case SINT32: {
            int32_t *u = (int32_t *)arr;
            int32_to_uint32(u, len);
            radix_sort_32((uint32_t *)u, len, bits_per_pass);
            uint32_to_int32((uint32_t *)u, len);
            break;
        }
        case FLOAT32: {
            float *u = (float *)arr;
            float32_to_int32(u, len);
            radix_sort_32((uint32_t *)u, len, bits_per_pass);
            uint32_to_float32((uint32_t *)u, len);
            break;
        }
        case FLOAT64: {
            double *u = (double *)arr;
            float64_to_int64(u, len);
            radix_sort_64((uint64_t *)u, len, bits_per_pass);
            uint64_to_float64((uint64_t *)u, len);
            break;
        }
    }
}
