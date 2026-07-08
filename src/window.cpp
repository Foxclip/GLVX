#include "glvis/window.h"
#include "glvis/render_texture.h"
#include "glvis/rectangle.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/shaders/subpixel.h"
#include "glvis/shaders/screen.h"
#include "glvis/uniform_buffer.h"
#include "glvis/image.h"
#include "glvis/utils.h"
#include <stdexcept>
#include <filesystem>

namespace glvis {

Window::~Window() {
    screen_rectangle_uptr.reset();
    screen_shader_uptr.reset();
    default_shader_uptr.reset();
    subpixel_shader_uptr.reset();
    uniform_buffer_uptr.reset();
    screen_texture_uptr.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::create(int width, int height, const char* title) {
    START_TRY
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    current_width = width;
    current_height = height;

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallbackGLFW);
    glfwSetMouseButtonCallback(window, mouseButtonCallbackGLFW);
    glfwSetScrollCallback(window, scrollCallbackGLFW);

    uniform_buffer_uptr = std::make_unique<UniformBuffer>();
    uniform_buffer_uptr->createCameraUBO();
    uniform_buffer_uptr->createObjectUBO();
    common::uniformBuffer = uniform_buffer_uptr.get();

    default_shader_uptr = std::make_unique<Shader>(shaders::simple_vert, shaders::simple_frag, true);
    common::defaultShader = default_shader_uptr.get();
    subpixel_shader_uptr = std::make_unique<Shader>(shaders::subpixel_vert, shaders::subpixel_frag, true);
    common::subpixelShader = subpixel_shader_uptr.get();
    screen_shader_uptr = std::make_unique<Shader>(shaders::screen_vert, shaders::screen_frag);

    screen_texture_uptr = std::make_unique<RenderTexture>(width, height);

    screen_rectangle_uptr = std::make_unique<Rectangle>(2.0f, 2.0f);
    screen_rectangle_uptr->setShader(screen_shader_uptr.get());
    screen_rectangle_uptr->setTexture(screen_texture_uptr.get());
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

Vector2i Window::getSize() const {
    return Vector2i(current_width, current_height);
}

Vector2f Window::getCenter() const {
    return Vector2f(static_cast<float>(current_width) / 2.0f, static_cast<float>(current_height) / 2.0f);
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

    // render quad with framebuffer to screen
    GL_CALL(glViewport(0, 0, current_width, current_height));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    screen_rectangle_uptr->render(from_glmMat4(glm::mat4(1.0f)), projection);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Image Window::readPixels() const {
    std::vector<unsigned char> pixels(current_width * current_height * 4);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glReadBuffer(GL_FRONT));
    GL_CALL(glReadPixels(0, 0, current_width, current_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    // Flip Y axis: OpenGL has (0,0) at bottom-left, but images typically have (0,0) at top-left
    flip_pixels_y(pixels, current_width, current_height);

    return Image(current_width, current_height, std::move(pixels));
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

unsigned int Window::getRenderTargetFbo() const {
    return screen_texture_uptr->getFBO();
}

int Window::getRenderTargetidth() const {
    return current_width;
}

int Window::getRenderTargetHeight() const {
    return current_height;
}

void Window::processWindowSize(int width, int height) {
    current_width = width;
    current_height = height;
    screen_texture_uptr->resize(width, height);
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
