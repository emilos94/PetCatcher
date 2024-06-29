#include "core/input.h"
#include "core/input_internal.h"

// internal input state
static int key_count = 1024;
static int mouse_key_count = 32;
static boolean keys_down[1024];
static boolean keys_just_down[1024];
static boolean mouse_keys_down[32];
static boolean mouse_keys_just_down[32];
static boolean mouse_keys_released[32];
static f32 mouse_x = 0.0, mouse_y = 0.0, mouse_last_x = 0.0, mouse_last_y = 0.0;
boolean first_mouse_movement = true, mouse_moved = false;

// forward declarations of window functions needed
f32 window_width(void);
f32 window_height(void);

void input_init(void) {
    for (int i = 0; i < key_count; i++) {
        keys_down[i] = false;
        keys_just_down[i] = false;
    }
    for (int i = 0; i < mouse_key_count; i++) {
        mouse_keys_down[i] = false;
        mouse_keys_just_down[i] = false;
        mouse_keys_released[i] = false;
    }
}

void input_keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (!keys_down[key]) {
        keys_just_down[key] = action == GLFW_PRESS;
    }

    switch (action)
    {
    case GLFW_PRESS:
        keys_down[key] = true;
        break;
    case GLFW_RELEASE:
        keys_down[key] = false;
        break;
    default:
        break;
    }
}

void input_mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    switch (action)
    {
    case GLFW_PRESS:
        if (!mouse_keys_down[button]) {
            mouse_keys_just_down[button] = true;
        }
        mouse_keys_down[button] = true;
        break;
    case GLFW_RELEASE:
        mouse_keys_down[button] = false;
        mouse_keys_released[button] = true;
        break;
    default:
        break;
    }
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
    for (int i = 0; i < mouse_key_count; i++) {
        mouse_keys_just_down[i] = false;
        mouse_keys_released[i] = false;
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
    return window_height() - mouse_y;
}

f32 input_mouse_x_rel(void) {
    return mouse_x / window_width();
}

f32 input_mouse_y_rel(void) {
    return 1.0 - (mouse_y / window_height());
}

f32 input_mouse_x_delta(void) {
    return mouse_moved ? mouse_x - mouse_last_x : 0.0;
}

f32 input_mouse_y_delta(void) {
    return mouse_moved ? mouse_last_y - mouse_y : 0.0;
}

boolean input_mouse_keydown(int key) {
    return mouse_keys_down[key];
}

boolean input_mouse_keyjustdown(int key) {
    return mouse_keys_just_down[key];
}

boolean input_mouse_keyrelease(int key) {
    return mouse_keys_released[key];
}
