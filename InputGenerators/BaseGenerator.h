#ifndef BASEGENERATOR_H
#define BASEGENERATOR_H


#include <stddef.h>

typedef void (*input_fill_generator)(void* arr, size_t len);

void gen_int_signed    (void* arr, size_t len);
void gen_int_unsigned  (void* arr, size_t len);
void gen_float_signed  (void* arr, size_t len);
void gen_float_unsigned(void* arr, size_t len);
void gen_double_signed  (void* arr, size_t len);
void gen_double_unsigned(void* arr, size_t len);

#endif //BASEGENERATOR_H
