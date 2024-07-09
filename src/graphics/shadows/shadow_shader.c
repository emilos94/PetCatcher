#include "shadow_shader.h"

boolean shadowshader_init(ShadowShader* shadowshader) {
    if (!shader_initialise(&shadowshader->handle, "shaders/shadow_vert.txt", "shaders/shadow_frag.txt")) {
        printf("[ERROR] Failed to initialise shadow shader!\n");
        return false;
    }

    shadowshader->model_matrices_location = shader_uniform_location(shadowshader->handle, "model_matrices");
    shadowshader->projection_matrix_location = shader_uniform_location(shadowshader->handle, "projection_matrix");
    shadowshader->view_matrix_location = shader_uniform_location(shadowshader->handle, "view_matrix");

    shader_unbind();

    return true;
}

void shadowshader_destroy(ShadowShader* shadowshader) {
    shader_destroy(shadowshader->handle);
}
