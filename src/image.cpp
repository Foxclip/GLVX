#include "glvis/image.h"
#include "glvis/color.h"

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
    size_t index = (y * width + x) * 4;
    return Color(data[index], data[index + 1], data[index + 2], data[index + 3]);
}

}
