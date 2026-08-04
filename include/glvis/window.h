#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/uniform_buffer.h"
#include "glvis/view.h"
#include "glvis/drawable.h"
#include "glvis/shader.h"
#include "glvis/image.h"
#include "glvis/vector.h"
#include "glvis/render_target.h"
#include "glvis/cursor.h"
#include "glvis/event.h"
#include <memory>
#include <queue>

class InputTestsModule;

namespace glvis {

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

class Window : public RenderTarget {
public:
    ~Window();
    void create(
        int width = DEFAULT_WINDOW_WIDTH,
        int height = DEFAULT_WINDOW_HEIGHT,
        const char* title = "GLVis window",
        int msaa_samples = 0
    );
    void close();
    bool isOpen() const;
    int getWidth() const;
    int getHeight() const;
    int getSamples() const;
    Vector2i getSize() const;
    Vector2f getCenter() const;
    void setView(const View& view) override;
    void setSize(int width, int height);
    void setSize(const Vector2i& size);
    void setTitle(const std::string& title) const;
    void display() const;
    Image readPixels() const;

    void setMouseCursor(const Cursor& cursor);
    void setCursorVisible(bool visible);
    void setMouseGrabEnabled(bool enabled);

    bool pollEvent(Event& event);
    bool waitEvent(Event& event);
    void clearEventQueue();

    GLFWwindow* getWindowHandle() const;

private:
    friend class ::InputTestsModule;

    GLFWwindow* window = nullptr;
    int current_width = 0;
    int current_height = 0;
    std::unique_ptr<Shader> default_shader_uptr = nullptr;
    std::unique_ptr<Shader> subpixel_shader_uptr = nullptr;
    std::unique_ptr<UniformBuffer> uniform_buffer_uptr = nullptr;
    int msaa_samples = 0;

    static constexpr size_t MaxEventQueueSize = 512;
    std::queue<Event> event_queue;

    unsigned int getRenderTargetFbo() const override;
    int getRenderTargetWidth() const override;
    int getRenderTargetHeight() const override;

    static int active_window_count;
    static bool glfw_initialized;

    void processWindowSize(int width, int height);
    void pushEvent(const Event& event);
    static void framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    static void mouseMoveCallbackGLFW(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow* window, double x, double y);
    static void keyCallbackGLFW(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void charCallbackGLFW(GLFWwindow* window, unsigned int codepoint);
    static void focusCallbackGLFW(GLFWwindow* window, int focused);
    static void windowPosCallbackGLFW(GLFWwindow* window, int x, int y);
    static void closeCallbackGLFW(GLFWwindow* window);

};

}
