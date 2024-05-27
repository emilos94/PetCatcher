#include "game.h"

static vec3 COLOR_GREEN = (vec3) { 0.2, 1.0, 0.2 };
static vec3 COLOR_WHITE = (vec3) { 1.0, 1.0, 1.0 };

#define PLAYER_HEIGHT 2.0
#define FRUIT_SPAWN_HEIGHT 10.0
#define OBSTACLE_SPAWN_HEIGHT 15.0

// :forward declarations
boolean aabb_aabb_collision(Boundingbox* either, Boundingbox* other);
void spawn_fruit(GameState* game_state, FruitType fruit_type, vec3 position);
void spawn_obstacle(GameState* game_state, ObstacleType obstacle_type, vec3 position);
f32 random_ratio(void);

boolean renderstate_init(RenderState* render_state) {
    if (!shader_initialise(&render_state->shader, "shaders/vert.txt", "shaders/frag.txt")) {
        printf("[ERROR] failed to initialise shader");
        return false;
    }

    if (!renderpipe_init(&render_state->render_pipe, 1000, 20)) {
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

    render_state->colors_location = shader_uniform_location(render_state->shader, "colors");
    render_state->use_colors_location = shader_uniform_location(render_state->shader, "use_colors");
    render_state->use_textures_location = shader_uniform_location(render_state->shader, "use_textures");
    render_state->model_matrices_location = shader_uniform_location(render_state->shader, "model_matrices");

    render_state->light_color_location = shader_uniform_location(render_state->shader, "light_color");
    render_state->light_color[0] = 0.9;
    render_state->light_color[1] = 0.9;
    render_state->light_color[2] = 0.9;
    shader_uniform_vec3(render_state->light_color_location, render_state->light_color);

    render_state->light_position_location = shader_uniform_location(render_state->shader, "light_position");
    render_state->light_position[0] = 0.0;
    render_state->light_position[1] = 10.9;
    render_state->light_position[2] = -10.9;
    shader_uniform_vec3(render_state->light_position_location, render_state->light_position);

    render_state->ambient_strenth_location = shader_uniform_location(render_state->shader, "ambient_strength");
    render_state->ambient_strength = 0.4;
    shader_uniform_f32(render_state->ambient_strenth_location, render_state->ambient_strength);
    
    return true;
}


boolean gamestate_init(GameState* game_state) {
    arraylist_initialise(&game_state->entities, 100, sizeof(Entity));

    { // :player
        Entity* player = arraylist_push(&game_state->entities);
        player->position[0] = 0.0;
        player->position[1] = PLAYER_HEIGHT;
        player->position[2] = 2.0;
        player->y_velocity = 0.0;
        player->in_air = false;
        player->jump_power = 27.0;
        player->mass = 1.0;
        player->movement_speed = 10.0;
        glm_vec3_copy(player->position, player->bounding_box.center);
        player->bounding_box.center[1] -= PLAYER_HEIGHT  / 2.0;
        glm_vec3_fill(player->bounding_box.half_size, PLAYER_HEIGHT / 2.0);
        player->flags = EntityFlag_Collider | EntityFlag_RigidBody;
        player->tag = EntityTag_Player;
        player->list_index = game_state->entities.element_count - 1;
        player->health = 20;
        game_state->player = player;
    }

    { // :assets
        if (!game_loadmap(game_state, "../res/maps/ground_cube.dae")) {
            return false;
        }

        if (!file_loadcollada(&game_state->apple_data, "../res/meshes/apple.dae")) {
            return false;
        }

        if (!file_loadcollada(&game_state->boulder_data, "../res/meshes/boulder.dae")) {
            return false;
        }
    }

    { // :camera
        game_state->camera_front[0] = 0.0;
        game_state->camera_front[1] = 0.0;
        game_state->camera_front[2] = -1.0;
        
        game_state->camera_up[0] = 0.0;
        game_state->camera_up[1] = 1.0;
        game_state->camera_up[2] = 0.0;

        game_state->camera_panning_speed = 10.0;
        game_state->camera_pan_sensitivity = 0.15;
        game_state->camera_yaw = -90.0;
        game_state->camera_pitch = 0.0;
    }

    { // :constants
        game_state->gravity = 0.04;
        game_state->ground_height = 0.0;
        game_state->score = 0;
        game_state->print_timer = 0.0;
    }

    { // :spawn 
        game_state->fruit_spawn_timer = 0.0;
        game_state->fruit_spawn_interval = 2.0;

        game_state->obstacle_spawn_timer = 0.0;
        game_state->obstacle_spawn_interval = 5.0;
    }

    return true;
}

boolean game_loadmap(GameState* game_state, char* path) {
    if (!file_loadcollada(&game_state->map_data, path)) {
        return false;
    }

    for (int i = 0; i < game_state->map_data.mesh_count; i++) {
        Mesh* mesh = game_state->map_data.meshes + i;

        Entity* entity = arraylist_push(&game_state->entities);

        entity->mesh = mesh;
        entity->mesh_count = 1;
        entity->position[0] = mesh->transform[3];
        entity->position[1] = mesh->transform[7];
        entity->position[2] = mesh->transform[11];
        entity->scale[0] = mesh->transform[0];
        entity->scale[1] = mesh->transform[5];
        entity->scale[2] = mesh->transform[10];
        
        glm_vec3_copy(entity->position, entity->bounding_box.center);
        glm_vec3_copy(entity->scale, entity->bounding_box.half_size);
        glm_vec3_scale(entity->bounding_box.half_size, 0.5, entity->bounding_box.half_size);
        
        if (string_equals_lit(mesh->name, "Ground")) {
            game_state->ground_box = entity->bounding_box;
            game_state->ground_height = entity->bounding_box.center[1] + entity->bounding_box.half_size[1];
        }

        glm_vec3_fill(entity->rotation, 0.0);
        
        glm_vec3_copy(mesh->color, entity->color);
        entity->flags = EntityFlag_Render | EntityFlag_UseColor | EntityFlag_Collider;
        entity->tag = EntityTag_Other;
    }

    return true;
}

// :camera
void camera_input(GameState* game_state, RenderState* render_state, f32 delta) {
    game_state->print_timer += delta;
    
    { // panning (look around)
        game_state->camera_yaw += input_mouse_x_delta() * game_state->camera_pan_sensitivity;
        game_state->camera_pitch += input_mouse_y_delta() * game_state->camera_pan_sensitivity;
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
        
        vec3 camera_right = GLM_VEC3_ZERO_INIT;
        glm_vec3_cross(GLM_YUP, look_direction, camera_right);
        glm_normalize(camera_right);

        glm_vec3_cross(look_direction, camera_right, game_state->camera_up);
        glm_normalize(game_state->camera_up);
    }

    { // movement   
        f32 movement_speed = game_state->player->movement_speed * delta;
        vec3 movement_direction = {game_state->camera_front[0], 0.0, game_state->camera_front[2]};
        if (input_keydown(GLFW_KEY_W)) {
            glm_vec3_muladds(
                movement_direction,
                movement_speed,
                game_state->player->position
            );
        }
        
        if (input_keydown(GLFW_KEY_S)) {
            glm_vec3_mulsubs(
                movement_direction,
                movement_speed,
                game_state->player->position
            );
        }    

        if (input_keydown(GLFW_KEY_A)) {
            vec3 delta = GLM_VEC3_ZERO_INIT;
            glm_vec3_cross(movement_direction, GLM_YUP, delta);
            glm_vec3_normalize(delta);

            glm_vec3_mulsubs(
                delta,
                movement_speed,
                game_state->player->position
            );
        }  

        if (input_keydown(GLFW_KEY_D)) {
            vec3 delta = GLM_VEC3_ZERO_INIT;
            glm_vec3_cross(movement_direction, GLM_YUP, delta);
            glm_vec3_normalize(delta);

            glm_vec3_muladds(
                delta,
                movement_speed,
                game_state->player->position
            );
        }
    }
    
    { // jumping 
        if (input_keyjustdown(GLFW_KEY_SPACE) && !game_state->player->in_air) {
            game_state->player->in_air = true;

            game_state->player->y_velocity = game_state->player->jump_power;
        }

        if (game_state->player->in_air) {
            game_state->player->y_velocity -= game_state->gravity;
            game_state->player->position[1] += game_state->player->y_velocity * delta;

            if (game_state->player->position[1] - PLAYER_HEIGHT <= game_state->ground_height) {
                game_state->player->position[1] = game_state->ground_height + PLAYER_HEIGHT;
                game_state->player->in_air = false;
                game_state->player->y_velocity = 0.0;
            }
        }
    }

    vec3 lookat = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(game_state->player->position, game_state->camera_front, lookat);

    glm_mat4_identity(render_state->view_matrix);
    glm_lookat(game_state->player->position, lookat, game_state->camera_up, render_state->view_matrix);

    shader_bind(render_state->shader);
    shader_uniform_mat4(render_state->view_matrix_location, render_state->view_matrix);
    shader_unbind();
}

void game_input(GameState* game_state) {
}

void game_update(GameState* game_state, f32 delta) {
    // entities
    ARRAYLIST_FOREACH(game_state->entities, Entity, entity) {
        // :collisions
        if (entity->flags & EntityFlag_Collider) {
            glm_vec3_copy(entity->position, entity->bounding_box.center);
            if (entity == game_state->player) {
                entity->bounding_box.center[1] -= PLAYER_HEIGHT / 2.0;
            }

            ARRAYLIST_FOREACHI(game_state->entities, j, Entity, _entity) {
                if (entity != _entity && 
                    _entity->flags & EntityFlag_Collider &&
                    aabb_aabb_collision(&entity->bounding_box, &_entity->bounding_box)) {

                    if (entity->tag == EntityTag_Player && _entity->tag == EntityTag_Fruit) {
                        _entity->queue_delete = true;
                        entity->health += _entity->health;
                        game_state->score += _entity->points;
                        printf("Caught fruit ! Score: %d, health: %d\n", game_state->score, entity->health);
                    }
                    if (entity->tag == EntityTag_Player && _entity->tag == EntityTag_Obstacle) {
                        _entity->queue_delete = true;
                        entity->health -= _entity->damage;
                        printf("Collided with obstacle! health: %d\n", entity->health);
                    }
                }
            }
        }

        // :rigidbody
        if (entity->flags & EntityFlag_RigidBody) {
            if (entity->in_air) {
                entity->y_velocity -= game_state->gravity * entity->mass;
                entity->position[1] += entity->y_velocity * delta;
            }

            if (entity->flags & EntityFlag_Collider &&
                entity->position[1] - entity->bounding_box.half_size[1] <= game_state->ground_height) {

                entity->position[1] = game_state->ground_height + entity->bounding_box.half_size[1] * 2.0;
                entity->in_air = false;
                entity->y_velocity = 0.0;
            }
        }

    }

    ARRAYLIST_FOREACHI(game_state->entities, i, Entity, e) {
        if (e->queue_delete) {
            u32 list_index = e->list_index;
            e->queue_delete = false;
            arraylist_remove(&game_state->entities, e->list_index);
            // arraylist graps last element and replaces removed to keep it dense
            // update list_index in entity replacing the removed
            if (list_index != game_state->entities.element_count) {
                Entity* old_last = arraylist_at(&game_state->entities, list_index);
                old_last->list_index = list_index;
            }
            i--;
        }
    }
    
    // :spawn fruit
    game_state->fruit_spawn_timer += delta; 
    if (game_state->fruit_spawn_timer >= game_state->fruit_spawn_interval) {
        f32 ratio_x = random_ratio();
        f32 ratio_z = random_ratio();

        f32 x_pos = game_state->ground_box.half_size[0] * 2.0 * ratio_x - game_state->ground_box.half_size[0];
        f32 z_pos = game_state->ground_box.half_size[2] * 2.0 * ratio_z - game_state->ground_box.half_size[2];

        spawn_fruit(game_state, FruitType_Apple, (vec3){x_pos, FRUIT_SPAWN_HEIGHT, z_pos});

        game_state->fruit_spawn_timer -= game_state->fruit_spawn_interval;
    }
    
    // :spawn obstacle
    game_state->obstacle_spawn_timer += delta; 
    if (game_state->obstacle_spawn_timer >= game_state->obstacle_spawn_interval) {
        f32 ratio_x = random_ratio();
        f32 ratio_z = random_ratio();

        f32 x_pos = game_state->ground_box.half_size[0] * 2.0 * ratio_x - game_state->ground_box.half_size[0];
        f32 z_pos = game_state->ground_box.half_size[2] * 2.0 * ratio_z - game_state->ground_box.half_size[2];

        spawn_obstacle(game_state, ObstacleType_Boulder, (vec3){x_pos, OBSTACLE_SPAWN_HEIGHT, z_pos});

        game_state->obstacle_spawn_timer -= game_state->obstacle_spawn_interval;
    }

    UIInfo box = ui_box("box", (vec2){0.0, 0.0}, (vec2){1.0, 0.07});
    
    if (box.hot) {
        glm_vec3_copy(COLOR_GREEN, box.widget->background_color);
        printf("Box hovered!\n");
    }
    else {
        glm_vec3_copy(COLOR_WHITE, box.widget->background_color);
    }

    // :timers
    game_state->print_timer += delta;

    if (game_state->print_timer >= 1.0) {
        game_state->print_timer -= 1.0;
    }
}

void game_render(GameState* game_state, RenderState* render_state, f32 delta) {
    shader_bind(render_state->shader);
    //texture_bind(&texture);

    // entities
    ARRAYLIST_FOREACH(game_state->entities, Entity, entity) {
        if (entity->flags & EntityFlag_Render) {
            entity_render(render_state, entity);
        }
    }
    
    render_flush(render_state, render_state->shader);
    shader_unbind();
    vertexarray_unbind();

    ui_render();
}

boolean entity_render(RenderState* render_state, Entity* entity) {
    if (!(entity->flags & EntityFlag_Render)) {
        return false;
    }

    RenderPipe* render_pipe = &render_state->render_pipe;
    for (int i = 0; i < entity->mesh_count; i++) {
        Mesh* mesh = entity->mesh + i;
        if (render_pipe->vertex_capacity < mesh->position_count / 3) {
            return false;
        }

        u32 vertex_capacity_remaining = render_pipe->vertex_capacity - render_pipe->vertex_count;
        if (vertex_capacity_remaining < mesh->position_count / 3 || render_pipe->entity_count >= render_pipe->entity_capacity) {
            render_flush(render_state, render_state->shader);
        }

        { // fill vertex attributes
            u32 vertex_offset = render_pipe->vertex_count * 3;
            for (int i = 0; i < mesh->position_count; i++) {
                render_pipe->mesh.positions[vertex_offset+i] = mesh->positions[i];
            }

            for (int i = 0; i < mesh->normals_count; i++) {
                render_pipe->mesh.normals[vertex_offset+i] = mesh->normals[i];
            }

            u32 uvs_offset = render_pipe->vertex_count * 2;
            for (int i = 0; i < mesh->uvs_count; i++) {
                render_pipe->mesh.uvs[uvs_offset+i] = mesh->uvs[i];
            }
        }

        { // model matrix 
            mat4 model_matrix = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model_matrix, entity->position);
            glm_rotate_x(model_matrix, entity->rotation[0], model_matrix);
            glm_rotate_y(model_matrix, entity->rotation[1], model_matrix);
            glm_rotate_z(model_matrix, entity->rotation[2], model_matrix);
            glm_scale(model_matrix, entity->scale);

            f32* model_matrix_values = (f32*)model_matrix;
            for (int i = 0; i < 16; i++) {
                render_pipe->model_matrices[render_pipe->entity_count * 16 + i] = model_matrix_values[i];
            }
        }

        if (entity->flags & EntityFlag_UseColor) {
            render_pipe->colors[render_pipe->entity_count * 3] = mesh->color[0];
            render_pipe->colors[render_pipe->entity_count * 3+1] = mesh->color[1];
            render_pipe->colors[render_pipe->entity_count * 3+2] = mesh->color[2];
            render_pipe->use_colors[render_pipe->entity_count] = true;
        }

        for (int i = 0; i < mesh->position_count / 3; i++) {
            render_pipe->index_buffer[ render_pipe->vertex_count + i] = render_pipe->entity_count;
        }

        // todo: implement use_texture

        render_state->render_pipe.vertex_count += mesh->position_count / 3;
        render_state->render_pipe.entity_count++;
        
    }
    
    return true;
}

