#include "glvis/abstract_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <cassert>

namespace glvis {

int AbstractTexture::getID() const {
    return ID;
}

int AbstractTexture::getWidth() const {
    return width;
}

int AbstractTexture::getHeight() const {
    return height;
}

void AbstractTexture::bind() const {
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
}

void AbstractTexture::unbind() const {
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Image AbstractTexture::readPixels() const {
    std::vector<unsigned char> data(width * height * 4);
    bind();
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
    unbind();
    return Image(width, height, std::move(data));
}

void AbstractTexture::resize(int newWidth, int newHeight) {
    resizeTexture(newWidth, newHeight);
}

AbstractTexture::~AbstractTexture() {
    GL_CALL(glDeleteTextures(1, &ID));
}

void AbstractTexture::createTexture(int width, int height, unsigned char* data, int channels) {
    START_TRY
    assert(width > 0);
    assert(height > 0);
    if (glfwGetCurrentContext() == nullptr) {
        throw std::runtime_error("Texture::create called outside of GLFW context");
    }
    if (channels == 4) {
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    } else {
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    }
    GL_CALL(glGenTextures(1, &ID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
    GLenum format;
    switch (channels) {
        case 1: format = GL_RED; break;
        case 2: format = GL_RG; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: throw std::runtime_error("Invalid number of channels: " + std::to_string(channels));
    }
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    this->width = width;
    this->height = height;
    END_TRY
}

void AbstractTexture::resizeTexture(int newWidth, int newHeight) {
    assert(newWidth > 0);
    assert(newHeight > 0);
    if (newWidth == width && newHeight == height) {
        return;
    }

    // Create a new texture with the desired size
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    GLuint newTextureID;
    GL_CALL(glGenTextures(1, &newTextureID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, newTextureID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    // Create framebuffers
    GLuint srcFBO, dstFBO;
    GL_CALL(glGenFramebuffers(1, &srcFBO));
    GL_CALL(glGenFramebuffers(1, &dstFBO));

    // Attach original texture to source FBO
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO));
    GL_CALL(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    assert(GL_CALL(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);

    // Attach new texture to destination FBO
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO));
    GL_CALL(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTextureID, 0));

    // Blit with bilinear interpolation
    GL_CALL(glBlitFramebuffer(0, 0, width, height, 0, 0, newWidth, newHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR));

    // Clean up
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glDeleteFramebuffers(1, &srcFBO));
    GL_CALL(glDeleteFramebuffers(1, &dstFBO));

    // Replace the old texture
    GL_CALL(glDeleteTextures(1, &ID));
    ID = newTextureID;
    this->width = newWidth;
    this->height = newHeight;
}

}
