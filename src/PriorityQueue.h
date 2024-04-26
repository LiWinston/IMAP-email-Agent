#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <stdbool.h>

typedef struct {
    bool sorted;
    size_t size;
    size_t cap;
    void** array;
} PriorityQueue;

void initPriorityQueue(PriorityQueue *pq);
void freePriorityQueue(PriorityQueue *pq);
void pushPriorityQueue(PriorityQueue *pq, void* val);
void* popPriorityQueue(PriorityQueue *pq);
void h2hPriorityQueue(PriorityQueue *source, PriorityQueue *dest, int amount, PriorityQueue *record);
void sortPriorityQueue(PriorityQueue *pq, int (*cmp)(const void*, const void*));

#endif /* PRIORITY_QUEUE_H */
