#include "glvx/image.h"
#include "glvx/color.h"
#include "glvx/vector.h"
#include <algorithm>
#include <cassert>

namespace glvx {

Image::Image(int width, int height, std::vector<unsigned char> data)
    : m_width(width), m_height(height), m_data(std::move(data)) {}

int Image::getWidth() const {
    return m_width;
}

int Image::getHeight() const {
    return m_height;
}

const std::vector<unsigned char>& Image::getData() const {
    return m_data;
}

Color Image::getPixel(int x, int y) const {
    assert(x >= 0 && x < m_width);
    assert(y >= 0 && y < m_height);
    size_t index = (y * m_width + x) * 4;
    return Color(m_data[index], m_data[index + 1], m_data[index + 2], m_data[index + 3]);
}

Color Image::getPixel(const Vector2i& pos) const {
    return getPixel(pos.x, pos.y);
}

void Image::flipY() {
    size_t rowSize = m_width * 4;
    for (int y = 0; y < m_height / 2; ++y) {
        int flipY = m_height - 1 - y;
        for (int x = 0; x < m_width; ++x) {
            size_t idxTop = y * rowSize + x * 4;
            size_t idxBottom = flipY * rowSize + x * 4;
            std::swap_ranges(
                m_data.begin() + idxTop,
                m_data.begin() + idxTop + 4,
                m_data.begin() + idxBottom
            );
        }
    }
}

}
