#ifndef WINDOW_H
#define WINDOW_H

#include "stdint.h"

#include "core/core.h"
#include "graphics/opengl_utils.h"
#include "core/input_internal.h"
#include "GLFW/glfw3.h"

boolean window_init(u32 width, u32 height, char* title);
boolean window_should_exit(void);
boolean window_destroy(void);
void window_swapandpoll(void);
void window_enable_cursor(boolean enabled);

f32 window_width(void);
f32 window_height(void);

void input_init(void);
void input_endframe(void);
boolean input_keydown(int key);
boolean input_keyjustdown(int key);
f32 input_mouse_x(void);
f32 input_mouse_y(void);
f32 input_mouse_x_rel(void);
f32 input_mouse_y_rel(void);
f32 input_mouse_x_delta(void);
f32 input_mouse_y_delta(void);

#endif