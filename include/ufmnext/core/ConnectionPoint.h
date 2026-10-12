#pragma once

#include "Point.h"

namespace ufmnext::core {

enum class AnchorLocation {
    Top = 0,
    Right,
    Bottom,
    Left,
    Center,
    Custom
};

struct ConnectionPoint {
    int index{0};
    uint64_t ownerShapeId{0};
    Point position{0.0, 0.0};
    Point normal{0.0, 0.0}; // outward unit normal
    AnchorLocation location{AnchorLocation::Center};

    constexpr ConnectionPoint() noexcept = default;
    constexpr ConnectionPoint(int idx, uint64_t ownerId, Point pos, Point norm, AnchorLocation loc) noexcept
        : index(idx), ownerShapeId(ownerId), position(pos), normal(norm), location(loc) {}
};

} // namespace ufmnext::core
