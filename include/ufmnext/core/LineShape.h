#pragma once

#include "Shape.h"

namespace ufmnext::core {

class LineShape : public Shape {
public:
    LineShape();
    LineShape(const Point& start, const Point& end);
    LineShape(double x1, double y1, double x2, double y2);

    void Draw(IDrawingContext& ctx) const override;
    ShapeType GetType() const override { return ShapeType::Line; }
    std::unique_ptr<Shape> Clone() const override;

    Rect GetBoundingBox() const override;
    bool HitTest(const Point& pt, double tolerance = 5.0) const override;

    Point GetStartPoint() const noexcept { return m_position; }
    void SetStartPoint(const Point& pt) noexcept;

    Point GetEndPoint() const noexcept { return Point(m_position.x + m_width, m_position.y + m_height); }
    void SetEndPoint(const Point& pt) noexcept;

    LineCapStyle GetStartCap() const noexcept { return m_startCap; }
    void SetStartCap(LineCapStyle cap) noexcept { m_startCap = cap; }

    LineCapStyle GetEndCap() const noexcept { return m_endCap; }
    void SetEndCap(LineCapStyle cap) noexcept { m_endCap = cap; }

    // Connection tracking
    uint64_t GetStartConnectedShapeId() const noexcept { return m_startConnectedShapeId; }
    void SetStartConnectedShapeId(uint64_t id, int anchorIndex = -1) noexcept {
        m_startConnectedShapeId = id;
        m_startAnchorIndex = anchorIndex;
    }
    int GetStartAnchorIndex() const noexcept { return m_startAnchorIndex; }

    uint64_t GetEndConnectedShapeId() const noexcept { return m_endConnectedShapeId; }
    void SetEndConnectedShapeId(uint64_t id, int anchorIndex = -1) noexcept {
        m_endConnectedShapeId = id;
        m_endAnchorIndex = anchorIndex;
    }
    int GetEndAnchorIndex() const noexcept { return m_endAnchorIndex; }

    void DisconnectStart() noexcept {
        m_startConnectedShapeId = 0;
        m_startAnchorIndex = -1;
    }

    void DisconnectEnd() noexcept {
        m_endConnectedShapeId = 0;
        m_endAnchorIndex = -1;
    }

    std::vector<ConnectionPoint> GetConnectionPoints() const override;

private:
    LineCapStyle m_startCap{LineCapStyle::None};
    LineCapStyle m_endCap{LineCapStyle::None};

    uint64_t m_startConnectedShapeId{0};
    int m_startAnchorIndex{-1};
    uint64_t m_endConnectedShapeId{0};
    int m_endAnchorIndex{-1};
};

} // namespace ufmnext::core
