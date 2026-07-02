#pragma once

#include "glvis/image.h"

namespace glvis {

enum class InterpolationType {
    Nearest,
    Linear
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
    void bind() const;
    void unbind() const;
    Image readPixels() const;
    virtual void resize(int newWidth, int newHeight, bool blitOldContents = true);
    ~AbstractTexture();

protected:
    unsigned int ID = 0;
    int width = 0;
    int height = 0;
    InterpolationType interpolation = InterpolationType::Nearest;
    void createTexture(
        int width, int height, unsigned char* data = nullptr, int channels = 4, bool is_mask = false,
        InterpolationType interp = InterpolationType::Nearest
    );
    void resizeTexture(int newWidth, int newHeight, bool blitOldContents, InterpolationType interp);

};

}
