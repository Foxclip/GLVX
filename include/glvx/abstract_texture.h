#pragma once

#include "glvx/image.h"

namespace glvx {

enum class InterpolationType {
    Nearest,
    Linear
};

enum class WrappingType {
    ClampToEdge,
    Repeat,
    MirroredRepeat,
    ClampToBorder
};

class AbstractTexture {
public:
    void create(
        int width, int height, unsigned char* data = nullptr, int channels = 4, bool is_mask = false
    );
    int getID() const;
    int getWidth() const;
    int getHeight() const;
    void setInterpolation(InterpolationType type);
    InterpolationType getInterpolation() const;
    void setWrapping(WrappingType type);
    WrappingType getWrapping() const;
    void bind() const;
    void unbind() const;
    virtual Image readPixels() const;
    virtual void resize(int new_width, int new_height, bool blit_old_contents = true);
    virtual bool isRenderTexture() const;
    ~AbstractTexture();

protected:
    unsigned int m_id = 0;
    int m_width = 0;
    int m_height = 0;
    InterpolationType m_interpolation = InterpolationType::Nearest;
    WrappingType m_wrapping = WrappingType::ClampToEdge;
    void createTexture(
        int width, int height, unsigned char* data = nullptr, int channels = 4, bool is_mask = false,
        InterpolationType interp = InterpolationType::Nearest,
        WrappingType wrap = WrappingType::ClampToEdge
    );
    void resizeTexture(int new_width, int new_height, bool blit_old_contents, InterpolationType interp, WrappingType wrap);
    Image readPixelsRaw() const;

};

}
