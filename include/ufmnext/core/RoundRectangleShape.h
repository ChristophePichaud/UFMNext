#pragma once

#include "Shape.h"

namespace ufmnext::core {

class RoundRectangleShape : public Shape {
public:
    RoundRectangleShape();
    RoundRectangleShape(double x, double y, double width, double height, double cornerRadius = 15.0);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::RoundRectangle; }
    std::unique_ptr<Shape> Clone() const override;

    double GetCornerRadius() const noexcept { return m_cornerRadius; }
    void SetCornerRadius(double radius) noexcept { m_cornerRadius = radius; }

private:
    double m_cornerRadius{15.0};
};

} // namespace ufmnext::core
