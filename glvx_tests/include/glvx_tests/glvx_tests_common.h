#pragma once

#include "test_lib/test.h"
#include "glvx/vector.h"
#include "glvx/window.h"

using namespace glvx;

// recreating window every time takes too much time,
// so we create it once and reuse it in all tests
extern Window window;

const Vector2i WINDOW_SIZE = Vector2i(100, 100);
const Vector2i RESIZED_WINDOW_SIZE = Vector2i(200, 200);

bool checkPixelColor(test::Test& test, const Image& image, int startX, int startY, int endX, int endY, const Color& expectedColor);
bool checkPixelColor(test::Test& test, const Image& image, const Vector2i& start, const Vector2i& end, const Color& expectedColor);
bool compareImages(test::Test& test, const Image& image1, const Image& image2);
