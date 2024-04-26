#ifndef TAG_MANAGER_H
#define TAG_MANAGER_H

#include "set.h"

#define TAG_MAX_LEN 10

typedef struct {
    Set* used_tags;
} TagManager;

TagManager* tag_manager_create();
void tag_manager_destroy(TagManager* tm);
char* generate_tag(TagManager* tm);
int is_tag_used(TagManager* tm, const char* tag);

#endif /* TAG_MANAGER_H */
// Path: src/tag_manager.c