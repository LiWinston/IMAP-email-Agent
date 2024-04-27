#include "PriorityQueue.h"
#include "connection_manager.h"
#include "tag_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#define DEBUG

typedef struct {
    char *username;
    char *password;
    char *folder;
    char *messageNum;
    char *command;
    char *server_name;
    int tls_flag;
} Arguments;

void print_usage() {
    fprintf(stderr,"Usage: fetchmail -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t] <command> <server_name>\n");
}

Arguments parse_arguments(int argc, char *argv[]) {
    Arguments args = {NULL, NULL, NULL, NULL, NULL, NULL, 0};

    if (argc < 7) {
        print_usage();
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (strcasecmp(argv[i], "-u") == 0) {
            args.username = argv[++i];
        } else if (strcasecmp(argv[i], "-p") == 0) {
            args.password = argv[++i];
        } else if (strcasecmp(argv[i], "-f") == 0) {
            args.folder = argv[++i];
        } else if (strcasecmp(argv[i], "-n") == 0) {
            args.messageNum = argv[++i];
        } else if (strcasecmp(argv[i], "-t") == 0) {
            args.tls_flag = 1;
        } else {
            if (args.command == NULL) {
                if (strcasecmp(argv[i], "retrieve") != 0 &&
                    strcasecmp(argv[i], "parse") != 0 &&
                    strcasecmp(argv[i], "mime") != 0 &&
                    strcasecmp(argv[i], "list") != 0) {
                    fprintf(stderr,"Invalid command: %s\n", argv[i]);
                    print_usage();
                    exit(1);
                }
                args.command = argv[i];
            } else {
                args.server_name = argv[i];
            }
        }
    }

    if (args.username == NULL || args.password == NULL || args.command == NULL || args.server_name == NULL) {
        print_usage();
        exit(1);
    }

    return args;
}

int main(int argc, char *argv[]) {
    Arguments args = parse_arguments(argc, argv);

    // printf("Username: %s\n", args.username);
    // printf("Password: %s\n", args.password);
    // printf("Folder: %s\n", args.folder != NULL ? args.folder : "(not specified)");
    // printf("MessageNum: %s\n", args.messageNum != NULL ? args.messageNum : "(not specified)");
    // printf("Command: %s\n", args.command);
    // printf("Server Name: %s\n", args.server_name);
    // printf("TLS flag: %d\n", args.tls_flag);

    // Connect to the IMAP server
    ConnectionManager* cm = connection_manager_create();
    if (!cm) {
        perror("Failed to create connection manager\n");
        return 1;
    }

    // Connect to the server
    if (connect_to_server(cm, args.server_name, args.tls_flag ? 993 : 143) != 0) {
        perror("Failed to connect to server\n");
        connection_manager_destroy(cm);
        return 1;
    }
#ifdef DEBUG
    printf("Connected to server,\n fd = %i\n", cm->socket_fd);
#endif

    // Login to the server
    if (login(cm, args.username, args.password) != 0) {
        perror("Login failed\n");
        connection_manager_destroy(cm);
        return 1;
    }

    select_folder(cm, args.folder);
    // Issue the specified command
    if (strcasecmp(args.command, "retrieve") == 0) {
        // Implement retrieve command
        if (args.messageNum == NULL) {
            fprintf(stderr,"Message number not specified, fetching the last added one\n");
            //TODO: retrieve the last added message in connection_manager
        }
        retrieve_ShowMessage(cm, args.messageNum);
    } else if (strcasecmp(args.command, "parse") == 0) {
        // Implement parse command
    } else if (strcasecmp(args.command, "mime") == 0) {
        // Implement mime command
    } else if (strcasecmp(args.command, "list") == 0) {
        // Implement list command
    } else {
        fprintf(stderr,"Invalid command: %s\n", args.command);
        print_usage();
        connection_manager_destroy(cm);
        return 1;
    }

    // Close the connection and clean up
    close_connection(cm);
    connection_manager_destroy(cm);

    printf("over!\n");
    return 0;
}