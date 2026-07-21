#include "glvis/image.h"
#include "glvis/color.h"
#include "glvis/vector.h"
#include <algorithm>
#include <cassert>

namespace glvis {

Image::Image(int width, int height, std::vector<unsigned char> data)
    : width(width), height(height), data(std::move(data)) {}

int Image::getWidth() const {
    return width;
}

int Image::getHeight() const {
    return height;
}

const std::vector<unsigned char>& Image::getData() const {
    return data;
}

Color Image::getPixel(int x, int y) const {
    assert(x >= 0 && x < width);
    assert(y >= 0 && y < height);
    size_t index = (y * width + x) * 4;
    return Color(data[index], data[index + 1], data[index + 2], data[index + 3]);
}

Color Image::getPixel(const Vector2i& pos) const {
    return getPixel(pos.x, pos.y);
}

void Image::flipY() {
    size_t rowSize = width * 4;
    for (int y = 0; y < height / 2; ++y) {
        int flipY = height - 1 - y;
        for (int x = 0; x < width; ++x) {
            size_t idxTop = y * rowSize + x * 4;
            size_t idxBottom = flipY * rowSize + x * 4;
            std::swap_ranges(
                data.begin() + idxTop,
                data.begin() + idxTop + 4,
                data.begin() + idxBottom
            );
        }
    }
}

}
