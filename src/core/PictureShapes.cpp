#include "ufmnext/core/PictureShapes.h"
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <vector>

namespace ufmnext::core {

namespace {

 double Pi = 3.14159265358979323846;
 LineCapStyle kNone = LineCapStyle::None;

// Draws in a unit square mapped onto the shape bounds
struct Canvas {
    IDrawingContext& ctx;
    Rect b;
    double sw;

    Point P(double fx, double fy) const { return Point(b.x + b.width * fx, b.y + b.height * fy); }
    Rect R(double fx, double fy, double fw, double fh) const {
        return Rect(b.x + b.width * fx, b.y + b.height * fy, b.width * fw, b.height * fh);
    }
    double Rad(double f) const { return std::min(b.width, b.height) * f; }

    void Poly(std::initializer_list<std::pair<double, double>> pts, const Color& fill,
              const Color& stroke = Color::DarkGray()) {
        std::vector<Point> v;
        for (const auto& p : pts) v.push_back(P(p.first, p.second));
        ctx.DrawPolygon(v, stroke, sw, true, fill);
    }
    void Box(double fx, double fy, double fw, double fh, const Color& fill) {
        ctx.DrawRect(R(fx, fy, fw, fh), Color::DarkGray(), sw, true, fill);
    }
    void Disc(double fx, double fy, double fr, const Color& fill) {
        ctx.DrawCircle(P(fx, fy), Rad(fr), Color::DarkGray(), sw, true, fill);
    }
    void Seg(double x1, double y1, double x2, double y2, const Color& c = Color::DarkGray()) {
        ctx.DrawLine(P(x1, y1), P(x2, y2), c, sw, kNone, kNone);
    }
};

 Color kBrown{150, 100, 60, 255};
 Color kLeaf{70, 170, 90, 255};
 Color kSky{150, 205, 245, 255};
 Color kSun{255, 205, 40, 255};
 Color kPink{240, 110, 150, 255};

} // namespace

PictureShapeBase::PictureShapeBase() : Shape() {
    m_width = 100.0;
    m_height = 100.0;
    m_strokeWidth = 1.5;
}

void HousePictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Box(0.15, 0.45, 0.7, 0.5, Color(250, 225, 170, 255));
    c.Poly({{0.08, 0.47}, {0.5, 0.08}, {0.92, 0.47}}, Color(200, 80, 70, 255));
    c.Box(0.42, 0.65, 0.16, 0.3, kBrown);
    c.Box(0.2, 0.55, 0.15, 0.15, kSky);
    c.Box(0.65, 0.55, 0.15, 0.15, kSky);
}

void TreePictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Box(0.42, 0.55, 0.16, 0.4, kBrown);
    c.Disc(0.5, 0.32, 0.28, kLeaf);
    c.Disc(0.3, 0.45, 0.18, kLeaf);
    c.Disc(0.7, 0.45, 0.18, kLeaf);
}

void SunPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    for (int i = 0; i < 8; ++i) {
        const double a = i * Pi / 4.0;
        c.Seg(0.5 + 0.3 * std::cos(a), 0.5 + 0.3 * std::sin(a), 0.5 + 0.46 * std::cos(a), 0.5 + 0.46 * std::sin(a),
              Color::Orange());
    }
    c.Disc(0.5, 0.5, 0.25, kSun);
}

void CloudPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    const Color w(240, 245, 250, 255);
    c.Disc(0.3, 0.6, 0.2, w);
    c.Disc(0.5, 0.45, 0.25, w);
    c.Disc(0.72, 0.6, 0.2, w);
    c.Box(0.3, 0.6, 0.42, 0.2, w);
    c.Disc(0.3, 0.65, 0.15, w);
    c.Disc(0.7, 0.65, 0.15, w);
}

void MoonPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    std::vector<Point> v;
    const int n = 24;
    for (int i = 0; i <= n; ++i) {  // outer arc
        const double a = Pi * 0.5 + Pi * i / n;
        v.push_back(c.P(0.55 + 0.4 * std::cos(a), 0.5 + 0.4 * std::sin(a)));
    }
    for (int i = n; i >= 0; --i) {  // inner arc
        const double a = Pi * 0.5 + Pi * i / n;
        v.push_back(c.P(0.55 + 0.15 * std::cos(a) + 0.12, 0.5 + 0.4 * std::sin(a)));
    }
    ctx.DrawPolygon(v, Color::DarkGray(), m_strokeWidth, true, kSun);
}

void HeartPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    std::vector<Point> v;
    const int n = 40;
    for (int i = 0; i < n; ++i) {
        const double t = 2.0 * Pi * i / n;
        const double x = 16.0 * std::pow(std::sin(t), 3);
        const double y = 13.0 * std::cos(t) - 5.0 * std::cos(2 * t) - 2.0 * std::cos(3 * t) - std::cos(4 * t);
        v.push_back(c.P(0.5 + x / 36.0, 0.45 - y / 36.0));
    }
    ctx.DrawPolygon(v, Color::DarkGray(), m_strokeWidth, true, Color::Red());
}

void FlowerPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Seg(0.5, 0.45, 0.5, 0.95, kLeaf);
    c.Poly({{0.5, 0.8}, {0.75, 0.65}, {0.5, 0.72}}, kLeaf);
    for (int i = 0; i < 6; ++i) {
        const double a = i * Pi / 3.0;
        c.Disc(0.5 + 0.17 * std::cos(a), 0.3 + 0.17 * std::sin(a), 0.14, kPink);
    }
    c.Disc(0.5, 0.3, 0.1, kSun);
}

void CarPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Poly({{0.28, 0.5}, {0.38, 0.28}, {0.65, 0.28}, {0.78, 0.5}}, kSky);
    c.Box(0.05, 0.48, 0.9, 0.25, Color(210, 60, 60, 255));
    c.Disc(0.27, 0.75, 0.11, Color::DarkGray());
    c.Disc(0.73, 0.75, 0.11, Color::DarkGray());
}

void BoatPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Seg(0.5, 0.1, 0.5, 0.7, kBrown);
    c.Poly({{0.52, 0.12}, {0.52, 0.62}, {0.85, 0.62}}, Color::White());
    c.Poly({{0.48, 0.22}, {0.48, 0.62}, {0.2, 0.62}}, Color(245, 245, 200, 255));
    c.Poly({{0.1, 0.7}, {0.9, 0.7}, {0.75, 0.9}, {0.25, 0.9}}, Color(60, 90, 160, 255));
}

void MountainPictureShape::Draw(IDrawingContext& ctx) const {
    if (!m_visible) return;
    Canvas c{ctx, GetBoundingBox(), m_strokeWidth};
    c.Poly({{0.0, 0.9}, {0.3, 0.3}, {0.55, 0.9}}, Color(120, 130, 140, 255));
    c.Poly({{0.35, 0.9}, {0.65, 0.15}, {1.0, 0.9}}, Color(150, 160, 170, 255));
    c.Poly({{0.55, 0.43}, {0.65, 0.15}, {0.75, 0.43}, {0.65, 0.38}}, Color::White());
}

} // namespace ufmnext::core
