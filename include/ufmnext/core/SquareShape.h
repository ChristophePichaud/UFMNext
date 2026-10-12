#pragma once

#include "Shape.h"

namespace ufmnext::core {

class SquareShape : public Shape {
public:
    SquareShape();
    SquareShape(double x, double y, double width, double height);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Square; }
    std::unique_ptr<Shape> Clone() const override;
};

} // namespace ufmnext::core
