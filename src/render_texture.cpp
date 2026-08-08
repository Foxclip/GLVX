#include "glvx/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/glvx_common.h"
#include "glvx/color.h"
#include "glvx/utils.h"
#include <algorithm>

namespace glvx {

RenderTexture::RenderTexture(int width, int height, int msaa_samples) {
    GL_CALL(glGenFramebuffers(1, &m_fbo));
    create(width, height, msaa_samples);
}

RenderTexture::~RenderTexture() {
    if (!has_active_gl_context()) {
        return;
    }
    if (m_msaa_texture != 0) {
        GL_CALL(glDeleteTextures(1, &m_msaa_texture));
        m_msaa_texture = 0;
    }
    if (m_msaa_fbo != 0) {
        GL_CALL(glDeleteFramebuffers(1, &m_msaa_fbo));
        m_msaa_fbo = 0;
    }
    GL_CALL(glDeleteFramebuffers(1, &m_fbo));
    m_fbo = 0;
}

unsigned int RenderTexture::getFBO() const {
    return m_fbo;
}

int RenderTexture::getSamples() const {
    return m_msaa_samples;
}

void RenderTexture::create(int width, int height, int msaa_samples) {
    START_TRY
    if (m_fbo == 0) {
        GL_CALL(glGenFramebuffers(1, &m_fbo));
    }

    if (msaa_samples > 1) {
        int max_samples = 0;
        GL_CALL(glGetIntegerv(GL_MAX_SAMPLES, &max_samples));
        m_msaa_samples = std::min(msaa_samples, max_samples);
    } else {
        m_msaa_samples = 0;
    }

    AbstractTexture::createTexture(width, height, nullptr, 4, false, m_interpolation, m_wrapping);

    if (m_msaa_samples > 0) {
        GL_CALL(glGenFramebuffers(1, &m_msaa_fbo));
        GL_CALL(glGenTextures(1, &m_msaa_texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_msaa_texture));
        GL_CALL(glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, m_msaa_samples, GL_RGBA, width, height, GL_TRUE
        ));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_fbo));
        GL_CALL(glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_msaa_texture, 0
        ));
        assert(GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    END_TRY
}

void RenderTexture::display() {
    if (m_msaa_samples == 0) {
        return;
    }

    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_msaa_fbo));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo));
    GL_CALL(glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTexture::resize(int new_width, int new_height, bool blit_old_contents) {
    if (m_fbo == 0) {
        GL_CALL(glGenFramebuffers(1, &m_fbo));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
    resizeTexture(new_width, new_height, blit_old_contents, m_interpolation, m_wrapping);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_fbo));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_id, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    if (m_msaa_samples > 0) {
        GL_CALL(glDeleteTextures(1, &m_msaa_texture));
        m_msaa_texture = 0;
        GL_CALL(glGenTextures(1, &m_msaa_texture));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_msaa_texture));
        GL_CALL(glTexImage2DMultisample(
            GL_TEXTURE_2D_MULTISAMPLE, m_msaa_samples, GL_RGBA, new_width, new_height, GL_TRUE
        ));
        GL_CALL(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, m_msaa_fbo));
        GL_CALL(glFramebufferTexture2D(
            GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_msaa_texture, 0
        ));
        assert(GL_CALL(glCheckFramebufferStatus(GL_FRAMEBUFFER)) == GL_FRAMEBUFFER_COMPLETE);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

bool RenderTexture::isRenderTexture() const {
    return true;
}

unsigned int RenderTexture::getRenderTargetFbo() const {
    if (m_msaa_samples > 0) {
        return m_msaa_fbo;
    }
    return m_fbo;
}

int RenderTexture::getRenderTargetWidth() const {
    return m_width;
}

int RenderTexture::getRenderTargetHeight() const {
    return m_height;
}

}
