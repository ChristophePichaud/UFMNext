#pragma once

#include "Point.h"
#include <algorithm>
#include <cmath>

namespace ufmnext::core {

struct Rect {
    double x{0.0};
    double y{0.0};
    double width{0.0};
    double height{0.0};

    constexpr Rect() noexcept = default;
    constexpr Rect(double inX, double inY, double inW, double inH) noexcept
        : x(inX), y(inY), width(inW), height(inH) {}

    constexpr double Left() const noexcept { return (width >= 0) ? x : x + width; }
    constexpr double Right() const noexcept { return (width >= 0) ? x + width : x; }
    constexpr double Top() const noexcept { return (height >= 0) ? y : y + height; }
    constexpr double Bottom() const noexcept { return (height >= 0) ? y + height : y; }

    constexpr Point Center() const noexcept {
        return Point(x + width * 0.5, y + height * 0.5);
    }

    constexpr Point TopLeft() const noexcept { return Point(Left(), Top()); }
    constexpr Point TopRight() const noexcept { return Point(Right(), Top()); }
    constexpr Point BottomLeft() const noexcept { return Point(Left(), Bottom()); }
    constexpr Point BottomRight() const noexcept { return Point(Right(), Bottom()); }

    constexpr Rect Normalized() const noexcept {
        const double l = (width >= 0) ? x : x + width;
        const double t = (height >= 0) ? y : y + height;
        const double w = std::abs(width);
        const double h = std::abs(height);
        return Rect(l, t, w, h);
    }

    constexpr bool Contains(const Point& pt) const noexcept {
        const Rect norm = Normalized();
        return (pt.x >= norm.x && pt.x <= norm.x + norm.width &&
                pt.y >= norm.y && pt.y <= norm.y + norm.height);
    }

    constexpr bool Intersects(const Rect& other) const noexcept {
        const Rect a = Normalized();
        const Rect b = other.Normalized();
        return !(a.Right() < b.Left() || a.Left() > b.Right() ||
                 a.Bottom() < b.Top() || a.Top() > b.Bottom());
    }

    constexpr Rect Inflated(double dx, double dy) const noexcept {
        return Rect(x - dx, y - dy, width + 2.0 * dx, height + 2.0 * dy);
    }

    constexpr bool operator==(const Rect& other) const noexcept {
        return std::abs(x - other.x) < 1e-6 &&
               std::abs(y - other.y) < 1e-6 &&
               std::abs(width - other.width) < 1e-6 &&
               std::abs(height - other.height) < 1e-6;
    }

    constexpr bool operator!=(const Rect& other) const noexcept {
        return !(*this == other);
    }
};

} // namespace ufmnext::core
