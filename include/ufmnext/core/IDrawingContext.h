#pragma once

#include "Point.h"
#include "Rect.h"
#include "Color.h"
#include "ShapeType.h"
#include "ConnectionPoint.h"
#include <vector>
#include <string>

namespace ufmnext::core {

class IDrawingContext {
public:
    virtual ~IDrawingContext() = default;

    virtual void DrawRect(const Rect& rect,
                          const Color& strokeColor,
                          double strokeWidth,
                          bool fill,
                          const Color& fillColor) = 0;

    virtual void DrawRoundedRect(const Rect& rect,
                                 double radiusX,
                                 double radiusY,
                                 const Color& strokeColor,
                                 double strokeWidth,
                                 bool fill,
                                 const Color& fillColor) = 0;

    virtual void DrawCircle(const Point& center,
                            double radius,
                            const Color& strokeColor,
                            double strokeWidth,
                            bool fill,
                            const Color& fillColor) = 0;

    virtual void DrawEllipse(const Rect& rect,
                             const Color& strokeColor,
                             double strokeWidth,
                             bool fill,
                             const Color& fillColor) = 0;

    virtual void DrawLine(const Point& start,
                          const Point& end,
                          const Color& strokeColor,
                          double strokeWidth,
                          LineCapStyle startCap,
                          LineCapStyle endCap) = 0;

    virtual void DrawPolygon(const std::vector<Point>& points,
                             const Color& strokeColor,
                             double strokeWidth,
                             bool fill,
                             const Color& fillColor) = 0;

    virtual void DrawText(const Rect& bounds,
                          const std::string& text,
                          const Color& fontColor,
                          double fontSize,
                          TextAlignment align) = 0;
};

} // namespace ufmnext::core
