#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "core/core.h"
#include "graphics/opengl_utils.h"

typedef GLuint ShaderProgram;

boolean shader_initialise(ShaderProgram* shader_program, char* vertex_path, char* fragment_path);
void shader_bind(ShaderProgram shader_program);
void shader_destroy(ShaderProgram shader_program);

#endif