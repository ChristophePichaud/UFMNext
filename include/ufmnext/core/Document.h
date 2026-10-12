#pragma once

#include "Shape.h"
#include "LineShape.h"
#include "Rect.h"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace ufmnext::core {

struct PaperSize {
    double width{1200.0};
    double height{1400.0};

    constexpr PaperSize() noexcept = default;
    constexpr PaperSize(double w, double h) noexcept : width(w), height(h) {}

    constexpr bool operator==(const PaperSize& other) const noexcept {
        return std::abs(width - other.width) < 1e-6 && std::abs(height - other.height) < 1e-6;
    }
};

class Document {
public:
    Document();
    explicit Document(const PaperSize& paperSize);

    // Paper size parametrization (default 1200 x 1400 or any resolution)
    PaperSize GetPaperSize() const noexcept { return m_paperSize; }
    void SetPaperSize(const PaperSize& size) noexcept { m_paperSize = size; }
    void SetPaperSize(double width, double height) noexcept { m_paperSize = PaperSize(width, height); }

    // Shape management
    uint64_t AddShape(std::shared_ptr<Shape> shape);
    bool RemoveShape(uint64_t id);
    void Clear();

    const std::vector<std::shared_ptr<Shape>>& GetShapes() const noexcept { return m_shapes; }
    std::shared_ptr<Shape> FindShapeById(uint64_t id) const;
    std::shared_ptr<Shape> FindShapeAt(const Point& pt, double tolerance = 5.0) const;

    // Fast rendering: returns only shapes that intersect the visible viewport
    std::vector<std::shared_ptr<Shape>> GetVisibleShapes(const Rect& viewport) const;

    // Selection management
    void SelectShape(uint64_t id, bool clearOthers = true);
    void DeselectShape(uint64_t id);
    void ClearSelection();
    std::vector<std::shared_ptr<Shape>> GetSelectedShapes() const;
    std::shared_ptr<Shape> GetPrimarySelectedShape() const;

    // Connection tracking updates: updates endpoints of lines connected to shapes
    void UpdateConnectedLines();

    // Z-order
    bool BringToFront(uint64_t id);
    bool SendToBack(uint64_t id);
    bool BringForward(uint64_t id);
    bool SendBackward(uint64_t id);

    // Serialization / Raw Data
    std::string SaveAsRawData() const;
    bool LoadFromRawData(const std::string& rawData);

    // Modified state
    bool IsModified() const noexcept { return m_modified; }
    void SetModified(bool mod) noexcept { m_modified = mod; }

private:
    PaperSize m_paperSize{1200.0, 1400.0};
    std::vector<std::shared_ptr<Shape>> m_shapes;
    uint64_t m_nextId{1};
    bool m_modified{false};
};

} // namespace ufmnext::core
