#include "graphics/texture.h"

boolean texture_init(Texture* texture, char* path) {
    GL_CALL(glGenTextures(1, &texture->handle));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->handle));
    int number_of_channels = 0;
    char* data = stbi_load(path, &texture->width, &texture->height, &number_of_channels, 0);

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));

    stbi_image_free(data);
}

void texture_bind(Texture* texture) {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->handle));
}

void texture_destroy(Texture* texture) {
    GL_CALL(glDeleteTextures(1, &texture->handle));
}
