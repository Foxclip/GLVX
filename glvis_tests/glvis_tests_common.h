#pragma once

#include "glvis/vector.h"
#include "glvis/window.h"

using namespace glvis;

// recreating window every time takes too much time,
// so we create it once and reuse it in all tests
extern Window window;

const Vector2i WINDOW_SIZE = Vector2i(100, 100);
const Vector2i RESIZED_WINDOW_SIZE = Vector2i(200, 200);
