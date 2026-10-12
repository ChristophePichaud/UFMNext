#pragma once

#include "Point.h"
#include "Rect.h"
#include "Color.h"
#include "ShapeType.h"
#include "ConnectionPoint.h"
#include "IDrawingContext.h"
#include <memory>
#include <string>
#include <vector>

namespace ufmnext::core {

class Shape {
public:
    Shape();
    virtual ~Shape() = default;

    // Pure virtual draw method - each derived shape overrides this
    virtual void Draw(IDrawingContext& ctx) const = 0;

    // Type identification
    virtual ShapeType GetType() const = 0;

    // Deep clone
    virtual std::unique_ptr<Shape> Clone() const = 0;

    // Spatial and hit testing
    virtual Rect GetBoundingBox() const;
    virtual bool HitTest(const Point& pt, double tolerance = 5.0) const;

    // Connection points for tracker & line connections
    virtual std::vector<ConnectionPoint> GetConnectionPoints() const;
    virtual bool GetConnectionPointByIndex(int index, ConnectionPoint& outPoint) const;
    virtual int FindClosestConnectionPoint(const Point& pt, double maxDistance = 25.0) const;

    // Common properties specified in requirements:
    // ID
    uint64_t GetId() const noexcept { return m_id; }
    void SetId(uint64_t id) noexcept { m_id = id; }

    // Position (x, y)
    Point GetPosition() const noexcept { return m_position; }
    void SetPosition(const Point& pos) noexcept { m_position = pos; }
    void SetPosition(double x, double y) noexcept { m_position = Point(x, y); }
    double GetX() const noexcept { return m_position.x; }
    double GetY() const noexcept { return m_position.y; }
    void SetX(double x) noexcept { m_position.x = x; }
    void SetY(double y) noexcept { m_position.y = y; }

    // Dimensions: width and length (height)
    double GetWidth() const noexcept { return m_width; }
    void SetWidth(double w) noexcept { m_width = w; }
    double GetHeight() const noexcept { return m_height; }
    void SetHeight(double h) noexcept { m_height = h; }
    double GetLength() const noexcept { return m_height; }
    void SetLength(double l) noexcept { m_height = l; }

    // Colors: color, fill color, outline color
    Color GetColor() const noexcept { return m_color; }
    void SetColor(const Color& c) noexcept { m_color = c; }
    Color GetFillColor() const noexcept { return m_fillColor; }
    void SetFillColor(const Color& c) noexcept { m_fillColor = c; }
    Color GetOutlineColor() const noexcept { return m_outlineColor; }
    void SetOutlineColor(const Color& c) noexcept { m_outlineColor = c; }

    // Fill enabled
    bool IsFillEnabled() const noexcept { return m_fillEnabled; }
    void SetFillEnabled(bool enabled) noexcept { m_fillEnabled = enabled; }

    // Visible
    bool IsVisible() const noexcept { return m_visible; }
    void SetVisible(bool vis) noexcept { m_visible = vis; }

    // Text, font size, font color
    const std::string& GetText() const noexcept { return m_text; }
    void SetText(const std::string& txt) { m_text = txt; }
    double GetFontSize() const noexcept { return m_fontSize; }
    void SetFontSize(double size) noexcept { m_fontSize = size; }
    Color GetFontColor() const noexcept { return m_fontColor; }
    void SetFontColor(const Color& c) noexcept { m_fontColor = c; }

    // Stroke width
    double GetStrokeWidth() const noexcept { return m_strokeWidth; }
    void SetStrokeWidth(double sw) noexcept { m_strokeWidth = sw; }

    // Selected state (used by UI/Tracker)
    bool IsSelected() const noexcept { return m_selected; }
    void SetSelected(bool sel) noexcept { m_selected = sel; }

    // Helper for translating shape
    virtual void MoveBy(const Point& delta);

protected:
    void DrawTextContent(IDrawingContext& ctx, const Rect& bounds) const;

    uint64_t m_id{0};
    Point m_position{0.0, 0.0};
    double m_width{100.0};
    double m_height{100.0};

    Color m_color{Color::Black()};
    Color m_outlineColor{Color::DarkGray()};
    Color m_fillColor{Color::SoftBlue()};
    bool m_fillEnabled{true};
    bool m_visible{true};

    std::string m_text;
    double m_fontSize{12.0};
    Color m_fontColor{Color::Black()};

    double m_strokeWidth{2.0};
    bool m_selected{false};
};

} // namespace ufmnext::core
