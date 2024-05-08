#include "graphics/render_pipe.h"

boolean renderpipe_init(RenderPipe* render_pipe, u32 vertex_capacity, u32 entity_capacity) {
    render_pipe->vertex_capacity = vertex_capacity;
    render_pipe->vertex_count = 0;

    mesh_init(&render_pipe->mesh, vertex_capacity);
    render_pipe->index_buffer = malloc(sizeof(int) * render_pipe->mesh.position_count / 3);

    vertexarray_initialise(&render_pipe->vertex_array);

    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.positions, render_pipe->mesh.position_count, GL_DYNAMIC_DRAW, 3, 0);
    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.normals, render_pipe->mesh.normals_count, GL_DYNAMIC_DRAW, 3, 0);
    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.uvs, render_pipe->mesh.uvs_count, GL_DYNAMIC_DRAW, 2, 0);
    vertexarray_addbufferi(&render_pipe->vertex_array, render_pipe->index_buffer, render_pipe->mesh.position_count / 3, GL_DYNAMIC_DRAW, 1);

    render_pipe->entity_capacity = 20;
    render_pipe->entity_count = 0;
    render_pipe->colors = malloc(sizeof(f32) * entity_capacity * 3);
    render_pipe->model_matrices = malloc(sizeof(f32) * entity_capacity * 16);
    render_pipe->use_colors = malloc(sizeof(boolean) * entity_capacity);
    render_pipe->use_textures = malloc(sizeof(boolean) * entity_capacity);

    return true;
}

boolean renderpipe_destroy(RenderPipe* render_pipe) {
    vertexarray_destroy(&render_pipe->vertex_array);
    mesh_free(&render_pipe->mesh);
    free(render_pipe->colors);
    free(render_pipe->model_matrices);
    return true;
}
