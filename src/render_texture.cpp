#include "glvis/render_texture.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "glvis/glvis_common.h"
#include "glvis/color.h"
#include "glvis/utils.h"

namespace glvis {

RenderTexture::RenderTexture(int width, int height) {
    GL_CALL(glGenFramebuffers(1, &FBO));
    create(width, height);
}

RenderTexture::~RenderTexture() {
    GL_CALL(glDeleteFramebuffers(1, &FBO));
}

unsigned int RenderTexture::getFBO() const {
    return FBO;
}

void RenderTexture::create(int width, int height) {
    // Create FBO if it doesn't exist
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    // Create texture first (while FBO is not bound)
    AbstractTexture::createTexture(width, height, nullptr, 4, false, interpolation, wrapping);

    // Then attach to FBO
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTexture::resize(int newWidth, int newHeight, bool blitOldContents) {
    // Create FBO if it doesn't exist
    if (FBO == 0) {
        GL_CALL(glGenFramebuffers(1, &FBO));
    }

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0));
    resizeTexture(newWidth, newHeight, blitOldContents, interpolation, wrapping);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ID, 0));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void RenderTexture::setView(const View& view) {
    this->view = view.getViewMatrix(static_cast<float>(width), static_cast<float>(height));
    invView = view.getInvViewMatrix(static_cast<float>(width), static_cast<float>(height));
    projection = view.getProjectionMatrix(static_cast<float>(width), static_cast<float>(height));
}

void RenderTexture::clear(const Color& color) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderTexture::draw(const Drawable& drawable, const RenderStates& states) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, FBO));
    GL_CALL(glViewport(0, 0, width, height));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    drawable.render(view, projection, states);
    GL_CALL(glDisable(GL_BLEND));
}

Vector2i RenderTexture::worldToScreen(float x, float y) const {
    glm::vec4 point = to_glmMat4(view) * glm::vec4(x, y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, height - point.y);
    return Vector2i(static_cast<int>(result.x), static_cast<int>(result.y));
}

Vector2i RenderTexture::worldToScreen(const Vector2f& worldPos) const {
    return worldToScreen(worldPos.x, worldPos.y);
}

Vector2f RenderTexture::screenToWorld(int x, int y) const {
    float x_shifted = x + 0.5f;
    float y_shifted = y + 0.5f;
    glm::vec4 point = to_glmMat4(invView) * glm::vec4(x_shifted, height - y_shifted, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, point.y);
    return from_glmVec2(result);
}

Vector2f RenderTexture::screenToWorld(const Vector2i& screenPos) const {
    return screenToWorld(screenPos.x, screenPos.y);
}

}
