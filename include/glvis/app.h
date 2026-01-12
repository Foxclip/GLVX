#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include "glvis/rectangle.h"
#include "glvis/circle.h"
#include "glvis/vertex_array.h"
#include "glvis/texture.h"
#include "glvis/render_texture.h"
#include "glvis/camera.h"

namespace glvis {

class App {

public:
    App(int width, int height);
    ~App();
    Camera& getCamera();
    void start();
    Texture* addTexture(const std::filesystem::path& path);
    void removeTexture(Texture* texture);
    void removeTexture(const std::filesystem::path& path);
    Rectangle* addRectangle(float width, float height);
    Circle* addCircle(float radius = 0.0f, size_t numSegments = 30);
    VertexArray* addVertexArray(PrimitiveType type, std::size_t vertexCount = 0);

private:
    GLFWwindow* window = nullptr;
    int currentWindowWidth = 0;
    int currentWindowHeight = 0;
    Camera camera;
    int mouseX = 0;
    int mouseY = 0;
    float mouseXWorld = 0.0f;
    float mouseYWorld = 0.0f;
    bool firstMouse = true;
    bool leftMousePressed = false;
    bool rightMousePressed = false;
    std::unique_ptr<Rectangle> screenRectangle = nullptr;
    std::vector<std::unique_ptr<Shape>> shapes;
    std::vector<std::unique_ptr<VertexArray>> vertexArrays;
    std::unique_ptr<Shader> defaultShaderUptr = nullptr;
    std::unique_ptr<Shader> screenShaderUptr = nullptr;
    std::unique_ptr<RenderTexture> screenTextureUptr = nullptr;
    std::map<std::string, std::unique_ptr<Texture>> textures;

    GLFWwindow* init(int width, int height);
    void mainLoop();
    glm::mat4 getViewMatrix();
    glm::mat4 getInvViewMatrix();
    glm::vec2 worldToScreen(float x, float y);
    glm::vec2 screenToWorld(int x, int y);
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double x, double y);
    static void glad_pre_callback(const char* name, void* funcptr, int len_args, ...);
    static void glad_post_callback(const char* name, void* funcptr, int len_args, ...);
    void processWindowSize(int width, int height);
    void processMouse(double xpos, double ypos);
    void processMousePress(int button, int action, int mods);
    void processMouseScroll(double x, double y);
    void processMouseLeftPress(int x, int y);
    void processMouseRightPress(int x, int y);

};

}
