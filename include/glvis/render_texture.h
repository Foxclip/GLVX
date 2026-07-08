#pragma once

#include "glvis/abstract_texture.h"
#include "glvis/drawable.h"

namespace glvis {

class RenderTexture : public AbstractTexture {
public:
    RenderTexture() = default;
    RenderTexture(int width, int height);
    ~RenderTexture();
    unsigned int getFBO() const;
    void create(int width, int height);
    void resize(int newWidth, int newHeight, bool blitOldContents = true) override;
    void draw(const Drawable& drawable, const Matrix4& view, const Matrix4& projection, const RenderStates& states = RenderStates()) const;

private:
    unsigned int FBO = 0;

};

}
