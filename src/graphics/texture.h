#ifndef TEXTURE_H
#define TEXTURE_H

#include "graphics/opengl_utils.h"
#include "core/core.h"
#include "core/log.h"
#include "stb/stb_image.h"

struct Texture {
    GLuint handle;
    u32 width, height;
};
typedef struct Texture Texture;

boolean texture_init(Texture* texture, char* path);
void texture_bind(Texture* texture);
void texture_destroy(Texture* texture);

#endif