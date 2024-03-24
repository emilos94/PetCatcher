#include "core/string.h"

void string_copy_n(String* destination, char* chars, u32 count) {
    destination->chars = malloc(count);
    destination->length = count;
    memcpy(destination->chars, chars, count);
}

boolean string_equals(String left, String right) {
    if (left.length != right.length) {
        return false;
    }

    for(u32 i = 0; i < left.length; i++) {
        if (left.chars[i] != right.chars[i]) {
            return false;
        }
    }

    return true;
}
