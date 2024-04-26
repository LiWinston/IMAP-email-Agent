#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tag_manager.h"

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
    printf("Usage: fetchmail -u <username> -p <password> [-f <folder>] [-n <messageNum>] [-t] <command> <server_name>\n");
}

Arguments parse_arguments(int argc, char *argv[]) {
    Arguments args = {NULL, NULL, NULL, NULL, NULL, NULL, 0};

    if (argc < 7) {
        print_usage();
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-u") == 0) {
            args.username = argv[++i];
        } else if (strcmp(argv[i], "-p") == 0) {
            args.password = argv[++i];
        } else if (strcmp(argv[i], "-f") == 0) {
            args.folder = argv[++i];
        } else if (strcmp(argv[i], "-n") == 0) {
            args.messageNum = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0) {
            args.tls_flag = 1;
        } else {
            if (args.command == NULL) {
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

    TagManager* tm = tag_manager_create();
    printf("get a unused tag: %s\n", generate_tag(tm));

    Set *set = set_create();

    // Insert some elements
    set_insert(set, "apple");
    set_insert(set, "banana");
    set_insert(set, "orange");

    // Check if elements are in the set
    printf("Is 'apple' in the set? %s\n", set_contains(set, "apple") ? "Yes" : "No");
    printf("Is 'grape' in the set? %s\n", set_contains(set, "grape") ? "Yes" : "No");

    // Remove an element
    printf("Removing 'apple' from the set...\n");
    set_remove(set, "apple");

    // Check if 'apple' is still in the set
    printf("Is 'apple' in the set after removal? %s\n", set_contains(set, "apple") ? "Yes" : "No");

    // Get the size of the set
    printf("Size of the set: %d\n", set_size(set));

    // Clean up
    set_destroy(set);

    return 0;
}
