#include "util/list.h"
#include <stdio.h>
#include <string.h>

List* list_init(size_t elem_size, size_t init_capacity, comparator cmp) {
    if (init_capacity == 0) init_capacity = 1;
    if (elem_size == 0) return NULL;

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
    list->cmp = cmp;

    return list;
}

int list_append(List* list, const void* elem) {
    if (list->size >= list->capacity) {
        size_t old_capacity = list->capacity;
        list->capacity *= 2;
        void* resize_ptr = realloc(list->ptr, list->capacity * list->elem_size);

        if (!resize_ptr) {
            return 1; 
        }

        // Zero new memory
        memset(
            (char*)resize_ptr + old_capacity * list->elem_size,
            0,
            (list->capacity - old_capacity) * list->elem_size);
        list->ptr = resize_ptr;
    }
    void* target = (char*)list->ptr + (list->size++ * list->elem_size);
    memcpy(target, elem, list->elem_size);
    return 0;
}

void* list_get(const List* list, size_t index) {
    if (index >= list->size) return NULL;

    return (char*)list->ptr + (index * list->elem_size);
}

ssize_t list_contains(const List* list, const void* elem) {
    if (!list || !list->ptr || !elem) return -1;
    size_t i;

    for (i = 0; i < list->size; i++) {
        void* target = (char*)list->ptr + i * list->elem_size;
        if (list->cmp && list->cmp(target, elem) == 0) {
            return (ssize_t)i;
        }
        else if (!list->cmp && memcmp(target, elem, list->elem_size) == 0) {
            return (ssize_t)i;
        }
    }
    return -1;
}

void list_remove_at(List* list, size_t idx) {
    if (!list || idx >= list->size) return;

    char* base = (char*)list->ptr;
    size_t bytes_after = (list->size - idx - 1) * list->elem_size;
    if (bytes_after > 0) {
        memmove(base + idx * list->elem_size,
                base + (idx + 1) * list->elem_size,
                bytes_after);
    }
    list->size--;
}

int list_remove(List *list, const void *elem) {
    if (!list || !list->ptr || list->size == 0) return 0;
    ssize_t i = list_contains(list, elem);
    if (i == -1) return 0;
    void* current = (char*)list->ptr + i * list->elem_size;
    void* last = (char*)list->ptr + (list->size - 1) * list->elem_size;
    if ((size_t)i != list->size-1) {
        memcpy(current, last, list->elem_size);
    }
    list->size--;
    return 1;
}

void list_free(List* list, void (*free_fn)(void*)) {
    if (!list) {
        return;
    }

    if (free_fn) {
        for (size_t i = 0; i < list->size; i++) {
            void* elem = (char*)list->ptr + i * list->elem_size;
            if (list->elem_size == sizeof(void*)) {
                free_fn(*(void**)elem);
            } else {
                free_fn(elem);
            }
        }
    }
    free(list->ptr);
    free(list);
}

void list_merge(List* main, List* extra) {
    if (!main || !extra || !main->ptr || !extra->ptr) return;

    for (ssize_t i = (ssize_t)extra->size-1; i >= 0; i--) {
        void* elem = list_get(extra, i);
        if (list_contains(main, elem) == -1) {
            list_append(main, elem);
        }
    }
}
