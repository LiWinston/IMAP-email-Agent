#include "tag_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ctype.h>

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

    printf("Username: %s\n", args.username);
    printf("Password: %s\n", args.password);
    printf("Folder: %s\n", args.folder != NULL ? args.folder : "(not specified)");
    printf("MessageNum: %s\n", args.messageNum != NULL ? args.messageNum : "(not specified)");
    printf("Command: %s\n", args.command);
    printf("Server Name: %s\n", args.server_name);
    printf("TLS flag: %d\n", args.tls_flag);


    return 0;
}
