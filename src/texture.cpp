#include "glvis/texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "glvis/stb_image.h"
#include "glvis/glvis_common.h"

namespace glvis {

Texture::Texture(int width, int height) {
    createEmptyTexture(width, height);
}

Texture::Texture(unsigned char* data, int width, int height) {
    this->width = width;
    this->height = height;
    create(data);
}

Texture::Texture(const std::filesystem::path& path) {
    START_TRY
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.string().c_str(), &width, &height, &nrChannels, 4);
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path.string() + "\n");
    }
    create(data);
    stbi_image_free(data);
    END_TRY
}

const std::filesystem::path& Texture::getPath() const {
    return path;
}

void Texture::create(unsigned char* data) {
    START_TRY
    assert(data);
    if (glfwGetCurrentContext() == nullptr) {
        throw std::runtime_error("Texture::create called outside of GLFW context");
    }
    GL_CALL(glGenTextures(1, &ID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    END_TRY
}

}
