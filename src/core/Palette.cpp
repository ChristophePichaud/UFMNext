#include "ufmnext/core/Palette.h"
#include "ufmnext/core/ShapeFactory.h"
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>

namespace ufmnext::core {

bool PaletteLoader::LoadFromString(const std::string& json, Palette& outPalette) {
    try {
        const auto doc = nlohmann::json::parse(json);
        if (!doc.is_object() || !doc.contains("name") || !doc["name"].is_string() ||
            !doc.contains("items") || !doc["items"].is_array()) {
            return false;
        }
        Palette result;
        result.name = doc["name"].get<std::string>();
        const auto& factory = ShapeFactory::Instance();
        for (const auto& item : doc["items"]) {
            if (!item.is_object() || !item.contains("shape") || !item["shape"].is_string()) {
                return false;
            }
            PaletteItem pi;
            pi.shapeClass = item["shape"].get<std::string>();
            pi.name = (item.contains("name") && item["name"].is_string()) ? item["name"].get<std::string>()
                                                                          : pi.shapeClass;
            if (item.contains("startCap") && item["startCap"].is_string()) {
                pi.startCap = item["startCap"].get<std::string>();
            }
            if (item.contains("endCap") && item["endCap"].is_string()) {
                pi.endCap = item["endCap"].get<std::string>();
            }
            if (factory.IsRegistered(pi.shapeClass)) {
                result.items.push_back(std::move(pi));
            }
        }
        outPalette = std::move(result);
        return true;
    } catch (const nlohmann::json::exception&) {
        return false;
    }
}

bool PaletteLoader::LoadFromFile(const std::string& path, Palette& outPalette) {
    std::ifstream in(path);
    if (!in) return false;
    std::ostringstream ss;
    ss << in.rdbuf();
    return LoadFromString(ss.str(), outPalette);
}

} // namespace ufmnext::core
