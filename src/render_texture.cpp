#include "glvis/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"

namespace glvis {

RenderTexture::RenderTexture(int width, int height) {
    GL_CALL(glGenFramebuffers(1, &FBO));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    createEmptyTexture(width, height);
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

RenderTexture::~RenderTexture() {
    GL_CALL(glDeleteFramebuffers(1, &FBO));
}

unsigned int RenderTexture::getFBO() const {
    return FBO;
}

void RenderTexture::resize(int newWidth, int newHeight) {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    resizeTexture(newWidth, newHeight);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

}
