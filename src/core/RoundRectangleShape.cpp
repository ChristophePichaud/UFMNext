#include "ufmnext/core/RoundRectangleShape.h"

namespace ufmnext::core {

RoundRectangleShape::RoundRectangleShape()
    : Shape()
    , m_cornerRadius(15.0)
{
}

RoundRectangleShape::RoundRectangleShape(double x, double y, double width, double height, double cornerRadius)
    : Shape()
    , m_cornerRadius(cornerRadius)
{
    m_position = Point(x, y);
    m_width = width;
    m_height = height;
}

void RoundRectangleShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const Rect bounds = GetBoundingBox();
    const double rx = (m_cornerRadius * 2.0 > bounds.width) ? bounds.width * 0.5 : m_cornerRadius;
    const double ry = (m_cornerRadius * 2.0 > bounds.height) ? bounds.height * 0.5 : m_cornerRadius;
    ctx.DrawRoundedRect(bounds, rx, ry, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, bounds);
}

std::unique_ptr<Shape> RoundRectangleShape::Clone() const {
    auto copy = std::make_unique<RoundRectangleShape>(m_position.x, m_position.y, m_width, m_height, m_cornerRadius);
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
