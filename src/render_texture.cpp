#include "glvis/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/color.h"
#include "glvis/utils.h"

namespace glvis {

RenderTexture::RenderTexture(int width, int height) {
    GL_CALL(glGenFramebuffers(1, &FBO));
    create(width, height);
}

RenderTexture::~RenderTexture() {
    GL_CALL(glDeleteFramebuffers(1, &FBO));
}

unsigned int RenderTexture::getFBO() const {
    return FBO;
}

void RenderTexture::create(int width, int height) {
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    AbstractTexture::createTexture(width, height, nullptr, 4, false, interpolation, wrapping);

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTexture::resize(int newWidth, int newHeight, bool blitOldContents) {
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
    resizeTexture(newWidth, newHeight, blitOldContents, interpolation, wrapping);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

Image RenderTexture::readPixels() const {
    return readPixelsInternal(false);
}

unsigned int RenderTexture::getRenderTargetFbo() const {
    return FBO;
}

int RenderTexture::getRenderTargetWidth() const {
    return width;
}

int RenderTexture::getRenderTargetHeight() const {
    return height;
}

}
