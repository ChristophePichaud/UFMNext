#include "ufmnext/core/ArrowShape.h"

namespace ufmnext::core {

ArrowShape::ArrowShape()
    : Shape()
    , m_direction(ArrowDirection::Right)
{
    m_fillColor = Color(255, 193, 7, 255);
}

ArrowShape::ArrowShape(double x, double y, double width, double height, ArrowDirection direction)
    : Shape()
    , m_direction(direction)
{
    m_position = Point(x, y);
    m_width = width;
    m_height = height;
    m_fillColor = Color(255, 193, 7, 255);
}

std::vector<Point> ArrowShape::ComputeArrowVertices() const {
    const Rect b = GetBoundingBox();
    std::vector<Point> pts;
    pts.reserve(7);

    switch (m_direction) {
        case ArrowDirection::Right: {
            pts.emplace_back(b.x, b.y + b.height * 0.25);
            pts.emplace_back(b.x + b.width * 0.6, b.y + b.height * 0.25);
            pts.emplace_back(b.x + b.width * 0.6, b.y);
            pts.emplace_back(b.x + b.width, b.y + b.height * 0.5);
            pts.emplace_back(b.x + b.width * 0.6, b.y + b.height);
            pts.emplace_back(b.x + b.width * 0.6, b.y + b.height * 0.75);
            pts.emplace_back(b.x, b.y + b.height * 0.75);
            break;
        }
        case ArrowDirection::Left: {
            pts.emplace_back(b.x + b.width, b.y + b.height * 0.25);
            pts.emplace_back(b.x + b.width * 0.4, b.y + b.height * 0.25);
            pts.emplace_back(b.x + b.width * 0.4, b.y);
            pts.emplace_back(b.x, b.y + b.height * 0.5);
            pts.emplace_back(b.x + b.width * 0.4, b.y + b.height);
            pts.emplace_back(b.x + b.width * 0.4, b.y + b.height * 0.75);
            pts.emplace_back(b.x + b.width, b.y + b.height * 0.75);
            break;
        }
        case ArrowDirection::Down: {
            pts.emplace_back(b.x + b.width * 0.25, b.y);
            pts.emplace_back(b.x + b.width * 0.75, b.y);
            pts.emplace_back(b.x + b.width * 0.75, b.y + b.height * 0.6);
            pts.emplace_back(b.x + b.width, b.y + b.height * 0.6);
            pts.emplace_back(b.x + b.width * 0.5, b.y + b.height);
            pts.emplace_back(b.x, b.y + b.height * 0.6);
            pts.emplace_back(b.x + b.width * 0.25, b.y + b.height * 0.6);
            break;
        }
        case ArrowDirection::Up: {
            pts.emplace_back(b.x + b.width * 0.25, b.y + b.height);
            pts.emplace_back(b.x + b.width * 0.75, b.y + b.height);
            pts.emplace_back(b.x + b.width * 0.75, b.y + b.height * 0.4);
            pts.emplace_back(b.x + b.width, b.y + b.height * 0.4);
            pts.emplace_back(b.x + b.width * 0.5, b.y);
            pts.emplace_back(b.x, b.y + b.height * 0.4);
            pts.emplace_back(b.x + b.width * 0.25, b.y + b.height * 0.4);
            break;
        }
    }

    return pts;
}

void ArrowShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const auto vertices = ComputeArrowVertices();
    ctx.DrawPolygon(vertices, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, GetBoundingBox());
}

std::unique_ptr<Shape> ArrowShape::Clone() const {
    auto copy = std::make_unique<ArrowShape>(m_position.x, m_position.y, m_width, m_height, m_direction);
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
