#ifndef SHADOW_RENDER_H
#define SHADOW_RENDER_H

#include "graphics/vertexarray.h"
#include "graphics/shadows/shadow_Shader.h"
#include "graphics/framebuffer.h"
#include "entity.h"

struct ShadowRender {
    ShadowShader shader;
    FrameBuffer fbo;

    VertexArray vao;

    u32 vertex_capacity, vertex_count, entity_capacity, entity_count;

    f32* positions;
    u32 positions_count;

    f32* model_matrices;
    u32 model_matrices_count;

    u32* ids;
    u32 ids_count;
};
typedef struct ShadowRender ShadowRender;

boolean shadowrender_init(ShadowRender* render, u32 vertex_capacity, u32 entity_capacity);
boolean render_shadow_entity(ShadowRender* render, Entity* entity, vec3 light_position, vec3 light_direction);
void shadowrender_flush(ShadowRender* render, vec3 light_position, vec3 light_direction);
void shadowrender_destroy(ShadowRender* render);

#endif // SHADOW_RENDER_H