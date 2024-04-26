#include "Email.h"
Email* email_create(int message_num, const char* subject) {
    Email* email = (Email*)malloc(sizeof(Email));
    if (!email) return NULL;

    email->message_num = message_num;
    email->subject = strdup(subject);

    return email;
}

void email_destroy(Email* email) {
    if (!email) return;
    free(email->subject);
    free(email);
}

int compareEmailByMessageNum(const void* a, const void* b) {
    const Email* emailA = (const Email*)a;
    const Email* emailB = (const Email*)b;
    return emailA->message_num - emailB->message_num;
}
