#include "glvx/abstract_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/glvx_common.h"
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include <cassert>

namespace glvx {

static GLenum filterFromInterpolation(InterpolationType interp) {
    switch (interp) {
        case InterpolationType::Nearest: return GL_NEAREST;
        case InterpolationType::Linear: return GL_LINEAR;
    }
    return GL_NEAREST;
}

static GLenum wrapFromWrapping(WrappingType wrap) {
    switch (wrap) {
        case WrappingType::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case WrappingType::Repeat: return GL_REPEAT;
        case WrappingType::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case WrappingType::ClampToBorder: return GL_CLAMP_TO_BORDER;
    }
    return GL_CLAMP_TO_EDGE;
}

void AbstractTexture::setInterpolation(InterpolationType type) {
    this->interpolation = type;
    if (ID != 0) {
        GLenum filter = filterFromInterpolation(type);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}

InterpolationType AbstractTexture::getInterpolation() const {
    return interpolation;
}

void AbstractTexture::setWrapping(WrappingType type) {
    this->wrapping = type;
    if (ID != 0) {
        GLenum wrap = wrapFromWrapping(type);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}

WrappingType AbstractTexture::getWrapping() const {
    return wrapping;
}

void AbstractTexture::create(int width, int height, unsigned char* data, int channels, bool is_mask) {
    createTexture(width, height, data, channels, is_mask);
}

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
    assert(ID != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(ID)));
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, ID));
}

void AbstractTexture::unbind() const {
    assert(ID != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(ID)));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Image AbstractTexture::readPixels() const {
    return readPixelsRaw();
}

void AbstractTexture::resize(int newWidth, int newHeight, bool blitOldContents) {
    resizeTexture(newWidth, newHeight, blitOldContents, this->interpolation, this->wrapping);
}

bool AbstractTexture::isRenderTexture() const {
    return false;
}

AbstractTexture::~AbstractTexture() {
    if (ID == 0) {
        return;
    }
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(ID)));
    GL_CALL(glDeleteTextures(1, &ID));
}

void AbstractTexture::createTexture(int width, int height, unsigned char* data, int channels, bool is_mask, InterpolationType interp, WrappingType wrap) {
    START_TRY
    assert(glfwGetCurrentContext() != nullptr);
    assert(width > 0);
    assert(height > 0);
    assert(!is_mask || channels == 1); // only 1 channel for masks

    // Delete existing texture if any
    if (ID != 0) {
        assert(glfwGetCurrentContext() != nullptr);
        assert(GL_CALL(glIsTexture(ID)));
        GL_CALL(glDeleteTextures(1, &ID));
        ID = 0;
    }

    switch (channels) {
        case 4: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); break;
        case 3: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        case 2: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 2)); break;
        case 1: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        default: throw std::runtime_error("Invalid number of channels: " + std::to_string(channels));
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
    GLenum filter = filterFromInterpolation(interp);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
    GLenum wrapMode = wrapFromWrapping(wrap);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
    if (channels == 1) {
        if (is_mask) {
            // return (1, 1, 1, R) instead of (R, 0, 0, 1)
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_ONE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_ONE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_ONE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED));
        } else {
            // swizzle to return (R, R, R, 1) instead of (R, 0, 0, 1)
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED));
        }
    }
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    this->width = width;
    this->height = height;
    this->interpolation = interp;
    this->wrapping = wrap;
    END_TRY
}

void AbstractTexture::resizeTexture(int newWidth, int newHeight, bool blitOldContents, InterpolationType interp, WrappingType wrap) {
    assert(ID != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(ID)));
    assert(newWidth > 0);
    assert(newHeight > 0);
    if (newWidth == width && newHeight == height) {
        return;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    GLuint newTextureID;
    GL_CALL(glGenTextures(1, &newTextureID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, newTextureID));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, newWidth, newHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    GLenum filter = filterFromInterpolation(interp);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
    GLenum wrapMode = wrapFromWrapping(wrap);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    if (blitOldContents) {
        GLuint srcFBO, dstFBO;
        GL_CALL(glGenFramebuffers(1, &srcFBO));
        GL_CALL(glGenFramebuffers(1, &dstFBO));

        GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO));
        GL_CALL(glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
        assert(GL_CALL(glCheckFramebufferStatus(GL_READ_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);

        GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO));
        GL_CALL(glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newTextureID, 0));

        GLenum blitFilter = filterFromInterpolation(interp);
        GL_CALL(glBlitFramebuffer(0, 0, width, height, 0, 0, newWidth, newHeight, GL_COLOR_BUFFER_BIT, blitFilter));

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GL_CALL(glDeleteFramebuffers(1, &srcFBO));
        GL_CALL(glDeleteFramebuffers(1, &dstFBO));
    }

    GL_CALL(glDeleteTextures(1, &ID));
    ID = newTextureID;
    this->width = newWidth;
    this->height = newHeight;
}

Image AbstractTexture::readPixelsRaw() const {
    std::vector<unsigned char> data(width * height * 4);
    bind();
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
    unbind();

    return Image(width, height, std::move(data));
}

}
