#include "ufmnext/core/OvalShape.h"

namespace ufmnext::core {

OvalShape::OvalShape()
    : Shape()
{
}

OvalShape::OvalShape(double x, double y, double width, double height)
    : Shape()
{
    m_position = Point(x, y);
    m_width = width;
    m_height = height;
}

void OvalShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const Rect bounds = GetBoundingBox();
    ctx.DrawEllipse(bounds, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, bounds);
}

bool OvalShape::HitTest(const Point& pt, double tolerance) const {
    if (!m_visible) {
        return false;
    }
    const Rect bounds = GetBoundingBox();
    const double a = (bounds.width * 0.5) + tolerance;
    const double b = (bounds.height * 0.5) + tolerance;
    if (a <= 0.0 || b <= 0.0) {
        return false;
    }
    const double cx = bounds.x + bounds.width * 0.5;
    const double cy = bounds.y + bounds.height * 0.5;
    const double dx = pt.x - cx;
    const double dy = pt.y - cy;
    return ((dx * dx) / (a * a) + (dy * dy) / (b * b)) <= 1.0;
}

std::unique_ptr<Shape> OvalShape::Clone() const {
    auto copy = std::make_unique<OvalShape>(m_position.x, m_position.y, m_width, m_height);
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
