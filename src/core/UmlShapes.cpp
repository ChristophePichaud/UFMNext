#include "ufmnext/core/UmlShapes.h"
#include <algorithm>

namespace ufmnext::core {

namespace {
Point P(const Rect& b, double fx, double fy) {
    return Point(b.x + b.width * fx, b.y + b.height * fy);
}
}

UmlShapeBase::UmlShapeBase() : Shape() {
    m_width = 120.0;
    m_height = 80.0;
}

void UmlShapeBase::DrawLabel(IDrawingContext& ctx, const Rect& bounds, const std::string& fallback) const {
    if (m_text.empty() && fallback.empty()) return;
    const std::string& txt = m_text.empty() ? fallback : m_text;
    ctx.DrawText(bounds, txt, m_fontColor, m_fontSize, TextAlignment::Center);
}

UmlClassShape::UmlClassShape() { m_fillColor = Color(255, 250, 205, 255); }

void UmlClassShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    ctx.DrawRect(b, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    const double headH = std::min(b.height * 0.3, 30.0);
    const double attrH = (b.height - headH) * 0.5;
    ctx.DrawLine(Point(b.x, b.y + headH), Point(b.Right(), b.y + headH), m_outlineColor, m_strokeWidth,
                 LineCapStyle::None, LineCapStyle::None);
    ctx.DrawLine(Point(b.x, b.y + headH + attrH), Point(b.Right(), b.y + headH + attrH), m_outlineColor,
                 m_strokeWidth, LineCapStyle::None, LineCapStyle::None);
    DrawLabel(ctx, Rect(b.x, b.y, b.width, headH), "ClassName");
}

UmlInterfaceShape::UmlInterfaceShape() { m_fillColor = Color(225, 240, 255, 255); }

void UmlInterfaceShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    ctx.DrawRect(b, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    const double headH = std::min(b.height * 0.4, 40.0);
    ctx.DrawLine(Point(b.x, b.y + headH), Point(b.Right(), b.y + headH), m_outlineColor, m_strokeWidth,
                 LineCapStyle::None, LineCapStyle::None);
    ctx.DrawText(Rect(b.x, b.y, b.width, headH * 0.5), "<<interface>>", m_fontColor,
                 m_fontSize * 0.8, TextAlignment::Center);
    DrawLabel(ctx, Rect(b.x, b.y + headH * 0.4, b.width, headH * 0.6), "IInterface");
}

UmlActorShape::UmlActorShape() { m_width = 60.0; m_height = 100.0; m_fillColor = Color::White(); }

void UmlActorShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    const double r = std::min(b.width, b.height) * 0.15;
    ctx.DrawCircle(P(b, 0.5, 0.12), r, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    const LineCapStyle n = LineCapStyle::None;
    ctx.DrawLine(P(b, 0.5, 0.12 + r / std::max(b.height, 1.0)), P(b, 0.5, 0.58), m_outlineColor, m_strokeWidth, n, n);
    ctx.DrawLine(P(b, 0.15, 0.35), P(b, 0.85, 0.35), m_outlineColor, m_strokeWidth, n, n);
    ctx.DrawLine(P(b, 0.5, 0.58), P(b, 0.15, 0.78), m_outlineColor, m_strokeWidth, n, n);
    ctx.DrawLine(P(b, 0.5, 0.58), P(b, 0.85, 0.78), m_outlineColor, m_strokeWidth, n, n);
    DrawLabel(ctx, Rect(b.x - b.width * 0.5, b.y + b.height * 0.8, b.width * 2.0, b.height * 0.2), "Actor");
}

UmlUseCaseShape::UmlUseCaseShape() { m_width = 140.0; m_height = 70.0; m_fillColor = Color(230, 255, 230, 255); }

void UmlUseCaseShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    ctx.DrawEllipse(b, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    DrawLabel(ctx, b, "Use Case");
}

UmlNoteShape::UmlNoteShape() { m_fillColor = Color(255, 247, 170, 255); }

void UmlNoteShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    const double f = std::min({b.width, b.height, 400.0}) * 0.2;
    std::vector<Point> body{Point(b.x, b.y), Point(b.Right() - f, b.y), Point(b.Right(), b.y + f),
                            Point(b.Right(), b.Bottom()), Point(b.x, b.Bottom())};
    ctx.DrawPolygon(body, m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    std::vector<Point> fold{Point(b.Right() - f, b.y), Point(b.Right() - f, b.y + f), Point(b.Right(), b.y + f)};
    ctx.DrawPolygon(fold, m_outlineColor, m_strokeWidth, false, m_fillColor);
    DrawLabel(ctx, b, "Note");
}

UmlPackageShape::UmlPackageShape() { m_width = 140.0; m_height = 100.0; m_fillColor = Color(240, 230, 255, 255); }

void UmlPackageShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    const Rect b = GetBoundingBox();
    const double tabH = std::min(b.height * 0.18, 24.0);
    const double tabW = b.width * 0.4;
    ctx.DrawRect(Rect(b.x, b.y, tabW, tabH), m_outlineColor, m_strokeWidth, m_fillEnabled, m_fillColor);
    ctx.DrawRect(Rect(b.x, b.y + tabH, b.width, b.height - tabH), m_outlineColor, m_strokeWidth, m_fillEnabled,
                 m_fillColor);
    DrawLabel(ctx, Rect(b.x, b.y + tabH, b.width, b.height - tabH), "Package");
}

} // namespace ufmnext::core
