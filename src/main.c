#include "connection_manager.h"
#include "tag_manager.h"

#include <assert.h>
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
void test_strstr_case_insensitive() {
    const char *haystack = "Hello, World!";
    const char *needle1 = "wor";
    const char *needle2 = "WO";
    const char *needle3 = "abc";

    // 测试用例1: 匹配情况
    char *result1 = strstr_case_insensitive(haystack, needle1);
    assert(result1 != NULL);
    printf("'%s' 匹配 '%s'，匹配结果：%s\n", haystack, needle1, result1);

    // 测试用例2: 大小写不同，但匹配情况
    char *result2 = strstr_case_insensitive(haystack, needle2);
    assert(result2 != NULL);
    printf("'%s' 匹配 '%s'，匹配结果：%s\n", haystack, needle2, result2);

    // 测试用例3: 不匹配情况
    char *result3 = strstr_case_insensitive(haystack, needle3);
    assert(result3 == NULL);
    printf("'%s' 不匹配 '%s'\n", haystack, needle3);
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

    TagManager *tm = tag_manager_create();
	//test connection

    test_strstr_case_insensitive();
    tag_manager_destroy(tm);
    return 0;
}

