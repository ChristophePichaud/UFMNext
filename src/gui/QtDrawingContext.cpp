#include "ufmnext/gui/QtDrawingContext.h"
#include <QPainterPath>
#include <QFont>
#include <cmath>

namespace ufmnext::gui {

QtDrawingContext::QtDrawingContext(QPainter* painter)
    : m_painter(painter)
{
}

QColor QtDrawingContext::ToQColor(const core::Color& c) {
    return QColor(c.r, c.g, c.b, c.a);
}

core::Color QtDrawingContext::FromQColor(const QColor& c) {
    return core::Color(static_cast<uint8_t>(c.red()),
                       static_cast<uint8_t>(c.green()),
                       static_cast<uint8_t>(c.blue()),
                       static_cast<uint8_t>(c.alpha()));
}

void QtDrawingContext::DrawRect(const core::Rect& rect,
                                const core::Color& strokeColor,
                                double strokeWidth,
                                bool fill,
                                const core::Color& fillColor)
{
    if (!m_painter) return;

    QPen pen(ToQColor(strokeColor), strokeWidth);
    pen.setJoinStyle(Qt::MiterJoin);
    m_painter->setPen(pen);

    if (fill) {
        m_painter->setBrush(QBrush(ToQColor(fillColor)));
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }

    const core::Rect n = rect.Normalized();
    m_painter->drawRect(QRectF(n.x, n.y, n.width, n.height));
}

void QtDrawingContext::DrawRoundedRect(const core::Rect& rect,
                                       double radiusX,
                                       double radiusY,
                                       const core::Color& strokeColor,
                                       double strokeWidth,
                                       bool fill,
                                       const core::Color& fillColor)
{
    if (!m_painter) return;

    QPen pen(ToQColor(strokeColor), strokeWidth);
    pen.setJoinStyle(Qt::RoundJoin);
    m_painter->setPen(pen);

    if (fill) {
        m_painter->setBrush(QBrush(ToQColor(fillColor)));
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }

    const core::Rect n = rect.Normalized();
    m_painter->drawRoundedRect(QRectF(n.x, n.y, n.width, n.height), radiusX, radiusY);
}

void QtDrawingContext::DrawCircle(const core::Point& center,
                                  double radius,
                                  const core::Color& strokeColor,
                                  double strokeWidth,
                                  bool fill,
                                  const core::Color& fillColor)
{
    if (!m_painter) return;

    QPen pen(ToQColor(strokeColor), strokeWidth);
    m_painter->setPen(pen);

    if (fill) {
        m_painter->setBrush(QBrush(ToQColor(fillColor)));
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }

    m_painter->drawEllipse(QPointF(center.x, center.y), radius, radius);
}

void QtDrawingContext::DrawEllipse(const core::Rect& rect,
                                   const core::Color& strokeColor,
                                   double strokeWidth,
                                   bool fill,
                                   const core::Color& fillColor)
{
    if (!m_painter) return;

    QPen pen(ToQColor(strokeColor), strokeWidth);
    m_painter->setPen(pen);

    if (fill) {
        m_painter->setBrush(QBrush(ToQColor(fillColor)));
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }

    const core::Rect n = rect.Normalized();
    m_painter->drawEllipse(QRectF(n.x, n.y, n.width, n.height));
}

void QtDrawingContext::DrawCap(const QPointF& pt, const QPointF& dir, core::LineCapStyle style, const QColor& color, double size) {
    if (style == core::LineCapStyle::None) {
        return;
    }

    const double len = std::hypot(dir.x(), dir.y());
    if (len < 1e-4) return;
    const QPointF unitDir = dir / len;
    const QPointF perp(-unitDir.y(), unitDir.x());

    if (style == core::LineCapStyle::Arrow) {
        const QPointF base = pt - unitDir * size;
        const QPointF left = base + perp * (size * 0.5);
        const QPointF right = base - perp * (size * 0.5);

        QPolygonF arrowPoly;
        arrowPoly << pt << left << right;

        m_painter->setPen(QPen(color, 1.0));
        m_painter->setBrush(QBrush(color));
        m_painter->drawPolygon(arrowPoly);
    } else if (style == core::LineCapStyle::Dot) {
        const double radius = size * 0.4;
        m_painter->setPen(QPen(color, 1.0));
        m_painter->setBrush(QBrush(color));
        m_painter->drawEllipse(pt, radius, radius);
    }
}

void QtDrawingContext::DrawLine(const core::Point& start,
                                const core::Point& end,
                                const core::Color& strokeColor,
                                double strokeWidth,
                                core::LineCapStyle startCap,
                                core::LineCapStyle endCap)
{
    if (!m_painter) return;

    QColor qColor = ToQColor(strokeColor);
    QPen pen(qColor, strokeWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    m_painter->setPen(pen);
    m_painter->setBrush(Qt::NoBrush);

    QPointF p1(start.x, start.y);
    QPointF p2(end.x, end.y);
    m_painter->drawLine(p1, p2);

    const double capSize = std::max(10.0, strokeWidth * 4.0);
    if (startCap != core::LineCapStyle::None) {
        DrawCap(p1, p1 - p2, startCap, qColor, capSize);
    }
    if (endCap != core::LineCapStyle::None) {
        DrawCap(p2, p2 - p1, endCap, qColor, capSize);
    }
}

void QtDrawingContext::DrawPolygon(const std::vector<core::Point>& points,
                                   const core::Color& strokeColor,
                                   double strokeWidth,
                                   bool fill,
                                   const core::Color& fillColor)
{
    if (!m_painter || points.empty()) return;

    QPolygonF poly;
    poly.reserve(static_cast<int>(points.size()));
    for (const auto& pt : points) {
        poly << QPointF(pt.x, pt.y);
    }

    QPen pen(ToQColor(strokeColor), strokeWidth);
    pen.setJoinStyle(Qt::RoundJoin);
    m_painter->setPen(pen);

    if (fill) {
        m_painter->setBrush(QBrush(ToQColor(fillColor)));
    } else {
        m_painter->setBrush(Qt::NoBrush);
    }

    m_painter->drawPolygon(poly);
}

void QtDrawingContext::DrawText(const core::Rect& bounds,
                                const std::string& text,
                                const core::Color& fontColor,
                                double fontSize,
                                core::TextAlignment align)
{
    if (!m_painter || text.empty()) return;

    QFont font = m_painter->font();
    font.setPointSizeF(fontSize);
    m_painter->setFont(font);
    m_painter->setPen(ToQColor(fontColor));

    Qt::Alignment flags = Qt::AlignVCenter;
    switch (align) {
        case core::TextAlignment::Left: flags |= Qt::AlignLeft; break;
        case core::TextAlignment::Right: flags |= Qt::AlignRight; break;
        case core::TextAlignment::Center:
        default: flags |= Qt::AlignHCenter; break;
    }

    const core::Rect n = bounds.Normalized();
    m_painter->drawText(QRectF(n.x, n.y, n.width, n.height), flags | Qt::TextWordWrap, QString::fromStdString(text));
}

} // namespace ufmnext::gui
