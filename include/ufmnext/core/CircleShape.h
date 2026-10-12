#pragma once

#include "Shape.h"

namespace ufmnext::core {

class CircleShape : public Shape {
public:
    CircleShape();
    CircleShape(double centerX, double centerY, double radius);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Circle; }
    std::unique_ptr<Shape> Clone() const override;

    double GetRadius() const noexcept { return m_width * 0.5; }
    void SetRadius(double radius) noexcept {
        m_width = radius * 2.0;
        m_height = radius * 2.0;
    }

    bool HitTest(const Point& pt, double tolerance = 5.0) const override;
};

} // namespace ufmnext::core
