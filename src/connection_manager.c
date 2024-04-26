// connection_manager.c

#include "connection_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

ConnectionManager* connection_manager_create() {
    ConnectionManager* cm = (ConnectionManager*)malloc(sizeof(ConnectionManager));
    if (!cm) return NULL;

    cm->socket_fd = -1;
    cm->tag_manager = tag_manager_create();
    if (!cm->tag_manager) {
        free(cm);
        return NULL;
    }

    return cm;
}

void connection_manager_destroy(ConnectionManager* cm) {
    if (!cm) return;

    if (cm->socket_fd != -1) {
        close(cm->socket_fd);
    }

    tag_manager_destroy(cm->tag_manager);
    free(cm);
}

int connect_to_server(ConnectionManager* cm, const char* server_name, int port) {
    // Create socket
    struct sockaddr_in server_addr;
    cm->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (cm->socket_fd < 0) {
        perror("Error opening socket");
        // return -1;
        _exit(2);
    }

    struct hostent *server = gethostbyname(server_name);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        return -1;
    }

    bzero((char*) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char*)server->h_addr_list[0], (char*)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Connect to server
    if (connect(cm->socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        return -1;
    }

    return 0;
}
char *strstr_case_insensitive(const char *haystack, const char *needle);
int login(ConnectionManager* cm, const char* username, const char* password) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));

    // Send login command
    sprintf(buffer, "%s LOGIN %s %s\r\n", tag, username, password);
    if (send(cm->socket_fd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending login command");
        return -1;
    }

    // Receive response
    if (recv(cm->socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Error receiving login response");
        return -1;
    }

    // Check response
    if (strstr_case_insensitive(buffer, "OK") == NULL) {
        printf("Login failure\n");
        return -1;
    }

    return 0;
}

int select_folder(ConnectionManager* cm, const char* folder) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));

    // Send select folder command
    sprintf(buffer, "%s SELECT %s\r\n", tag, folder);
    if (send(cm->socket_fd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending select folder command");
        return -1;
    }

    // Receive response
    if (recv(cm->socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Error receiving select folder response");
        return -1;
    }

    // Check response
    if (strstr(buffer, "OK") == NULL) {
        printf("Folder not found\n");
        _exit(3);
    }

    return 0;
}

void close_connection(ConnectionManager* cm) {
    if (cm->socket_fd != -1) {
        close(cm->socket_fd);
        cm->socket_fd = -1;
    }
}

char *strstr_case_insensitive(const char *haystack, const char *needle) {
    // Case-insensitive version of strstr
    size_t needle_len = strlen(needle);
    if (needle_len == 0) {
        return (char *)haystack;
    }

    while (*haystack) {
        if (strncasecmp(haystack, needle, needle_len) == 0) {
            return (char *)haystack;
        }
        haystack++;
    }

    return NULL;
}
