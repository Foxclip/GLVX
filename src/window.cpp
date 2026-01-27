#include "glvis/window.h"
#include "glvis/render_texture.h"
#include "glvis/rectangle.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/shaders/screen.h"
#include "glvis/image.h"
#include "glvis/utils.h"
#include <stdexcept>
#include <filesystem>

namespace glvis {

Window::~Window() {
    screenRectangleUptr.reset();
    screenShaderUptr.reset();
    defaultShaderUptr.reset();
    screenTextureUptr.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::create(int width, int height, const char* title) {
    START_TRY
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(window);

    currentWidth = width;
    currentHeight = height;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(window, this);

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseMoveCallbackGLFW);
    glfwSetMouseButtonCallback(window, mouseButtonCallbackGLFW);
    glfwSetScrollCallback(window, scrollCallbackGLFW);

    defaultShaderUptr = std::make_unique<Shader>(shaders::simple_vert, shaders::simple_frag);
    common::defaultShader = defaultShaderUptr.get();
    screenShaderUptr = std::make_unique<Shader>(shaders::screen_vert, shaders::screen_frag);

    screenTextureUptr = std::make_unique<RenderTexture>(width, height);

    screenRectangleUptr = std::make_unique<Rectangle>(2.0f, 2.0f);
    screenRectangleUptr->setShader(screenShaderUptr.get());
    screenRectangleUptr->setTexture(screenTextureUptr.get());
    END_TRY
}

bool Window::isOpen() const {
    return !glfwWindowShouldClose(window);
}

int Window::getWidth() const {
    return currentWidth;
}

int Window::getHeight() const {
    return currentHeight;
}

Vector2i Window::getSize() const {
    return Vector2i(currentWidth, currentHeight);
}

Vector2f Window::getCenter() const {
    return Vector2f((float)currentWidth / 2.0f, (float)currentHeight / 2.0f);
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

void Window::setView(const View& view) {
    this->view = view.getViewMatrix((float)currentWidth, (float)currentHeight);
    invView = view.getInvViewMatrix((float)currentWidth, (float)currentHeight);
    projection = view.getProjectionMatrix((float)currentWidth, (float)currentHeight);
}

void Window::clear(const Color& color) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, screenTextureUptr->getFBO()));
    GL_CALL(glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void Window::draw(const Drawable& drawable) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, screenTextureUptr->getFBO()));
    drawable.render(view, projection);
}

void Window::draw(const Drawable& drawable, const RenderStates& states) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, screenTextureUptr->getFBO()));
    drawable.render(states, view, projection);
}

void Window::display() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

    // render quad with framebuffer to screen
    GL_CALL(glViewport(0, 0, currentWidth, currentHeight));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    screenRectangleUptr->render(from_glmMat4(glm::mat4(1.0f)), projection);

    glfwSwapBuffers(window);
    glfwPollEvents();
}

Image Window::readPixels() const {
    std::vector<unsigned char> pixels(currentWidth * currentHeight * 4);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glReadBuffer(GL_FRONT));
    GL_CALL(glReadPixels(0, 0, currentWidth, currentHeight, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    // Flip Y axis: OpenGL has (0,0) at bottom-left, but images typically have (0,0) at top-left
    std::vector<unsigned char> flippedPixels(currentWidth * currentHeight * 4);
    size_t rowSize = currentWidth * 4;
    for (int y = 0; y < currentHeight; ++y) {
        int srcY = currentHeight - 1 - y;
        std::copy(pixels.begin() + srcY * rowSize,
                  pixels.begin() + (srcY + 1) * rowSize,
                  flippedPixels.begin() + y * rowSize);
    }

    return Image(currentWidth, currentHeight, std::move(flippedPixels));
}

Vector2f Window::worldToScreen(float x, float y) const {
    glm::vec4 point = to_glmMat4(view) * glm::vec4(x, y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, currentHeight - point.y);
    return from_glmVec2(result);
}

Vector2f Window::screenToWorld(int x, int y) const {
    glm::vec4 point = to_glmMat4(invView) * glm::vec4(x, currentHeight - y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, point.y);
    return from_glmVec2(result);
}

void Window::setMouseCallback(const mouseCallbackFuncType& callback) {
    mouseMoveCallback = callback;
}

void Window::setMouseButtonCallback(const mouseButtonCallbackFuncType& callback) {
    mouseButtonCallback = callback;
}

void Window::setScrollCallback(const scrollCallbackFuncType& callback) {
    scrollCallback = callback;
}

void Window::processWindowSize(int width, int height) {
    currentWidth = width;
    currentHeight = height;
    screenTextureUptr->resize(width, height);
    GL_CALL(glViewport(0, 0, currentWidth, currentHeight));
}

void Window::framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->processWindowSize(width, height);
    }
}

void Window::mouseMoveCallbackGLFW(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->mouseMoveCallback(xpos, ypos);
    }
}

void Window::mouseButtonCallbackGLFW(GLFWwindow* glfwWindow, int button, int action, int mods) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->mouseButtonCallback(button, action, mods);
    }
}

void Window::scrollCallbackGLFW(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    if (Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        window->scrollCallback(xoffset, yoffset);
    }
}

}
