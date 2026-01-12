#include <iostream>
#include "glvis/utils.h"
#include "glvis/app.h"
#include "glvis/shader.h"
#include "glvis/glvis_common.h"

namespace glvis {

App::App(int width, int height) {
    START_TRY
    window = init(width, height);
    if (!window) {
        throw std::runtime_error("Failed to initialize GLFW window");
    }
    END_TRY
}

App::~App() {

    screenRectangle.reset();
    drawables.clear();
    screenShaderUptr.reset();
    defaultShaderUptr.reset();
    screenTextureUptr.reset();
    textures.clear();

    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();

}

    Camera& App::getCamera() {
        return camera;
    }

void App::start() {
    START_TRY
    mainLoop();
    END_TRY
}

Texture* App::addTexture(const std::filesystem::path& path) {
    std::unique_ptr<Texture> texture = std::make_unique<Texture>(path);
    Texture* texturePtr = texture.get();
    textures.emplace(path.string(), std::move(texture));
    return texturePtr;
}

void App::removeTexture(Texture* texture) {
    auto it = textures.find(texture->getPath().string());
    if (it != textures.end()) {
        textures.erase(it);
    }
}

void App::removeTexture(const std::filesystem::path& path) {
    textures.erase(path.string());
}

Rectangle* App::addRectangle(float width, float height) {
    std::unique_ptr<Rectangle> rect = std::make_unique<Rectangle>(width, height);
    Rectangle* rectPtr = rect.get();
    drawables.push_back(std::move(rect));
    return rectPtr;
}

Circle* App::addCircle(float radius, size_t numSegments) {
    std::unique_ptr<Circle> circle = std::make_unique<Circle>(radius, numSegments);
    Circle* circlePtr = circle.get();
    drawables.push_back(std::move(circle));
    return circlePtr;
}

VertexArray* App::addVertexArray(PrimitiveType type, std::size_t vertexCount) {
    std::unique_ptr<VertexArray> va = std::make_unique<VertexArray>(type, vertexCount);
    VertexArray* vaPtr = va.get();
    drawables.push_back(std::move(va));
    return vaPtr;
}

GLFWwindow* App::init(int width, int height) {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return nullptr;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Simple GLFW Window", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);

    currentWindowWidth = width;
    currentWindowHeight = height;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return nullptr;
    }

    glfwSetWindowUserPointer(window, this);
    
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);  
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
#ifndef NDEBUG
    glad_set_pre_callback(glad_pre_callback);
    glad_set_post_callback(glad_post_callback);
#endif

    defaultShaderUptr = std::make_unique<Shader>("shaders/simple.vert", "shaders/simple.frag");
    common::defaultShader = defaultShaderUptr.get();
    screenShaderUptr = std::make_unique<Shader>("shaders/screen.vert", "shaders/screen.frag");
    
    screenTextureUptr = std::make_unique<RenderTexture>(width, height);

    screenRectangle = std::make_unique<Rectangle>(2.0f, 2.0f);
    screenRectangle->setShader(screenShaderUptr.get());
    screenRectangle->setTexture(screenTextureUptr.get());

    return window;
}

void App::mainLoop() {

    while (!glfwWindowShouldClose(window)) {

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, screenTextureUptr->getFBO()));
        GL_CALL(glClearColor(0.2f, 0.3f, 0.8f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
        GL_CALL(glViewport(0, 0, currentWindowWidth, currentWindowHeight));

        glm::mat4 view = getViewMatrix();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::ortho(0.0f, (float)currentWindowWidth, 0.0f, (float)currentWindowHeight, -1.0f, 1.0f);

        for (auto& drawable : drawables) {
            drawable->render(view, projection);
        }

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));

        // render quad with framebuffer to screen
        GL_CALL(glViewport(0, 0, currentWindowWidth, currentWindowHeight));
        GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
        screenRectangle->render(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

glm::mat4 App::getViewMatrix() {
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(currentWindowWidth / 2, currentWindowHeight / 2, 0.0f));
    view = glm::scale(view, glm::vec3(camera.getZoom(), -camera.getZoom(), 1.0f));
    view = glm::translate(view, glm::vec3(-camera.getPosition().x, -camera.getPosition().y, 0.0f));
    return view;
}

