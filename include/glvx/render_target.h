#pragma once

#include <glad/glad.h>
#include "glvx/glvx_common.h"
#include "glvx/drawable.h"
#include "glvx/color.h"
#include "glvx/view.h"
#include "glvx/render_states.h"
#include "glvx/matrix.h"
#include "glvx/vector.h"

namespace glvx {

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    virtual void setView(const View& view);
    void clear(const Color& color) const;
    void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) const;
    Vector2i worldToScreen(float x, float y) const;
    Vector2i worldToScreen(const Vector2f& worldPos) const;
    Vector2f screenToWorld(int x, int y) const;
    Vector2f screenToWorld(const Vector2i& screenPos) const;

protected:
    Matrix4 m_view;
    Matrix4 m_inv_view;
    Matrix4 m_projection;

    virtual unsigned int getRenderTargetFbo() const = 0;
    virtual int getRenderTargetWidth() const = 0;
    virtual int getRenderTargetHeight() const = 0;
};

}
