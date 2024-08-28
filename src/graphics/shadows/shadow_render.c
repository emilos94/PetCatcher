#include "shadow_render.h"

// forward function declarations
void _shadow_update_view_proj_mat(ShadowRender* render, Camera* camera, vec3 light_position, vec3 min, vec3 max);
void calculate_frustum_bbox(ShadowRender* render, Camera* camera, vec3 light_position);

// global declarations
extern f32 g_field_of_view;
extern f32 g_near_plane;
extern f32 g_far_plane;
extern vec3 G_UP_VECTOR;

#define SHADOW_DISTANCE 50.0
#define SHADOW_MIN_HEIGHT 25.0
#define SHADOW_BOX_OFFSET 3.0
#define SHADOW_Y_LOOKAT 0.5
#define SHADOW_TEXTURE_DIMENSIONS 2048

boolean shadowrender_init(ShadowRender* render, u32 vertex_capacity, u32 entity_capacity) {
    if (!shadowshader_init(&render->shader)) {
        return false;
    }

    if (!framebuffer_init(&render->fbo, 2048, 2048)) {
        printf("[ERROR] Failed to initialise shadow frame buffer!\n");
        return false;
    }

    render->entity_capacity = entity_capacity;
    render->entity_count = 0;
    render->vertex_capacity = vertex_capacity;
    render->vertex_count = 0;
    render->positions = malloc(sizeof(f32) * vertex_capacity * 3);
    render->positions_count = vertex_capacity * 3;
    render->model_matrices = malloc(sizeof(f32) * entity_capacity * 16);
    render->model_matrices_count = entity_capacity * 16;
    render->ids = malloc(sizeof(u32) * vertex_capacity);
    render->ids_count = vertex_capacity;

    vertexarray_initialise(&render->vao);
    vertexarray_addbufferf(
         &render->vao, 
         render->positions,
         render->positions_count,
         GL_DYNAMIC_DRAW,
         3,
         0
    );
    vertexarray_addbufferi(
         &render->vao, 
         render->ids,
         render->ids_count,
         GL_DYNAMIC_DRAW,
         1
    );

    shader_unbind();
    vertexarray_unbind();
    framebuffer_unbind();

    return true;
}

boolean render_shadow_entity(ShadowRender* render, Camera* camera, Entity* entity, vec3 light_position, mat4 world_view, mat4 world_proj, vec3 min, vec3 max) {
    u32 vertex_offset = render->vertex_count;

    if (vertex_offset >= render->vertex_capacity) {
        shadowrender_flush(render, camera, light_position, world_view, world_proj, min, max);
        vertex_offset = 0;
    }

    for (int i = 0; i < entity->mesh_count; i++) {
        Mesh* mesh = entity->mesh + i;
        u32 mesh_vertex_count = mesh->position_count / 3;

        if (mesh_vertex_count >= render->vertex_capacity) {
            log_msg("[ERROR] Mesh vertex count higher than render capacity!\n");
            return false;
        }

        if (render->entity_count >= render->entity_capacity ||
            vertex_offset + mesh_vertex_count >= render->vertex_capacity) {
            shadowrender_flush(render, camera, light_position, world_view, world_proj, min, max);
            vertex_offset = 0;
        }

        u32 positions_offset = vertex_offset * 3;
        // positions
        for (int i = 0; i < mesh->position_count; i++) {
            render->positions[positions_offset+i] = mesh->positions[i];
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
                render->model_matrices[render->entity_count * 16 + i] = model_matrix_values[i];
            }
        }

        // ids
        for (int i = 0; i < mesh_vertex_count; i++) {
            render->ids[vertex_offset + i] = render->entity_count;
        }

        render->entity_count++;
        render->vertex_count += mesh_vertex_count;
        vertex_offset += mesh_vertex_count;
    }
}

void shadowrender_flush(ShadowRender* render, Camera* camera, vec3 light_position, mat4 world_view, mat4 world_proj, vec3 min, vec3 max) {
    vertexarray_buffersubdata_f(&render->vao, 0, render->positions, render->vertex_count * 3);
    vertexarray_buffersubdata_i(&render->vao, 1, render->ids, render->vertex_count);

    shader_uniform_mat4n(render->shader.model_matrices_location, render->model_matrices, render->entity_count);

    _shadow_update_view_proj_mat(render, camera, light_position, min, max);
    vec3 light_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_sub(GLM_VEC3_ZERO, light_position, light_direction);
    glm_vec3_normalize(light_direction);

    shader_uniform_mat4(render->shader.view_matrix_location, render->shader.view_matrix);
    shader_uniform_mat4(render->shader.projection_matrix_location, render->shader.projection_matrix);

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, render->vertex_count));

    render->vertex_count = 0;
    render->entity_count = 0;
}

