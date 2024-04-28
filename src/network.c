#include "network.h"
#include "ssl.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define PORT 143
#define PORT_TLS 993
#define BUFFER_SIZE 4 * 1024 * 1024

ByteList byteList;

static int sockfd;
static ByteList buffer;
static char buffer_bytes[BUFFER_SIZE];

static int (*n_write)(const void *, ssize_t);
static int (*n_read)(void *, ssize_t);
static int (*ssl_initial)(int sockfd);
static void (*ssl_free)();

static void empty_void() {}
static int empty_int(int _) {
    return 0;
}

// Double call is ugly
// But i think it's not bad for expensive operation like read/write
static int plain_write(const void *buf, ssize_t n) {
    return write(sockfd, buf, n);
}

static int plain_read(void *buf, ssize_t n) {
    return read(sockfd, buf, n);
}

int n_send(char *msg) {
    ssize_t target = strlen(msg);
    ssize_t n = 0;
    while (n != target) {
        ssize_t w = n_write(msg, target - n);
        if (w <= 0) {
            perror("Error writing to socket");
            return 3;
        }
        n += w;
    }
    return 0;
}

int n_readline() {
    // Each line of characters MUST be no more than 998 characters, so do not
    // need worry the size of bytelist
    byteList.cur = 0;

    do {
        while (buffer.cur != buffer.size) {
            byteList.bytes[byteList.cur] = buffer.bytes[buffer.cur];
            byteList.cur++;
            buffer.cur++;
            // Read a end of line
            if (byteList.bytes[byteList.cur - 1] == '\n' && byteList.cur > 1 &&
                byteList.bytes[byteList.cur - 2] == '\r') {
                byteList.bytes[byteList.cur] = '\0';
                return 0;
            }
        }

        buffer.cur = 0;
        buffer.size = n_read(buffer.bytes, BUFFER_SIZE);

        if (buffer.size <= 0) {
            perror("Error reading from socket");
            return 3;
        }
    } while (true);

    return 0;
}

int n_readBytes(int target) {
    byteList.cur = 0;
    if (byteList.size <= target) {
        byteList.size = target + 1;
        byteList.bytes = realloc(byteList.bytes, sizeof(char) * byteList.size);
    }

    do {
        int n = MIN(buffer.size - buffer.cur, target);
        memcpy(byteList.bytes + byteList.cur, buffer.bytes + buffer.cur, n);
        byteList.cur += n;
        target -= n;
        buffer.cur += n;
        if (target == 0) {
            break;
        }

        buffer.cur = 0;
        buffer.size = n_read(buffer.bytes, BUFFER_SIZE);

        if (buffer.size <= 0) {
            perror("Error reading from socket");
            return 3;
        }
    } while (true);

    byteList.bytes[byteList.cur] = '\0';

    return 0;
}

int n_connect(char *server_name, bool tls) {
    sockfd = -1;
    buffer.bytes = buffer_bytes;
    byteList.size = BUFFER_SIZE;
    byteList.bytes = malloc(sizeof(char) * byteList.size);

    int port;

    // Initial write and read
    if (!tls) {
        port = PORT;
        n_write = plain_write;
        n_read = plain_read;
        ssl_initial = empty_int;
        ssl_free = empty_void;
    } else {
        port = PORT_TLS;
        n_write = ssl_write;
        n_read = ssl_read;
        ssl_initial = ssl_initial_;
        ssl_free = ssl_free_;
    }

    struct hostent *host_info;
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));

    // Resolve server ip
    host_info = gethostbyname(server_name);
    if (host_info == NULL) {
        fprintf(stderr, "Cannot resolve hostname %s\n", server_name);
        return 3;
    }

    // Set server address struct
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr, host_info->h_addr_list[0],
           host_info->h_length);

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        return 3;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0) {
        perror("Error connecting to server");
        return 3;
    }

    ssl_initial(sockfd);

    return 0;
}

void n_free() {
    ssl_free();
    close(sockfd);
    free(byteList.bytes);
}