boolean render_flush(RenderState* render_state, ShaderProgram shader_program) {
    RenderPipe* render_pipe = &render_state->render_pipe;
    shader_bind(shader_program);
    vertexarray_bind(&render_pipe->vertex_array);

    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 0, render_pipe->mesh.positions, render_pipe->vertex_count * 3);
    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 1, render_pipe->mesh.normals, render_pipe->vertex_count * 3);
    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 2, render_pipe->mesh.uvs, render_pipe->vertex_count * 2);
    vertexarray_buffersubdata_i(&render_pipe->vertex_array, 3, render_pipe->index_buffer, render_pipe->vertex_count);
    shader_uniform_mat4n(render_state->model_matrices_location, render_pipe->model_matrices, render_pipe->entity_count);
    shader_uniform_vec3n(render_state->colors_location, render_pipe->colors, render_pipe->entity_count);
    shader_uniform_booln(render_state->use_colors_location, render_pipe->use_colors, render_pipe->entity_count);
    shader_uniform_booln(render_state->use_textures_location, render_pipe->use_textures, render_pipe->entity_count);

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, render_pipe->vertex_count));

    render_pipe->vertex_count = 0;
    render_pipe->entity_count = 0;
    return true;
}

boolean aabb_aabb_collision(Boundingbox* either, Boundingbox* other) {
    f32 min_x0 = either->center[0] - either->half_size[0];
    f32 min_y0 = either->center[1] - either->half_size[1];
    f32 min_z0 = either->center[2] - either->half_size[2];
    f32 max_x0 = either->center[0] + either->half_size[0];
    f32 max_y0 = either->center[1] + either->half_size[1];
    f32 max_z0 = either->center[2] + either->half_size[2];

    f32 min_x1 = other->center[0] - other->half_size[0];
    f32 min_y1 = other->center[1] - other->half_size[1];
    f32 min_z1 = other->center[2] - other->half_size[2];
    f32 max_x1 = other->center[0] + other->half_size[0];
    f32 max_y1 = other->center[1] + other->half_size[1];
    f32 max_z1 = other->center[2] + other->half_size[2];

    boolean result =
        min_x0 <= max_x1 &&
        max_x0 >= min_x1 &&
        min_y0 <= max_y1 &&
        max_y0 >= min_y1 &&
        min_z0 <= max_z1 &&
        max_z0 >= min_z1;

    return result;
}

