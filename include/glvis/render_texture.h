#pragma once

#include "glvis/abstract_texture.h"

namespace glvis {

    class RenderTexture : public AbstractTexture {
    public:
        RenderTexture(int width, int height);
        ~RenderTexture();
        unsigned int getFBO() const;
        void resize(int newWidth, int newHeight) override;

    private:
        unsigned int FBO = 0;

    };

}
