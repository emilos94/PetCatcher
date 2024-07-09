#include "graphics/framebuffer.h"

boolean framebuffer_init(FrameBuffer* frame_buffer, u32 width, u32 height) {
    GL_CALL(glGenFramebuffers(1, &frame_buffer->handle));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle));
    GL_CALL(glDrawBuffer(GL_NONE));

    GL_CALL(glGenTextures(1, &frame_buffer->texture.handle));
    texture_bind(&frame_buffer->texture);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, frame_buffer->texture.handle, 0));

    return true;
}

void framebuffer_bind(FrameBuffer* frame_buffer) {
    texture_bind(&frame_buffer->texture);
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frame_buffer->handle));
    GL_CALL(glViewport(0, 0, frame_buffer->width, frame_buffer->height));
}

void framebuffer_unbind() {
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    GL_CALL(glViewport(0, 0, window_width(), window_height()));
}

void framebuffer_destroy(FrameBuffer* frame_buffer) {
    GL_CALL(glDeleteFramebuffers(1, &frame_buffer->handle));
    texture_destroy(&frame_buffer->texture);
}
