#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "core/core.h"
#include "graphics/opengl_utils.h"

#include "cglm/cglm.h"

typedef GLuint ShaderProgram;

boolean shader_initialise(ShaderProgram* shader_program, char* vertex_path, char* fragment_path);
void shader_bind(ShaderProgram shader_program);
void shader_unbind(void);
void shader_destroy(ShaderProgram shader_program);

int shader_uniform_location(ShaderProgram shader_program, char* name);
void shader_uniform_mat4(int location, mat4 mat);
void shader_uniform_vec3(int location, vec3 vec);
void shader_uniform_f32(int location, f32 value);
void shader_uniform_vec3n(int location, f32* values, u32 count);
void shader_uniform_mat4n(int location, f32* values, u32 count);
void shader_uniform_bool(int location, boolean value);
void shader_uniform_booln(int location, boolean* values, u32 count);

#endif