#ifndef SSL_H
#define SSL_H
#include <openssl/err.h>
#include <openssl/ssl.h>

extern SSL_CTX *ctx;
extern SSL *ssl;

int ssl_read(void *buf, ssize_t n) __attribute__((warn_unused_result));
int ssl_write(const void *buf, ssize_t n) __attribute__((warn_unused_result));
int ssl_initial_(int sockfd) __attribute__((warn_unused_result));
void ssl_free_();

#endif /* SSL_H */