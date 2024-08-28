#ifndef ENTITY_H
#define ENTITY_H

#include "core/core.h"
#include "cglm/cglm.h"
#include "cglm/struct.h"
#include "core/file_loader.h"

enum EntityTag {
    EntityTag_Player,
    EntityTag_Fruit,
    EntityTag_Obstacle,
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

enum ObstacleType {
    ObstacleType_Boulder
};
typedef enum ObstacleType ObstacleType;

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
    f32 jump_power, movement_speed, hunger;

    // rigid body
    boolean in_air; 
    f32 y_velocity, mass;

    // collider
    Boundingbox bounding_box;

    // fruit data
    u32 points;

    // obstacle data
    u32 damage;

    // shared
    int health;
};
typedef struct Entity Entity;

#endif // ENTITY_H