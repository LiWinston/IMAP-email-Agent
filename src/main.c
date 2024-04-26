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




typedef struct {
    int num;
    char* str;
} Item;

// 比较函数，按照字符串长度递减顺序排序
int compareStringLenDesc(const void* a, const void* b) {
    const Item* itemA = (const Item*)a;
    const Item* itemB = (const Item*)b;
    return strlen(itemB->str) - strlen(itemA->str);
}

// 比较函数，按照数字大小递减顺序排序
int compareNumDesc(const void* a, const void* b) {
    const Item* itemA = (const Item*)a;
    const Item* itemB = (const Item*)b;
    return itemB->num - itemA->num;
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


    PriorityQueue* pq = priority_queue_create(10, compareStringLenDesc);

    // 添加一些元素到优先队列中
    Item* item1 = (Item*)malloc(sizeof(Item));
    item1->num = 5;
    item1->str = "Fiverrrrrrrrrrrrrrrr";
    priority_queue_push(pq, item1);

    Item* item2 = (Item*)malloc(sizeof(Item));
    item2->num = 10;
    item2->str = "TenTenn";
    priority_queue_push(pq, item2);

    Item* item3 = (Item*)malloc(sizeof(Item));
    item3->num = 3;
    item3->str = "ThreeThreeThree";
    priority_queue_push(pq, item3);

    Item* item5 = (Item*)malloc(sizeof(Item));
        item5->num = 7;
    item5 -> str = "Seven";
        priority_queue_push(pq, item5);

    while (!priority_queue_empty(pq)) {
        Item* item = (Item*)priority_queue_pop(pq);
        printf("num: %d, str: %s\n", item->num, item->str);
        free(item);
    }

    // 更改比较函数为按照数字大小递减顺序排序
    pq->cmp = compareNumDesc;

    return 0;
}
