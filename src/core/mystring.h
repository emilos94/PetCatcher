#ifndef STRING_H
#define STRING_H

#include "stdint.h"
#include "core/core.h"
#include "string.h"
#include "core/generic_utils.h"

struct String {
    char* chars;
    u32 length;
};
typedef struct String String;

void string_copy(String* destination, String* source);
void string_copy_n(String* destination, char* chars, u32 count);
void string_copy_lit(String* destination, char* lit);
boolean string_equals(String left, String right);
boolean string_equals_lit(String left, char* chars);
boolean string_endswith_lit(String left, char* chars);
boolean string_startswith(String str, char* literal);
boolean string_chars_startswith(char* source, char* literal);
void string_tofloatarray(String str, float** out, char delimiter, u32 count);
void string_tointarray(String str, int** out, char delimiter, u32 count);
void string_lit_concat(String* destination, char* left, char* right);

#endif