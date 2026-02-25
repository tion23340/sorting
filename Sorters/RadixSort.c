#include "Base.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "memory_tracker.h"
#include "BaseComparators.h"
// Note: Using tracked_malloc and tracked_free to monitor heap usage.



typedef enum { UINT32, SINT32, FLOAT32, FLOAT64 } DataType;
typedef enum { bit8 = 8, bit16 = 16 } BitsPerPass;


void radix_sort_32(uint32_t *arr, size_t n, BitsPerPass bits_per_pass) {
    uint32_t *output = tracked_malloc(n * sizeof(uint32_t));
    if (!output) return;
    
    size_t base = 1ULL << bits_per_pass; 
    size_t passes = 32 / bits_per_pass;

    // Optimization: Allocate count array once outside the loop
    size_t *count = tracked_malloc(base * sizeof(size_t));
    if (!count) { tracked_free(output); return; }

    for (size_t pass = 0; pass < passes; pass++) {
        memset(count, 0, base * sizeof(size_t));

        for (size_t i = 0; i < n; i++) {
            size_t digit = (arr[i] >> (pass * bits_per_pass)) & (base - 1);
            count[digit]++;
        }

        for (size_t i = 1; i < base; i++) {
            count[i] += count[i - 1];
        }

        for (size_t i = n; i > 0; i--) {
            uint32_t value = arr[i - 1];
            size_t digit = (value >> (pass * bits_per_pass)) & (base - 1);
            output[--count[digit]] = value;
        }

        memcpy(arr, output, n * sizeof(uint32_t));
    }
    
    tracked_free(count);
    tracked_free(output);
}

void radix_sort_64(uint64_t *arr, size_t n, BitsPerPass bits_per_pass) {
    uint64_t *output = tracked_malloc(n * sizeof(uint64_t));
    if (!output) return;

    size_t base = 1ULL << bits_per_pass;
    size_t passes = 64 / bits_per_pass;

    size_t *count = tracked_malloc(base * sizeof(size_t));
    if (!count) { tracked_free(output); return; }

    for (size_t pass = 0; pass < passes; pass++) {
        memset(count, 0, base * sizeof(size_t));

        for (size_t i = 0; i < n; i++) {
            size_t digit = (arr[i] >> (pass * bits_per_pass)) & (base - 1);
            count[digit]++;
        }

        for (size_t i = 1; i < base; i++) {
            count[i] += count[i - 1];
        }

        for (size_t i = n; i > 0; i--) {
            uint64_t value = arr[i - 1];
            size_t digit = (value >> (pass * bits_per_pass)) & (base - 1);
            output[--count[digit]] = value;
        }

        memcpy(arr, output, n * sizeof(uint64_t));
    }

    tracked_free(count);
    tracked_free(output);
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




static DataType detect_type(size_t size, cmp_func compar) {
    if (size == 4) {
        if (compar == compare_float_asc || compar == compare_float_desc) 
            return FLOAT32;
        return UINT32; 
    } else if (size == 8) {
        if (compar == compare_double_asc || compar == compare_double_desc) 
            return FLOAT64;
        return FLOAT64; 
    }
    return UINT32;
}


void radix_sort(void* arr, size_t len, size_t size, cmp_func compar) {
    if (arr == NULL || len <= 1) return;

    BitsPerPass bits_per_pass = bit8;
    DataType type = detect_type(size, compar);

    switch(type) {
        case UINT32: {
            if (compar == compare_int_asc || compar == compare_int_desc) {
                int32_t *u = (int32_t *)arr;
                int32_to_uint32(u, len);
                radix_sort_32((uint32_t *)u, len, bits_per_pass);
                uint32_to_int32((uint32_t *)u, len);
            } else {
                radix_sort_32((uint32_t *)arr, len, bits_per_pass);
            }
            break;
        }
        case SINT32: { 
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