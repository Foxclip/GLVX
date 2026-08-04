#pragma once

#include "glvx/abstract_texture.h"
#include "glvx/drawable.h"
#include "glvx/view.h"
#include "glvx/render_target.h"

namespace glvx {

class RenderTexture : public AbstractTexture, public RenderTarget {
public:
    RenderTexture() = default;
    RenderTexture(int width, int height, int msaa_samples = 0);
    ~RenderTexture();
    unsigned int getFBO() const;
    void create(int width, int height, int msaa_samples = 0);
    void resize(int newWidth, int newHeight, bool blitOldContents = true) override;
    bool isRenderTexture() const override;
    int getSamples() const;
    void display();

private:
    unsigned int FBO = 0;
    unsigned int msaa_fbo = 0;
    unsigned int msaa_texture = 0;
    int msaa_samples = 0;

    unsigned int getRenderTargetFbo() const override;
    int getRenderTargetWidth() const override;
    int getRenderTargetHeight() const override;
};

}
