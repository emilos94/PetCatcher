#include "core/input_internal.h"
#include "core/input.h"

static int key_count = 1024;
static boolean keys_down[1024];

void input_initialize() {
    for (int i = 0; i < key_count; i++) {
        keys_down[i] = false;
    }
}

void input_keycallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    keys_down[key] = action == GLFW_PRESS;
}

boolean input_keydown(int key) {
    assert(key < key_count);

    return keys_down[key];
}
