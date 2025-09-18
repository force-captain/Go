#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\033[31m"
#define NORMAL "\033[0m"

List* init_list(size_t elem_size, size_t init_capacity) {
	if (init_capacity == 0) init_capacity = 1;
	
	List* list = malloc(sizeof(List));
	if (!list) return NULL;

	list->ptr = calloc(init_capacity, elem_size);
	if (!list->ptr) {
		free(list);
		return NULL;
	}

	list->elem_size = elem_size;
	list->capacity = init_capacity;
	list->size = 0;

	return list;
}

int list_append(List* list, const void* elem) {
	if (list->size >= list->capacity) {
		list->capacity *= 2;
		void* resize_ptr = realloc(list->ptr, list->capacity * list->elem_size);

		if (!resize_ptr) {
			printf("[%sERROR%s]: Cannot resize list!", RED, NORMAL);
			return 1; 
		}
		list->ptr = resize_ptr;
	}
	void* target = (char*)list->ptr + (list->size++ * list->elem_size);
	memcpy(target, elem, list->elem_size);
	return 0;
}

// Caller must free
void* list_get(const List* list, size_t index) {
	if (index >= list->size) return NULL;

	void* target = malloc(list->elem_size);
	if (!target) return NULL;

	void* src = (char*)list->ptr + (index * list->elem_size);
	memcpy(target, src, list->elem_size);

	return target;
}

int list_contains(const List* list, const void* elem) {
	size_t i;

	for (i = 0; i < list->size; i++) {
		void* target = (char*)list->ptr + i * list->elem_size;
		if (memcmp(target, elem, list->elem_size) == 0) return 1;
	}
	return 0;
}

int list_contains_cmp(const List* list, const void* elem, comparator cmp) {
	size_t i;

	for (i = 0; i < list->size; i++) {
		void* target = (char*)list->ptr + i * list->elem_size;
		if (cmp(target, elem) == 0) {
			return 1;
		}
	}
	return 0;
}

void list_free(List* list) {
	free(list->ptr);
	free(list);
}
