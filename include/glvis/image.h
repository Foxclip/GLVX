#pragma once

#include <vector>
#include "glvis/color.h"
#include "glvis/vector.h"

namespace glvis {

class Image {
public:
    Image(int width, int height, std::vector<unsigned char> data);

    int getWidth() const;
    int getHeight() const;
    const std::vector<unsigned char>& getData() const;
    Color getPixel(int x, int y) const;
    Color getPixel(const Vector2i& pos) const;
    Color getPixel(const Vector2f& pos) const;

private:
    int width;
    int height;
    std::vector<unsigned char> data;
};

}
