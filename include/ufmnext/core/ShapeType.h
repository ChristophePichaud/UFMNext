#pragma once

#include <string>

namespace ufmnext::core {

enum class ShapeType {
    None = 0,
    Square,         // Square / Rectangle
    RoundRectangle, // Rounded Rectangle
    Circle,         // Circle
    Oval,           // Oval / Ellipse
    Line,           // Line with optional arrow/dot endpoints
    Star,           // 5-Point Star
    Arrow,          // Block Arrow
    Custom          // Factory-created shape (UML, pictures, plugins); see Shape::GetClassName()
};

inline const char* ShapeTypeToString(ShapeType type) {
    switch (type) {
        case ShapeType::Square: return "Square";
        case ShapeType::RoundRectangle: return "RoundRectangle";
        case ShapeType::Circle: return "Circle";
        case ShapeType::Oval: return "Oval";
        case ShapeType::Line: return "Line";
        case ShapeType::Star: return "Star";
        case ShapeType::Arrow: return "Arrow";
        case ShapeType::Custom: return "Custom";
        default: return "None";
    }
}

inline ShapeType StringToShapeType(const std::string& str) {
    if (str == "Square" || str == "Rectangle") return ShapeType::Square;
    if (str == "RoundRectangle" || str == "Round") return ShapeType::RoundRectangle;
    if (str == "Circle") return ShapeType::Circle;
    if (str == "Oval" || str == "Ellipse") return ShapeType::Oval;
    if (str == "Line") return ShapeType::Line;
    if (str == "Star") return ShapeType::Star;
    if (str == "Arrow") return ShapeType::Arrow;
    if (str == "Custom") return ShapeType::Custom;
    return ShapeType::None;
}

enum class LineCapStyle {
    None = 0,
    Arrow,
    Dot
};

inline const char* LineCapStyleToString(LineCapStyle style) {
    switch (style) {
        case LineCapStyle::Arrow: return "Arrow";
        case LineCapStyle::Dot: return "Dot";
        default: return "None";
    }
}

inline LineCapStyle StringToLineCapStyle(const std::string& str) {
    if (str == "Arrow") return LineCapStyle::Arrow;
    if (str == "Dot") return LineCapStyle::Dot;
    return LineCapStyle::None;
}

enum class TextAlignment {
    Center = 0,
    Left,
    Right
};

} // namespace ufmnext::core
