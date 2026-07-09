#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/uniform_buffer.h"
#include "glvis/view.h"
#include "glvis/drawable.h"
#include "glvis/rectangle.h"
#include "glvis/shader.h"
#include "glvis/render_texture.h"
#include "glvis/image.h"
#include "glvis/vector.h"
#include "glvis/render_target.h"
#include <memory>
#include <functional>

namespace glvis {

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

class Window : public RenderTarget {
public:
    ~Window();
    void create(int width = DEFAULT_WINDOW_WIDTH, int height = DEFAULT_WINDOW_HEIGHT, const char* title = "GLVis window");
    bool isOpen() const;
    int getWidth() const;
    int getHeight() const;
    Vector2i getSize() const;
    Vector2f getCenter() const;
    void setSize(int width, int height);
    void setSize(const Vector2i& size);
    void setTitle(const std::string& title) const;
    void display() const;
    Image readPixels() const;

    using mouseCallbackFuncType = std::function<void(double xpos, double ypos)>;
    using mouseButtonCallbackFuncType = std::function<void(int button, int action, int mods)>;
    using scrollCallbackFuncType = std::function<void(double xoffset, double yoffset)>;
    void setMouseCallback(const mouseCallbackFuncType& callback);
    void setMouseButtonCallback(const mouseButtonCallbackFuncType& callback);
    void setScrollCallback(const scrollCallbackFuncType& callback);

private:
    GLFWwindow* window = nullptr;
    int current_width = 0;
    int current_height = 0;
    std::unique_ptr<Rectangle> screen_rectangle_uptr = nullptr;
    std::unique_ptr<Shader> default_shader_uptr = nullptr;
    std::unique_ptr<Shader> subpixel_shader_uptr = nullptr;
    std::unique_ptr<Shader> screen_shader_uptr = nullptr;
    std::unique_ptr<RenderTexture> screen_texture_uptr = nullptr;
    std::unique_ptr<UniformBuffer> uniform_buffer_uptr = nullptr;

    mouseCallbackFuncType mouse_move_callback = [](double xpos, double ypos) { };
    mouseButtonCallbackFuncType mouse_button_callback = [](int button, int action, int mods) { };
    scrollCallbackFuncType scroll_callback = [](double xoffset, double yoffset) { };

    unsigned int getRenderTargetFbo() const override;
    int getRenderTargetidth() const override;
    int getRenderTargetHeight() const override;

    void processWindowSize(int width, int height);
    static void framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    static void mouseMoveCallbackGLFW(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow* window, double x, double y);

};

}
