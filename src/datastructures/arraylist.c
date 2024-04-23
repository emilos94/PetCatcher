#include "datastructures/arraylist.h"

void arraylist_initialise(ArrayList* list, u32 capacity, u32 element_byte_size) {
    list->data = malloc(capacity * element_byte_size);
    list->element_byte_size = element_byte_size;
    list->element_capacity = capacity;
    list->element_count = 0;
}

void* arraylist_push(ArrayList* list) {
    if (list->element_count >= list->element_capacity) {
        // resize
        // todo: might mess up pointer locations if referenced outside of list
        list->data = realloc(list->data, list->element_byte_size * list->element_capacity * 2);
        list->element_capacity *= 2;
    }

    void* ptr = list->data + list->element_count * list->element_byte_size;
    list->element_count++;
    return ptr;
}

void* arraylist_at(ArrayList* list, u32 index) {
    assert(index < list->element_count);

    return list->data + index * list->element_byte_size;
}

void arraylist_remove(ArrayList* list, u32 index) {
    memcpy(list->data + list->element_byte_size * index, list->data + list->element_count * list->element_byte_size, list->element_byte_size);
    list->element_count--;
}

void arraylist_free(ArrayList* list) {
    free(list->data);
}
