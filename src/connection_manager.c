// connection_manager.c

#include "connection_manager.h"
#include "Email.h"
#include "PriorityQueue.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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
        exit(2);
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

void login_failure() {
    printf("Login failure\n");
    exit(3);
}

int login(const ConnectionManager* cm, const char* username, const char* password) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));

    // Send login command
    sprintf(buffer, "%s LOGIN %s %s\r\n", tag, username, password);
    if (send(cm->socket_fd, buffer, strlen(buffer), 0) < 0) {
        perror("Error sending login command");
        login_failure();
    }

    // Receive response
    if (recv(cm->socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Error receiving login response");
        login_failure();
    }

    // Check response
    if (strstr_case_insensitive(buffer, "OK") == NULL) {
        login_failure();
    }

    return 0;
}

int select_folder(const ConnectionManager * cm, const char* folder) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));


    // Send select folder command
    // If folder is NULL(init value), select INBOX
    sprintf(buffer, "%s SELECT %s\r\n", tag, folder ? folder : "INBOX");
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
    if (strstr_case_insensitive(buffer, "OK") == NULL) {
        printf("Folder not found\n");
        exit(3);
    }

    return 0;
}

int retrieve_ShowMessage(const ConnectionManager * cm, const char* messageNum) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));

    // Construct FETCH command
    char command[MAX_BUFFER_SIZE];
    if (messageNum == NULL) {
        sprintf(command, "%s FETCH * BODY.PEEK[]\r\n", tag);
    } else {
        sprintf(command, "%s FETCH %s BODY.PEEK[]\r\n", tag, messageNum);
    }

    // Send FETCH command
    if (send(cm->socket_fd, command, strlen(command), 0) < 0) {
        perror("Error sending FETCH command");
        return -1;
    }

    // Receive response
    if (recv(cm->socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Error receiving FETCH response");
        return -1;
    }

    // Check response
    if (strstr_case_insensitive(buffer, "OK") == NULL) {
        printf(" Message not found\n");
        exit(3);
    }

    // Print raw email
    printf("%s", buffer);
    exit(0);
}

PriorityQueue* retrieve_ListSubjects(const ConnectionManager * cm, const char* folder) {
    char buffer[MAX_BUFFER_SIZE];
    char tag[TAG_MAX_LEN];
    sprintf(tag, "%s", generate_tag(cm->tag_manager));

    if (select_folder(cm, folder) < 0) {
        printf("Failed to select folder\n");
        return NULL;
    }

    if (send(cm->socket_fd, "FETCH 1:* BODY[HEADER.FIELDS (SUBJECT)]", strlen("FETCH 1:* BODY[HEADER.FIELDS (SUBJECT)]"), 0) < 0) {
        perror("Error sending FETCH command");
        return NULL;
    }

    if (recv(cm->socket_fd, buffer, MAX_BUFFER_SIZE, 0) < 0) {
        perror("Error receiving FETCH response");
        return NULL;
    }

    if (strstr_case_insensitive(buffer, "OK") == NULL) {
        printf("Failed to retrieve email\n");
        return NULL;
    }

    PriorityQueue* pq = priority_queue_create(10, compareEmailByMessageNum);
    if (!pq) {
        perror("Failed to create priority queue\n");
        return NULL;
    }

    char* line = strtok(buffer, "\r\n");
    while (line != NULL) {
        char* seqNumStr = strstr_case_insensitive(line, "FETCH");
        if (seqNumStr != NULL) {
            int seqNum;
            if (sscanf(seqNumStr, "FETCH %d", &seqNum) != 1) {
                perror("Failed to parse sequence number\n");
                return NULL;
            }

            // 读主题行实际内容
            char* subject = strstr_case_insensitive(line, "Subject:");
            if (subject != NULL) {
                subject += strlen("Subject:");
                // Trim
                while (isspace(*subject)) {
                    subject++;
                }
                char* end = subject + strlen(subject) - 1;
                while (isspace(*end)) {
                    end--;
                }
                end[1] = '\0'; // Terminate

                Email* email = email_create(seqNum, subject);
                if (!email) {
                    perror("Failed to create email\n");
                    return NULL;
                }
                priority_queue_push(pq, email);
            } else {
                Email* email = email_create(seqNum, "<No subject>");
                if (!email) {
                    perror("Failed to create email\n");
                    return NULL;
                }
                priority_queue_push(pq, email);
            }
        }

        line = strtok(NULL, "\r\n");
    }

    return pq;
}

void list_emails(const ConnectionManager * cm, const char* folder) {
    PriorityQueue* pq = retrieve_ListSubjects(cm, folder);
    if (!pq) {
        perror("Trace back to retrieve_ListSubjects()\n");
        return;
    }

    // Check mailbox emptiness
    if (priority_queue_empty(pq)) {
        exit(0);
    }

    // Print subject lines sorted by message sequence number
    while (!priority_queue_empty(pq)) {
        Email* email = priority_queue_pop(pq);
        printf("%d: ", email->message_num);
        printf("%s\n", email->subject);
        email_destroy(email);
    }

    // Destroy the priority queue
    priority_queue_destroy(pq);
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
