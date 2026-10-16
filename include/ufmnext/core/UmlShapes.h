#pragma once

#include "Shape.h"

namespace ufmnext::core {

// Base for UML shapes: reports ShapeType::Custom and clones via the copy constructor.
class UmlShapeBase : public Shape {
public:
    ShapeType GetType() const override { return ShapeType::Custom; }

protected:
    UmlShapeBase();
    // Draws the (first line of the) text centered in the given bounds
    void DrawLabel(IDrawingContext& ctx, const Rect& bounds, const std::string& fallback) const;
};

#define UFMNEXT_DECLARE_UML_SHAPE(ClassName)                                   \
class ClassName : public UmlShapeBase {                                        \
public:                                                                        \
    ClassName();                                                               \
    void Draw(IDrawingContext& ctx) const override;                            \
    std::string GetClassName() const override { return #ClassName; }           \
    std::unique_ptr<Shape> Clone() const override {                            \
        return std::make_unique<ClassName>(*this);                             \
    }                                                                          \
};

UFMNEXT_DECLARE_UML_SHAPE(UmlClassShape)
UFMNEXT_DECLARE_UML_SHAPE(UmlInterfaceShape)
UFMNEXT_DECLARE_UML_SHAPE(UmlActorShape)
UFMNEXT_DECLARE_UML_SHAPE(UmlUseCaseShape)
UFMNEXT_DECLARE_UML_SHAPE(UmlNoteShape)
UFMNEXT_DECLARE_UML_SHAPE(UmlPackageShape)

} // namespace ufmnext::core
