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


f32 window_width(void) {
    return window.width;
}

f32 window_height(void) {
    return window.height;
}