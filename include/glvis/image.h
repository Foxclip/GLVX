#pragma once

#include <vector>
#include "glvis/color.h"

namespace glvis {

class Image {
public:
    Image(int width, int height, std::vector<unsigned char> data);

    int getWidth() const;
    int getHeight() const;
    const std::vector<unsigned char>& getData() const;
    Color getPixel(int x, int y) const;

private:
    int width;
    int height;
    std::vector<unsigned char> data;
};

}
