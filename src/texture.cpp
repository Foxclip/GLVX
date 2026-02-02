#include "glvis/texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "glvis/stb_image.h"
#include "glvis/glvis_common.h"
#include <memory>

namespace glvis {

Texture::Texture(int width, int height) {
    createTexture(width, height);
}

Texture::Texture(unsigned char* data, int width, int height) {
    createTexture(width, height, data);
}

Texture::Texture(const std::filesystem::path& path) {
    START_TRY
    if (!std::filesystem::exists(path)) {
        throw std::runtime_error("File not found: " + path.string());
    }
    int width, height, nrChannels;
    std::unique_ptr<unsigned char, decltype(&stbi_image_free)> data(
        stbi_load(path.string().c_str(), &width, &height, &nrChannels, 4),
        stbi_image_free
    );
    if (!data) {
        throw std::runtime_error("Failed to load texture: " + path.string());
    }
    this->path = path;
    createTexture(width, height, data.get());
    END_TRY
}

const std::filesystem::path& Texture::getPath() const {
    return path;
}

}
