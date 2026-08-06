#include "glvx/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/glvx_common.h"
#include "glvx/color.h"
#include "glvx/utils.h"
#include <algorithm>

namespace glvx {

RenderTexture::RenderTexture(int width, int height, int msaa_samples) {
    GL_CALL(glGenFramebuffers(1, &FBO));
    create(width, height, msaa_samples);
}

RenderTexture::~RenderTexture() {
    if (!has_active_gl_context()) {
        return;
    }
    if (msaa_texture != 0) {
        GL_CALL(glDeleteTextures(1, &msaa_texture));
        msaa_texture = 0;
    }
    if (msaa_fbo != 0) {
        GL_CALL(glDeleteFramebuffers(1, &msaa_fbo));
        msaa_fbo = 0;
    }
    GL_CALL(glDeleteFramebuffers(1, &FBO));
    FBO = 0;
}

unsigned int RenderTexture::getFBO() const {
    return FBO;
}

int RenderTexture::getSamples() const {
    return msaa_samples;
}

void RenderTexture::create(int width, int height, int msaa_samples) {
    START_TRY
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    if (msaa_samples > 1) {
        int max_samples = 0;
        GL_CALL(glGetIntegerv(GL_MAX_SAMPLES, &max_samples));
        this->msaa_samples = std::min(msaa_samples, max_samples);
    } else {
        this->msaa_samples = 0;
    }

    AbstractTexture::createTexture(width, height, nullptr, 4, false, interpolation, wrapping);

    if (this->msaa_samples > 0) {
        GL_CALL(glGenFramebuffers(1, &msaa_fbo));
        GL_CALL(glGenTextures(1, &msaa_texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_texture));
        GL_CALL(glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, this->msaa_samples, GL_RGBA, width, height, GL_TRUE
        ));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, msaa_fbo));
        GL_CALL(glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaa_texture, 0
        ));
        assert(GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    END_TRY
}

void RenderTexture::display() {
    if (msaa_samples == 0) {
        return;
    }

    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_fbo));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO));
    GL_CALL(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
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

    if (msaa_samples > 0) {
        GL_CALL(glDeleteTextures(1, &msaa_texture));
        msaa_texture = 0;
        GL_CALL(glGenTextures(1, &msaa_texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, msaa_texture));
        GL_CALL(glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, msaa_samples, GL_RGBA, newWidth, newHeight, GL_TRUE
        ));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, msaa_fbo));
        GL_CALL(glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, msaa_texture, 0
        ));
        assert(GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

bool RenderTexture::isRenderTexture() const {
    return true;
}

unsigned int RenderTexture::getRenderTargetFbo() const {
    if (msaa_samples > 0) {
        return msaa_fbo;
    }
    return FBO;
}

int RenderTexture::getRenderTargetWidth() const {
    return width;
}

int RenderTexture::getRenderTargetHeight() const {
    return height;
}

}
