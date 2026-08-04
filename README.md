# GLVX

A 2D OpenGL rendering library providing an SFML-inspired API for drawing shapes with transformations, shaders, and textures. Built on top of **GLFW**, **GLAD**, and **GLM**.

## Features

- **Shape primitives** — [`Rectangle`](include/glvx/rectangle.h) and [`Circle`](include/glvx/circle.h) with customizable size, color, and transformations
- **Vertex arrays** — Dynamic [`VertexArray`](include/glvx/vertex_array.h) for custom geometry with automatic GPU synchronization
- **Transformation system** — Position, rotation, scale, and origin via [`Transformable`](include/glvx/transformable.h)
- **Shader support** — Built-in default shader with the ability to use custom GLSL shaders via [`Shader`](include/glvx/shader.h)
- **Texture support** — Texture mapping on shapes and render-to-texture
- **Camera/View system** — [`View`](include/glvx/view.h) class with zoom, pan, and rotation
- **Coordinate conversion** — World-to-screen and screen-to-world transformations
- **Input callbacks** — Mouse move, button, and scroll callbacks on [`Window`](include/glvx/window.h)

## Dependencies

- [GLFW](https://www.glfw.org/) - Window creation and input handling
- [GLM](https://github.com/g-truc/glm) - Mathematics (vectors, matrices)
- [cpp-test-lib](https://github.com/Foxclip/cpp-test-lib.git) - Unit testing framework

All dependencies are included as Git submodules. GLAD source code is included in the project.

## Building

```bash
git clone --recursive https://github.com/Foxclip/glvx.git
cd glvx
mkdir build
cd build
cmake ..
cmake --build .
```

Requires **CMake 3.10+** and a **C++20** compatible compiler.

## Quick Start

```cpp
#include <glvx/window.h>
#include <glvx/rectangle.h>
#include <glvx/circle.h>
#include <glvx/view.h>

int main() {
    glvx::Window window;
    window.create(800, 600, "My App");

    // Create shapes
    glvx::Rectangle rect(100.0f, 50.0f);
    rect.setPosition(200.0f, 150.0f);
    rect.setColor(glvx::Color::Red);

    glvx::Circle circle(30.0f);
    circle.setPosition(400.0f, 300.0f);
    circle.setColor(glvx::Color::Blue);

    // Set up camera
    glvx::View view;
    window.setView(view);

    // Render loop
    while (window.isOpen()) {
        window.clear(glvx::Color::Black);
        window.draw(rect);
        window.draw(circle);
        window.display();
    }

    return 0;
}
```
