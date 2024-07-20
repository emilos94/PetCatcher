#include "shadow_render.h"

boolean shadowrender_init(ShadowRender* render, u32 vertex_capacity, u32 entity_capacity) {
    if (!shadowshader_init(&render->shader)) {
        return false;
    }

    if (!framebuffer_init(&render->fbo, window_width() / 4, window_height()  / 4)) {
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

boolean render_shadow_entity(ShadowRender* render, Entity* entity, vec3 light_position, vec3 light_direction) {
    u32 vertex_offset = render->vertex_count * 3;

    u32 vertex_remaining = render->vertex_capacity - vertex_offset;
    
    for (int i = 0; i < entity->mesh_count; i++) {
        Mesh* mesh = entity->mesh + i;
        u32 mesh_vertex_count = mesh->position_count / 3;

        if (mesh_vertex_count > render->vertex_capacity) {
            printf("[ERROR] Mesh vertex count higher than render capacity!\n");
            return false;
        }

        if (mesh_vertex_count > vertex_remaining || render->entity_count >= render->entity_capacity) {
            shadowrender_flush(render, light_position, light_direction);
            vertex_offset = 0;
        }

        // positions
        for (int i = 0; i < mesh->position_count; i++) {
            render->positions[vertex_offset+i] = mesh->positions[i];
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
        for (int i = 0; i < mesh->position_count / 3; i++) {
            render->ids[vertex_offset + i] = render->entity_count;
        }

        render->entity_count++;
        render->vertex_count += mesh_vertex_count;
    }
}

void shadowrender_flush(ShadowRender* render, vec3 light_position, vec3 light_direction) {
    shader_bind(render->shader.handle);
    vertexarray_bind(&render->vao);
    framebuffer_bind(&render->fbo);

    vertexarray_buffersubdata_f(&render->vao, 0, render->positions, render->vertex_count * 3);
    vertexarray_buffersubdata_i(&render->vao, 1, render->ids, render->vertex_count);

    shader_uniform_mat4n(render->shader.model_matrices_location, render->model_matrices, render->entity_count);

    // view matrix
    mat4 light_view_matrix = GLM_MAT4_IDENTITY_INIT;
    vec3 lookat = GLM_VEC3_ZERO_INIT;
    glm_vec3_add(light_position, light_direction, lookat);
    glm_lookat(light_position, lookat, (vec3){0, 1, 0}, light_view_matrix);
    shader_uniform_mat4(render->shader.view_matrix_location, light_view_matrix);

    // projection matrix
    mat4 light_projection_matrix = GLM_MAT4_IDENTITY_INIT;
    glm_ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1, 7.5, light_projection_matrix);
    shader_uniform_mat4(render->shader.projection_matrix_location, light_projection_matrix);

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, render->vertex_count));

    shader_unbind();
    vertexarray_unbind();
    framebuffer_unbind();
    glViewport(0, 0, window_width(), window_height());
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
