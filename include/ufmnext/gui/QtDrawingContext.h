#pragma once

#include "ufmnext/core/IDrawingContext.h"
#include <QPainter>
#include <QColor>
#include <QRectF>
#include <QPointF>

namespace ufmnext::gui {

class QtDrawingContext : public core::IDrawingContext {
public:
    explicit QtDrawingContext(QPainter* painter);
    ~QtDrawingContext() override = default;

    void DrawRect(const core::Rect& rect,
                  const core::Color& strokeColor,
                  double strokeWidth,
                  bool fill,
                  const core::Color& fillColor) override;

    void DrawRoundedRect(const core::Rect& rect,
                         double radiusX,
                         double radiusY,
                         const core::Color& strokeColor,
                         double strokeWidth,
                         bool fill,
                         const core::Color& fillColor) override;

    void DrawCircle(const core::Point& center,
                    double radius,
                    const core::Color& strokeColor,
                    double strokeWidth,
                    bool fill,
                    const core::Color& fillColor) override;

    void DrawEllipse(const core::Rect& rect,
                     const core::Color& strokeColor,
                     double strokeWidth,
                     bool fill,
                     const core::Color& fillColor) override;

    void DrawLine(const core::Point& start,
                  const core::Point& end,
                  const core::Color& strokeColor,
                  double strokeWidth,
                  core::LineCapStyle startCap,
                  core::LineCapStyle endCap) override;

    void DrawPolygon(const std::vector<core::Point>& points,
                     const core::Color& strokeColor,
                     double strokeWidth,
                     bool fill,
                     const core::Color& fillColor) override;

    void DrawText(const core::Rect& bounds,
                  const std::string& text,
                  const core::Color& fontColor,
                  double fontSize,
                  core::TextAlignment align) override;

    static QColor ToQColor(const core::Color& c);
    static core::Color FromQColor(const QColor& c);

private:
    void DrawCap(const QPointF& pt, const QPointF& dir, core::LineCapStyle style, const QColor& color, double size);

    QPainter* m_painter{nullptr};
};

} // namespace ufmnext::gui
