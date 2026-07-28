#include "glvis/window.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/shaders/subpixel.h"
#include "glvis/uniform_buffer.h"
#include "glvis/image.h"
#include <stdexcept>
#include <filesystem>

namespace glvis {

int Window::active_window_count = 0;
bool Window::glfw_initialized = false;

Window::~Window() {
    --active_window_count;
    glfwMakeContextCurrent(window);

    default_shader_uptr.reset();
    subpixel_shader_uptr.reset();
    uniform_buffer_uptr.reset();

    glfwDestroyWindow(window);

    if (active_window_count == 0) {
        glfwTerminate();
        glfw_initialized = false;
    }
}

void Window::create(int width, int height, const char* title, int msaa_samples) {
    START_TRY
    if (!glfw_initialized) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        glfw_initialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, msaa_samples);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    current_width = width;
    current_height = height;
    this->msaa_samples = glfwGetWindowAttrib(window, GLFW_SAMPLES);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window);
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallbackGLFW);
    glfwSetMouseButtonCallback(window, mouseButtonCallbackGLFW);
    glfwSetScrollCallback(window, scrollCallbackGLFW);

    uniform_buffer_uptr = std::make_unique<UniformBuffer>();
    uniform_buffer_uptr->createObjectUBO();
    common::uniformBuffer = uniform_buffer_uptr.get();

    default_shader_uptr = std::make_unique<Shader>(shaders::simple_vert, shaders::simple_frag, true);
    common::defaultShader = default_shader_uptr.get();
    subpixel_shader_uptr = std::make_unique<Shader>(shaders::subpixel_vert, shaders::subpixel_frag, true);
    common::subpixelShader = subpixel_shader_uptr.get();
    ++active_window_count;
    END_TRY
}

bool Window::isOpen() const {
    return !glfwWindowShouldClose(window);
}

int Window::getWidth() const {
    return current_width;
}

int Window::getHeight() const {
    return current_height;
}

int Window::getSamples() const {
    return msaa_samples;
}

Vector2i Window::getSize() const {
    return Vector2i(current_width, current_height);
}

Vector2f Window::getCenter() const {
    return Vector2f(static_cast<float>(current_width) / 2.0f, static_cast<float>(current_height) / 2.0f);
}

void glvis::Window::setView(const View& view) {
    this->view = view.getViewMatrix(
        static_cast<float>(current_width),
        static_cast<float>(current_height),
        true // cooridnate system is y-down, not y-up like opengl
    );
    this->inv_view = view.getInvViewMatrix(
        static_cast<float>(current_width),
        static_cast<float>(current_height),
        true
    );
    this->projection = view.getProjectionMatrix(
        static_cast<float>(current_width),
        static_cast<float>(current_height)
    );
}

void Window::setSize(int width, int height) {
    glfwSetWindowSize(window, width, height);
    processWindowSize(width, height);
}

void Window::setSize(const Vector2i& size) {
    setSize(size.x, size.y);
}

void Window::setTitle(const std::string& title) const {
    glfwSetWindowTitle(window, title.c_str());
}

void Window::display() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    glfwSwapBuffers(window);
    glfwPollEvents();
}

Image Window::readPixels() const {
    std::vector<unsigned char> pixels(current_width * current_height * 4);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glReadBuffer(GL_FRONT));
    GL_CALL(glReadPixels(0, 0, current_width, current_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    Image image(current_width, current_height, std::move(pixels));
    image.flipY();
    return image;
}

void Window::setMouseCallback(const mouseCallbackFuncType& callback) {
    mouse_move_callback = callback;
}

void Window::setMouseButtonCallback(const mouseButtonCallbackFuncType& callback) {
    mouse_button_callback = callback;
}

void Window::setScrollCallback(const scrollCallbackFuncType& callback) {
    scroll_callback = callback;
}

void Window::setMouseCursor(const Cursor& cursor) {
    if (cursor.glfw_cursor) {
        glfwSetCursor(window, cursor.glfw_cursor);
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void Window::setCursorVisible(bool visible) {
    glfwSetInputMode(window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void Window::setMouseGrabEnabled(bool enabled) {
    glfwSetInputMode(window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

unsigned int Window::getRenderTargetFbo() const {
    return 0;
}

int Window::getRenderTargetWidth() const {
    return current_width;
}

int Window::getRenderTargetHeight() const {
    return current_height;
}

void Window::processWindowSize(int width, int height) {
    current_width = width;
    current_height = height;
    GL_CALL(glViewport(0, 0, current_width, current_height));
}

void Window::framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->processWindowSize(width, height);
    }
}

void Window::mouseMoveCallbackGLFW(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->mouse_move_callback(xpos, ypos);
    }
}

void Window::mouseButtonCallbackGLFW(GLFWwindow* glfwWindow, int button, int action, int mods) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->mouse_button_callback(button, action, mods);
    }
}

void Window::scrollCallbackGLFW(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->scroll_callback(xoffset, yoffset);
    }
}

}
