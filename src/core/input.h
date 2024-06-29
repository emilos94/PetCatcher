#ifndef INPUT_H
#define INPUT_H

#include "core/core.h"

void input_init(void);
void input_endframe(void);
boolean input_keydown(int key);
boolean input_keyjustdown(int key);
boolean input_mouse_keydown(int key);
boolean input_mouse_keyjustdown(int key);
boolean input_mouse_keyrelease(int key);
f32 input_mouse_x(void);
f32 input_mouse_y(void);
f32 input_mouse_x_rel(void);
f32 input_mouse_y_rel(void);
f32 input_mouse_x_delta(void);
f32 input_mouse_y_delta(void);

#endif // INPUT_H