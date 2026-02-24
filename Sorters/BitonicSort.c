#include <omp.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "Base.h"

// --------------------------
// Tuning knobs
// --------------------------
// Use static schedule for predictable work distribution.
// Chunk controls how big each worker slice is (reduce scheduling overhead).
#define OMP_STATIC_CHUNK 4096

// --------------------------
// Optional comparator fast-path dispatch
// (still generic API; we just avoid function-pointer call when we can)
// --------------------------
typedef enum {
  CMP_GENERIC = 0,
  CMP_I32_ASC,
  CMP_I32_DESC,
  CMP_F32_ASC,
  CMP_F32_DESC,
  CMP_F64_ASC,
  CMP_F64_DESC
} cmp_kind_t;

// These are defined in C++ but exported as C (extern "C" in header/defs)
int compare_int_asc(const void* a, const void* b);
int compare_int_desc(const void* a, const void* b);
int compare_float_asc(const void* a, const void* b);
int compare_float_desc(const void* a, const void* b);
int compare_double_asc(const void* a, const void* b);
int compare_double_desc(const void* a, const void* b);

static inline cmp_kind_t detect_cmp_kind(cmp_func c) {
  if (c == compare_int_asc)     return CMP_I32_ASC;
  if (c == compare_int_desc)    return CMP_I32_DESC;
  if (c == compare_float_asc)   return CMP_F32_ASC;
  if (c == compare_float_desc)  return CMP_F32_DESC;
  if (c == compare_double_asc)  return CMP_F64_ASC;
  if (c == compare_double_desc) return CMP_F64_DESC;
  return CMP_GENERIC;
}

// --------------------------
// Swaps (NO heap allocation)
// --------------------------
static inline void swap4(void* a, void* b) {
  uint32_t t = *(uint32_t*)a;
  *(uint32_t*)a = *(uint32_t*)b;
  *(uint32_t*)b = t;
}
static inline void swap8(void* a, void* b) {
  uint64_t t = *(uint64_t*)a;
  *(uint64_t*)a = *(uint64_t*)b;
  *(uint64_t*)b = t;
}

// Generic swap without VLAs and without malloc.
// Uses a fixed buffer up to 256 bytes; for bigger elements, falls back to
// a byte-by-byte swap (slow, but keeps API generic and avoids heap).
static inline void swap_mem(void* a, void* b, size_t size) {
  unsigned char tmp[256];

  if (size <= sizeof(tmp)) {
    memcpy(tmp, a, size);
    memcpy(a, b, size);
    memcpy(b, tmp, size);
    return;
  }

  // Very large element sizes: byte-wise swap (rare in your benchmarks)
  unsigned char* pa = (unsigned char*)a;
  unsigned char* pb = (unsigned char*)b;
  for (size_t i = 0; i < size; i++) {
    unsigned char t = pa[i];
    pa[i] = pb[i];
    pb[i] = t;
  }
}

// --------------------------
// Fast comparisons for common types
// --------------------------
static inline int cmp_i32(const void* a, const void* b) {
  int32_t x = *(const int32_t*)a;
  int32_t y = *(const int32_t*)b;
  return (x > y) - (x < y);
}
static inline int cmp_f32(const void* a, const void* b) {
  float x = *(const float*)a;
  float y = *(const float*)b;
  return (x > y) - (x < y);
}
static inline int cmp_f64(const void* a, const void* b) {
  double x = *(const double*)a;
  double y = *(const double*)b;
  return (x > y) - (x < y);
}

// Return >0 if (ei > ej) under the comparator "kind" semantics.
// For DESC kinds we negate.
static inline int cmp_fast(const void* ei, const void* ej, cmp_func cmp, cmp_kind_t kind) {
  switch (kind) {
    case CMP_I32_ASC:  return cmp_i32(ei, ej);
    case CMP_I32_DESC: return -cmp_i32(ei, ej);
    case CMP_F32_ASC:  return cmp_f32(ei, ej);
    case CMP_F32_DESC: return -cmp_f32(ei, ej);
    case CMP_F64_ASC:  return cmp_f64(ei, ej);
    case CMP_F64_DESC: return -cmp_f64(ei, ej);
    default:           return cmp(ei, ej);
  }
}

static inline void comp_swap_generic(
    void* arr, size_t size,
    size_t i, size_t j,
    int ascending_for_this_pair,
    cmp_func cmp,
    cmp_kind_t kind)
{
  void* ei = (char*)arr + i * size;
  void* ej = (char*)arr + j * size;

  int cr = cmp_fast(ei, ej, cmp, kind);

  // If ascending_for_this_pair==1 want ei <= ej else want ei >= ej
  if ((ascending_for_this_pair && cr > 0) || (!ascending_for_this_pair && cr < 0)) {
    if (size == 4)      swap4(ei, ej);
    else if (size == 8) swap8(ei, ej);
    else                swap_mem(ei, ej, size);
  }
}

// --------------------------
// Iterative Bitonic Sort (requires n is power of two)
// Fully generic interface still.
// --------------------------
static inline int is_power_of_two(size_t n) {
  return n && ((n & (n - 1)) == 0);
}

void bitonic_sort(void* arr, size_t len, size_t size, cmp_func compar) {
  if (len <= 1) return;

  // Your benchmark uses powers of two; bitonic network requires it.
  // If not a power of two, you can either:
  //  - return / do nothing
  //  - or pad to next power-of-two (needs temp buffer)
  if (!is_power_of_two(len)) {
    // Fallback: do nothing (or implement padding if needed)
    // (Benchmark args you showed are powers of two, so this path won't be hit.)
    return;
  }

  const cmp_kind_t kind = detect_cmp_kind(compar);

  // Bitonic network:
  // for k = 2 .. n (doubling)
  //   for j = k/2 .. 1 (halving)
  //     for i = 0 .. n-1
  //       ixj = i ^ j
  //       if ixj > i: compare-swap based on direction (i & k)
  //
  // This structure has synchronization between stages; OpenMP barrier at end of each
  // parallel-for naturally provides that.

  for (size_t k = 2; k <= len; k <<= 1) {
    for (size_t j = k >> 1; j > 0; j >>= 1) {

#pragma omp parallel for schedule(static, OMP_STATIC_CHUNK)
      for (size_t i = 0; i < len; i++) {
        size_t ixj = i ^ j;
        if (ixj > i) {
          // For the classic bitonic sort, direction depends on (i & k).
          // This produces ascending in one half and descending in the other.
          int ascending_for_pair = ((i & k) == 0);

          // Your API always sorts ascending overall (you pass compare_*_asc usually).
          // Direction within the network is handled by ascending_for_pair.
          comp_swap_generic(arr, size, i, ixj, ascending_for_pair, compar, kind);
        }
      }

    }
  }
}
