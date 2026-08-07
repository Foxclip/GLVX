#include "glvx/drawable.h"
#include "glvx/shader.h"
#include "glvx/abstract_texture.h"
#include "glvx/vertex_buffer.h"
#include "glvx/uniform_buffer.h"
#include "glvx/glvx_common.h"
#include "glvx/utils.h"
#include <cassert>

namespace glvx {

Transform Drawable::getTransform() const {
    return Transform();
}

Color Drawable::getColor() const {
    return m_color;
}

void Drawable::setColor(const Color& color) {
    m_color = color;
}

Shader* Drawable::getShader() const {
    return m_shader;
}

void Drawable::setShader(Shader* shader) {
    m_shader = shader;
}

AbstractTexture* Drawable::getTexture() const {
    return m_texture;
}

void Drawable::setTexture(AbstractTexture* texture) {
    m_texture = texture;
}

void Drawable::renderBase(
    Shader* shader,
    const AbstractTexture* texture,
    const Color& color,
    const Transform& model,
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    const VertexBuffer& vertex_buffer = getVertexBuffer();
    if (vertex_buffer.getVertexCount() == 0) return;
    Shader* render_shader = states.shader ? states.shader : shader ? shader : common::default_shader;
    assert(render_shader);
    const AbstractTexture* render_texture = states.texture ? states.texture : texture;
    Matrix4 combined_model = (states.transform * model).toMatrix4();
    bool texture_is_premultiplied = render_texture != nullptr && render_texture->isRenderTexture();
    render_shader->use();

    if (render_shader->isUsingUBO()) {
        common::uniform_buffer->updateObjectUBO(
            combined_model, color, render_texture != nullptr, !texture_is_premultiplied, view, projection
        );
        common::uniform_buffer->bindObjectUBO();
        render_shader->setInt("tex", 0);
        if (render_texture) {
            render_texture->bind();
        }
    } else {
        render_shader->setVec4("color", Vector4(color.r, color.g, color.b, color.a));
        render_shader->setMat4("model", combined_model);
        render_shader->setMat4("view", view);
        render_shader->setMat4("projection", projection);
        render_shader->setInt("tex", 0);
        if (render_texture) {
            render_shader->setBool("hasTexture", true);
            render_texture->bind();
        } else {
            render_shader->setBool("hasTexture", false);
        }
    }

    vertex_buffer.render();
}

void Drawable::render(
    const Matrix4& view,
    const Matrix4& projection,
    const RenderStates& states
) const {
    renderBase(m_shader, m_texture, m_color, getTransform(), view, projection, states);
}

}
