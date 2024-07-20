#include "graphics/texture.h"

boolean texture_init(Texture* texture, char* path) {
    GL_CALL(glGenTextures(1, &texture->handle));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->handle));
    int number_of_channels = 0;
    char* data = stbi_load(path, &texture->width, &texture->height, &number_of_channels, 0);

    if (!data) {
        printf("[ERROR] Failed to load image from disc %s\n", path);
        return false;
    }

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

    GLenum format;
    switch (number_of_channels)
    {
    case 1:
        format = GL_LUMINANCE;
        break;
    case 2:
        format = GL_LUMINANCE_ALPHA;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    }

    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data));
    stbi_image_free(data);
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
}

void texture_bind(Texture* texture) {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture->handle));
}

void texture_destroy(Texture* texture) {
    GL_CALL(glDeleteTextures(1, &texture->handle));
}
