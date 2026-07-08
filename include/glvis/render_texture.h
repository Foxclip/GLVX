#pragma once

#include "glvis/abstract_texture.h"
#include "glvis/drawable.h"
#include "glvis/view.h"

namespace glvis {

class RenderTexture : public AbstractTexture {
public:
    RenderTexture() = default;
    RenderTexture(int width, int height);
    ~RenderTexture();
    unsigned int getFBO() const;
    void create(int width, int height);
    void resize(int newWidth, int newHeight, bool blitOldContents = true) override;
    void setView(const View& view);
    void clear(const Color& color) const;
    void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) const;

private:
    unsigned int FBO = 0;
    Matrix4 view;
    Matrix4 invView;
    Matrix4 projection;
};

}
