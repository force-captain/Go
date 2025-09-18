#ifndef LIST_H
#define LIST_H

#include <stddef.h>
#include <stdint.h>

typedef struct List {
	void* ptr;
	size_t size;
	size_t capacity;
	size_t elem_size;
} List;

typedef int (*comparator)(const void* a, const void* b);

List* list_init(size_t elem_size, size_t init_capacity);
int list_append(List* list, const void* elem);
void* list_get(const List* list, size_t index);
int list_contains_cmp(const List* list, const void* elem, comparator cmp);
int list_contains(const List* list, const void* elem);
void list_free(List* list);

#endif
