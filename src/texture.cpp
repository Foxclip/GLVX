#include "glvx/texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include "glvx/stb_image.h"
#include "glvx/glvx_common.h"
#include <memory>
#include <stdexcept>

namespace glvx {

static void setUnpackAlignment(int channels) {
    switch (channels) {
        case 4: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); break;
        case 3: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        case 2: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 2)); break;
        case 1: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        default: throw std::runtime_error("Invalid number of channels: " + std::to_string(channels));
    }
}

static GLenum formatFromChannels(int channels) {
    switch (channels) {
        case 1: return GL_RED;
        case 2: return GL_RG;
        case 3: return GL_RGB;
        case 4: return GL_RGBA;
    }
    throw std::runtime_error("Invalid number of channels: " + std::to_string(channels));
}

Texture::Texture(int width, int height, InterpolationType interp) {
    createTexture(width, height, nullptr, 4, false, interp);
}

Texture::Texture(unsigned char* data, int width, int height, int channels, InterpolationType interp) {
    createTexture(width, height, data, channels, false, interp);
}

Texture::Texture(const std::filesystem::path& path, InterpolationType interp) {
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
    createTexture(width, height, data.get(), 4, false, interp);
    END_TRY
}

void Texture::update(const unsigned char* data, int width, int height, int channels) {
    assert(glfwGetCurrentContext() != nullptr);
    assert(data != nullptr);
    assert(width > 0);
    assert(height > 0);
    if (m_id == 0 || m_width != width || m_height != height || m_channels != channels) {
        // No texture yet, or a size/channel change: (re)create it, keeping
        // the current filtering and wrapping settings.
        createTexture(width, height, data, channels, false, m_interpolation, m_wrapping);
        return;
    }
    // Same size and channels: replace the texels in place so the GL texture
    // object (and its parameters) are reused.
    setUnpackAlignment(channels);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
    GL_CALL(glTexSubImage2D(
        GL_TEXTURE_2D, 0, 0, 0,
        width, height,
        formatFromChannels(channels),
        GL_UNSIGNED_BYTE,
        data
    ));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Image Texture::readPixels() const {
    // Unlike Window::readPixels (glReadPixels, which is bottom-up),
    // glGetTexImage already returns the texels in data order (row 0 first),
    // which is also the order the texture is drawn, so no flipping is needed.
    return readPixelsRaw();
}

}
