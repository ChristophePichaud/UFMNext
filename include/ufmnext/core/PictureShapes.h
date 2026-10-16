#pragma once

#include "Shape.h"

namespace ufmnext::core {

// Base for picture shapes: a small vector picture scaled to the shape bounds.
class PictureShapeBase : public Shape {
public:
    ShapeType GetType() const override { return ShapeType::Custom; }

protected:
    PictureShapeBase();
};

#define UFMNEXT_DECLARE_PICTURE_SHAPE(ClassName)                               \
class ClassName : public PictureShapeBase {                                    \
public:                                                                        \
    ClassName() = default;                                                     \
    void Draw(IDrawingContext& ctx) const override;                            \
    std::string GetClassName() const override { return #ClassName; }           \
    std::unique_ptr<Shape> Clone() const override {                            \
        return std::make_unique<ClassName>(*this);                             \
    }                                                                          \
};

UFMNEXT_DECLARE_PICTURE_SHAPE(HousePictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(TreePictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(SunPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(CloudPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(MoonPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(HeartPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(FlowerPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(CarPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(BoatPictureShape)
UFMNEXT_DECLARE_PICTURE_SHAPE(MountainPictureShape)

} // namespace ufmnext::core
