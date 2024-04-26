#ifndef EMAIL_H
#define EMAIL_H

#include <stddef.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int message_num;
    char* subject;
} Email;


Email* email_create(int message_num, const char* subject);

void email_destroy(Email* email);

int compareEmailByMessageNum(const void* a, const void* b);

#endif /* EMAIL_H */
