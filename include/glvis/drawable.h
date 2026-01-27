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
    virtual void render(
        const Matrix4& view,
        const Matrix4& projection,
        const RenderStates& states = RenderStates()
    ) const = 0;

protected:
    void renderBase(Shader* shader, AbstractTexture* texture, const Matrix4& model, const Matrix4& view, const Matrix4& projection) const;

private:
    Color color = Color::White;

};

}
