#include "ufmnext/core/SquareShape.h"

namespace ufmnext::core {

SquareShape::SquareShape()
    : Shape()
{
}

SquareShape::SquareShape(double x, double y, double width, double height)
    : Shape()
{
    m_position = Point(x, y);
    m_width = width;
    m_height = height;
}

void SquareShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const Rect bounds = GetBoundingBox();
    ctx.DrawRect(bounds, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, bounds);
}

std::unique_ptr<Shape> SquareShape::Clone() const {
    auto copy = std::make_unique<SquareShape>(m_position.x, m_position.y, m_width, m_height);
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
