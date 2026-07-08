#include "glvis/render_target.h"
#include "glvis/utils.h"

namespace glvis {

void RenderTarget::setView(const View& view) {
    float width = static_cast<float>(getRenderTargetidth());
    float height = static_cast<float>(getRenderTargetHeight());
    this->view = view.getViewMatrix(width, height);
    this->inv_view = view.getInvViewMatrix(width, height);
    this->projection = view.getProjectionMatrix(width, height);
}

void RenderTarget::clear(const Color& color) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, getRenderTargetFbo()));
    GL_CALL(glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderTarget::draw(const Drawable& drawable, const RenderStates& states) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, getRenderTargetFbo()));
    GL_CALL(glViewport(0, 0, getRenderTargetidth(), getRenderTargetHeight()));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    drawable.render(view, projection, states);
    GL_CALL(glDisable(GL_BLEND));
}

Vector2i RenderTarget::worldToScreen(float x, float y) const {
    glm::vec4 point = to_glmMat4(view) * glm::vec4(x, y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, getRenderTargetHeight() - point.y);
    return Vector2i(static_cast<int>(result.x), static_cast<int>(result.y));
}

Vector2i RenderTarget::worldToScreen(const Vector2f& worldPos) const {
    return worldToScreen(worldPos.x, worldPos.y);
}

Vector2f RenderTarget::screenToWorld(int x, int y) const {
    float x_shifted = x + 0.5f;
    float y_shifted = y + 0.5f;
    glm::vec4 point = to_glmMat4(inv_view) * glm::vec4(
        x_shifted,
        getRenderTargetHeight() - y_shifted,
        0.0f,
        1.0f
    );
    glm::vec2 result = glm::vec2(point.x, point.y);
    return from_glmVec2(result);
}

Vector2f RenderTarget::screenToWorld(const Vector2i& screenPos) const {
    return screenToWorld(screenPos.x, screenPos.y);
}

}
