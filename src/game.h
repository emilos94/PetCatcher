#ifndef GAME_H
#define GAME_H

#include "graphics/render_pipe.h"
#include "graphics/window.h"
#include "core/input.h"

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
};
typedef struct RenderState RenderState;

struct Player {
    vec3 position;

    f32 jump_power, movement_speed, up_velocity;
    boolean in_air;
};
typedef struct Player Player;

enum EntityTag {
    EntityTag_Player,
    EntityTag_Fruit,
    EntityTag_Other
};
typedef enum EntityTag EntityTag;

enum EntityFlags {
    EntityFlag_Render = 1 << 0,
    EntityFlag_UseColor = 1 << 1,
    EntityFlag_UseTexture = 1 << 2,
    EntityFlag_Collider = 1 << 3,
    EntityFlag_RigidBody = 1 << 4
};
typedef u32 EntityFlags;

enum FruitType {
    FruitType_Apple
};
typedef enum FruitType FruitType;

struct Boundingbox {
    vec3 center;
    vec3 half_size;
};
typedef struct Boundingbox Boundingbox;

struct Entity {
    vec3 position, rotation, scale;
    EntityTag tag;
    EntityFlags flags;
    u32 list_index;
    boolean queue_delete;

    Mesh* mesh;
    u32 mesh_count;

    vec3 color;

    // player data
    f32 jump_power, movement_speed;
    
    // rigid body
    boolean in_air; 
    f32 y_velocity, mass;

    // collider
    Boundingbox bounding_box;

    // fruit data
    u32 points;
};
typedef struct Entity Entity;

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

    // assets
    ColladaData map_data;
    ColladaData apple_data;

    Boundingbox ground_box;

    // fruit spawning
    f32 fruit_spawn_timer, fruit_spawn_interval;

    // misc
    f32 print_timer;
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