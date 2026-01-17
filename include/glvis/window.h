#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/camera.h"
#include "glvis/drawable.h"
#include "glvis/rectangle.h"
#include "glvis/shader.h"
#include "glvis/render_texture.h"
#include "glvis/image.h"
#include <memory>

namespace glvis {

const int DEFAULT_WINDOW_WIDTH = 800;
const int DEFAULT_WINDOW_HEIGHT = 600;

class Window {
public:
    ~Window();
    void create(int width = DEFAULT_WINDOW_WIDTH, int height = DEFAULT_WINDOW_HEIGHT, const char* title = "GLVis window");
    bool isOpen() const;
    void setCamera(const Camera& camera);
    void clear() const;
    void draw(const Drawable& drawable) const;
    void display() const;
    Image readPixels() const;
    glm::vec2 worldToScreen(float x, float y) const;
    glm::vec2 screenToWorld(int x, int y) const;

    using mouseCallbackFuncType = std::function<void(double xpos, double ypos)>;
    using mouseButtonCallbackFuncType = std::function<void(int button, int action, int mods)>;
    using scrollCallbackFuncType = std::function<void(double xoffset, double yoffset)>;
    void setMouseCallback(const mouseCallbackFuncType& callback);
    void setMouseButtonCallback(const mouseButtonCallbackFuncType& callback);
    void setScrollCallback(const scrollCallbackFuncType& callback);

private:
    GLFWwindow* window = nullptr;
    int currentWidth = 0;
    int currentHeight = 0;
    std::unique_ptr<Rectangle> screenRectangleUptr = nullptr;
    std::unique_ptr<Shader> defaultShaderUptr = nullptr;
    std::unique_ptr<Shader> screenShaderUptr = nullptr;
    std::unique_ptr<RenderTexture> screenTextureUptr = nullptr;
    glm::mat4 view;
    glm::mat4 invView;
    glm::mat4 projection;

    mouseCallbackFuncType mouseMoveCallback = [](double xpos, double ypos) { };
    mouseButtonCallbackFuncType mouseButtonCallback = [](int button, int action, int mods) { };
    scrollCallbackFuncType scrollCallback = [](double xoffset, double yoffset) { };

    static void framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    void processWindowSize(int width, int height);
    static void mouseMoveCallbackGLFW(GLFWwindow* window, double xpos, double ypos);
    static void mouseButtonCallbackGLFW(GLFWwindow* window, int button, int action, int mods);
    static void scrollCallbackGLFW(GLFWwindow* window, double x, double y);

};

}
