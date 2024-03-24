#ifndef ARRAYLIST_H
#define ARRAYLIST_H

#include "core/core.h"
#include "string.h"

struct ArrayList {
    void* data;
    u32 element_capacity, element_count, element_byte_size;
};
typedef struct ArrayList ArrayList;

#define ARRAYLIST_FOREACH(list, type, name) \
    int index = 0;\
    type* name = 0;\
    for(index = 0, name = list->data; index < list->element_count; index++, index < list->element_count ? name = list->data + index * list->element_byte_size : 0)

void arraylist_initialise(ArrayList* list, u32 capacity, u32 element_byte_size);
void* arraylist_push(ArrayList* list);
void arraylist_remove(ArrayList* list, u32 index);
void arraylist_free(ArrayList* list);


#endif // ARRAYLIST_H