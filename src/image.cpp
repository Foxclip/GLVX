#include "glvis/image.h"
#include "glvis/color.h"

namespace glvis {

Image::Image(int width, int height, std::vector<unsigned char> data)
    : width(width), height(height), data(std::move(data)) {}

int Image::getWidth() const {
    return width;
}

int glvis::Image::getHeight() const {
    return height;
}

const std::vector<unsigned char>& Image::getData() const {
    return data;
}

ColorRGBA Image::getPixel(int x, int y) const {
    size_t index = (y * width + x) * 4;
    return ColorRGBA(
        data[index] / 255.0f,
        data[index + 1] / 255.0f,
        data[index + 2] / 255.0f,
        data[index + 3] / 255.0f
    );
}

}
