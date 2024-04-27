#include "PriorityQueue.h"
// Refenrenced from heap.c in project 1, add generic support

PriorityQueue * priority_queue_create(size_t capacity, int (*cmp)(const void*, const void*)) {
    PriorityQueue* pq = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!pq) return NULL;

    pq->data = (void**)malloc(capacity * sizeof(void*));
    if (!pq->data) {
        free(pq);
        return NULL;
    }

    pq->size = 0;
    pq->capacity = capacity;
    pq->cmp = cmp;

    return pq;
}

void priority_queue_destroy(PriorityQueue* pq) {
    if (!pq) return;

    free(pq->data);
    free(pq);
}

void priority_queue_push(PriorityQueue* pq, void* item) {
    if (pq->size == pq->capacity) {
        // Resize the array if full
        pq->capacity *= 2;
        pq->data = (void**)realloc(pq->data, pq->capacity * sizeof(void*));
        if (!pq->data) {
            // Memory allocation failed
            return;
        }
    }

    // Add the new item to the end
    pq->data[pq->size++] = item;

    // Perform up-heap (percolate-up) operation to maintain heap property
    size_t i = pq->size - 1;
    while (i > 0 && pq->cmp(pq->data[i], pq->data[(i - 1) / 2]) < 0) {
        void* temp = pq->data[i];
        pq->data[i] = pq->data[(i - 1) / 2];
        pq->data[(i - 1) / 2] = temp;
        i = (i - 1) / 2;
    }
}

void* priority_queue_pop(PriorityQueue* pq) {
    if (pq->size == 0) return NULL;

    // Remove the root item
    void* item = pq->data[0];

    // Move the last item to the root
    pq->data[0] = pq->data[--pq->size];

    // Perform down-heap (percolate-down) operation to maintain heap property
    size_t i = 0;
    while (2 * i + 1 < pq->size) {
        size_t left_child = 2 * i + 1;
        size_t right_child = 2 * i + 2;
        size_t min_child = left_child;

        if (right_child < pq->size && pq->cmp(pq->data[right_child], pq->data[left_child]) < 0) {
            min_child = right_child;
        }

        if (pq->cmp(pq->data[i], pq->data[min_child]) > 0) {
            void* temp = pq->data[i];
            pq->data[i] = pq->data[min_child];
            pq->data[min_child] = temp;
            i = min_child;
        } else {
            break;
        }
    }

    return item;
}

bool priority_queue_empty(PriorityQueue* pq) {
    return pq->size == 0;
}
