#include "core/input_internal.h"
#include "core/input.h"

static int key_count = 1024;
static boolean keys_down[1024];
static boolean keys_just_down[1024];

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

void input_endframe(void) {
    for (int i = 0; i < key_count; i++) {
        keys_just_down[i] = false;
    }
}

boolean input_keydown(int key) {
    assert(key < key_count);

    return keys_down[key];
}

boolean input_keyjustdown(int key) {
    assert(key < key_count);

    return keys_just_down[key];
}
