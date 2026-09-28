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

void RenderTexture::copyFrom(const RenderTarget& source) {
    const int source_width = source.getRenderTargetWidth();
    const int source_height = source.getRenderTargetHeight();
    if (source_width == 0 || source_height == 0) {
        return;
    }
    const unsigned int source_fbo = source.getRenderTargetFbo();
    // Only the default framebuffer has a read buffer, so glReadBuffer is only
    // needed (and only allowed) when the source is a window. A minimized
    // window's default framebuffer has no back buffer, and glReadBuffer of a
    // missing buffer generates GL_INVALID_OPERATION.
    GLint previous_read_buffer = 0;
    if (source_fbo == 0) {
        GL_CALL(glGetIntegerv(GL_READ_BUFFER, &previous_read_buffer));
        // Read from the back buffer, i.e. the one currently being drawn to,
        // since Window::readPixels leaves the read buffer set to GL_FRONT.
        GL_CALL(glReadBuffer(GL_BACK));
    }
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, source_fbo));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getRenderTargetFbo()));
    // Flip the destination vertically: the source framebuffer stores its
    // bottom row at v=0, but the library's UV convention maps v=0 to the top
    // of a shape, so this makes the copy display like an image texture does.
    GL_CALL(glBlitFramebuffer(
        0, 0, source_width, source_height,
        0, m_height, m_width, 0,
        GL_COLOR_BUFFER_BIT,
        GL_LINEAR
    ));
    GL_CALL(glBindFramebuffer(GL_READ_FRAMEBUFFER, 0));
    GL_CALL(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    if (source_fbo == 0) {
        GL_CALL(glReadBuffer(previous_read_buffer));
    }
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

Image RenderTexture::readPixels() const {
    // Render texture contents are stored premultiplied (like any render
    // target); convert back to straight alpha for the caller.
    Image image = AbstractTexture::readPixels();
    image.unpremultiply();
    return image;
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
