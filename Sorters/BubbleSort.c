#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "Base.h"
#include "memory_tracker.h"

void bubble_sort(void *arr, size_t len, size_t size, cmp_func compar) {
    if (len < 2) return;

    char *base = (char *)arr;

    void *temp = tracked_malloc(size);
    if (!temp) return;
    for (size_t i = 0; i < len - 1; i++) {
        for (size_t j = 0; j < len - i - 1; j++) {
            void *elem1 = base + (j * size);
            void *elem2 = base + ((j + 1) * size);

            if (compar(elem1, elem2) > 0) {
                memcpy(temp, elem1, size);
                memcpy(elem1, elem2, size);
                memcpy(elem2, temp, size);
            }
        }
    }

    tracked_free(temp);
}
