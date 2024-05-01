#ifndef SSL_H
#define SSL_H
#include <openssl/err.h>
#include <openssl/ssl.h>

int ssl_read(void *buf, ssize_t n) __attribute__((warn_unused_result));
int ssl_write(const void *buf, ssize_t n) __attribute__((warn_unused_result));
int ssl_initial(int sockfd) __attribute__((warn_unused_result));
void ssl_free();

#endif /* SSL_H */