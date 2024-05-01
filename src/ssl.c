#include "ssl.h"

SSL_CTX *ctx;
SSL *ssl;

int ssl_write(const void *buf, ssize_t n) {
    return SSL_write(ssl, buf, n);
}

int ssl_read(void *buf, ssize_t n) {
    return SSL_read(ssl, buf, n);
}

int ssl_initial(int sockfd) {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL) {
        printf("Cannot create SSL context\n");
        return 2;
    }

    ssl = SSL_new(ctx);
    if (ssl == NULL) {
        printf("Cannot create SSL structure\n");
        SSL_CTX_free(ctx);
        return 2;
    }

    SSL_set_fd(ssl, sockfd);
    if (SSL_connect(ssl) <= 0) {
        printf("SSL handshake failed\n");
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        return 2;
    }

    return 0;
}

void ssl_free() {
    if (ssl) {
        SSL_free(ssl);
    }
    if (ctx) {
        SSL_CTX_free(ctx);
    }
}