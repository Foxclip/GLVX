#pragma once

#include <vector>
#include "glvx/color.h"
#include "glvx/vector.h"

namespace glvx {

class Image {
public:
    Image(int width, int height, std::vector<unsigned char> data);

    int getWidth() const;
    int getHeight() const;
    const std::vector<unsigned char>& getData() const;
    Color getPixel(int x, int y) const;
    Color getPixel(const Vector2i& pos) const;
    void flipY();

private:
    int m_width;
    int m_height;
    std::vector<unsigned char> m_data;
};

}
