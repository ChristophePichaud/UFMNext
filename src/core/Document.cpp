#include "ufmnext/core/Document.h"
#include "ufmnext/core/Serializer.h"
#include <algorithm>

namespace ufmnext::core {

Document::Document()
    : m_paperSize(1200.0, 1400.0)
    , m_nextId(1)
    , m_modified(false)
{
}

Document::Document(const PaperSize& paperSize)
    : m_paperSize(paperSize)
    , m_nextId(1)
    , m_modified(false)
{
}

uint64_t Document::AddShape(std::shared_ptr<Shape> shape) {
    if (!shape) {
        return 0;
    }
    if (shape->GetId() == 0) {
        shape->SetId(m_nextId++);
    } else if (shape->GetId() >= m_nextId) {
        m_nextId = shape->GetId() + 1;
    }

    m_shapes.push_back(shape);
    m_modified = true;
    UpdateConnectedLines();
    return shape->GetId();
}

bool Document::RemoveShape(uint64_t id) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) {
        return s && s->GetId() == id;
    });

    if (it == m_shapes.end()) {
        return false;
    }

    m_shapes.erase(it);

    // Clean up any connected lines
    for (auto& shape : m_shapes) {
        if (shape && shape->GetType() == ShapeType::Line) {
            auto* line = dynamic_cast<LineShape*>(shape.get());
            if (line) {
                if (line->GetStartConnectedShapeId() == id) {
                    line->DisconnectStart();
                }
                if (line->GetEndConnectedShapeId() == id) {
                    line->DisconnectEnd();
                }
            }
        }
    }

    m_modified = true;
    return true;
}

void Document::Clear() {
    m_shapes.clear();
    m_nextId = 1;
    m_modified = true;
}

std::shared_ptr<Shape> Document::FindShapeById(uint64_t id) const {
    for (const auto& shape : m_shapes) {
        if (shape && shape->GetId() == id) {
            return shape;
        }
    }
    return nullptr;
}

std::shared_ptr<Shape> Document::FindShapeAt(const Point& pt, double tolerance) const {
    // Iterate in reverse z-order so top-most shapes are hit first
    for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it) {
        if (*it && (*it)->HitTest(pt, tolerance)) {
            return *it;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Shape>> Document::GetVisibleShapes(const Rect& viewport) const {
    std::vector<std::shared_ptr<Shape>> visible;
    visible.reserve(m_shapes.size());

    for (const auto& shape : m_shapes) {
        if (!shape || !shape->IsVisible()) {
            continue;
        }
        if (shape->GetBoundingBox().Intersects(viewport)) {
            visible.push_back(shape);
        }
    }
    return visible;
}

void Document::SelectShape(uint64_t id, bool clearOthers) {
    for (auto& shape : m_shapes) {
        if (!shape) continue;
        if (shape->GetId() == id) {
            shape->SetSelected(true);
        } else if (clearOthers) {
            shape->SetSelected(false);
        }
    }
}

void Document::DeselectShape(uint64_t id) {
    for (auto& shape : m_shapes) {
        if (shape && shape->GetId() == id) {
            shape->SetSelected(false);
        }
    }
}

void Document::ClearSelection() {
    for (auto& shape : m_shapes) {
        if (shape) {
            shape->SetSelected(false);
        }
    }
}

std::vector<std::shared_ptr<Shape>> Document::GetSelectedShapes() const {
    std::vector<std::shared_ptr<Shape>> selected;
    for (const auto& shape : m_shapes) {
        if (shape && shape->IsSelected()) {
            selected.push_back(shape);
        }
    }
    return selected;
}

std::shared_ptr<Shape> Document::GetPrimarySelectedShape() const {
    for (const auto& shape : m_shapes) {
        if (shape && shape->IsSelected()) {
            return shape;
        }
    }
    return nullptr;
}

void Document::UpdateConnectedLines() {
    for (auto& shape : m_shapes) {
        if (!shape || shape->GetType() != ShapeType::Line) {
            continue;
        }

        auto* line = dynamic_cast<LineShape*>(shape.get());
        if (!line) {
            continue;
        }

        if (line->GetStartConnectedShapeId() != 0) {
            auto target = FindShapeById(line->GetStartConnectedShapeId());
            if (target) {
                ConnectionPoint cp;
                if (target->GetConnectionPointByIndex(line->GetStartAnchorIndex(), cp)) {
                    line->SetStartPoint(cp.position);
                }
            } else {
                line->DisconnectStart();
            }
        }

        if (line->GetEndConnectedShapeId() != 0) {
            auto target = FindShapeById(line->GetEndConnectedShapeId());
            if (target) {
                ConnectionPoint cp;
                if (target->GetConnectionPointByIndex(line->GetEndAnchorIndex(), cp)) {
                    line->SetEndPoint(cp.position);
                }
            } else {
                line->DisconnectEnd();
            }
        }
    }
}

bool Document::BringToFront(uint64_t id) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) {
        return s && s->GetId() == id;
    });
    if (it != m_shapes.end() && it + 1 != m_shapes.end()) {
        auto shape = *it;
        m_shapes.erase(it);
        m_shapes.push_back(shape);
        m_modified = true;
        return true;
    }
    return false;
}

bool Document::SendToBack(uint64_t id) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) {
        return s && s->GetId() == id;
    });
    if (it != m_shapes.end() && it != m_shapes.begin()) {
        auto shape = *it;
        m_shapes.erase(it);
        m_shapes.insert(m_shapes.begin(), shape);
        m_modified = true;
        return true;
    }
    return false;
}

bool Document::BringForward(uint64_t id) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) {
        return s && s->GetId() == id;
    });
    if (it != m_shapes.end() && it + 1 != m_shapes.end()) {
        std::iter_swap(it, it + 1);
        m_modified = true;
        return true;
    }
    return false;
}

bool Document::SendBackward(uint64_t id) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(), [id](const auto& s) {
        return s && s->GetId() == id;
    });
    if (it != m_shapes.end() && it != m_shapes.begin()) {
        std::iter_swap(it, it - 1);
        m_modified = true;
        return true;
    }
    return false;
}

std::string Document::SaveAsRawData() const {
    return Serializer::SerializeDocumentToRawData(*this);
}

bool Document::LoadFromRawData(const std::string& rawData) {
    return Serializer::DeserializeDocumentFromRawData(rawData, *this);
}

} // namespace ufmnext::core
