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

    u32 entity_capacity, entity_count;
    u32* index_buffer;
    f32* colors;
    f32* model_matrices;
    boolean* use_colors;
    boolean* use_textures;

    VertexArray vertex_array;
};
typedef struct RenderPipe RenderPipe;

boolean renderpipe_init(RenderPipe* render_pipe, u32 vertex_capacity, u32 entity_capacity);
boolean renderpipe_destroy(RenderPipe* render_pipe);

#endif