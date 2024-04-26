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


boolean gamestate_init(GameState* game_state) {
    game_state->camera_position[0] = 0.0;
    game_state->camera_position[1] = 0.0;
    game_state->camera_position[2] = 0.0;

    game_state->camera_front[0] = 0.0;
    game_state->camera_front[1] = 0.0;
    game_state->camera_front[2] = -1.0;
    
    game_state->camera_up[0] = 0.0;
    game_state->camera_up[1] = 1.0;
    game_state->camera_up[2] = 0.0;

    game_state->camera_panning_speed = 10.0;
    game_state->camera_movement_speed = 10.0;
    game_state->camera_pan_sensitivity = 0.2;
    game_state->camera_yaw = -90.0;
    game_state->camera_pitch = 0.0;

    game_state->print_timer = 0.0;

    return true;
}

void camera_input(GameState* game_state, RenderState* render_state, f32 delta) {
    game_state->print_timer += delta;

    // camera panning (look around)
    { 
        game_state->camera_yaw += input_mouse_x_delta() * game_state->camera_pan_sensitivity;
        game_state->camera_pitch += input_mouse_y_delta() * game_state->camera_pan_sensitivity;
        if (game_state->print_timer >= 1.0) {
            printf("camera: yaw %.2f pitch %.2f\n", game_state->camera_yaw, game_state->camera_pitch);
        }

        if (game_state->camera_pitch > 89.0) {
            game_state->camera_pitch = 89.0;
        }
        if (game_state->camera_pitch < -89.0) {
            game_state->camera_pitch = -89.0;
        }

        vec3 look_direction = GLM_VEC3_ZERO_INIT;
        look_direction[0] = cos(glm_rad(game_state->camera_yaw)) * cos(glm_rad(game_state->camera_pitch));
        look_direction[1] = sin(glm_rad(game_state->camera_pitch));
        look_direction[2] = sin(glm_rad(game_state->camera_yaw)) * cos(glm_rad(game_state->camera_pitch));
        glm_normalize(look_direction);
        glm_vec3_copy(look_direction, game_state->camera_front);
        
        if (game_state->print_timer >= 1.0) {
            printf("%.2f %.2f %.2f\n", look_direction[0], look_direction[1], look_direction[2]);
        }

        vec3 camera_right = GLM_VEC3_ZERO_INIT;
        glm_vec3_cross(GLM_YUP, look_direction, camera_right);
        glm_normalize(camera_right);

        glm_vec3_cross(look_direction, camera_right, game_state->camera_up);
        glm_normalize(game_state->camera_up);
    }

    { // Camera movement   
        f32 movement_speed = game_state->camera_movement_speed * delta;
        if (input_keydown(GLFW_KEY_W)) {
            glm_vec3_muladds(
                game_state->camera_front,
                movement_speed,
                game_state->camera_position
            );
        }
        
        if (input_keydown(GLFW_KEY_S)) {
            glm_vec3_mulsubs(
                game_state->camera_front,
                movement_speed,
                game_state->camera_position
            );
        }    

        if (input_keydown(GLFW_KEY_A)) {
            vec3 delta = GLM_VEC3_ZERO_INIT;
            glm_vec3_cross(game_state->camera_front, GLM_YUP, delta);
            glm_vec3_normalize(delta);

            glm_vec3_mulsubs(
                delta,
                movement_speed,
                game_state->camera_position
            );
        }  

        if (input_keydown(GLFW_KEY_D)) {
            vec3 delta = GLM_VEC3_ZERO_INIT;
            glm_vec3_cross(game_state->camera_front, GLM_YUP, delta);
            glm_vec3_normalize(delta);

            glm_vec3_muladds(
                delta,
                movement_speed,
                game_state->camera_position
            );
        }
    }

    vec3 lookat = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(game_state->camera_position, game_state->camera_front, lookat);

    glm_mat4_identity(render_state->view_matrix);
    glm_lookat(game_state->camera_position, lookat, game_state->camera_up, render_state->view_matrix);

    shader_uniform_mat4(render_state->view_matrix_location, render_state->view_matrix);

    if (game_state->print_timer >= 1.0) {
        game_state->print_timer -= 1.0;
    }
}