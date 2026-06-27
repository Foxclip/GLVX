#include "glvis/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"

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
    // Create FBO if it doesn't exist
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    // Create texture first (while FBO is not bound)
    AbstractTexture::createTexture(width, height);

    // Then attach to FBO
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTexture::resize(int newWidth, int newHeight, bool blitOldContents) {
    // Create FBO if it doesn't exist
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
    resizeTexture(newWidth, newHeight, blitOldContents);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

}
