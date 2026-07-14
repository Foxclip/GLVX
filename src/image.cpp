#include "glvis/image.h"
#include "glvis/color.h"
#include "glvis/vector.h"
#include "glvis/utils.h"
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
    flip_pixels_y(data, width, height);
}

}
