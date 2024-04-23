#ifndef RENDER_PIPE_H
#define RENDER_PIPE_H

#include "core/core.h"
#include "core/file_loader.h"
#include "graphics/opengl_utils.h"
#include "graphics/vertexarray.h"
#include "graphics/shaderprogram.h"
#include "graphics/texture.h"

struct RenderPipe {
    Mesh mesh;
    u32 vertex_count, vertex_capacity;

    VertexArray vertex_array;
};
typedef struct RenderPipe RenderPipe;

boolean renderpipe_init(RenderPipe* render_pipe, u32 vertex_capacity);
boolean renderpipe_render_mesh(RenderPipe* render_pipe, ShaderProgram shader_program, Mesh* mesh);

boolean renderpipe_render_mesh_color(RenderPipe* render_pipe, ShaderProgram shader_program, Mesh* mesh, Texture* texture, u32 color_index);
boolean renderpipe_flush(RenderPipe* render_pipe, ShaderProgram shader_program);
boolean renderpipe_destroy(RenderPipe* render_pipe);

#endif