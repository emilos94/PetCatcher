#ifndef SHADOW_RENDER_H
#define SHADOW_RENDER_H

#include "graphics/vertexarray.h"
#include "graphics/shadows/shadow_Shader.h"
#include "graphics/framebuffer.h"
#include "graphics/camera.h"
#include "entity.h"
#include "math.h"
#include "core/generic_utils.h"

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

    vec4 frustum_vertices[8];
    vec3 box_min, box_max;

    f32 ortho_box_width, ortho_box_height, ortho_box_depth;
    vec3 ortho_center;
};
typedef struct ShadowRender ShadowRender;

boolean shadowrender_init(ShadowRender* render, u32 vertex_capacity, u32 entity_capacity);
boolean render_shadow_entity(ShadowRender* render, Camera* camera, Entity* entity, vec3 light_position, mat4 world_view, mat4 world_proj, vec3 min, vec3 max);
void shadowrender_flush(ShadowRender* render, Camera* camera, vec3 light_position, mat4 world_view, mat4 world_proj, vec3 min, vec3 max);
void shadowrender_destroy(ShadowRender* render);

#endif // SHADOW_RENDER_H