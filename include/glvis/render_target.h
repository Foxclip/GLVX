#pragma once

#include <glad/glad.h>
#include "glvis/glvis_common.h"
#include "glvis/drawable.h"
#include "glvis/color.h"
#include "glvis/view.h"
#include "glvis/render_states.h"
#include "glvis/matrix.h"
#include "glvis/vector.h"

namespace glvis {

class RenderTarget {
public:
    virtual ~RenderTarget() = default;

    void setView(const View& view);
    void clear(const Color& color) const;
    void draw(const Drawable& drawable, const RenderStates& states = RenderStates()) const;
    Vector2i worldToScreen(float x, float y) const;
    Vector2i worldToScreen(const Vector2f& worldPos) const;
    Vector2f screenToWorld(int x, int y) const;
    Vector2f screenToWorld(const Vector2i& screenPos) const;

protected:
    Matrix4 view;
    Matrix4 inv_view;
    Matrix4 projection;

    virtual unsigned int get_fbo() const = 0;
    virtual int get_width() const = 0;
    virtual int get_height() const = 0;
};

}
