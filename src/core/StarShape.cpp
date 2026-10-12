#include "ufmnext/core/StarShape.h"
#include <cmath>

namespace ufmnext::core {

static constexpr double Pi = 3.14159265358979323846;

StarShape::StarShape()
    : Shape()
    , m_points(5)
{
    m_fillColor = Color::Yellow();
}

StarShape::StarShape(double x, double y, double width, double height, int points)
    : Shape()
    , m_points((points < 3) ? 3 : points)
{
    m_position = Point(x, y);
    m_width = width;
    m_height = height;
    m_fillColor = Color::Yellow();
}

std::vector<Point> StarShape::ComputeStarVertices() const {
    const Rect bounds = GetBoundingBox();
    const double cx = bounds.x + bounds.width * 0.5;
    const double cy = bounds.y + bounds.height * 0.5;
    const double rxOuter = bounds.width * 0.5;
    const double ryOuter = bounds.height * 0.5;
    const double rxInner = rxOuter * 0.45;
    const double ryInner = ryOuter * 0.45;

    const int totalVertices = m_points * 2;
    std::vector<Point> vertices;
    vertices.reserve(totalVertices);

    // Start pointing up (-Pi / 2)
    const double startAngle = -Pi * 0.5;
    const double step = (2.0 * Pi) / totalVertices;

    for (int i = 0; i < totalVertices; ++i) {
        const double angle = startAngle + i * step;
        const double rx = (i % 2 == 0) ? rxOuter : rxInner;
        const double ry = (i % 2 == 0) ? ryOuter : ryInner;
        vertices.emplace_back(cx + rx * std::cos(angle), cy + ry * std::sin(angle));
    }

    return vertices;
}

void StarShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) {
        return;
    }
    const auto vertices = ComputeStarVertices();
    ctx.DrawPolygon(vertices, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawTextContent(ctx, GetBoundingBox());
}

std::unique_ptr<Shape> StarShape::Clone() const {
    auto copy = std::make_unique<StarShape>(m_position.x, m_position.y, m_width, m_height, m_points);
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
