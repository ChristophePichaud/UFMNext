#include "ufmnext/core/Tracker.h"
#include "ufmnext/core/LineShape.h"
#include <algorithm>
#include <cmath>

namespace ufmnext::core {

std::vector<TrackerHandle> Tracker::GetHandles(const Shape& shape, double handleSize) {
    std::vector<TrackerHandle> handles;
    const double halfSize = handleSize * 0.5;

    if (shape.GetType() == ShapeType::Line) {
        const auto* line = dynamic_cast<const LineShape*>(&shape);
        if (line) {
            const Point p1 = line->GetStartPoint();
            const Point p2 = line->GetEndPoint();

            handles.push_back({HandleId::LineStart, p1, Rect(p1.x - halfSize, p1.y - halfSize, handleSize, handleSize)});
            handles.push_back({HandleId::LineEnd, p2, Rect(p2.x - halfSize, p2.y - halfSize, handleSize, handleSize)});
            return handles;
        }
    }

    const Rect b = shape.GetBoundingBox();
    const double midX = b.x + b.width * 0.5;
    const double midY = b.y + b.height * 0.5;
    const double right = b.Right();
    const double bottom = b.Bottom();

    auto addHandle = [&](HandleId id, double hx, double hy) {
        handles.push_back({id, Point(hx, hy), Rect(hx - halfSize, hy - halfSize, handleSize, handleSize)});
    };

    addHandle(HandleId::TopLeft, b.x, b.y);
    addHandle(HandleId::TopCenter, midX, b.y);
    addHandle(HandleId::TopRight, right, b.y);
    addHandle(HandleId::MiddleLeft, b.x, midY);
    addHandle(HandleId::MiddleRight, right, midY);
    addHandle(HandleId::BottomLeft, b.x, bottom);
    addHandle(HandleId::BottomCenter, midX, bottom);
    addHandle(HandleId::BottomRight, right, bottom);

    return handles;
}

HandleId Tracker::HitTestHandle(const Shape& shape, const Point& pt, double handleSize) {
    const auto handles = GetHandles(shape, handleSize);
    for (const auto& h : handles) {
        if (h.rect.Contains(pt)) {
            return h.id;
        }
    }
    if (shape.HitTest(pt, 2.0)) {
        return HandleId::Body;
    }
    return HandleId::None;
}

void Tracker::ResizeShape(Shape& shape,
                          HandleId handle,
                          const Point& currentPt,
                          const Rect& originalBounds,
                          bool keepAspectRatio)
{
    if (shape.GetType() == ShapeType::Line) {
        auto* line = dynamic_cast<LineShape*>(&shape);
        if (line) {
            if (handle == HandleId::LineStart) {
                line->SetStartPoint(currentPt);
            } else if (handle == HandleId::LineEnd) {
                line->SetEndPoint(currentPt);
            }
        }
        return;
    }

    double left = originalBounds.x;
    double top = originalBounds.y;
    double right = originalBounds.Right();
    double bottom = originalBounds.Bottom();

    switch (handle) {
        case HandleId::TopLeft:
            left = std::min(currentPt.x, right - 10.0);
            top = std::min(currentPt.y, bottom - 10.0);
            break;
        case HandleId::TopCenter:
            top = std::min(currentPt.y, bottom - 10.0);
            break;
        case HandleId::TopRight:
            right = std::max(currentPt.x, left + 10.0);
            top = std::min(currentPt.y, bottom - 10.0);
            break;
        case HandleId::MiddleLeft:
            left = std::min(currentPt.x, right - 10.0);
            break;
        case HandleId::MiddleRight:
            right = std::max(currentPt.x, left + 10.0);
            break;
        case HandleId::BottomLeft:
            left = std::min(currentPt.x, right - 10.0);
            bottom = std::max(currentPt.y, top + 10.0);
            break;
        case HandleId::BottomCenter:
            bottom = std::max(currentPt.y, top + 10.0);
            break;
        case HandleId::BottomRight:
            right = std::max(currentPt.x, left + 10.0);
            bottom = std::max(currentPt.y, top + 10.0);
            break;
        default:
            return;
    }

    double w = right - left;
    double h = bottom - top;

    if (keepAspectRatio && originalBounds.width > 0 && originalBounds.height > 0) {
        const double aspect = originalBounds.width / originalBounds.height;
        if (w / h > aspect) {
            w = h * aspect;
        } else {
            h = w / aspect;
        }
    }

    shape.SetPosition(left, top);
    shape.SetWidth(w);
    shape.SetHeight(h);
}

void Tracker::MoveShape(Shape& shape, const Point& delta) {
    shape.MoveBy(delta);
}

SnapResult Tracker::FindSnapAnchor(const Point& pt,
                                   const std::vector<std::shared_ptr<Shape>>& shapes,
                                   uint64_t ignoreShapeId,
                                   double snapRadius)
{
    SnapResult result;
    double closestDist = snapRadius;

    for (const auto& shape : shapes) {
        if (!shape || shape->GetId() == ignoreShapeId || !shape->IsVisible()) {
            continue;
        }

        const auto connectionPoints = shape->GetConnectionPoints();
        for (const auto& cp : connectionPoints) {
            const double dist = pt.DistanceTo(cp.position);
            if (dist < closestDist) {
                closestDist = dist;
                result.snapped = true;
                result.targetShapeId = shape->GetId();
                result.anchorIndex = cp.index;
                result.anchorPosition = cp.position;
            }
        }
    }

    return result;
}

} // namespace ufmnext::core
