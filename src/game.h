#ifndef GAME_H
#define GAME_H

#include "graphics/render_pipe.h"
#include "graphics/window.h"
#include "graphics/framebuffer.h"
#include "ui/ui.h"
#include "entity.h"
#include "graphics/shadows/shadow_render.h"

struct RenderState {
    RenderPipe render_pipe;

    // shader
    ShaderProgram shader;
    int view_matrix_location, projection_matrix_location, use_colors_location, use_textures_location,
        colors_location, ambient_strenth_location, light_color_location, light_position_location,
        model_matrices_location;

    mat4 view_matrix, projection_matrix;

    // light
    f32 ambient_strength;
    vec3 light_color, light_position;

    // shadows
    ShadowRender shadow_render;
};
typedef struct RenderState RenderState;

struct GameState {
    // camera
    vec3 camera_front, camera_up;
    f32 camera_panning_speed, camera_pan_sensitivity, camera_yaw, camera_pitch;

    // entities
    ArrayList entities;
    Entity* player;

    // world
    f32 ground_height, gravity;

    // game
    u32 score;
    char score_label_buffer[50];

    // assets
    ColladaData map_data;
    ColladaData apple_data;
    ColladaData boulder_data;

    Boundingbox ground_box;

    // fruit spawning
    f32 fruit_spawn_timer, fruit_spawn_interval;

    // obstacle spawning
    f32 obstacle_spawn_timer, obstacle_spawn_interval;

    // misc
    f32 second_timer;
    u32 fps;
    u64 update_count;

    // flags
    boolean paused;
    boolean quiting;
    boolean game_over;
};
typedef struct GameState GameState;

boolean renderstate_init(RenderState* render_state);
boolean gamestate_init(GameState* game_state);

void game_input(GameState* game_state);
void game_update(GameState* game_state, f32 delta);
void game_render(GameState* game_state, RenderState* render_state, f32 delta);
boolean game_loadmap(GameState* game_state, char* path);


boolean entity_render(RenderState* render_state, Entity* entity);
boolean render_flush(RenderState* render_state, ShaderProgram shader_program);

void camera_input(GameState* game_state, RenderState* render_state, f32 delta);

#endif // GAME_H