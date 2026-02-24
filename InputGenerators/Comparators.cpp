#include "BaseComparators.h"

extern "C" {

// -----------------------
// INT
// -----------------------
int compare_int_asc(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (x > y) - (x < y);
}

int compare_int_desc(const void* a, const void* b) {
    int x = *(const int*)a;
    int y = *(const int*)b;
    return (y > x) - (y < x);
}

// -----------------------
// FLOAT
// -----------------------
int compare_float_asc(const void* a, const void* b) {
    float x = *(const float*)a;
    float y = *(const float*)b;
    return (x > y) - (x < y);
}

int compare_float_desc(const void* a, const void* b) {
    float x = *(const float*)a;
    float y = *(const float*)b;
    return (y > x) - (y < x);
}

// -----------------------
// DOUBLE
// -----------------------
int compare_double_asc(const void* a, const void* b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    return (x > y) - (x < y);
}

int compare_double_desc(const void* a, const void* b) {
    double x = *(const double*)a;
    double y = *(const double*)b;
    return (y > x) - (y < x);
}

} // extern "C"
