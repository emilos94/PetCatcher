#include "graphics/window.h"

struct Window {
    GLFWwindow* handle;
    f32 width, height;
};
typedef struct Window Window;

static Window window;

void window_size_callback(GLFWwindow* window_handle, int width, int height);

boolean window_init(u32 width, u32 height, char* title) {
    //glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return false;
    }
    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window.handle = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window.handle) {
        glfwTerminate();
        return false;
    }

    window.width = (f32)width;
    window.height = (f32)height;

    glfwMakeContextCurrent(window.handle);
    gladLoadGL();

    glfwSetKeyCallback(window.handle, input_keycallback);
    glfwSetWindowSizeCallback(window.handle, window_size_callback);
    glfwSetCursorPosCallback(window.handle, input_mousecallback);

    glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  

    return true;
}

boolean window_should_exit() {
    return glfwWindowShouldClose(window.handle);
}

boolean window_destroy(void) {
    glfwDestroyWindow(window.handle);
    glfwTerminate();
    return true;
}

void window_swapandpoll(void) {
    glfwSwapBuffers(window.handle);
    glfwPollEvents();
}

void window_size_callback(GLFWwindow* window_handle, int width, int height) {
    window.width = width;
    window.height = height;
    glViewport(0, 0, width, height);
}

void window_enable_cursor(boolean enabled) {
    if (enabled) {
        glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
    }
    else {
        glfwSetInputMode(window.handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    }
}

f32 window_width(void) {
    return window.width;
}

f32 window_height(void) {
    return window.height;
}

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
