#pragma once

#include <vector>
#include "glvis/color.h"

namespace glvis {

template<typename ColorType>
class Image {
public:
    Image(int width, int height, std::vector<unsigned char> data);

    int getWidth() const;
    int getHeight() const;
    const std::vector<unsigned char>& getData() const;
    ColorType getPixel(int x, int y) const;

private:
    int width;
    int height;
    std::vector<unsigned char> data;
};

}
