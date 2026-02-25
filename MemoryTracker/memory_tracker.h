#pragma once
#ifndef MEMORY_TRACKER_H
#define MEMORY_TRACKER_H

#include <stddef.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    size_t total_allocated;
    size_t peak_allocated;
    int allocation_count;
    size_t current_usage;
} MemTracker;

void mem_reset(MemTracker* t);
//void mem_snapshot(MemTracker* t);

void* tracked_malloc(size_t size);
void* tracked_realloc(void* ptr, size_t size);
void tracked_free(void* ptr);

void mem_set_tracker(MemTracker* t);
MemTracker* mem_get_tracker(void);

#ifdef __cplusplus
}
#endif

#endif
