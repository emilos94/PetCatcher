#include "core/mystring.h"


void string_copy(String* destination, String* source) {
    string_copy_n(destination, source->chars, source->length);
}

void string_copy_n(String* destination, char* chars, u32 count) {
    destination->chars = malloc(count+1);
    destination->length = count;
    memcpy(destination->chars, chars, count);
    destination->chars[count] = '\0';
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

boolean string_equals_lit(String left, char* chars) {
    if (!chars) {
        return false;
    }

    for(int i = 0; i < left.length; i++) {
        if (chars[i] == '\0' || left.chars[i] != chars[i]) {
            return false;
        }
    }

    return chars[left.length] == '\0';
}

boolean string_endswith_lit(String left, char* chars) {
    if (!chars) {
        return false;
    }

    u32 chars_length = strlen(chars);
    if (chars_length > left.length) {
        return false;
    }
    
    int chars_marker = 0;
    for (int i = left.length - chars_length; i < left.length; i++) {
        if (left.chars[i] != chars[chars_marker++]) {
            return false;
        }
    }

    return true;
}

void string_tofloatarray(String str, float** out, char delimiter, u32 count) {
    assert(out != NULL);

    f32* result = malloc(count * sizeof(f32));
    int marker = 0;
    int float_counter = 0;
    char buffer[64];
    for (int i = 0; i < str.length; i++) {
        buffer[i - marker] = str.chars[i];
        if (str.chars[i] == delimiter) {
            buffer[i - marker] = '\0';
            f32 value = (f32)atof(&buffer[0]);
            result[float_counter++] = value;
            marker = i+1;
        }					
    }
    
    buffer[str.length - marker] = '\0';
    result[float_counter++] = (f32)atof(&buffer[0]);
    *out = result;
}

void string_tointarray(String str, int** out, char delimiter, u32 count) {
    int* result = malloc(count * sizeof(int));
    int marker = 0;
    int int_counter = 0;
    char buffer[64];
    for (int i = 0; i < str.length; i++) {
        buffer[i - marker] = str.chars[i];
        if (str.chars[i] == delimiter) {
            buffer[i - marker] = '\0';
            result[int_counter++] = atoi(&buffer[0]);
            marker = i+1;
        }					
    }
    
    buffer[str.length - marker] = '\0';
    result[int_counter++] = atoi(&buffer[0]);
    
    *out = result;
}
