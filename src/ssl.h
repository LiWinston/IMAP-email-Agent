#ifndef SSL_H
#define SSL_H
#include <openssl/err.h>
#include <openssl/ssl.h>

extern SSL_CTX *ctx;
extern SSL *ssl;

int ssl_read(void *buf, ssize_t n);
int ssl_write(const void *buf, ssize_t n);
int ssl_initial_(int sockfd);
void ssl_free_();

#endif /* SSL_H */