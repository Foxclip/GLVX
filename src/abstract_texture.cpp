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
    m_interpolation = type;
    if (m_id != 0) {
        GLenum filter = filterFromInterpolation(type);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}

InterpolationType AbstractTexture::getInterpolation() const {
    return m_interpolation;
}

void AbstractTexture::setWrapping(WrappingType type) {
    m_wrapping = type;
    if (m_id != 0) {
        GLenum wrap = wrapFromWrapping(type);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
}

WrappingType AbstractTexture::getWrapping() const {
    return m_wrapping;
}

void AbstractTexture::create(int width, int height, unsigned char* data, int channels, bool is_mask) {
    createTexture(width, height, data, channels, is_mask);
}

int AbstractTexture::getID() const {
    return m_id;
}

int AbstractTexture::getWidth() const {
    return m_width;
}

int AbstractTexture::getHeight() const {
    return m_height;
}

void AbstractTexture::bind() const {
    assert(m_id != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(m_id)));
    GL_CALL(glActiveTexture(GL_TEXTURE0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
}

void AbstractTexture::unbind() const {
    assert(m_id != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(m_id)));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

Image AbstractTexture::readPixels() const {
    return readPixelsRaw();
}

void AbstractTexture::resize(int new_width, int new_height, bool blit_old_contents) {
    resizeTexture(new_width, new_height, blit_old_contents, m_interpolation, m_wrapping);
}

bool AbstractTexture::isRenderTexture() const {
    return false;
}

AbstractTexture::~AbstractTexture() {
    if (m_id == 0) {
        return;
    }
    if (!has_active_gl_context()) {
        return;
    }
    assert(GL_CALL(glIsTexture(m_id)));
    GL_CALL(glDeleteTextures(1, &m_id));
}

void AbstractTexture::createTexture(int width, int height, const unsigned char* data, int channels, bool is_mask, InterpolationType interp, WrappingType wrap) {
    START_TRY
    assert(glfwGetCurrentContext() != nullptr);
    assert(width > 0);
    assert(height > 0);
    assert(!is_mask || channels == 1); // only 1 channel for masks

    // Delete existing texture if any
    if (m_id != 0) {
        assert(glfwGetCurrentContext() != nullptr);
        assert(GL_CALL(glIsTexture(m_id)));
        GL_CALL(glDeleteTextures(1, &m_id));
        m_id = 0;
    }

    switch (channels) {
        case 4: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4)); break;
        case 3: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        case 2: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 2)); break;
        case 1: GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1)); break;
        default: throw std::runtime_error("Invalid number of channels: " + std::to_string(channels));
    }
    GL_CALL(glGenTextures(1, &m_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
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
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_interpolation = interp;
    m_wrapping = wrap;
    END_TRY
}

// Bilinear sample of one channel at a texel-center coordinate (texel i
// occupies [i - 0.5, i + 0.5)), clamping to the edge texel, matching
// GL_LINEAR blit sampling.
static float sample_linear_image(const std::vector<float>& src, int sw, int sh, float sx, float sy, int c) {
    const float scx = std::clamp(sx, 0.0f, static_cast<float>(sw - 1));
    const float scy = std::clamp(sy, 0.0f, static_cast<float>(sh - 1));
    const int x0 = static_cast<int>(scx);
    const int y0 = static_cast<int>(scy);
    const int x1 = std::min(x0 + 1, sw - 1);
    const int y1 = std::min(y0 + 1, sh - 1);
    const float fx = scx - static_cast<float>(x0);
    const float fy = scy - static_cast<float>(y0);
    const float top = (1.0f - fx) * src[(size_t)(y0 * sw + x0) * 4 + c] + fx * src[(size_t)(y0 * sw + x1) * 4 + c];
    const float bot = (1.0f - fx) * src[(size_t)(y1 * sw + x0) * 4 + c] + fx * src[(size_t)(y1 * sw + x1) * 4 + c];
    return (1.0f - fy) * top + fy * bot;
}

// Resamples a RGBA image (values in [0, 255]) from (sw, sh) to (dw, dh) with
// the same texel-center mapping as glBlitFramebuffer. Done on the CPU because
// this machine's GL driver (Mesa/llvmpipe 24.0.9) truncates float -> UNORM8
// in glBlitFramebuffer instead of rounding to nearest (e.g. 127.5 -> 127),
// which breaks the pixel-exact values the tests expect.
static void resample_image(
    const std::vector<float>& src, int sw, int sh,
    std::vector<float>& dst, int dw, int dh, InterpolationType interp
) {
    for (int y = 0; y < dh; ++y) {
        const float sy = (static_cast<float>(y) + 0.5f) * (static_cast<float>(sh) / dh) - 0.5f;
        for (int x = 0; x < dw; ++x) {
            const float sx = (static_cast<float>(x) + 0.5f) * (static_cast<float>(sw) / dw) - 0.5f;
            for (int c = 0; c < 4; ++c) {
                float v;
                if (interp == InterpolationType::Linear) {
                    v = sample_linear_image(src, sw, sh, sx, sy, c);
                } else {
                    const int ix = std::clamp(static_cast<int>(std::floor(sx + 0.5f)), 0, sw - 1);
                    const int iy = std::clamp(static_cast<int>(std::floor(sy + 0.5f)), 0, sh - 1);
                    v = src[(size_t)(iy * sw + ix) * 4 + c];
                }
                dst[(size_t)(y * dw + x) * 4 + c] = v;
            }
        }
    }
}

void AbstractTexture::resizeTexture(int new_width, int new_height, bool blit_old_contents, InterpolationType interp, WrappingType wrap) {
    assert(m_id != 0);
    assert(glfwGetCurrentContext() != nullptr);
    assert(GL_CALL(glIsTexture(m_id)));
    assert(new_width > 0);
    assert(new_height > 0);
    if (new_width == m_width && new_height == m_height) {
        return;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    GLuint new_texture_id;
    GL_CALL(glGenTextures(1, &new_texture_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, new_texture_id));
    if (m_float_storage) {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, new_width, new_height, 0, GL_RGBA, GL_HALF_FLOAT, nullptr));
    } else {
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, new_width, new_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr));
    }
    GLenum filter = filterFromInterpolation(interp);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
    GLenum wrap_mode = wrapFromWrapping(wrap);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode));

    if (blit_old_contents) {
        std::vector<float> src(m_width * m_height * 4);
        if (m_float_storage) {
            GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
            GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, src.data()));
        } else {
            Image old = readPixelsRaw();
            const std::vector<unsigned char>& old_data = old.getData();
            for (size_t i = 0; i < src.size(); ++i) {
                src[i] = static_cast<float>(old_data[i]);
            }
        }
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

        std::vector<float> dst(new_width * new_height * 4);
        resample_image(src, m_width, m_height, dst, new_width, new_height, interp);

        GL_CALL(glBindTexture(GL_TEXTURE_2D, new_texture_id));
        if (m_float_storage) {
            GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, new_width, new_height, GL_RGBA, GL_HALF_FLOAT, dst.data()));
        } else {
            std::vector<unsigned char> bytes(dst.size());
            for (size_t i = 0; i < dst.size(); ++i) {
                const int v = static_cast<int>(std::lroundf(std::clamp(dst[i], 0.0f, 255.0f)));
                bytes[i] = static_cast<unsigned char>(v);
            }
            GL_CALL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, new_width, new_height, GL_RGBA, GL_UNSIGNED_BYTE, bytes.data()));
        }
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    } else {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }

    GL_CALL(glDeleteTextures(1, &m_id));
    m_id = new_texture_id;
    m_width = new_width;
    m_height = new_height;
}

