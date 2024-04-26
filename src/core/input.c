#include "core/input_internal.h"
#include "core/input.h"

static int key_count = 1024;
static boolean keys_down[1024];
static boolean keys_just_down[1024];
static f32 mouse_x = 0.0, mouse_y = 0.0, mouse_last_x = 0.0, mouse_last_y = 0.0;
boolean first_mouse_movement = true, mouse_moved = false;

void input_init(void) {
    for (int i = 0; i < key_count; i++) {
        keys_down[i] = false;
        keys_just_down[i] = false;
    }
}

void input_keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!keys_down[key]) {
        keys_just_down[key] = action == GLFW_PRESS;
    }

    keys_down[key] = action == GLFW_PRESS;
}

void input_mousecallback(GLFWwindow* window, double x, double y) {
    if (first_mouse_movement) {
        mouse_last_x = x;
        mouse_last_y = y;
        first_mouse_movement = false;
    }
    else {
        mouse_last_x = mouse_x;
        mouse_last_y = mouse_y;
    }

    mouse_x = x;
    mouse_y = y;

    mouse_moved = true;
}

void input_endframe(void) {
    for (int i = 0; i < key_count; i++) {
        keys_just_down[i] = false;
    }

    mouse_moved = false;
}

boolean input_keydown(int key) {
    assert(key < key_count);

    return keys_down[key];
}

boolean input_keyjustdown(int key) {
    assert(key < key_count);

    return keys_just_down[key];
}

f32 input_mouse_x(void) {
    return mouse_x;
}

f32 input_mouse_y(void) {
    return mouse_y;
}

f32 input_mouse_x_delta(void) {
    return mouse_moved ? mouse_x - mouse_last_x : 0.0;
}

f32 input_mouse_y_delta(void) {
    return mouse_moved ? mouse_last_y - mouse_y : 0.0;
}
