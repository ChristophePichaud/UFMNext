#include "ufmnext/core/ShapeFactory.h"
#include "ufmnext/core/SquareShape.h"
#include "ufmnext/core/RoundRectangleShape.h"
#include "ufmnext/core/CircleShape.h"
#include "ufmnext/core/OvalShape.h"
#include "ufmnext/core/LineShape.h"
#include "ufmnext/core/StarShape.h"
#include "ufmnext/core/ArrowShape.h"
#include "ufmnext/core/UmlShapes.h"
#include "ufmnext/core/PictureShapes.h"

namespace ufmnext::core {

ShapeFactory& ShapeFactory::Instance() {
    static ShapeFactory factory;
    return factory;
}

ShapeFactory::ShapeFactory() {
    RegisterBuiltIns();
}

bool ShapeFactory::Register(const std::string& className, Creator creator) {
    if (className.empty() || !creator) return false;
    return m_creators.emplace(className, std::move(creator)).second;
}

bool ShapeFactory::IsRegistered(const std::string& className) const {
    return m_creators.count(className) != 0;
}

std::shared_ptr<Shape> ShapeFactory::Create(const std::string& className) const {
    const auto it = m_creators.find(className);
    return it == m_creators.end() ? nullptr : it->second();
}

std::vector<std::string> ShapeFactory::GetRegisteredNames() const {
    std::vector<std::string> names;
    names.reserve(m_creators.size());
    for (const auto& kv : m_creators) names.push_back(kv.first);
    return names;
}

void ShapeFactory::RegisterBuiltIns() {
#define UFMNEXT_REGISTER(T) Register(#T, [] { return std::make_shared<T>(); })
    UFMNEXT_REGISTER(SquareShape);
    UFMNEXT_REGISTER(RoundRectangleShape);
    UFMNEXT_REGISTER(CircleShape);
    UFMNEXT_REGISTER(OvalShape);
    UFMNEXT_REGISTER(LineShape);
    UFMNEXT_REGISTER(StarShape);
    UFMNEXT_REGISTER(ArrowShape);

    UFMNEXT_REGISTER(UmlClassShape);
    UFMNEXT_REGISTER(UmlInterfaceShape);
    UFMNEXT_REGISTER(UmlActorShape);
    UFMNEXT_REGISTER(UmlUseCaseShape);
    UFMNEXT_REGISTER(UmlNoteShape);
    UFMNEXT_REGISTER(UmlPackageShape);

    UFMNEXT_REGISTER(HousePictureShape);
    UFMNEXT_REGISTER(TreePictureShape);
    UFMNEXT_REGISTER(SunPictureShape);
    UFMNEXT_REGISTER(CloudPictureShape);
    UFMNEXT_REGISTER(MoonPictureShape);
    UFMNEXT_REGISTER(HeartPictureShape);
    UFMNEXT_REGISTER(FlowerPictureShape);
    UFMNEXT_REGISTER(CarPictureShape);
    UFMNEXT_REGISTER(BoatPictureShape);
    UFMNEXT_REGISTER(MountainPictureShape);
#undef UFMNEXT_REGISTER
}

} // namespace ufmnext::core
