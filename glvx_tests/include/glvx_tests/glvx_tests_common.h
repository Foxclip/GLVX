#pragma once

#include "test_lib/test.h"
#include "glvx/vector.h"
#include "glvx/window.h"

using namespace glvx;

// recreating window every time takes too much time,
// so we create it once and reuse it in all tests
extern Window window;

// set by the --minimized command-line parameter;
// when true, all test windows are created minimized
extern bool minimized;

const Vector2i WINDOW_SIZE = Vector2i(100, 100);
const Vector2i RESIZED_WINDOW_SIZE = Vector2i(200, 200);

bool checkPixelColor(test::Test& test, const Image& image, int start_x, int start_y, int end_x, int end_y, const Color& expected_color);
bool checkPixelColor(test::Test& test, const Image& image, const Vector2i& start, const Vector2i& end, const Color& expected_color);
bool compareImages(test::Test& test, const Image& image1, const Image& image2);

// Some GL drivers truncate the float->8-bit color
// conversion (127.5 -> 127) instead of rounding to nearest (127.5 -> 128).
// Pass as T_COMPARE's trailing comparator to allow ±1 per channel on
// rendered-pixel reads; compareFail still prints the exact expected/actual.
bool colorTolerant(const Color& a, const Color& b);
