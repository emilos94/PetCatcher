#ifndef FRAME_BUFFER_H
#define FRAME_BUFFER_H

#include "graphics/window.h"
#include "graphics/texture.h"
#include "core/core.h"

struct FrameBuffer {
    u32 width, height;
    GLuint handle;
    Texture texture;
};
typedef struct FrameBuffer FrameBuffer;

boolean framebuffer_init(FrameBuffer* frame_buffer, u32 width, u32 height);
void framebuffer_bind(FrameBuffer* frame_buffer);
void framebuffer_unbind();
void framebuffer_destroy(FrameBuffer* frame_buffer);

#endif // FRAME_BUFFER_H