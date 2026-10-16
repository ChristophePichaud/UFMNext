#pragma once

#include "Shape.h"
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace ufmnext::core {

// Creates shapes by their derived class name (e.g. "StarShape", "UmlClassShape").
// Built-in shapes are registered on first use of Instance(); plugins add their own.
class ShapeFactory {
public:
    using Creator = std::function<std::shared_ptr<Shape>()>;

    static ShapeFactory& Instance();

    // Returns false if the name is empty, the creator is empty or the name is already registered.
    bool Register(const std::string& className, Creator creator);
    bool IsRegistered(const std::string& className) const;

    // Returns nullptr for unknown class names.
    std::shared_ptr<Shape> Create(const std::string& className) const;

    std::vector<std::string> GetRegisteredNames() const;

private:
    ShapeFactory();
    void RegisterBuiltIns();

    std::map<std::string, Creator> m_creators;
};

} // namespace ufmnext::core
