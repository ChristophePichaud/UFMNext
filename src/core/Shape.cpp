#include "ufmnext/core/Shape.h"
#include <cmath>
#include <limits>

namespace ufmnext::core {

Shape::Shape()
    : m_id(0)
    , m_position(0.0, 0.0)
    , m_width(100.0)
    , m_height(100.0)
    , m_color(Color::Black())
    , m_outlineColor(Color::DarkGray())
    , m_fillColor(Color(204, 229, 255, 255))
    , m_fillEnabled(true)
    , m_visible(true)
    , m_text("")
    , m_fontSize(12.0)
    , m_fontColor(Color::Black())
    , m_strokeWidth(2.0)
    , m_selected(false)
{
}

Rect Shape::GetBoundingBox() const {
    return Rect(m_position.x, m_position.y, m_width, m_height).Normalized();
}

bool Shape::HitTest(const Point& pt, double tolerance) const {
    if (!m_visible) {
        return false;
    }
    const Rect bounds = GetBoundingBox().Inflated(tolerance, tolerance);
    return bounds.Contains(pt);
}

std::vector<ConnectionPoint> Shape::GetConnectionPoints() const {
    const Rect bounds = GetBoundingBox();
    const double midX = bounds.x + bounds.width * 0.5;
    const double midY = bounds.y + bounds.height * 0.5;

    std::vector<ConnectionPoint> points;
    points.reserve(5);

    // 0: Top
    points.emplace_back(0, m_id, Point(midX, bounds.y), Point(0.0, -1.0), AnchorLocation::Top);
    // 1: Right
    points.emplace_back(1, m_id, Point(bounds.Right(), midY), Point(1.0, 0.0), AnchorLocation::Right);
    // 2: Bottom
    points.emplace_back(2, m_id, Point(midX, bounds.Bottom()), Point(0.0, 1.0), AnchorLocation::Bottom);
    // 3: Left
    points.emplace_back(3, m_id, Point(bounds.x, midY), Point(-1.0, 0.0), AnchorLocation::Left);
    // 4: Center
    points.emplace_back(4, m_id, Point(midX, midY), Point(0.0, 0.0), AnchorLocation::Center);

    return points;
}

bool Shape::GetConnectionPointByIndex(int index, ConnectionPoint& outPoint) const {
    const auto points = GetConnectionPoints();
    if (index >= 0 && index < static_cast<int>(points.size())) {
        outPoint = points[index];
        return true;
    }
    return false;
}

int Shape::FindClosestConnectionPoint(const Point& pt, double maxDistance) const {
    const auto points = GetConnectionPoints();
    int bestIndex = -1;
    double bestDist = maxDistance;

    for (const auto& cp : points) {
        const double dist = pt.DistanceTo(cp.position);
        if (dist < bestDist) {
            bestDist = dist;
            bestIndex = cp.index;
        }
    }
    return bestIndex;
}

void Shape::MoveBy(const Point& delta) {
    m_position += delta;
}

void Shape::DrawTextContent(IDrawingContext& ctx, const Rect& bounds) const {
    if (!m_text.empty()) {
        ctx.DrawText(bounds, m_text, m_fontColor, m_fontSize, TextAlignment::Center);
    }
}

} // namespace ufmnext::core
