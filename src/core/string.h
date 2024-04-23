#ifndef STRING_H
#define STRING_H

#include "stdint.h"
#include "core/core.h"
#include "string.h"

struct String {
    char* chars;
    u32 length;
};
typedef struct String String;

void string_copy_n(String* destination, char* chars, u32 count);
boolean string_equals(String left, String right);
boolean string_equals_lit(String left, char* chars);
void string_tofloatarray(String str, float** out, char delimiter, u32 count);
void string_tointarray(String str, int** out, char delimiter, u32 count);

#endif