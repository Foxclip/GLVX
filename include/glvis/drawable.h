#pragma once

#include "glvis/color.h"
#include "glvis/matrix.h"
#include "glvis/render_states.h"

namespace glvis {

class Shader;
class AbstractTexture;
class VertexBuffer;

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual Matrix4 getModelMatrix() const;
    virtual const VertexBuffer& getVertexBuffer() const = 0;
    Color getColor() const;
    void setColor(const Color& color);
    Shader* getShader() const;
    void setShader(Shader* shader);
    AbstractTexture* getTexture() const;
    void setTexture(AbstractTexture* texture);
    virtual Shader* getDefaultShader() const;
    virtual void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const;

protected:
    Color color = Color::White;
    Shader* shader = nullptr;
    AbstractTexture* texture = nullptr;

    void renderBase(
        Shader* shader,
        const AbstractTexture* texture,
        const Color& color,
        const Matrix4& model,
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const;

};

}
