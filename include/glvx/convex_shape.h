#pragma once

#include <cstddef>
#include "glvx/vector.h"
#include "glvx/shape.h"

namespace glvx {

class ConvexShape : public Shape {
public:
    ConvexShape(std::size_t point_count = 3);
    ~ConvexShape();

    void setPointCount(std::size_t point_count);
    std::size_t getPointCount() const;
    void setPoint(std::size_t index, const Vector2f& point);
    const Vector2f& getPoint(std::size_t index) const;

private:
    void updateVertices();
};

}
