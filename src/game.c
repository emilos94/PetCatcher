#include "game.h"

boolean renderstate_init(RenderState* render_state) {
    if (!shader_initialise(&render_state->shader, "shaders/vert.txt", "shaders/frag.txt")) {
        printf("[ERROR] failed to initialise shader");
        return false;
    }

    if (!renderpipe_init(&render_state->render_pipe, 1000)) {
        printf("[ERROR] failed to initialise render pipe");
        return false;
    }
    
    { // view and projection matrix
        glm_mat4_identity(render_state->projection_matrix);
        f32 aspect = window_width() / window_height();
        glm_perspective(glm_rad(70), aspect, 0.1, 100.0, render_state->projection_matrix);

        render_state->projection_matrix_location = shader_uniform_location(render_state->shader, "projection_matrix");
        shader_uniform_mat4(render_state->projection_matrix_location, render_state->projection_matrix);

        // view matrix
        glm_mat4_identity(render_state->view_matrix);
        glm_lookat((vec3){0.0, 0.0, 10.0}, (vec3){0.0, 0.0, 0.0}, (vec3){0.0, 1.0, 0.0}, render_state->view_matrix);

        render_state->view_matrix_location = shader_uniform_location(render_state->shader, "view_matrix");
        shader_uniform_mat4(render_state->view_matrix_location, render_state->view_matrix);
    }

    render_state->color_location = shader_uniform_location(render_state->shader, "color");
    shader_uniform_vec3(render_state->color_location, (vec3){1.0, 0.5, 0.7});

    boolean use_color = true;
    render_state->use_color_location = shader_uniform_location(render_state->shader, "use_color");
    shader_uniform_bool(render_state->use_color_location, use_color);

    boolean use_texture = false;
    render_state->use_texture_location = shader_uniform_location(render_state->shader, "use_texture");
    shader_uniform_bool(render_state->use_texture_location, use_texture);

    render_state->light_color_location = shader_uniform_location(render_state->shader, "light_color");
    render_state->light_color[0] = 0.9;
    render_state->light_color[1] = 0.9;
    render_state->light_color[2] = 0.9;
    shader_uniform_vec3(render_state->light_color_location, render_state->light_color);

    render_state->ambient_strenth_location = shader_uniform_location(render_state->shader, "ambient_strength");
    render_state->ambient_strength = 0.4;
    shader_uniform_f32(render_state->ambient_strenth_location, render_state->ambient_strength);
    
    return true;
}