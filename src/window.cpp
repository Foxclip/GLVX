#include "glvx/window.h"
#include "glvx/shader.h"
#include "glvx/shaders/simple.h"
#include "glvx/shaders/subpixel.h"
#include "glvx/uniform_buffer.h"
#include "glvx/image.h"
#include "glvx/keyboard.h"
#include "glvx/mouse.h"
#include <stdexcept>
#include <filesystem>

namespace glvx {

int Window::m_active_window_count = 0;
bool Window::m_glfw_initialized = false;

Window::~Window() {
    close();
}

void Window::create(int width, int height, const char* title, int msaa_samples) {
    START_TRY
    close();

    if (!m_glfw_initialized) {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }
        m_glfw_initialized = true;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, msaa_samples);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!m_window) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwMakeContextCurrent(m_window);

    m_current_width = width;
    m_current_height = height;
    m_msaa_samples = glfwGetWindowAttrib(m_window, GLFW_SAMPLES);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        glfwDestroyWindow(m_window);
        throw std::runtime_error("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseMoveCallbackGLFW);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallbackGLFW);
    glfwSetScrollCallback(m_window, scrollCallbackGLFW);
    glfwSetKeyCallback(m_window, keyCallbackGLFW);
    glfwSetCharCallback(m_window, charCallbackGLFW);
    glfwSetWindowFocusCallback(m_window, focusCallbackGLFW);
    glfwSetWindowPosCallback(m_window, windowPosCallbackGLFW);
    glfwSetWindowCloseCallback(m_window, closeCallbackGLFW);

    m_uniform_buffer_uptr = std::make_unique<UniformBuffer>();
    m_uniform_buffer_uptr->createObjectUBO();
    common::uniform_buffer = m_uniform_buffer_uptr.get();

    m_default_shader_uptr = std::make_unique<Shader>(shaders::simple_vert, shaders::simple_frag, true);
    common::default_shader = m_default_shader_uptr.get();
    m_subpixel_shader_uptr = std::make_unique<Shader>(shaders::subpixel_vert, shaders::subpixel_frag, true);
    common::subpixel_shader = m_subpixel_shader_uptr.get();
    ++m_active_window_count;
    END_TRY
}

bool Window::isOpen() const {
    return m_window != nullptr;
}

int Window::getWidth() const {
    return m_current_width;
}

int Window::getHeight() const {
    return m_current_height;
}

int Window::getSamples() const {
    return m_msaa_samples;
}

Vector2i Window::getSize() const {
    return Vector2i(m_current_width, m_current_height);
}

Vector2f Window::getCenter() const {
    return Vector2f(static_cast<float>(m_current_width) / 2.0f, static_cast<float>(m_current_height) / 2.0f);
}

void glvx::Window::setView(const View& view) {
    m_view = view.getViewMatrix(
        static_cast<float>(m_current_width),
        static_cast<float>(m_current_height),
        true
    );
    m_inv_view = view.getInvViewMatrix(
        static_cast<float>(m_current_width),
        static_cast<float>(m_current_height),
        true
    );
    m_projection = view.getProjectionMatrix(
        static_cast<float>(m_current_width),
        static_cast<float>(m_current_height)
    );
}

void Window::setSize(int width, int height) {
    glfwSetWindowSize(m_window, width, height);
    processWindowSize(width, height);
}

void Window::setSize(const Vector2i& size) {
    setSize(size.x, size.y);
}

void Window::setTitle(const std::string& title) const {
    glfwSetWindowTitle(m_window, title.c_str());
}

void Window::display() const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    glfwSwapBuffers(m_window);
}

