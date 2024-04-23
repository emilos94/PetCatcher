#include "graphics/shaderprogram.h"
#include "graphics/shaderprogram_internal.h"


boolean shader_initialise(ShaderProgram* shader_program, char* vertex_path, char* fragment_path) {
    u32 vertex_handle;
    if (!shader_load_source(vertex_path, GL_VERTEX_SHADER, &vertex_handle)) {
        return false;
    }

    u32 fragment_handle;
    if (!shader_load_source(fragment_path, GL_FRAGMENT_SHADER, &fragment_handle)) {
        return false;
    }

    GLuint shader_handle = glCreateProgram();
    GL_CALL(glAttachShader(shader_handle, vertex_handle));
    GL_CALL(glAttachShader(shader_handle, fragment_handle));
    GL_CALL(glLinkProgram(shader_handle));

    int success;
	char infoLog[512];
	GL_CALL(glGetProgramiv(shader_handle, GL_LINK_STATUS, &success));
	if (!success) {
		GL_CALL(glGetProgramInfoLog(shader_handle, 512, NULL, infoLog));
		printf("ERROR: Program linking failed\n%s\n", infoLog);
		return false;
	}

    GL_CALL(glUseProgram(shader_handle));
	GL_CALL(glDeleteShader(vertex_handle));
	GL_CALL(glDeleteShader(fragment_handle));

    *shader_program = shader_handle;

    return true;
}


void shader_bind(ShaderProgram shader_program) {
    GL_CALL(glUseProgram(shader_program));
}

void shader_destroy(ShaderProgram shader_program) {
    GL_CALL(glDeleteProgram(shader_program));
}

int shader_uniform_location(ShaderProgram shader_program, char* name) {
    return glGetUniformLocation(shader_program, name);
}

void shader_uniform_mat4(int location, mat4 mat) {
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, mat[0]));
}

void shader_uniform_vec3(int location, vec3 vec) {
    GL_CALL(glUniform3fv(location, 1, vec));
}

void shader_uniform_f32(int location, f32 value) {
    GL_CALL(glUniform1f(location, value));
}

void shader_uniform_bool(int location, boolean value) {
    GL_CALL(glUniform1f(location, value ? 1.0 : 0.0));
}