void shadowrender_destroy(ShadowRender* render) {
    shader_destroy(render->shader.handle);
    vertexarray_destroy(&render->vao);
    framebuffer_destroy(&render->fbo);
    free(render->positions);
    free(render->model_matrices);
    free(render->ids);
}

void _shadow_update_view_proj_mat(ShadowRender* render, Camera* camera, vec3 light_position, vec3 min, vec3 max) {
    vec3s entity_bbox_center = {
        (min[0] + max[0]) / 2.0,
        SHADOW_Y_LOOKAT,
        (min[2] + max[2]) / 2.0
    };

    vec3 light_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_sub(GLM_VEC3_ZERO, light_position, light_direction);
    glm_vec3_normalize(light_direction);

    vec3 to_light_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_negate_to(light_direction, to_light_direction);

    vec3s eye = glms_vec3_add(entity_bbox_center, (vec3s){to_light_direction[0], to_light_direction[1], to_light_direction[2]});

    mat4 initial_light_view_matrix = GLM_MAT4_IDENTITY_INIT;
    glm_lookat(
        (vec3){eye.x, eye.y, eye.z},
        (vec3){entity_bbox_center.x, entity_bbox_center.y, entity_bbox_center.z},
        G_UP_VECTOR,
        initial_light_view_matrix
    );
    
    float box_width = float_diff(max[0], min[0]);
    float box_height = fmax(float_diff(max[1], min[1]), SHADOW_MIN_HEIGHT);
    float box_length = float_diff(max[2], min[2]);

    glm_mat4_identity(render->shader.projection_matrix);
    glm_ortho(
        -box_width, 
        box_width, 
        -box_height, 
        box_height,
        -box_length,
        box_length, 
        render->shader.projection_matrix
    );

    // view matrix
    vec3 ortho_box_center = {
        (min[0] + max[0]) / 2.0,
        SHADOW_Y_LOOKAT,
        (min[2] + max[2]) / 2.0
    };

    mat4 inverted_light_view = GLM_MAT4_IDENTITY_INIT;
    glm_mat4_inv(initial_light_view_matrix, inverted_light_view);
    glm_mat4_mulv3(inverted_light_view, ortho_box_center, 1.0, ortho_box_center);

    vec3s to_light_vec = glms_vec3_scale((vec3s){to_light_direction[0], to_light_direction[1], to_light_direction[2]}, box_height / 2);

    vec3s shadow_eye = glms_vec3_add(
        (vec3s){ortho_box_center[0], ortho_box_center[1], ortho_box_center[2]}, 
        to_light_vec
    );

    glm_mat4_identity(render->shader.view_matrix);
    glm_lookat(
        (vec3){shadow_eye.x, shadow_eye.y, shadow_eye.z},
        ortho_box_center, 
        G_UP_VECTOR, 
        render->shader.view_matrix
    );
}

