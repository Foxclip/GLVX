#include "glvis/window.h"
#include "glvis/shader.h"
#include "glvis/shaders/simple.h"
#include "glvis/shaders/subpixel.h"
#include "glvis/uniform_buffer.h"
#include "glvis/image.h"
#include "glvis/keyboard.h"
#include "glvis/mouse.h"
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
        Keyboard::reset();
        Mouse::reset();
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
    glfwSetKeyCallback(window, keyCallbackGLFW);
    glfwSetCharCallback(window, charCallbackGLFW);
    glfwSetWindowFocusCallback(window, focusCallbackGLFW);
    glfwSetWindowPosCallback(window, windowPosCallbackGLFW);

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
        true
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

GLFWwindow* Window::getWindowHandle() const {
    return window;
}

void Window::pushEvent(const Event& event) {
    if (event_queue.size() > MaxEventQueueSize) {
        event_queue.pop();
    }
    event_queue.push(event);
}

bool Window::pollEvent(Event& event) {
    if (event_queue.empty()) {
        return false;
    }
    event = event_queue.front();
    event_queue.pop();
    return true;
}

bool Window::waitEvent(Event& event) {
    while (event_queue.empty()) {
        glfwWaitEvents();
    }
    event = event_queue.front();
    event_queue.pop();
    return true;
}

void Window::clearEventQueue() {
    std::queue<Event> empty;
    std::swap(event_queue, empty);
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
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        win->processWindowSize(width, height);

        Event event;
        event.type = EventType::Resized;
        event.size.width = static_cast<unsigned int>(width);
        event.size.height = static_cast<unsigned int>(height);
        win->pushEvent(event);
    }
}

void Window::mouseMoveCallbackGLFW(GLFWwindow* glfwWindow, double xpos, double ypos) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        Event event;
        event.type = EventType::MouseMoved;
        event.mouseMove.x = static_cast<int>(xpos);
        event.mouseMove.y = static_cast<int>(ypos);
        win->pushEvent(event);
    }
}

void Window::mouseButtonCallbackGLFW(GLFWwindow* glfwWindow, int button, int action, int mods) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);

        Mouse::Button mb = static_cast<Mouse::Button>(button);
        bool pressed = (action == GLFW_PRESS);
        Mouse::setButtonState(glfwWindow, mb, pressed);

        Event event;
        event.type = pressed ? EventType::MouseButtonPressed : EventType::MouseButtonReleased;
        event.mouseButton.button = mb;
        event.mouseButton.x = static_cast<int>(xpos);
        event.mouseButton.y = static_cast<int>(ypos);
        win->pushEvent(event);
    }
}

void Window::scrollCallbackGLFW(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        double xpos, ypos;
        glfwGetCursorPos(glfwWindow, &xpos, &ypos);

        Event event;
        event.type = EventType::MouseWheelScrolled;
        event.mouseWheel.delta = static_cast<float>(yoffset);
        event.mouseWheel.x = static_cast<int>(xpos);
        event.mouseWheel.y = static_cast<int>(ypos);
        win->pushEvent(event);
    }
}

void Window::keyCallbackGLFW(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        Key k = static_cast<Key>(key);
        Modifier modifier = glfwToModifier(mods);

        bool pressed = (action == GLFW_PRESS);
        bool repeated = (action == GLFW_REPEAT);

        Keyboard::setKeyState(k, pressed);

        Event event;
        event.type = pressed ? EventType::KeyPressed : EventType::KeyReleased;
        event.key.code = k;
        event.key.modifier = modifier;
        event.key.altGr = (mods == (GLFW_MOD_CONTROL | GLFW_MOD_ALT));
        win->pushEvent(event);
    }
}

void Window::charCallbackGLFW(GLFWwindow* glfwWindow, unsigned int codepoint) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        Event event;
        event.type = EventType::TextEntered;
        event.text.unicode = static_cast<char32_t>(codepoint);
        win->pushEvent(event);
    }
}

void Window::focusCallbackGLFW(GLFWwindow* glfwWindow, int focused) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        Event event;
        event.type = focused ? EventType::FocusGained : EventType::FocusLost;
        win->pushEvent(event);
    }
}

void Window::windowPosCallbackGLFW(GLFWwindow* glfwWindow, int x, int y) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow))) {
        Event event;
        event.type = EventType::Moved;
        event.pos.x = x;
        event.pos.y = y;
        win->pushEvent(event);
    }
}

}
