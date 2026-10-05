# Tests

- Before running tests, make sure that they are built with CMake (CMake subdirectory glvx_tests)
- Tests are run by launching test executable (CMake target glvx_tests)
- When running tests, run them from project root, not build/ folder
- When running tests executable, launch it minimized with `--minimized` flag
- When running tests executable, launch it in foreground unless absolutely necessary by the nature of the task, or explicitly requested by the user
- If you want to take a screenshot of a test, you can use Window::saveScreenshot() method by injecting it into test

# Sandbox

- When running sandbox executable, launch it minimized with `--minimized` flag
- When running sandbox executable, launch it in foreground unless absolutely necessary by the nature of the task, or explicitly requested by the user
- If you want to take a screenshot, use `--screenshot` flag
