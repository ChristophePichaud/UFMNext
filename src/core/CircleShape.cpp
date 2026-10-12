#include "ufmnext/core/CircleShape.h"
#include <algorithm>

namespace ufmnext::core {

CircleShape::CircleShape()
    : Shape()
{
}

CircleShape::CircleShape(double centerX, double centerY, double radius)
    : Shape()
{
    m_position = Point(centerX - radius, centerY - radius);
    m_width = radius * 2.0;
    m_height = radius * 2.0;
}

void CircleShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const Rect bounds = GetBoundingBox();
    const double radius = std::min(bounds.width, bounds.height) * 0.5;
    const Point center(bounds.x + bounds.width * 0.5, bounds.y + bounds.height * 0.5);
    ctx.DrawCircle(center, radius, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, bounds);
}

bool CircleShape::HitTest(const Point& pt, double tolerance) const {
    if (!m_visible) {
        return false;
    }
    const Rect bounds = GetBoundingBox();
    const double radius = std::min(bounds.width, bounds.height) * 0.5;
    const Point center(bounds.x + bounds.width * 0.5, bounds.y + bounds.height * 0.5);
    return pt.DistanceTo(center) <= (radius + tolerance);
}

std::unique_ptr<Shape> CircleShape::Clone() const {
    auto copy = std::make_unique<CircleShape>();
    copy->SetPosition(m_position);
    copy->SetWidth(m_width);
    copy->SetHeight(m_height);
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
    return copy;
}

} // namespace ufmnext::core