// :spawn fruit
void spawn_fruit(GameState* game_state, FruitType fruit_type, vec3 position) {
        u32 index = game_state->entities.element_count;
        Entity* fruit = arraylist_push(&game_state->entities);
        fruit->list_index = index;
        glm_vec3_copy(position, fruit->position);

        switch (fruit_type)
        {
        case FruitType_Apple:
            fruit->mesh = game_state->apple_data.meshes;
            fruit->mesh_count = game_state->apple_data.mesh_count;
            fruit->points = 5;
            fruit->health = 2;
            fruit->mass = 10.0;
            glm_vec3_fill(fruit->scale, 0.3);
            break;
        
        default:
            break;
        }

        glm_vec3_fill(fruit->rotation, 0.0);
        glm_vec3_copy(fruit->position, fruit->bounding_box.center);
        glm_vec3_copy(fruit->scale, fruit->bounding_box.half_size);
        glm_vec3_scale(fruit->bounding_box.half_size, 1.5, fruit->bounding_box.half_size);
        
        fruit->flags = EntityFlag_Render | EntityFlag_UseColor | EntityFlag_Collider | EntityFlag_RigidBody;
        fruit->in_air = true;
        fruit->y_velocity = 0.0;
        fruit->tag = EntityTag_Fruit;
}