Image AbstractTexture::readPixelsRaw() const {
    std::vector<unsigned char> data(m_width * m_height * 4);
    bind();
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data()));
    unbind();

    return Image(m_width, m_height, std::move(data));
}

void AbstractTexture::createFloatTexture(int width, int height, InterpolationType interp, WrappingType wrap) {
    START_TRY
    assert(glfwGetCurrentContext() != nullptr);
    assert(width > 0);
    assert(height > 0);

    if (m_id != 0) {
        GL_CALL(glDeleteTextures(1, &m_id));
        m_id = 0;
    }

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    GL_CALL(glGenTextures(1, &m_id));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, m_id));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_HALF_FLOAT, nullptr));
    const GLenum filter = filterFromInterpolation(interp);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter));
    const GLenum wrap_mode = wrapFromWrapping(wrap);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    m_width = width;
    m_height = height;
    m_channels = 4;
    m_interpolation = interp;
    m_wrapping = wrap;
    m_float_storage = true;
    END_TRY
}

Image AbstractTexture::readPixelsFloatRaw() const {
    std::vector<float> data(m_width * m_height * 4);
    bind();
    GL_CALL(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data.data()));
    unbind();

    // Convert to UNORM8 with round-to-nearest, as the GL spec requires for
    // fixed-point conversion (driver-side conversion is not guaranteed to).
    std::vector<unsigned char> bytes(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        const int v = static_cast<int>(std::lroundf(std::clamp(data[i] * 255.0f, 0.0f, 255.0f)));
        bytes[i] = static_cast<unsigned char>(v);
    }
    return Image(m_width, m_height, std::move(bytes));
}

}
