#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvx/glvx_common.h"
#include "glvx/uniform_buffer.h"
#include "glvx/view.h"
#include "glvx/drawable.h"
#include "glvx/shader.h"
#include "glvx/image.h"
#include "glvx/vector.h"
#include "glvx/render_target.h"
#include "glvx/cursor.h"
#include "glvx/event.h"
#include <memory>
#include <queue>

class InputTestsModule;
class ApplicationTestsModule;

namespace glvx {

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

class Window : public RenderTarget {
public:
    ~Window();
    void create(
        int width = DEFAULT_WINDOW_WIDTH,
        int height = DEFAULT_WINDOW_HEIGHT,
        const char* title = "GLVX window",
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

    void setVerticalSyncEnabled(bool enabled);

    bool pollEvent(Event& event);
    bool waitEvent(Event& event);
    void clearEventQueue();

    GLFWwindow* getWindowHandle() const;

private:
    friend class ::InputTestsModule;
    friend class ::ApplicationTestsModule;

    GLFWwindow* m_window = nullptr;
    int m_current_width = 0;
    int m_current_height = 0;
    std::unique_ptr<Shader> m_default_shader_uptr = nullptr;
    std::unique_ptr<Shader> m_subpixel_shader_uptr = nullptr;
    std::unique_ptr<UniformBuffer> m_uniform_buffer_uptr = nullptr;
    int m_msaa_samples = 0;

    static constexpr size_t max_event_queue_size = 512;
    std::queue<Event> m_event_queue;

    unsigned int getRenderTargetFbo() const override;
    int getRenderTargetWidth() const override;
    int getRenderTargetHeight() const override;

    static int m_active_window_count;
    static bool m_glfw_initialized;

    void processWindowSize(int width, int height);
    void pushEvent(const Event& event);
    static void framebufferSizeCallback(GLFWwindow* glfw_window, int width, int height);
    static void mouseMoveCallbackGLFW(GLFWwindow* window, double x_pos, double y_pos);
    static void mouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow* window, double x, double y);
    static void keyCallbackGLFW(GLFWwindow* window, int key, int scan_code, int action, int mods);
    static void charCallbackGLFW(GLFWwindow* window, unsigned int code_point);
    static void focusCallbackGLFW(GLFWwindow* window, int focused);
    static void windowPosCallbackGLFW(GLFWwindow* window, int x, int y);
    static void closeCallbackGLFW(GLFWwindow* window);

};

}
