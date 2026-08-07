#pragma once

#include "glvx/color.h"
#include "glvx/matrix.h"
#include "glvx/render_states.h"

namespace glvx {

class Shader;
class AbstractTexture;
class VertexBuffer;

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual Transform getTransform() const;
    virtual const VertexBuffer& getVertexBuffer() const = 0;
    Color getColor() const;
    void setColor(const Color& color);
    Shader* getShader() const;
    void setShader(Shader* shader);
    AbstractTexture* getTexture() const;
    void setTexture(AbstractTexture* texture);
    virtual void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const;

protected:
    Color m_color = Color::White;
    Shader* m_shader = nullptr;
    AbstractTexture* m_texture = nullptr;

    void renderBase(
        Shader* shader,
        const AbstractTexture* texture,
        const Color& color,
        const Transform& model,
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const;

};

}
