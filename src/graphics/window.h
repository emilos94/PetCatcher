#ifndef WINDOW_H
#define WINDOW_H

#include "stdint.h"

#include "core/core.h"
#include <glad/glad.h>
#include "core/input_internal.h"
#include "GLFW/glfw3.h"

boolean window_init(u32 width, u32 height, char* title);
boolean window_should_exit(void);
boolean window_destroy(void);
void window_swapandpoll(void);

f32 window_width(void);
f32 window_height(void);

#endif