// Note: not used, keeping around for future reference
void calculate_frustum_bbox(ShadowRender* render, Camera* camera, vec3 light_position) {
    vec3 camera_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_negate_to(camera->right, camera_left);

    vec3 camera_down = GLM_VEC3_ZERO_INIT;
    glm_vec3_negate_to(camera->up, camera_down);

    vec3 scaled_front_near = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->front, g_near_plane, scaled_front_near);

    vec3 near_center = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(camera->position, scaled_front_near, near_center);
    
    vec3 scaled_front_far = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->front, g_far_plane, scaled_front_far);

    vec3 far_center = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(camera->position, scaled_front_far, far_center);

    f32 aspect_ratio = window_width() / window_height();

    f32 near_height = 2.0 * tanf(glm_rad(g_field_of_view / 2.0)) * g_near_plane;
    f32 far_height = 2.0 * tanf(glm_rad(g_field_of_view / 2.0)) * g_far_plane;
    f32 near_width = near_height * aspect_ratio;
    f32 far_width = far_height * aspect_ratio;

    vec3 far_to_top = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->up, far_height * 0.5, far_to_top);

    vec3 far_to_bottom = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera_down, far_height * 0.5, far_to_bottom);
    
    vec3 far_to_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->right, far_width * 0.5, far_to_right);

    vec3 far_to_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera_left, far_width * 0.5, far_to_left);

    vec3 near_to_top = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->up, near_height * 0.5, near_to_top);

    vec3 near_to_bottom = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera_down, near_height * 0.5, near_to_bottom);
    
    vec3 near_to_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera->right, near_width * 0.5, near_to_right);

    vec3 near_to_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_scale(camera_left, near_width * 0.5, near_to_left);
    
    vec3 far_top_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(far_center, far_top_left);
    glm_vec3_add(far_top_left, far_to_top, far_top_left);
    glm_vec3_add(far_top_left, far_to_left, far_top_left);
    
    vec3 far_top_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(far_center, far_top_right);
    glm_vec3_add(far_top_right, far_to_top, far_top_right);
    glm_vec3_add(far_top_right, far_to_right, far_top_right);

    vec3 far_bottom_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(far_center, far_bottom_left);
    glm_vec3_add(far_bottom_left, far_to_bottom, far_bottom_left);
    glm_vec3_add(far_bottom_left, far_to_left, far_bottom_left);

    vec3 far_bottom_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(far_center, far_bottom_right);
    glm_vec3_add(far_bottom_right, far_to_bottom, far_bottom_right);
    glm_vec3_add(far_bottom_right, far_to_right, far_bottom_right);
    
    vec3 near_top_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(near_center, near_top_left);
    glm_vec3_add(near_top_left, near_to_top, near_top_left);
    glm_vec3_add(near_top_left, near_to_left, near_top_left);
    
    vec3 near_top_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(near_center, near_top_right);
    glm_vec3_add(near_top_right, near_to_top, near_top_right);
    glm_vec3_add(near_top_right, near_to_right, near_top_right);

    vec3 near_bottom_left = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(near_center, near_bottom_left);
    glm_vec3_add(near_bottom_left, near_to_bottom, near_bottom_left);
    glm_vec3_add(near_bottom_left, near_to_left, near_bottom_left);

    vec3 near_bottom_right = GLM_VEC3_ZERO_INIT;
    glm_vec3_copy(near_center, near_bottom_right);
    glm_vec3_add(near_bottom_right, near_to_bottom, near_bottom_right);
    glm_vec3_add(near_bottom_right, near_to_right, near_bottom_right);

    vec3s frustum_center = glms_vec3_add(
        (vec3s){camera->position[0],camera->position[1],camera->position[2]}, 
        glms_vec3_scale((vec3s){camera->front[0],camera->front[1],camera->front[2]}, g_far_plane / 2.0)
    );

    vec3 light_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_sub(GLM_VEC3_ZERO, light_position, light_direction);
    glm_vec3_normalize(light_direction);

    vec3 to_light_direction = GLM_VEC3_ZERO_INIT;
    glm_vec3_negate_to(light_direction, to_light_direction);

    vec3s eye = glms_vec3_add(frustum_center, (vec3s){to_light_direction[0], to_light_direction[1], to_light_direction[2]});

    mat4 initial_light_view_matrix = GLM_MAT4_IDENTITY_INIT;
    glm_lookat(
        (vec3){eye.x, eye.y, eye.z},
        (vec3){frustum_center.x, frustum_center.y, frustum_center.z},
        G_UP_VECTOR,
        initial_light_view_matrix
    );

    glm_mat4_mulv3(initial_light_view_matrix, far_top_left, 1.0, render->frustum_vertices[0]);
    glm_mat4_mulv3(initial_light_view_matrix, far_top_right, 1.0, render->frustum_vertices[1]);
    glm_mat4_mulv3(initial_light_view_matrix, far_bottom_left, 1.0, render->frustum_vertices[2]);
    glm_mat4_mulv3(initial_light_view_matrix, far_bottom_right, 1.0, render->frustum_vertices[3]);
    glm_mat4_mulv3(initial_light_view_matrix, near_top_left, 1.0, render->frustum_vertices[4]);
    glm_mat4_mulv3(initial_light_view_matrix, near_top_right, 1.0, render->frustum_vertices[5]);
    glm_mat4_mulv3(initial_light_view_matrix, near_bottom_left, 1.0, render->frustum_vertices[6]);
    glm_mat4_mulv3(initial_light_view_matrix, near_bottom_right, 1.0, render->frustum_vertices[7]);

    f32 min_x = 100000, min_y = 100000, min_z = 100000, max_x = -100000, max_y = -100000, max_z = -100000;
    for (int i = 0; i < 8; i++) {
        f32 x = render->frustum_vertices[i][0];
        f32 y = render->frustum_vertices[i][1];
        f32 z = render->frustum_vertices[i][2];
        if (x > max_x) {
            max_x = x;
        }
        else if (x < min_x) {
            min_x = x;
        }
        
        if (y > max_y) {
            max_y = y;
        }
        else if (y < min_y) {
            min_y = y;
        }
        
        if (z > max_z) {
            max_z = z;
        }
        else if (z < min_z) {
            min_z = z;
        }
    }

    float box_width = float_diff(max_x, min_x);
    float box_height = fmax(float_diff(max_y, min_y), SHADOW_MIN_HEIGHT);
    float box_length = float_diff(max_z, min_z);
}
