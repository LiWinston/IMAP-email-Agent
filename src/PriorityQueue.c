#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "PriorityQueue.h"
#define PAGE_NUMBER 512

//Refactored from heap.c in project 1, originally implemented by Rong Wang
//add generic support, use void* to store data, use function pointer to compare data
void initPriorityQueue(PriorityQueue *pq) {
  pq->sorted = false;
  pq->size = 0;
  pq->cap = 16;
  pq->array = malloc(pq->cap * sizeof(void*));
}

void freePriorityQueue(PriorityQueue *pq) {
  pq->size = pq->cap = 0;
  free(pq->array);
}

void pushPriorityQueue(PriorityQueue *pq, void* val) {
  pq->sorted = false;

  if (pq->size == pq->cap) {
    pq->cap += pq->cap;
    pq->array = realloc(pq->array, sizeof(void*) * pq->cap);
  }

  pq->array[pq->size] = val;
  for (int cur = pq->size; cur; cur = (cur - 1) / 2) {
    if (pq->array[(cur - 1) / 2] < val)
      break;
    pq->array[cur] = pq->array[(cur - 1) / 2];
    pq->array[(cur - 1) / 2] = val;
  }

  pq->size++;
}

void* popPriorityQueue(PriorityQueue *pq) {
  void* ans = pq->array[0];
  pq->sorted = false;

  pq->size--;
  pq->array[0] = pq->array[pq->size];
  // replace last element with inf
  pq->array[pq->size] = NULL;
  int cur = 0;
  while (true) {
    int l = cur + cur + 1, r = l + 1;
    if (l >= pq->size)
      break;
    if (pq->array[l] < pq->array[r] && pq->array[l] < pq->array[cur]) {
      void* temp = pq->array[l];
      pq->array[l] = pq->array[cur];
      pq->array[cur] = temp;
      cur = l;
    } else if (pq->array[r] < pq->array[cur]) {
      void* temp = pq->array[r];
      pq->array[r] = pq->array[cur];
      pq->array[cur] = temp;
      cur = r;
    } else {
      break;
    }
  }
  return ans;
}

void h2hPriorityQueue(PriorityQueue *source, PriorityQueue *dest, int amount, PriorityQueue *record) {
  if (amount == source->size) {
    while (source->size > 0) {
      source->size--;
      if (record != NULL)
        pushPriorityQueue(record, source->array[source->size]);
      pushPriorityQueue(dest, source->array[source->size]);
    }
  } else {
    for (int _ = 0; _ < amount; _++) {
      void* top = popPriorityQueue(source);
      if (record != NULL)
        pushPriorityQueue(record, top);
      pushPriorityQueue(dest, top);
    }
    if (dest->size == amount)
      dest->sorted = true;
  }
}

void sortPriorityQueue(PriorityQueue *pq, int (*cmp)(const void*, const void*)){
    if (pq->sorted)
        return;
    void* tmp[PAGE_NUMBER];
    int size = pq->size;
    for (int i = 0; i < size; i++) {
        tmp[i] = popPriorityQueue(pq);
    }
    memcpy(pq->array, tmp, sizeof(void*) * size);
    pq->size = size;
    pq->sorted = true;
}
