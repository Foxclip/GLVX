#include "application.h"
#include <cstring>

int main(int argc, char* argv[]) {
    bool minimized = false;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--minimized") == 0) {
            minimized = true;
        }
    }

    Application application;
    application.init(minimized);
    application.run();

    // TODO: textures, interpolation and wrapping
    // TODO: RenderTexture (minimap)
    // TODO: render a shape with msaa
    // TODO: keyboard input
    // TODO: custom shaders
    // TODO: Text: highlight character under cursor
    // TODO: cursor visibility: drag a shape with cursor
    // TODO: window focus indicators

    return 0;
}
