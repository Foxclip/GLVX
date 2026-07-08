#pragma once

#include "glvis/abstract_texture.h"
#include "glvis/drawable.h"
#include "glvis/view.h"
#include "glvis/render_target.h"

namespace glvis {

class RenderTexture : public AbstractTexture, public RenderTarget {
public:
    RenderTexture() = default;
    RenderTexture(int width, int height);
    ~RenderTexture();
    unsigned int getFBO() const;
    void create(int width, int height);
    void resize(int newWidth, int newHeight, bool blitOldContents = true) override;

private:
    unsigned int FBO = 0;

    unsigned int get_fbo() const override;
    int get_width() const override;
    int get_height() const override;
};

}
