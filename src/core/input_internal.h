#ifndef INPUT_INTERNAL_H
#define INPUT_INTERNAL_H

#include "GLFW/glfw3.h"

void input_keycallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void input_mousecallback(GLFWwindow* window, double x, double y);
void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

#endif // INPUT_INTERNAL_H