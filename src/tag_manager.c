#include "tag_manager.h"
#include <stdlib.h>
#include <stdio.h>

#define TAG_START_INDEX 1000

TagManager* tag_manager_create() {
    TagManager* tm = (TagManager*)malloc(sizeof(TagManager));
    if (!tm) return NULL;

    tm->used_tags = set_create();
    if (!tm->used_tags) {
        free(tm);
        return NULL;
    }

    return tm;
}

void tag_manager_destroy(TagManager* tm) {
    if (!tm) return;

    set_destroy(tm->used_tags);
    free(tm);
}

char* generate_tag(TagManager* tm) {
    char* tag = (char*)malloc(sizeof(char) * (TAG_MAX_LEN + 1));
    if (!tag) return NULL;

    static int tag_index = TAG_START_INDEX;

    do {
        snprintf(tag, TAG_MAX_LEN + 1, "A%04d", tag_index++);
    } while (is_tag_used(tm, tag));

    set_insert(tm->used_tags, tag);

    return tag;
}

int is_tag_used(TagManager* tm, const char* tag) {
    return set_contains(tm->used_tags, tag);
}
// Path: src/main.c