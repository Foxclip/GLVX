#include "application.h"
#include <cstring>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    bool minimized = false;
    bool screenshot = false;
    std::string screenshot_path;
    for (int i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "--minimized") == 0) {
            minimized = true;
        }
        if (std::strcmp(argv[i], "--screenshot") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "ERROR: --screenshot requires a file path argument" << std::endl;
                return 1;
            }
            screenshot = true;
            screenshot_path = argv[i + 1];
            minimized = true;
        }
    }

    Application application;
    application.init(minimized);
    if (screenshot) {
        if (!application.captureScreenshot(screenshot_path)) {
            return 1;
        }
    } else {
        application.run();
    }

    return 0;
}

// TODO: cursor visibility: drag a shape with cursor
// TODO: window focus indicators
