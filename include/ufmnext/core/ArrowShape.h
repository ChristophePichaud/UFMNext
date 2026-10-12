#pragma once

#include "Shape.h"

namespace ufmnext::core {

enum class ArrowDirection {
    Right = 0,
    Left,
    Up,
    Down
};

class ArrowShape : public Shape {
public:
    ArrowShape();
    ArrowShape(double x, double y, double width, double height, ArrowDirection direction = ArrowDirection::Right);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Arrow; }
    std::unique_ptr<Shape> Clone() const override;

    ArrowDirection GetDirection() const noexcept { return m_direction; }
    void SetDirection(ArrowDirection dir) noexcept { m_direction = dir; }

    std::vector<Point> ComputeArrowVertices() const;

private:
    ArrowDirection m_direction{ArrowDirection::Right};
};

} // namespace ufmnext::core
