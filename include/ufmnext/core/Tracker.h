#pragma once

#include "Point.h"
#include "Rect.h"
#include "Shape.h"
#include "ConnectionPoint.h"
#include <vector>
#include <memory>

namespace ufmnext::core {

enum class HandleId {
    None = 0,
    Body,
    TopLeft,
    TopCenter,
    TopRight,
    MiddleLeft,
    MiddleRight,
    BottomLeft,
    BottomCenter,
    BottomRight,
    LineStart,
    LineEnd
};

struct TrackerHandle {
    HandleId id{HandleId::None};
    Point position{0.0, 0.0};
    Rect rect{0.0, 0.0, 0.0, 0.0};
};

struct SnapResult {
    bool snapped{false};
    uint64_t targetShapeId{0};
    int anchorIndex{-1};
    Point anchorPosition{0.0, 0.0};
};

class Tracker {
public:
    static constexpr double DefaultHandleSize = 8.0;

    static std::vector<TrackerHandle> GetHandles(const Shape& shape, double handleSize = DefaultHandleSize);
    static HandleId HitTestHandle(const Shape& shape, const Point& pt, double handleSize = DefaultHandleSize);

    static void ResizeShape(Shape& shape,
                            HandleId handle,
                            const Point& currentPt,
                            const Rect& originalBounds,
                            bool keepAspectRatio = false);

    static void MoveShape(Shape& shape, const Point& delta);

    static SnapResult FindSnapAnchor(const Point& pt,
                                     const std::vector<std::shared_ptr<Shape>>& shapes,
                                     uint64_t ignoreShapeId = 0,
                                     double snapRadius = 16.0);
};

} // namespace ufmnext::core
