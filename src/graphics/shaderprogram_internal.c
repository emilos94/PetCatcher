#include "shaderprogram_internal.h"


boolean shader_load_source(char* path, u32 shader_type, GLuint* shader_handle) {
    String result;
    if (!file_loadstring(&result, path)) {
        return false;
    }
    
	GLuint shader = glCreateShader(shader_type);
	GL_CALL(glShaderSource(shader, 1, (const GLchar**)&result.chars, NULL));
	GL_CALL(glCompileShader(shader));

	int success;
	char info_log[512];
	GL_CALL(glGetShaderiv(shader, GL_COMPILE_STATUS, &success))
	if (!success) {
		GL_CALL(glGetShaderInfoLog(shader, 512, NULL, info_log));
		printf("ERROR: Shader compilation failed\n%s\n", info_log);
		return false;
	}

	*shader_handle = shader;
    free(result.chars);
	return true;
}

