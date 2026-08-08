#include "glvx_tests/glvx_tests_common.h"

Window window;

bool checkPixelColor(test::Test& test, const Image& image, int start_x, int start_y, int end_x, int end_y, const Color& expected_color) {
    for (int x = start_x; x < end_x; ++x) {
        for (int y = start_y; y < end_y; ++y) {
            T_CONTAINER(std::format("x: {}, y: {}", x, y));
            if (!T_COMPARE(image.getPixel(x, y), expected_color, &Color::toString)) {
                return false;
            }
        }
    }
    return true;
}

bool checkPixelColor(test::Test& test, const Image& image, const Vector2i& start, const Vector2i& end, const Color& expected_color) {
    return checkPixelColor(test, image, start.x, start.y, end.x, end.y, expected_color);
}

bool compareImages(test::Test& test, const Image& image1, const Image& image2) {
    int width = image1.getWidth();
    int height = image1.getHeight();
    if (image2.getWidth() != width || image2.getHeight() != height) {
        T_MESSAGE("Images have different dimensions");
        return false;
    }
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            T_CONTAINER(std::format("x: {}, y: {}", x, y));
            if (!T_COMPARE(image1.getPixel(x, y), image2.getPixel(x, y), &Color::toString)) {
                return false;
            }
        }
    }
    return true;
}
