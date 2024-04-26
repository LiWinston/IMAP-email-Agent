#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    int (*cmp)(const void*, const void*);
} PriorityQueue;

PriorityQueue* priority_queue_create(size_t capacity, int (*cmp)(const void*, const void*));
void priority_queue_destroy(PriorityQueue* pq);
void priority_queue_push(PriorityQueue* pq, void* item);
void* priority_queue_pop(PriorityQueue* pq);
bool priority_queue_empty(PriorityQueue* pq);

#endif /* PRIORITY_QUEUE_H */
