#ifndef GAME_H
#define GAME_H

#include "graphics/render_pipe.h"
#include "graphics/window.h"
#include "core/input.h"

struct RenderState {
    RenderPipe render_pipe;

    // shader
    ShaderProgram shader;
    int view_matrix_location, projection_matrix_location, use_color_location, use_texture_location,
        color_location, ambient_strenth_location, light_color_location, light_position_location;

    mat4 view_matrix, projection_matrix;

    boolean use_color, use_texture;

    // light
    f32 ambient_strength;
    vec3 light_color, light_position;
};
typedef struct RenderState RenderState;

struct GameState {
    // camera
    vec3 camera_position, camera_front, camera_up;
    f32 camera_panning_speed, camera_movement_speed, camera_pan_sensitivity, camera_yaw, camera_pitch;

    // jumping
    f32 jump_power, ground_height, up_velocity, gravity;
    boolean in_air;

    f32 print_timer;
};
typedef struct GameState GameState;

boolean renderstate_init(RenderState* render_state);
boolean gamestate_init(GameState* game_state);

void camera_input(GameState* game_state, RenderState* render_state, f32 delta);

#endif // GAME_H