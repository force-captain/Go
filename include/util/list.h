#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef int (*comparator)(const void* a, const void* b);

typedef struct List {
	void* ptr;
    comparator cmp; // NULL = memcmp
	size_t size;
	size_t capacity;
	size_t elem_size;
} List;


List* list_init(size_t elem_size, size_t init_capacity, comparator cmp);
int list_append(List* list, const void* elem);
void* list_get(const List* list, size_t index);
ssize_t list_contains(const List* list, const void* elem);
void list_free(List* list, void (*free_fn)(void*));
int list_remove(List* list, const void* elem);

void list_merge(List* main, List* extra);

#endif
