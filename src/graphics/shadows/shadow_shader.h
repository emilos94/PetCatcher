#ifndef SHADOW_SHADER_H
#define SHADOW_SHADER_H

#include "core/log.h"
#include "graphics/shaderprogram.h"

struct ShadowShader {
    ShaderProgram handle;
    int view_matrix_location, projection_matrix_location, model_matrices_location;

    mat4 view_matrix, projection_matrix;
};
typedef struct ShadowShader ShadowShader;

boolean shadowshader_init(ShadowShader* shadowshader);
void shadowshader_destroy(ShadowShader* shadowshader);

#endif // SHADOW_SHADER_H