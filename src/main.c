#include "connection_manager.h"
#include "tag_manager.h"

#include <assert.h>
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

    test_strstr_case_insensitive();
    // 添加一些元素到优先队列中
    Item* item1 = (Item*)malloc(sizeof(Item));
    item1->num = 5;
    item1->str = "Five";
    priority_queue_push(pq, item1);

    // 添加一些元素到优先队列中
    Item* item11 = (Item*)malloc(sizeof(Item));
    item11->num = 5;
    item11->str = "Five";
    priority_queue_push(pq, item11);

    Item* item2 = (Item*)malloc(sizeof(Item));
    item2->num = 10;
    item2->str = "TenTen";
    priority_queue_push(pq, item2);

    Item* item3 = (Item*)malloc(sizeof(Item));
    item3->num = 3;
    item3->str = "ThreeThreeThree";
    priority_queue_push(pq, item3);

    // 更改比较函数为按照数字大小递减顺序排序
    pq->cmp = compareNumDesc;

    // 添加一个数字更大的元素到优先队列中
    Item* item4 = (Item*)malloc(sizeof(Item));
    item4->num = 20;
    item4->str = "Twenty";
    priority_queue_push(pq, item4);

    // 从优先队列中弹出并打印元素，直到队列为空
    while (!priority_queue_empty(pq)) {
        Item* item = (Item*)priority_queue_pop(pq);
        printf("num: %d, str: %s\n", item->num, item->str);
        free(item);
    }

    // 销毁优先队列
    priority_queue_destroy(pq);


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

