#include "ufmnext/core/LineShape.h"
#include <algorithm>
#include <cmath>

namespace ufmnext::core {

LineShape::LineShape()
    : Shape()
    , m_startCap(LineCapStyle::None)
    , m_endCap(LineCapStyle::None)
{
    m_position = Point(0.0, 0.0);
    m_width = 100.0;
    m_height = 0.0;
    m_outlineColor = Color::Black();
}

LineShape::LineShape(const Point& start, const Point& end)
    : Shape()
    , m_startCap(LineCapStyle::None)
    , m_endCap(LineCapStyle::None)
{
    m_position = start;
    m_width = end.x - start.x;
    m_height = end.y - start.y;
    m_outlineColor = Color::Black();
}

LineShape::LineShape(double x1, double y1, double x2, double y2)
    : Shape()
    , m_startCap(LineCapStyle::None)
    , m_endCap(LineCapStyle::None)
{
    m_position = Point(x1, y1);
    m_width = x2 - x1;
    m_height = y2 - y1;
    m_outlineColor = Color::Black();
}

void LineShape::SetStartPoint(const Point& pt) noexcept {
    const Point end = GetEndPoint();
    m_position = pt;
    m_width = end.x - pt.x;
    m_height = end.y - pt.y;
}

void LineShape::SetEndPoint(const Point& pt) noexcept {
    m_width = pt.x - m_position.x;
    m_height = pt.y - m_position.y;
}

Rect LineShape::GetBoundingBox() const {
    const Point start = GetStartPoint();
    const Point end = GetEndPoint();
    const double minX = std::min(start.x, end.x);
    const double minY = std::min(start.y, end.y);
    const double maxX = std::max(start.x, end.x);
    const double maxY = std::max(start.y, end.y);
    const double pad = std::max(10.0, m_strokeWidth * 2.0);
    return Rect(minX - pad, minY - pad, (maxX - minX) + 2.0 * pad, (maxY - minY) + 2.0 * pad);
}

bool LineShape::HitTest(const Point& pt, double tolerance) const {
    if (!m_visible) {
        return false;
    }
    const Point a = GetStartPoint();
    const Point b = GetEndPoint();

    const double l2 = (b.x - a.x) * (b.x - a.x) + (b.y - a.y) * (b.y - a.y);
    if (l2 < 1e-6) {
        return pt.DistanceTo(a) <= (tolerance + m_strokeWidth * 0.5);
    }

    const double t = std::clamp(((pt.x - a.x) * (b.x - a.x) + (pt.y - a.y) * (b.y - a.y)) / l2, 0.0, 1.0);
    const Point proj(a.x + t * (b.x - a.x), a.y + t * (b.y - a.y));
    return pt.DistanceTo(proj) <= (tolerance + m_strokeWidth * 0.5);
}

std::vector<ConnectionPoint> LineShape::GetConnectionPoints() const {
    std::vector<ConnectionPoint> points;
    points.reserve(2);
    points.emplace_back(0, m_id, GetStartPoint(), Point(0.0, 0.0), AnchorLocation::Custom);
    points.emplace_back(1, m_id, GetEndPoint(), Point(0.0, 0.0), AnchorLocation::Custom);
    return points;
}

void LineShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    ctx.DrawLine(GetStartPoint(), GetEndPoint(), m_outlineColor, m_strokeWidth, m_startCap, m_endCap);
}

std::unique_ptr<Shape> LineShape::Clone() const {
    auto copy = std::make_unique<LineShape>(GetStartPoint(), GetEndPoint());
    copy->SetId(m_id);
    copy->SetColor(m_color);
    copy->SetOutlineColor(m_outlineColor);
    copy->SetFillColor(m_fillColor);
    copy->SetFillEnabled(m_fillEnabled);
    copy->SetVisible(m_visible);
    copy->SetText(m_text);
    copy->SetFontSize(m_fontSize);
    copy->SetFontColor(m_fontColor);
    copy->SetStrokeWidth(m_strokeWidth);
    copy->SetSelected(m_selected);
    copy->SetStartCap(m_startCap);
    copy->SetEndCap(m_endCap);
    copy->SetStartConnectedShapeId(m_startConnectedShapeId, m_startAnchorIndex);
    copy->SetEndConnectedShapeId(m_endConnectedShapeId, m_endAnchorIndex);
    return copy;
}

} // namespace ufmnext::core
