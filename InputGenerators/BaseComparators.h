#ifndef COMPARATORS_H
#define COMPARATORS_H

typedef int (*comparator)(const void *a, const void *b);

// int
int compare_int_asc  (const void* a, const void* b);
int compare_int_desc (const void* a, const void* b);

// float
int compare_float_asc  (const void* a, const void* b);
int compare_float_desc (const void* a, const void* b);

// double
int compare_double_asc  (const void* a, const void* b);
int compare_double_desc (const void* a, const void* b);

#endif // COMPARATORS_H