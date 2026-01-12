#pragma once

#include "glvis/vector.h"
#include <glm/glm.hpp>
#include "glvis/angle.h"
#include "glvis/drawable.h"

namespace glvis {

class Shader;

class Shape : public Drawable {
public:
    float getX() const;
    float getY() const;
    const Vector2& getPosition() const;
    const Angle& getRotation() const;
    const Vector2& getScale() const;
    const Vector2& getOrigin() const;
    glm::mat4 getModelMatrix() const;
    void setPosition(float x, float y);
    void setPosition(const Vector2& position);
    void setRotation(const Angle& rotation);
    void setScale(float x, float y);
    void setScale(const Vector2& scale);
    void setOrigin(float x, float y);
    void setOrigin(const Vector2& origin);
    void setShader(Shader* shader);
    virtual void render(const glm::mat4& view, const glm::mat4& projection) const = 0;

protected:
    Shader* shader = nullptr;
    Vector2 position;
    Angle rotation;
    Vector2 scale = Vector2(1.0f, 1.0f);
    Vector2 origin = Vector2(0.0f, 0.0f);

private:

};

}
