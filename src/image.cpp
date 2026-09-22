#include "glvx/image.h"
#include "glvx/color.h"
#include "glvx/vector.h"
#include <algorithm>
#include <cassert>
#include <cmath>

namespace glvx {

static unsigned char unpremultiply_channel(unsigned char channel, unsigned char alpha) {
    float value = std::round(static_cast<float>(channel) * 255.0f / alpha);
    return static_cast<unsigned char>(std::min(value, 255.0f));
}

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
    size_t row_size = m_width * 4;
    for (int y = 0; y < m_height / 2; ++y) {
        int flip_y = m_height - 1 - y;
        for (int x = 0; x < m_width; ++x) {
            size_t idx_top = y * row_size + x * 4;
            size_t idx_bottom = flip_y * row_size + x * 4;
            std::swap_ranges(
                m_data.begin() + idx_top,
                m_data.begin() + idx_top + 4,
                m_data.begin() + idx_bottom
            );
        }
    }
}

void Image::unpremultiply() {
    size_t row_size = m_width * 4;
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            size_t idx = y * row_size + x * 4;
            unsigned char a = m_data[idx + 3];
            if (a == 0) {
                m_data[idx] = 0;
                m_data[idx + 1] = 0;
                m_data[idx + 2] = 0;
            } else if (a < 255) {
                m_data[idx] = unpremultiply_channel(m_data[idx], a);
                m_data[idx + 1] = unpremultiply_channel(m_data[idx + 1], a);
                m_data[idx + 2] = unpremultiply_channel(m_data[idx + 2], a);
            }
        }
    }
}

}
