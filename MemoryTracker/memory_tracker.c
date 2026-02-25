#include "memory_tracker.h"
#include <stdlib.h>
#include <string.h>
#include <malloc.h> // Required for malloc_usable_size on Linux

static MemTracker global_tracker;

static _Thread_local MemTracker* current_tracker = NULL;

void mem_reset(MemTracker* t) {
    t->total_allocated = 0;   // Cumulative (Odometer)
    t->peak_allocated = 0;    // Highest point reached
    t->current_usage = 0;     // Current bytes on heap
    t->allocation_count = 0;
}

void mem_set_tracker(MemTracker* t) {
    current_tracker = t;
}

MemTracker* mem_get_tracker(void) {
    return current_tracker;
}

void* tracked_malloc(size_t size) {
    void* ptr = malloc(size);
    if (current_tracker && ptr) {
        // Use the actual size allocated by the OS
        size_t actual_size = malloc_usable_size(ptr);
        
        current_tracker->total_allocated += actual_size; 
        current_tracker->current_usage += actual_size;
        current_tracker->allocation_count++;

        // Peak only updates if CURRENT usage hits a new high
        if (current_tracker->current_usage > current_tracker->peak_allocated) {
            current_tracker->peak_allocated = current_tracker->current_usage;
        }
    }
    return ptr;
}

void* tracked_realloc(void* ptr, size_t size) {
    if (!ptr) return tracked_malloc(size);
    
    size_t old_actual_size = malloc_usable_size(ptr);
    void* new_ptr = realloc(ptr, size);
    
    if (current_tracker && new_ptr) {
        size_t new_actual_size = malloc_usable_size(new_ptr);
        
        // Adjust current usage by the difference
        current_tracker->current_usage -= old_actual_size;
        current_tracker->current_usage += new_actual_size;
        
        // If it grew, it might contribute to a new total/cumulative
        if (new_actual_size > old_actual_size) {
            current_tracker->total_allocated += (new_actual_size - old_actual_size);
        }

        if (current_tracker->current_usage > current_tracker->peak_allocated) {
            current_tracker->peak_allocated = current_tracker->current_usage;
        }
    }
    return new_ptr;
}

void tracked_free(void* ptr) {
    if (!ptr) return;

    if (current_tracker) {
        size_t actual_size = malloc_usable_size(ptr);
        // Reduce the current water level
        current_tracker->current_usage -= actual_size;
    }
    
    free(ptr);
}