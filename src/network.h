#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>

typedef struct {
    char *bytes;
    int cur;
    int size;
} ByteList;

extern ByteList byteList;

int n_send(char *msg) __attribute__((warn_unused_result));
int n_readLine() __attribute__((warn_unused_result));
int n_readBytes(int target) __attribute__((warn_unused_result));
int n_connect(char *server_name, bool tls) __attribute__((warn_unused_result));
void n_free();

#endif /* NETWORK_H */