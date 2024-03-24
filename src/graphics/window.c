#include "graphics/window.h"

boolean window_init(Window* window, u32 width, u32 height) {
    //glfwSetErrorCallback(error_callback);
    if (!glfwInit()) {
        return false;
    }
    
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    window->handle = glfwCreateWindow(width, height, "Simple example", NULL, NULL);
    if (!window->handle) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window->handle);
    gladLoadGL();

    glfwSetKeyCallback(window->handle, input_keycallback);

    return true;
}

boolean window_should_exit(Window* window) {
    return glfwWindowShouldClose(window->handle);
}

boolean window_destroy(Window* window) {
    glfwDestroyWindow(window->handle);
    glfwTerminate();
    return true;
}


void window_swapandpoll(Window* window) {
    glfwSwapBuffers(window->handle);
    glfwPollEvents();
}
