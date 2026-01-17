#include "glvis/image.h"
#include "glvis/color.h"

namespace glvis {

template<typename ColorType>
Image<ColorType>::Image(int width, int height, std::vector<unsigned char> data)
    : width(width), height(height), data(std::move(data)) {}

template<typename ColorType>
int Image<ColorType>::getWidth() const {
    return width;
}

template<typename ColorType>
int Image<ColorType>::getHeight() const {
    return height;
}

template<typename ColorType>
const std::vector<unsigned char>& Image<ColorType>::getData() const {
    return data;
}

template<typename ColorType>
ColorType Image<ColorType>::getPixel(int x, int y) const {
    constexpr size_t channels = sizeof(ColorType);
    size_t index = (y * width + x) * channels;
    if constexpr (channels == 3) {
        return ColorType(
            data[index] / 255.0f,
            data[index + 1] / 255.0f,
            data[index + 2] / 255.0f
        );
    } else if constexpr (channels == 4) {
        return ColorType(
            data[index] / 255.0f,
            data[index + 1] / 255.0f,
            data[index + 2] / 255.0f,
            data[index + 3] / 255.0f
        );
    } else {
        static_assert(false, "Unsupported color type");
    }
}

template class Image<ColorRGB>;
template class Image<ColorRGBA>;

}
