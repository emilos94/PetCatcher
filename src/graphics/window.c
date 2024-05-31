#include "graphics/window.h"

static GLFWwindow* window_handle[1] = {NULL};
static f32 window_size[2] = {0.0, 0.0};
static boolean cursor_enabled[1] = { false };

void window_error_callback(int code, const char* error_message);
void window_size_callback(GLFWwindow* window_handle, int width, int height);

boolean window_init(u32 width, u32 height, char* title) {
    glfwSetErrorCallback(window_error_callback);
    if (!glfwInit()) {
        return false;
    }
    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_handle[0] = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window_handle[0]) {
        glfwTerminate();
        return false;
    }

    window_size[0] = (f32)width;
    window_size[1] = (f32)height;

    glfwMakeContextCurrent(window_handle[0]);
    gladLoadGL();

    glfwSetKeyCallback(window_handle[0], input_keycallback);
    glfwSetWindowSizeCallback(window_handle[0], window_size_callback);
    glfwSetCursorPosCallback(window_handle[0], input_mousecallback);
    glfwSetMouseButtonCallback(window_handle[0], input_mouse_button_callback);

    glfwSetInputMode(window_handle[0], GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    return true;
}

boolean window_should_exit(void) {
    return glfwWindowShouldClose(window_handle[0]);
}

boolean window_destroy(void) {
    glfwDestroyWindow(window_handle[0]);
    glfwTerminate();
    return true;
}

void window_swapandpoll(void) {
    glfwSwapBuffers(window_handle[0]);
    glfwPollEvents();
}

void window_size_callback(GLFWwindow* window_handle, int width, int height) {
    window_size[0] = (f32)width;
    window_size[1] = (f32)height;
    glViewport(0, 0, width, height);
}

void window_enable_cursor(boolean enabled) {
    if (enabled == cursor_enabled[0]) {
        return;
    }

    if (enabled) {
        glfwSetInputMode(window_handle[0], GLFW_CURSOR, GLFW_CURSOR_NORMAL);  
    }
    else {
        glfwSetInputMode(window_handle[0], GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    }
    
    cursor_enabled[0] = enabled;
}

f32 window_width(void) {
    return window_size[0];
}

f32 window_height(void) {
    return window_size[1];
}

void window_error_callback(int code, const char* error_message) {
    printf("[ERROR GLFW] %d %s\n", code, error_message);
}
