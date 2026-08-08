#include "glvx/render_target.h"
#include "glvx/utils.h"

namespace glvx {

static GLenum to_gl_factor(BlendMode::Factor f) {
    switch (f) {
        case BlendMode::Factor::Zero: return GL_ZERO;
        case BlendMode::Factor::One: return GL_ONE;
        case BlendMode::Factor::SrcColor: return GL_SRC_COLOR;
        case BlendMode::Factor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
        case BlendMode::Factor::DstColor: return GL_DST_COLOR;
        case BlendMode::Factor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
        case BlendMode::Factor::SrcAlpha: return GL_SRC_ALPHA;
        case BlendMode::Factor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case BlendMode::Factor::DstAlpha: return GL_DST_ALPHA;
        case BlendMode::Factor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
    }
    return GL_ZERO;
}

static GLenum to_gl_equation(BlendMode::Equation e) {
    switch (e) {
        case BlendMode::Equation::Add: return GL_FUNC_ADD;
        case BlendMode::Equation::Subtract: return GL_FUNC_SUBTRACT;
        case BlendMode::Equation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
    }
    return GL_FUNC_ADD;
}

void RenderTarget::setView(const View& view) {
    float width = static_cast<float>(getRenderTargetWidth());
    float height = static_cast<float>(getRenderTargetHeight());
    m_view = view.getViewMatrix(width, height);
    m_inv_view = view.getInvViewMatrix(width, height);
    m_projection = view.getProjectionMatrix(width, height);
}

void RenderTarget::clear(const Color& color) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, getRenderTargetFbo()));
    GL_CALL(glClearColor(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void RenderTarget::draw(const Drawable& drawable, const RenderStates& states) const {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, getRenderTargetFbo()));
    GL_CALL(glViewport(0, 0, getRenderTargetWidth(), getRenderTargetHeight()));
    const BlendMode& bm = states.blend_mode;
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFuncSeparate(
        to_gl_factor(bm.colorSrcFactor),
        to_gl_factor(bm.colorDstFactor),
        to_gl_factor(bm.alphaSrcFactor),
        to_gl_factor(bm.alphaDstFactor)
    ));
    GL_CALL(glBlendEquationSeparate(
        to_gl_equation(bm.colorEquation), to_gl_equation(bm.alphaEquation)
    ));
    drawable.render(m_view, m_projection, states);
    GL_CALL(glDisable(GL_BLEND));
}

Vector2i RenderTarget::worldToScreen(float x, float y) const {
    glm::vec4 point = to_glmMat4(m_view) * glm::vec4(x, y, 0.0f, 1.0f);
    glm::vec2 result = glm::vec2(point.x, getRenderTargetHeight() - point.y);
    return Vector2i(static_cast<int>(result.x), static_cast<int>(result.y));
}

Vector2i RenderTarget::worldToScreen(const Vector2f& world_pos) const {
    return worldToScreen(world_pos.x, world_pos.y);
}

Vector2f RenderTarget::screenToWorld(int x, int y) const {
    float x_shifted = x + 0.5f;
    float y_shifted = y + 0.5f;
    glm::vec4 point = to_glmMat4(m_inv_view) * glm::vec4(
        x_shifted,
        getRenderTargetHeight() - y_shifted,
        0.0f,
        1.0f
    );
    glm::vec2 result = glm::vec2(point.x, point.y);
    return from_glmVec2(result);
}

Vector2f RenderTarget::screenToWorld(const Vector2i& screen_pos) const {
    return screenToWorld(screen_pos.x, screen_pos.y);
}

}
