#include "tag_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PriorityQueue.h"
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

int intCompare(const void *a, const void *b) {
    int intA = *(int*)a;
    int intB = *(int*)b;
    if (intA == intB) return 0;
    return (intA < intB) ? -1 : 1;
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



    //
    // 初始化一个整数类型的优先队列
    PriorityQueue pq;
    initPriorityQueue(&pq);

    // 添加一些元素
    int values[] = {10, 5, 15, 3, 7};
    for (int i = 0; i < sizeof(values) / sizeof(values[0]); ++i) {
        pushPriorityQueue(&pq, &values[i]);
    }

    // 打印当前队列中的元素
    printf("Current elements in the priority queue:\n");
    for (size_t i = 0; i < pq.size; ++i) {
        printf("%d ", *((int*)pq.array[i]));
    }
    printf("\n");

    // 将队列排序
    sortPriorityQueue(&pq, intCompare);

    // 打印排序后的队列元素
    printf("Elements in the priority queue after sorting:\n");
    for (size_t i = 0; i < pq.size; ++i) {
        printf("%d ", *((int*)pq.array[i]));
    }
    printf("\n");

    // // 弹出并打印队列中的元素，直到队列为空
    // printf("Elements popped from the priority queue:\n");
    // while (pq.size > 0) {
    //     int* poppedValue = (int*)popPriorityQueue(&pq);
    //     printf("%d ", *poppedValue);
    // }
    // printf("\n");

    // 释放优先队列的内存空间
    freePriorityQueue(&pq);


    return 0;
}
