#pragma once

#include "Shape.h"

namespace ufmnext::core {

class OvalShape : public Shape {
public:
    OvalShape();
    OvalShape(double x, double y, double width, double height);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Oval; }
    std::unique_ptr<Shape> Clone() const override;

    bool HitTest(const Point& pt, double tolerance = 5.0) const override;
};

} // namespace ufmnext::core
