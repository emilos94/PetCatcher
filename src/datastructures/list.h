#ifndef LIST_H
#define LIST_H

#include "core/core.h"

#define LIST_DEFINE(type)\
struct List_##type {\
    type* values;\
    int capacity, count;\
};\
typedef struct List_##type List_##type;\
bool type##_list_init(List_##type* list, int capacity) {\
    list->values = malloc(sizeof(type) * capacity);\
    if (!list->values) { return false; }\
    list->capacity = capacity;\
    list->count = 0;\
    return true;\
};\
\
void _##type##_list_resize(List_##type* list, int new_capacity) {\
    list->values = realloc(list->values, sizeof(type) * new_capacity);\
    list->capacity = new_capacity;\
}\
void type##_list_add(List_##type* list, type value) {\
    if (list->count == list->capacity) {\
        _##type##_list_resize(list, list->capacity * 2);\
    }\
    list->values[list->count++] = value;\
}\
type* type##_list_push(List_##type* list) {\
    if (list->count == list->capacity) {\
        _##type##_list_resize(list, list->capacity * 2);\
    }\
    type* ptr = list->values + list->count;\
    list->count++;\
    return ptr;\
}\
type* type##_list_pop(List_##type* list) {\
    if (list->count == 0) return NULL;\
    type* ptr = list->values + (list->count - 1);\
    list->count--;\
    if (list->count < list->capacity / 4) {\
        _##type##_list_resize(list, list->capacity / 2);\
    }\
    return ptr;\
}\


#endif // LIST_H