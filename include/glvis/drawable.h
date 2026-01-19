#pragma once

#include <glm/glm.hpp>
#include "glvis/color.h"

namespace glvis {

class Shader;
class AbstractTexture;
class VertexBuffer;

class Drawable {
public:
    virtual ~Drawable() = default;

    virtual glm::mat4 getModelMatrix() const;
    virtual const VertexBuffer& getVertexBuffer() const = 0;
    ColorRGBA getColor() const;
    void setColor(const ColorRGBA& color);
    virtual void render(const glm::mat4& view, const glm::mat4& projection) const = 0;
    
protected:
    void renderBase(Shader* shader, AbstractTexture* texture, const glm::mat4& view, const glm::mat4& projection) const;

private:
    ColorRGBA color = ColorRGBA::White;

};

}
