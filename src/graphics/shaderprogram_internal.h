#ifndef SHADERPROGRAM_INTERNAL_H
#define SHADERPROGRAM_INTERNAL_H

#include "core/mystring.h"
#include "core/file_loader.h"
#include "graphics/window.h"

boolean shader_load_source(char* path, u32 shader_type, GLuint* shader_handle);

#endif