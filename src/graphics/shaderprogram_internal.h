#ifndef SHADERPROGRAM_INTERNAL_H
#define SHADERPROGRAM_INTERNAL_H

#include "core/core.h"
#include "core/file_loader.h"
#include "core/string.h"
#include "graphics/opengl_utils.h"

boolean shader_load_source(char* path, u32 shader_type, GLuint* shader_handle);

#endif