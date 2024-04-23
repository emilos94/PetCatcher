#ifndef GAME_H
#define GAME_H

#include "graphics/render_pipe.h"
#include "graphics/window.h"

struct RenderState {
    RenderPipe render_pipe;

    // shader
    ShaderProgram shader;
    int view_matrix_location, projection_matrix_location, use_color_location, use_texture_location,
        color_location, ambient_strenth_location, light_color_location;

    mat4 view_matrix, projection_matrix;

    boolean use_color, use_texture;

    // light
    f32 ambient_strength;
    vec3 light_color;
};
typedef struct RenderState RenderState;

boolean renderstate_init(RenderState* render_state);

#endif // GAME_H