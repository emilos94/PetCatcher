#ifndef OPENGL_UTILS_H
#define OPENGL_UTILS_H

#include <glad/glad.h>
#include <stdio.h>
#include "core/core.h"

#define GL_ASSERT_NO_ERRORS(blockName) assert(GLLogCall(blockName, __FILE__, __LINE__))

#define GL_CALL(x) GLClearErrors();\
	x;\
	assert(GLLogCall(#x, __FILE__, __LINE__));

static void GLClearErrors() {
	while (glGetError() != GL_NO_ERROR);
}

static boolean GLLogCall(const char* functionName, const char* fileName, int line) {
	boolean noErrors = true;
	GLenum error = glGetError();
	while (error != GL_NO_ERROR) {
		printf("[OpenGL ERROR]: (%d) %s %s %d\n", error, functionName, fileName, line);
		noErrors = false;
		GLenum error = glGetError();
	}

	return noErrors;
}

#endif
