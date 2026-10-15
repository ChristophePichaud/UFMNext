#pragma once

#include <string>
#include <vector>

namespace ufmnext::core {

struct PaletteItem {
    std::string name;       // display name
    std::string shapeClass; // derived class name, resolved through ShapeFactory
    std::string startCap;   // optional, for LineShape: "None", "Arrow" or "Dot"
    std::string endCap;     // optional, same values
};

struct Palette {
    std::string name;
    std::vector<PaletteItem> items;
};

// JSON format (nlohmann json):
//   { "name": "UML", "items": [ { "name": "Class", "shape": "UmlClassShape" }, ... ] }
// Items may also carry "startCap" / "endCap" (line shapes).
class PaletteLoader {
public:
    // Both return false on malformed JSON or missing/invalid fields; outPalette is untouched then.
    // Items whose class is not registered in ShapeFactory are skipped.
    static bool LoadFromString(const std::string& json, Palette& outPalette);
    static bool LoadFromFile(const std::string& path, Palette& outPalette);
};

} // namespace ufmnext::core
