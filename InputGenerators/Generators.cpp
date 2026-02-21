#include <cstdlib>
#include <random>

void gen_int_signed(void* arr, size_t len) {
    int* a = (int*)arr;
    for (size_t i = 0; i < len; i++)
        a[i] = (std::rand() % 200001) - 100000;
}

void gen_int_unsigned(void* arr, size_t len) {
    int* a = (int*)arr;
    for (size_t i = 0; i < len; i++)
        a[i] = std::rand();
}

void gen_float_signed(void* arr, size_t len) {
    float* a = (float*)arr;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(-1e6f, 1e6f);
    for (size_t i = 0; i < len; i++)
        a[i] = dist(rng);
}

void gen_float_unsigned(void* arr, size_t len) {
    float* a = (float*)arr;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(0.0f, 1e6f);
    for (size_t i = 0; i < len; i++)
        a[i] = dist(rng);
}

void gen_double_signed(void* arr, size_t len) {
    double* a = (double*)arr;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(-1e12, 1e12);
    for (size_t i = 0; i < len; i++)
        a[i] = dist(rng);
}

void gen_double_unsigned(void* arr, size_t len) {
    double* a = (double*)arr;
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1e12);
    for (size_t i = 0; i < len; i++)
        a[i] = dist(rng);
}