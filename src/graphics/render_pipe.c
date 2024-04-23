#include "graphics/render_pipe.h"

boolean renderpipe_init(RenderPipe* render_pipe, u32 vertex_capacity) {
    render_pipe->vertex_capacity = vertex_capacity;
    render_pipe->vertex_count = 0;

    mesh_init(&render_pipe->mesh, vertex_capacity);

    vertexarray_initialise(&render_pipe->vertex_array);

    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.positions, render_pipe->mesh.position_count, GL_DYNAMIC_DRAW, 3, 0);
    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.normals, render_pipe->mesh.normals_count, GL_DYNAMIC_DRAW, 3, 0);
    vertexarray_addbufferf(&render_pipe->vertex_array, render_pipe->mesh.uvs, render_pipe->mesh.uvs_count, GL_DYNAMIC_DRAW, 2, 0);

    return true;
}


boolean renderpipe_render_mesh(RenderPipe* render_pipe, ShaderProgram shader_program, Mesh* mesh) {
    if (render_pipe->vertex_capacity < mesh->position_count / 3) {
        return false;
    }

    u32 vertex_capacity_remaining = render_pipe->vertex_capacity - render_pipe->vertex_count;
    if (vertex_capacity_remaining < mesh->position_count / 3) {
        renderpipe_flush(render_pipe, shader_program);
    }

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

    render_pipe->vertex_count += mesh->position_count / 3;
    return true;
}

boolean renderpipe_render_mesh_color(RenderPipe* render_pipe, ShaderProgram shader_program, Mesh* mesh, Texture* texture, u32 color_index) {
    if (render_pipe->vertex_capacity < mesh->position_count / 3) {
        return false;
    }

    u32 vertex_capacity_remaining = render_pipe->vertex_capacity - render_pipe->vertex_count;
    if (vertex_capacity_remaining < mesh->position_count / 3) {
        renderpipe_flush(render_pipe, shader_program);
    }

    u32 vertex_offset = render_pipe->vertex_count * 3;
    for (int i = 0; i < mesh->position_count; i++) {
        render_pipe->mesh.positions[vertex_offset+i] = mesh->positions[i];
    }

    for (int i = 0; i < mesh->normals_count; i++) {
        render_pipe->mesh.normals[vertex_offset+i] = mesh->normals[i];
    }

    f32 number_of_colors = 46; // strip of colors

    f32 texture_x = number_of_colors / (f32)texture->width * (f32)color_index;
    f32 texture_y = (f32)texture->height / 2.0;

    u32 uvs_offset = render_pipe->vertex_count * 2;
    for (int i = 0; i < mesh->uvs_count; i+=2) {
        render_pipe->mesh.uvs[uvs_offset+i] = texture_x;
        render_pipe->mesh.uvs[uvs_offset+i+1] = texture_y;
    }

    render_pipe->vertex_count += mesh->position_count / 3;
    return true;
}



boolean renderpipe_flush(RenderPipe* render_pipe, ShaderProgram shader_program) {
    shader_bind(shader_program);
    vertexarray_bind(&render_pipe->vertex_array);

    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 0, render_pipe->mesh.positions, render_pipe->vertex_count * 3);
    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 1, render_pipe->mesh.normals, render_pipe->vertex_count * 3);
    vertexarray_buffersubdata_f(&render_pipe->vertex_array, 2, render_pipe->mesh.uvs, render_pipe->vertex_count * 2);

    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, render_pipe->vertex_count));

    render_pipe->vertex_count = 0;
    return true;
}

boolean renderpipe_destroy(RenderPipe* render_pipe) {
    vertexarray_destroy(&render_pipe->vertex_array);
    mesh_free(&render_pipe->mesh);
    return true;
}
