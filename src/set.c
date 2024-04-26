#include "set.h"
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1000
#define MOD 1610612741LL
#define PRIME 805306457LL

typedef struct node {
    char key[256];
    struct node* next;
} Node;

struct set {
    Node* table[TABLE_SIZE];
    int size;
};

static unsigned int hash(const char* key) {
    unsigned long long hash = 0;
    while (*key) {
        hash = (hash * PRIME + *key++) % MOD;
    }
    return hash % TABLE_SIZE;
}

Set* set_create() {
    Set* set = (Set*)malloc(sizeof(Set));
    if (!set) return NULL;

    for (int i = 0; i < TABLE_SIZE; i++) {
        set->table[i] = NULL;
    }
    set->size = 0;

    return set;
}

void set_destroy(Set* set) {
    if (!set) return;

    for (int i = 0; i < TABLE_SIZE; i++) {
        Node* current = set->table[i];
        while (current) {
            Node* next = current->next;
            free(current);
            current = next;
        }
    }

    free(set);
}

int set_insert(Set* set, const char* key) {
    unsigned int index = hash(key);
    Node* current = set->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return 0; // Key already exists
        }
        current = current->next;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) return -1;

    strcpy(newNode->key, key);
    newNode->next = set->table[index];
    set->table[index] = newNode;
    set->size++;

    return 1; // Insertion successful
}

int set_contains(Set* set, const char* key) {
    unsigned int index = hash(key);
    Node* current = set->table[index];
    while (current) {
        if (strcmp(current->key, key) == 0) {
            return 1; // Key found
        }
        current = current->next;
    }
    return 0; // Key not found
}

int set_remove(Set* set, const char* key) {
    unsigned int index = hash(key);
    Node** currentPtr = &(set->table[index]);
    while (*currentPtr) {
        if (strcmp((*currentPtr)->key, key) == 0) {
            Node* temp = *currentPtr;
            *currentPtr = (*currentPtr)->next;
            free(temp);
            set->size--;
            return 1;
        }
        currentPtr = &((*currentPtr)->next);
    }
    return 0; // Key not found
}

int set_size(Set* set) {
    return set->size;
}
