#ifndef EMAIL_CLIENT_H
#define EMAIL_CLIENT_H

#include <stdbool.h>

typedef struct {
    char *username;
    char *password;
    char *folder;
    int messageNum;
    char *command;
    char *server_name;
    bool tls_flag;
} Arguments;

extern Arguments arg;

int c_run() __attribute__((warn_unused_result));
void c_free();

#endif /* EMAIL_CLIENT_H */