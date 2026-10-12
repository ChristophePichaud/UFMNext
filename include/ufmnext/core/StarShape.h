#pragma once

#include "Shape.h"

namespace ufmnext::core {

class StarShape : public Shape {
public:
    StarShape();
    StarShape(double x, double y, double width, double height, int points = 5);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Star; }
    std::unique_ptr<Shape> Clone() const override;

    int GetPointsCount() const noexcept { return m_points; }
    void SetPointsCount(int count) noexcept { m_points = (count < 3) ? 3 : count; }

    std::vector<Point> ComputeStarVertices() const;

private:
    int m_points{5};
};

} // namespace ufmnext::core
