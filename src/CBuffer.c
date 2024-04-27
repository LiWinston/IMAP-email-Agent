#include "CBuffer.h"
#include <string.h>

#define DEFAULT_BUFFER_SIZE 1024
#define TEMP_FILE_PREFIX "cb_tempfile_"

CBuffer *cb_init() {
    CBuffer *cb = malloc(sizeof(CBuffer));
    if (!cb) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    cb->data = malloc(DEFAULT_BUFFER_SIZE);
    if (!cb->data) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    cb->size = 0;
    cb->capacity = DEFAULT_BUFFER_SIZE;
    cb->file = NULL;
    cb->tempfile = NULL;

    return cb;
}

void cb_expand(CBuffer *cb, size_t newCapacity) {
    cb->data = realloc(cb->data, newCapacity);
    if (!cb->data) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    cb->capacity = newCapacity;
}

void cb_read_file(CBuffer *cb, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open file");
        return;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    if (fileSize > cb->capacity) {
        if (!cb->tempfile) {
            char tempFileName[L_tmpnam];
            if (!tmpnam(tempFileName)) {
                perror("Failed to generate temporary file name");
                exit(EXIT_FAILURE);
            }
            cb->tempfile = fopen(tempFileName, "wb+");
            if (!cb->tempfile) {
                perror("Failed to create temporary file");
                exit(EXIT_FAILURE);
            }
            fwrite(cb->data, sizeof(char), cb->size, cb->tempfile);
            free(cb->data);
            cb->data = NULL;
            cb->capacity = 0;
        }

        char *tempData = malloc(fileSize);
        if (!tempData) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        fread(tempData, sizeof(char), fileSize, file);
        fwrite(tempData, sizeof(char), fileSize, cb->tempfile);
        free(tempData);
    } else {
        fread(cb->data, sizeof(char), fileSize, file);
        cb->size = fileSize;
    }

    fclose(file);
}

char *cb_read(CBuffer *cb, size_t *size) {
    if (!cb->tempfile) {
        *size = cb->size;
        return cb->data;
    } else {
        fseek(cb->tempfile, 0, SEEK_END);
        *size = ftell(cb->tempfile);
        fseek(cb->tempfile, 0, SEEK_SET);

        char *data = malloc(*size);
        if (!data) {
            perror("Memory allocation failed");
            exit(EXIT_FAILURE);
        }

        fread(data, sizeof(char), *size, cb->tempfile);
        return data;
    }
}

void cb_write(CBuffer *cb, const char *data, size_t size) {
    if (cb->size + size > cb->capacity) {
        if (!cb->tempfile) {
            char tempFileName[L_tmpnam];
            if (!tmpnam(tempFileName)) {
                perror("Failed to generate temporary file name");
                exit(EXIT_FAILURE);
            }
            cb->tempfile = fopen(tempFileName, "wb+");
            if (!cb->tempfile) {
                perror("Failed to create temporary file");
                exit(EXIT_FAILURE);
            }
            if (cb->data) {
                fwrite(cb->data, sizeof(char), cb->size, cb->tempfile);
                free(cb->data);
                cb->data = NULL;
                cb->capacity = 0;
            }
        }
        // 将文件指针移到文件末尾
        fseek(cb->tempfile, 0, SEEK_END);
        // 将新数据写入临时文件
        fwrite(data, sizeof(char), size, cb->tempfile);
    } else {
        if (cb->size + size > cb->capacity) {
            size_t newCapacity = cb->capacity * 2;
            cb_expand(cb, newCapacity);
        }
        memcpy(cb->data + cb->size, data, size);
        cb->size += size;
    }
}

void cb_clear(CBuffer *cb) {
    if (cb->tempfile) {
        fclose(cb->tempfile);
        cb->tempfile = NULL;
    } else {
        cb->size = 0;
    }
}

void cb_destroy(CBuffer *cb) {
    if (cb->tempfile) {
        fclose(cb->tempfile);
        cb->tempfile = NULL;
        char tempFileName[256];
        strncpy(tempFileName, TEMP_FILE_PREFIX, sizeof(tempFileName));
        strcat(tempFileName, "XXXXXX");
        remove(tempFileName);  // 删除临时文件
    } else {
        if (cb->data) {
            //free(cb->data); 会导致 free(): double free detected
        }
    }
    free(cb);
    cb = NULL;
}