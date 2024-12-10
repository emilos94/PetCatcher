#ifndef GAME_H
#define GAME_H

#include "graphics/render_pipe.h"
#include "graphics/window.h"
#include "graphics/framebuffer.h"
#include "graphics/camera.h"
#include "ui/ui.h"
#include "entity.h"
#include "graphics/shadows/shadow_render.h"

struct RenderState {
    RenderPipe render_pipe;

    // shader
    ShaderProgram shader;
    int view_matrix_location, projection_matrix_location, use_colors_location, use_textures_location,
        colors_location, ambient_strenth_location, light_color_location, light_position_location,
        model_matrices_location, light_view_matrix_location, light_projection_matrix_location;

    mat4 view_matrix, projection_matrix;

    // light
    f32 ambient_strength;
    vec3 light_color, light_position;

    // shadows
    ShadowRender shadow_render;
};
typedef struct RenderState RenderState;

struct GameState {
    Camera camera;

    // entities
    ArrayList entities;
    Entity* player;

    f32 player_move_timer, player_move_time_total;
    u32 player_lane, player_lane_prev;
    boolean player_moving;

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
    vec3 spawn_point_left;
    vec3 spawn_point_middle;
    vec3 spawn_point_right;
    vec3 delete_point;

    // spawning
    f32 last_spawn_time, spawn_timer, spawn_delay_per_entity, spawn_time_delay;
    u32 spawn_count, spawn_max;
    int current_spawn_lane;
    boolean spawning;
    EntityTag entity_to_spawn;

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

void player_movement(GameState* game_state, RenderState* render_state, f32 delta);

#endif // GAME_H