Image Window::readPixels() const {
    std::vector<unsigned char> pixels(m_current_width * m_current_height * 4);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glReadBuffer(GL_FRONT));
    GL_CALL(glReadPixels(0, 0, m_current_width, m_current_height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

    Image image(m_current_width, m_current_height, std::move(pixels));
    image.flipY();
    return image;
}

void Window::setMouseCursor(const Cursor& cursor) {
    if (cursor.m_glfw_cursor) {
        glfwSetCursor(m_window, cursor.m_glfw_cursor);
    } else {
        glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
}

void Window::setCursorVisible(bool visible) {
    glfwSetInputMode(m_window, GLFW_CURSOR, visible ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN);
}

void Window::setMouseGrabEnabled(bool enabled) {
    glfwSetInputMode(m_window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Window::setVerticalSyncEnabled(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

GLFWwindow* Window::getWindowHandle() const {
    return m_window;
}

void Window::pushEvent(const Event& event) {
    if (m_event_queue.size() > max_event_queue_size) {
        m_event_queue.pop();
    }
    m_event_queue.push(event);
}

bool Window::pollEvent(Event& event) {
    glfwPollEvents();
    if (m_event_queue.empty()) {
        return false;
    }
    event = m_event_queue.front();
    m_event_queue.pop();
    return true;
}

bool Window::waitEvent(Event& event) {
    while (m_event_queue.empty()) {
        glfwWaitEvents();
    }
    event = m_event_queue.front();
    m_event_queue.pop();
    return true;
}

void Window::clearEventQueue() {
    std::queue<Event> empty;
    std::swap(m_event_queue, empty);
}

unsigned int Window::getRenderTargetFbo() const {
    return 0;
}

int Window::getRenderTargetWidth() const {
    return m_current_width;
}

int Window::getRenderTargetHeight() const {
    return m_current_height;
}

void Window::processWindowSize(int width, int height) {
    m_current_width = width;
    m_current_height = height;
    GL_CALL(glViewport(0, 0, m_current_width, m_current_height));
}

void Window::framebufferSizeCallback(GLFWwindow* glfw_window, int width, int height) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(glfw_window))) {
        win->processWindowSize(width, height);

        Event event;
        event.type = EventType::Resized;
        event.size.width = static_cast<unsigned int>(width);
        event.size.height = static_cast<unsigned int>(height);
        win->pushEvent(event);
    }
}

void Window::mouseMoveCallbackGLFW(GLFWwindow* window, double x_pos, double y_pos) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Event event;
        event.type = EventType::MouseMoved;
        event.mouseMove.x = static_cast<int>(x_pos);
        event.mouseMove.y = static_cast<int>(y_pos);
        win->pushEvent(event);
    }
}

void Window::mouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        double x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);

        Mouse::Button mb = static_cast<Mouse::Button>(button);
        bool pressed = (action == GLFW_PRESS);
        Mouse::setButtonState(window, mb, pressed);

        Event event;
        event.type = pressed ? EventType::MouseButtonPressed : EventType::MouseButtonReleased;
        event.mouseButton.button = mb;
        event.mouseButton.x = static_cast<int>(x_pos);
        event.mouseButton.y = static_cast<int>(y_pos);
        win->pushEvent(event);
    }
}

void Window::scrollCallbackGLFW(GLFWwindow* window, double x_offset, double y_offset) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        double x_pos, y_pos;
        glfwGetCursorPos(window, &x_pos, &y_pos);

        Event event;
        event.type = EventType::MouseWheelScrolled;
        event.mouseWheel.delta = static_cast<float>(y_offset);
        event.mouseWheel.x = static_cast<int>(x_pos);
        event.mouseWheel.y = static_cast<int>(y_pos);
        win->pushEvent(event);
    }
}

void Window::keyCallbackGLFW(GLFWwindow* window, int key, int scan_code, int action, int mods) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Key k = static_cast<Key>(key);
        Modifier modifier = glfwToModifier(mods);

        bool pressed = (action == GLFW_PRESS);
        bool repeated = (action == GLFW_REPEAT);

        Keyboard::setKeyState(k, pressed);

        Event event;
        event.type = pressed ? EventType::KeyPressed : EventType::KeyReleased;
        event.key.code = k;
        event.key.modifier = modifier;
        event.key.alt_gr = (mods == (GLFW_MOD_CONTROL | GLFW_MOD_ALT));
        win->pushEvent(event);
    }
}

void Window::charCallbackGLFW(GLFWwindow* window, unsigned int code_point) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Event event;
        event.type = EventType::TextEntered;
        event.text.unicode = static_cast<char32_t>(code_point);
        win->pushEvent(event);
    }
}

void Window::focusCallbackGLFW(GLFWwindow* window, int focused) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Event event;
        event.type = focused ? EventType::FocusGained : EventType::FocusLost;
        win->pushEvent(event);
    }
}

void Window::windowPosCallbackGLFW(GLFWwindow* window, int x, int y) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Event event;
        event.type = EventType::Moved;
        event.pos.x = x;
        event.pos.y = y;
        win->pushEvent(event);
    }
}

void Window::closeCallbackGLFW(GLFWwindow* window) {
    if (Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window))) {
        Event event;
        event.type = EventType::Closed;
        win->pushEvent(event);
    }
}

void Window::close() {
    if (m_window == nullptr) {
        return;
    }

    --m_active_window_count;

    if (m_glfw_initialized) {
        glfwMakeContextCurrent(m_window);

        m_default_shader_uptr.reset();
        m_subpixel_shader_uptr.reset();
        m_uniform_buffer_uptr.reset();

        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    if (m_active_window_count == 0) {
        Keyboard::reset();
        Mouse::reset();
        glfwTerminate();
        m_glfw_initialized = false;
    }
}

}
