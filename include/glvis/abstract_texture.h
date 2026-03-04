#pragma once

#include "glvis/image.h"

namespace glvis {

class AbstractTexture {
public:
    void create(int width, int height, unsigned char* data = nullptr, int channels = 4);
    int getID() const;
    int getWidth() const;
    int getHeight() const;
    void bind() const;
    void unbind() const;
    Image readPixels() const;
    virtual void resize(int newWidth, int newHeight);
    ~AbstractTexture();

protected:
    unsigned int ID = 0;
    int width = 0;
    int height = 0;
    void createTexture(int width, int height, unsigned char* data = nullptr, int channels = 4);
    void resizeTexture(int newWidth, int newHeight);

};

}