glm::mat4 App::getInvViewMatrix() {
    glm::mat4 invView = glm::mat4(1.0f);
    invView = glm::translate(invView, glm::vec3(camera.getPosition().x, camera.getPosition().y, 0.0f));
    invView = glm::scale(invView, glm::vec3(1.0f / camera.getZoom(), -1.0f / camera.getZoom(), 1.0f));
    invView = glm::translate(invView, glm::vec3(-currentWindowWidth / 2, -currentWindowHeight / 2, 0.0f));
    return invView;
}

glm::vec2 App::worldToScreen(float x, float y) {
    glm::mat4 view = getViewMatrix();
    glm::vec4 point = view * glm::vec4(x, y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, currentWindowHeight - point.y);
    return result;
}

glm::vec2 App::screenToWorld(int x, int y) {
    glm::mat4 invView = getInvViewMatrix();
    glm::vec4 point = invView * glm::vec4(x, currentWindowHeight - y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, point.y);
    return result;
}

void App::framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    if (App* app = static_cast<App*>(glfwGetWindowUserPointer(window))) {
        app->processWindowSize(width, height);
    }
}

void App::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (App* app = static_cast<App*>(glfwGetWindowUserPointer(window))) {
        app->processMouse(xpos, ypos);
    }
}

void App::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (App* app = static_cast<App*>(glfwGetWindowUserPointer(window))) {
        app->processMousePress(button, action, mods);
    }
}

void App::scroll_callback(GLFWwindow *window, double x, double y) {
    if (App* app = static_cast<App*>(glfwGetWindowUserPointer(window))) {
        app->processMouseScroll(x, y);
    }
}

void App::glad_pre_callback(const char* name, void *funcptr, int len_args, ...) { }

void App::glad_post_callback(const char* name, void* funcptr, int len_args, ...) { }

void App::processWindowSize(int width, int height) {
    currentWindowWidth = width;
    currentWindowHeight = height;
    screenTextureUptr->resize(width, height);
}

void App::processMouse(double xpos, double ypos) {
    if (firstMouse) {
        mouseX = (int)xpos;
        mouseY = (int)ypos;
        mouseXWorld = screenToWorld(mouseX, mouseY).x;
        mouseYWorld = screenToWorld(mouseX, mouseY).y;
        firstMouse = false;
    }
    float xoffset = (float)xpos - mouseX;
    float yoffset = (float)ypos - mouseY;
    if (rightMousePressed) {
        glm::vec2 offset = glm::vec2(xoffset / camera.getZoom(), yoffset / camera.getZoom());
        camera.setPosition(camera.getPosition() - offset);
    }
    mouseX = (int)xpos;
    mouseY = (int)ypos;
    mouseXWorld = screenToWorld(mouseX, mouseY).x;
    mouseYWorld = screenToWorld(mouseX, mouseY).y;
}

void App::processMousePress(int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        leftMousePressed = true;
        processMouseLeftPress(mouseX, mouseY);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        leftMousePressed = false;
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rightMousePressed = true;
        processMouseRightPress(mouseX, mouseY);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        rightMousePressed = false;
    }
}

void App::processMouseScroll(double x, double y) {
    float zoomFactor = powf(CAMERA_ZOOM_FACTOR, (float)y);
    camera.setZoom(camera.getZoom() * zoomFactor);
    glm::vec2 mouseWorld = glm::vec2(mouseXWorld, mouseYWorld);
    camera.setPosition((camera.getPosition() - mouseWorld) / zoomFactor + mouseWorld);
}

void App::processMouseLeftPress(int x, int y) {
    glm::vec2 worldPos = screenToWorld(x, y);
    std::cout << std::format("Screen: ({}, {}) World: ({}, {})", x, y, worldPos.x, worldPos.y) << std::endl;
}

void App::processMouseRightPress(int x, int y) {
}

}
