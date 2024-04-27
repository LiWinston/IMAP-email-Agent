#ifndef CBUFFER_H
#define CBUFFER_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *data;
    size_t size;
    size_t capacity;
    FILE *file;
    FILE *tempfile;
} CBuffer;

CBuffer *cb_init();
void cb_read_file(CBuffer *cb, const char *filename);
void cb_expand(CBuffer *cb, size_t newCapacity);
char *cb_read(CBuffer *cb, size_t *size);
void cb_write(CBuffer *cb, const char *data, size_t size);
void cb_clear(CBuffer *cb);
void cb_destroy(CBuffer *cb);

#endif /* CBUFFER_H */
