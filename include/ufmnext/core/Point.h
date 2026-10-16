#pragma once

#include <cmath>

namespace ufmnext::core {

struct Point {
    double x{0.0};
    double y{0.0};

     Point() noexcept = default;
     Point(double inX, double inY) noexcept : x(inX), y(inY) {}

     Point operator+(const Point& other) const noexcept {
        return Point(x + other.x, y + other.y);
    }

     Point operator-(const Point& other) const noexcept {
        return Point(x - other.x, y - other.y);
    }

     Point operator*(double scalar) const noexcept {
        return Point(x * scalar, y * scalar);
    }

    Point& operator+=(const Point& other) noexcept {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point& operator-=(const Point& other) noexcept {
        x -= other.x;
        y -= other.y;
        return *this;
    }

     bool operator==(const Point& other) const noexcept {
        return std::abs(x - other.x) < 1e-6 && std::abs(y - other.y) < 1e-6;
    }

     bool operator!=(const Point& other) const noexcept {
        return !(*this == other);
    }

    double DistanceTo(const Point& other) const noexcept {
        const double dx = x - other.x;
        const double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};

} // namespace ufmnext::core
