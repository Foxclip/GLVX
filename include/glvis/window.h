#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/camera.h"
#include "glvis/drawable.h"
#include <memory>

namespace glvis {

class Rectangle;
class Shader;
class RenderTexture;

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
    glm::vec2 worldToScreen(float x, float y) const;
    glm::vec2 screenToWorld(int x, int y) const;

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

    static void framebufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    void processWindowSize(int width, int height);


};

}
