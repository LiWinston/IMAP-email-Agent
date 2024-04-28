#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>

typedef struct {
    char *bytes;
    int cur;
    int size;
} ByteList;

extern ByteList byteList;

int n_send(char *msg);
int n_readline();
int n_readBytes(int target);
int n_connect(char *server_name, bool tls);
void n_free();

#endif /* NETWORK_H */