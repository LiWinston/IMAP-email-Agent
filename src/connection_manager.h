// connection_manager.h

#ifndef CONNECTION_MANAGER_H
#define CONNECTION_MANAGER_H

#include "tag_manager.h"

#define MAX_BUFFER_SIZE 1024

typedef struct {
    int socket_fd;
    TagManager* tag_manager;
} ConnectionManager;

ConnectionManager* connection_manager_create();
void connection_manager_destroy(ConnectionManager* cm);
int connect_to_server(ConnectionManager* cm, const char* server_name, int port);
int login(ConnectionManager* cm, const char* username, const char* password);
int select_folder(ConnectionManager* cm, const char* folder);
int retrieve_message(ConnectionManager* cm, const char* messageNum);
void close_connection(ConnectionManager* cm);
char *strstr_case_insensitive(const char *haystack, const char *needle);

#endif /* CONNECTION_MANAGER_H */
