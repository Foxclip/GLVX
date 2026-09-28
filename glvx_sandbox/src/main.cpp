#include "application.h"
#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool minimized = false;
    bool screenshot = false;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--minimized") == 0) {
            minimized = true;
        }
        if (std::strcmp(argv[i], "--screenshot") == 0) {
            screenshot = true;
            minimized = true;
        }
    }

    Application application;
    application.init(minimized);
    if (screenshot) {
        if (argc < 3) {
            std::cerr << "ERROR: --screenshot requires a file path argument" << std::endl;
            return 1;
        }
        std::string screenshot_path;
        screenshot_path = argv[2];
        if (!application.captureScreenshot(screenshot_path)) {
            return 1;
        }
    } else {
        application.run();
    }

    return 0;
}

// TODO: keyboard input
// TODO: Text: highlight character under cursor
// TODO: cursor visibility: drag a shape with cursor
// TODO: window focus indicators