// :spawn obstacle
void spawn_obstacle(GameState* game_state, ObstacleType obstacle_type, vec3 position) {
        u32 index = game_state->entities.element_count;
        Entity* obstacle = arraylist_push(&game_state->entities);
        obstacle->list_index = index;
        glm_vec3_copy(position, obstacle->position);

        switch (obstacle_type)
        {
        case ObstacleType_Boulder:
            obstacle->mesh = game_state->boulder_data.meshes;
            obstacle->mesh_count = game_state->boulder_data.mesh_count;
            obstacle->damage = 5;
            obstacle->mass = 20.0;
            glm_vec3_fill(obstacle->scale, 1.0);
            break;
        
        default:
            break;
        }

        glm_vec3_fill(obstacle->rotation, 0.0);
        glm_vec3_copy(obstacle->position, obstacle->bounding_box.center);
        glm_vec3_copy(obstacle->scale, obstacle->bounding_box.half_size);
        glm_vec3_scale(obstacle->bounding_box.half_size, 1.0, obstacle->bounding_box.half_size);
        
        obstacle->flags = EntityFlag_Render | EntityFlag_UseColor | EntityFlag_Collider | EntityFlag_RigidBody;
        obstacle->in_air = true;
        obstacle->y_velocity = 0.0;
        obstacle->tag = EntityTag_Obstacle;
}


f32 random_ratio(void) {
    return (float)rand() / (float)RAND_MAX;
}

/* // might come in handy todo
boolean aabb_ray_intersects(vec3 ray_origin, vec3 ray_direction, BoundingBox* other) {
    vec3 dir_frac = GLM_VEC3_ONE_INIT;
    glm new Vector3f(1f).div(rayDirection);

    Vector3f actualPos = new Vector3f(basePosition);
    actualPos.add(offset);

    Vector3f min = new Vector3f(), max = new Vector3f();
    actualPos.sub(halfSize, min);
    actualPos.add(halfSize, max);

    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    // r.org is origin of ray
    float t1 = (min.x - rayOrigin.x) * dirFrac.x;
    float t2 = (max.x - rayOrigin.x) * dirFrac.x;
    float t3 = (min.y - rayOrigin.y) * dirFrac.y;
    float t4 = (max.y - rayOrigin.y) * dirFrac.y;
    float t5 = (min.z - rayOrigin.z) * dirFrac.z;
    float t6 = (max.z - rayOrigin.z) * dirFrac.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        yield new IntersectionResult(false, tmax);
    }

// if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        yield new IntersectionResult(false, tmax);
    }

    yield new IntersectionResult(true, tmin);
}*/