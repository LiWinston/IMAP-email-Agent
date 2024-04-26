#ifndef SET_H
#define SET_H

typedef struct set Set;

Set* set_create();
void set_destroy(Set* set);
int set_insert(Set* set, const char* key);
int set_contains(Set* set, const char* key);
int set_remove(Set* set, const char* key);
int set_size(Set* set);

#endif /* SET_H */
