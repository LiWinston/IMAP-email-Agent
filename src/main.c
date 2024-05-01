#include "emailClient.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_USAGE                                                            \
    fprintf(stderr,                                                            \
            "Usage: fetchmail -u <username> -p <password> [-f <folder>] [-n "  \
            "<messageNum>] [-t] <command> <server_name>\n");

int main(int argc, char *argv[]) {
    arg = (Arguments){NULL, NULL, NULL, 0, NULL, NULL, false};

    if (argc < 7) {
        PRINT_USAGE
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0 && i < argc - 1) {
            arg.username = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0 && i < argc - 1) {
            arg.password = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0 && i < argc - 1) {
            arg.folder = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0 && i < argc - 1) {
            arg.messageNum = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            arg.tls_flag = true;
        } else if (arg.command == NULL) {
            arg.command = argv[i];
        } else if (arg.server_name == NULL) {
            arg.server_name = argv[i];
        } else {
            PRINT_USAGE
        }
    }

    if (arg.username == NULL || arg.password == NULL || arg.command == NULL ||
        arg.server_name == NULL) {
        PRINT_USAGE
        return 1;
    }

    if (strcmp(arg.command, "retrieve") && strcmp(arg.command, "parse") &&
        strcmp(arg.command, "mime") && strcmp(arg.command, "list")) {
        fprintf(stderr, "Command \"%s\" is unknown\n", arg.command);
        return 1;
    }

    int err = 0;
    err = c_run();

    c_free();

    return err;
}
