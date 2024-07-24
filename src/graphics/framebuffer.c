#include "graphics/framebuffer.h"

boolean framebuffer_init(FrameBuffer* frame_buffer, u32 width, u32 height) {
    GL_CALL(glGenFramebuffers(1, &frame_buffer->handle));
    frame_buffer->width = width;
    frame_buffer->height = height;

    GL_CALL(glGenTextures(1, &frame_buffer->texture.handle));
    texture_bind(&frame_buffer->texture);
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    frame_buffer->texture.width = width;
    frame_buffer->texture.height = height;

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frame_buffer->handle, 0));
    GL_CALL(glDrawBuffer(GL_NONE));

    if (!glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
        printf("[ERROR] Frame buffer incomplete!\n");
        return false;    
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));    
    GL_CALL(glDrawBuffer(GL_BACK));

    return true;
}

void framebuffer_bind(FrameBuffer* frame_buffer) {
    texture_bind(&frame_buffer->texture);
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer->handle));
    GL_CALL(glViewport(0, 0, frame_buffer->width, frame_buffer->height));    
    GL_CALL(glDrawBuffer(GL_NONE));
    GL_CALL(glReadBuffer(GL_NONE));
}

void framebuffer_unbind() {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glViewport(0, 0, window_width(), window_height()));
}

void framebuffer_destroy(FrameBuffer* frame_buffer) {
    GL_CALL(glDeleteFramebuffers(1, &frame_buffer->handle));
    texture_destroy(&frame_buffer->texture);
}
