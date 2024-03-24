#ifndef WINDOW_H
#define WINDOW_H

#include "stdint.h"

#include "core/core.h"
#include <glad/glad.h>
#include "core/input_internal.h"
#include "GLFW/glfw3.h"

struct Window {
    GLFWwindow* handle;
    u32 width;
    u32 height;    
};
typedef struct Window Window;

boolean window_init(Window* window, u32 width, u32 height);
boolean window_should_exit(Window* window);
boolean window_destroy(Window* window);
void window_swapandpoll(Window* window);

#endif