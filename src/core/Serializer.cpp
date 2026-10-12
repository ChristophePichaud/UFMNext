#include "ufmnext/core/Serializer.h"
#include "ufmnext/core/SquareShape.h"
#include "ufmnext/core/RoundRectangleShape.h"
#include "ufmnext/core/CircleShape.h"
#include "ufmnext/core/OvalShape.h"
#include "ufmnext/core/LineShape.h"
#include "ufmnext/core/StarShape.h"
#include "ufmnext/core/ArrowShape.h"

#include <fstream>
#include <sstream>
#include <iomanip>

namespace ufmnext::core {

static std::string EscapeString(const std::string& str) {
    std::ostringstream ss;
    for (char c : str) {
        if (c == '\\') ss << "\\\\";
        else if (c == '\n') ss << "\\n";
        else if (c == '\r') ss << "\\r";
        else if (c == '\t') ss << "\\t";
        else ss << c;
    }
    return ss.str();
}

static std::string UnescapeString(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            ++i;
            switch (str[i]) {
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                case '\\': out += '\\'; break;
                default: out += str[i]; break;
            }
        } else {
            out += str[i];
        }
    }
    return out;
}

std::string Serializer::SerializeDocumentToRawData(const Document& doc) {
    std::ostringstream os;
    os << "# UFMNext Raw Drawing Data v1.0\n";
    os << "paper_width " << doc.GetPaperSize().width << "\n";
    os << "paper_height " << doc.GetPaperSize().height << "\n";
    os << "shape_count " << doc.GetShapes().size() << "\n\n";

    for (const auto& shape : doc.GetShapes()) {
        if (!shape) continue;

        os << "begin_shape\n";
        os << "type " << ShapeTypeToString(shape->GetType()) << "\n";
        os << "id " << shape->GetId() << "\n";
        os << "x " << shape->GetX() << "\n";
        os << "y " << shape->GetY() << "\n";
        os << "width " << shape->GetWidth() << "\n";
        os << "height " << shape->GetHeight() << "\n";
        os << "color " << shape->GetColor().ToHex(true) << "\n";
        os << "outline_color " << shape->GetOutlineColor().ToHex(true) << "\n";
        os << "fill_color " << shape->GetFillColor().ToHex(true) << "\n";
        os << "fill_enabled " << (shape->IsFillEnabled() ? 1 : 0) << "\n";
        os << "visible " << (shape->IsVisible() ? 1 : 0) << "\n";
        os << "font_size " << shape->GetFontSize() << "\n";
        os << "font_color " << shape->GetFontColor().ToHex(true) << "\n";
        os << "stroke_width " << shape->GetStrokeWidth() << "\n";
        os << "text " << EscapeString(shape->GetText()) << "\n";

        switch (shape->GetType()) {
            case ShapeType::RoundRectangle: {
                const auto* rr = dynamic_cast<const RoundRectangleShape*>(shape.get());
                if (rr) {
                    os << "corner_radius " << rr->GetCornerRadius() << "\n";
                }
                break;
            }
            case ShapeType::Star: {
                const auto* star = dynamic_cast<const StarShape*>(shape.get());
                if (star) {
                    os << "star_points " << star->GetPointsCount() << "\n";
                }
                break;
            }
            case ShapeType::Arrow: {
                const auto* arrow = dynamic_cast<const ArrowShape*>(shape.get());
                if (arrow) {
                    os << "arrow_direction " << static_cast<int>(arrow->GetDirection()) << "\n";
                }
                break;
            }
            case ShapeType::Line: {
                const auto* line = dynamic_cast<const LineShape*>(shape.get());
                if (line) {
                    os << "start_cap " << LineCapStyleToString(line->GetStartCap()) << "\n";
                    os << "end_cap " << LineCapStyleToString(line->GetEndCap()) << "\n";
                    os << "start_connected_id " << line->GetStartConnectedShapeId() << "\n";
                    os << "start_anchor_idx " << line->GetStartAnchorIndex() << "\n";
                    os << "end_connected_id " << line->GetEndConnectedShapeId() << "\n";
                    os << "end_anchor_idx " << line->GetEndAnchorIndex() << "\n";
                }
                break;
            }
            default:
                break;
        }

        os << "end_shape\n\n";
    }

    return os.str();
}

bool Serializer::DeserializeDocumentFromRawData(const std::string& data, Document& doc) {
    std::istringstream is(data);
    std::string line;

    std::getline(is, line);
    if (line.find("# UFMNext Raw Drawing Data") == std::string::npos) {
        return false;
    }

    doc.Clear();

    double paperW = 1200.0;
    double paperH = 1400.0;

    std::shared_ptr<Shape> currentShape;
    std::string currentType;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream lineStream(line);
        std::string key;
        lineStream >> key;

        if (key == "paper_width") {
            lineStream >> paperW;
        } else if (key == "paper_height") {
            lineStream >> paperH;
        } else if (key == "begin_shape") {
            currentShape.reset();
            currentType.clear();
        } else if (key == "type") {
            lineStream >> currentType;
            ShapeType t = StringToShapeType(currentType);
            switch (t) {
                case ShapeType::Square:
                    currentShape = std::make_shared<SquareShape>();
                    break;
                case ShapeType::RoundRectangle:
                    currentShape = std::make_shared<RoundRectangleShape>();
                    break;
                case ShapeType::Circle:
                    currentShape = std::make_shared<CircleShape>();
                    break;
                case ShapeType::Oval:
                    currentShape = std::make_shared<OvalShape>();
                    break;
                case ShapeType::Line:
                    currentShape = std::make_shared<LineShape>();
                    break;
                case ShapeType::Star:
                    currentShape = std::make_shared<StarShape>();
                    break;
                case ShapeType::Arrow:
                    currentShape = std::make_shared<ArrowShape>();
                    break;
                default:
                    currentShape = std::make_shared<SquareShape>();
                    break;
            }
        } else if (currentShape) {
            if (key == "id") {
                uint64_t id = 0;
                lineStream >> id;
                currentShape->SetId(id);
            } else if (key == "x") {
                double x = 0.0;
                lineStream >> x;
                currentShape->SetX(x);
            } else if (key == "y") {
                double y = 0.0;
                lineStream >> y;
                currentShape->SetY(y);
            } else if (key == "width") {
                double w = 0.0;
                lineStream >> w;
                currentShape->SetWidth(w);
            } else if (key == "height") {
                double h = 0.0;
                lineStream >> h;
                currentShape->SetHeight(h);
            } else if (key == "color") {
                std::string hex;
                lineStream >> hex;
                currentShape->SetColor(Color::FromHex(hex));
            } else if (key == "outline_color") {
                std::string hex;
                lineStream >> hex;
                currentShape->SetOutlineColor(Color::FromHex(hex));
            } else if (key == "fill_color") {
                std::string hex;
                lineStream >> hex;
                currentShape->SetFillColor(Color::FromHex(hex));
            } else if (key == "fill_enabled") {
                int fe = 1;
                lineStream >> fe;
                currentShape->SetFillEnabled(fe != 0);
            } else if (key == "visible") {
                int vis = 1;
                lineStream >> vis;
                currentShape->SetVisible(vis != 0);
            } else if (key == "font_size") {
                double fs = 12.0;
                lineStream >> fs;
                currentShape->SetFontSize(fs);
            } else if (key == "font_color") {
                std::string hex;
                lineStream >> hex;
                currentShape->SetFontColor(Color::FromHex(hex));
            } else if (key == "stroke_width") {
                double sw = 2.0;
                lineStream >> sw;
                currentShape->SetStrokeWidth(sw);
            } else if (key == "text") {
                std::string rawTxt;
                std::getline(lineStream, rawTxt);
                if (!rawTxt.empty() && rawTxt[0] == ' ') {
                    rawTxt = rawTxt.substr(1);
                }
                currentShape->SetText(UnescapeString(rawTxt));
            } else if (key == "corner_radius") {
                auto* rr = dynamic_cast<RoundRectangleShape*>(currentShape.get());
                if (rr) {
                    double cr = 15.0;
                    lineStream >> cr;
                    rr->SetCornerRadius(cr);
                }
            } else if (key == "star_points") {
                auto* star = dynamic_cast<StarShape*>(currentShape.get());
                if (star) {
                    int pts = 5;
                    lineStream >> pts;
                    star->SetPointsCount(pts);
                }
            } else if (key == "arrow_direction") {
                auto* arrow = dynamic_cast<ArrowShape*>(currentShape.get());
                if (arrow) {
                    int dir = 0;
                    lineStream >> dir;
                    arrow->SetDirection(static_cast<ArrowDirection>(dir));
                }
            } else if (key == "start_cap") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    std::string cap;
                    lineStream >> cap;
                    lineShape->SetStartCap(StringToLineCapStyle(cap));
                }
            } else if (key == "end_cap") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    std::string cap;
                    lineStream >> cap;
                    lineShape->SetEndCap(StringToLineCapStyle(cap));
                }
            } else if (key == "start_connected_id") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    uint64_t scId = 0;
                    lineStream >> scId;
                    lineShape->SetStartConnectedShapeId(scId, lineShape->GetStartAnchorIndex());
                }
            } else if (key == "start_anchor_idx") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    int sIdx = -1;
                    lineStream >> sIdx;
                    lineShape->SetStartConnectedShapeId(lineShape->GetStartConnectedShapeId(), sIdx);
                }
            } else if (key == "end_connected_id") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    uint64_t ecId = 0;
                    lineStream >> ecId;
                    lineShape->SetEndConnectedShapeId(ecId, lineShape->GetEndAnchorIndex());
                }
            } else if (key == "end_anchor_idx") {
                auto* lineShape = dynamic_cast<LineShape*>(currentShape.get());
                if (lineShape) {
                    int eIdx = -1;
                    lineStream >> eIdx;
                    lineShape->SetEndConnectedShapeId(lineShape->GetEndConnectedShapeId(), eIdx);
                }
            } else if (key == "end_shape") {
                doc.AddShape(currentShape);
                currentShape.reset();
            }
        }
    }

    doc.SetPaperSize(PaperSize(paperW, paperH));
    doc.UpdateConnectedLines();
    doc.SetModified(false);
    return true;
}

bool Serializer::SaveToFile(const Document& doc, const std::string& filePath) {
    std::ofstream ofs(filePath);
    if (!ofs.is_open()) {
        return false;
    }
    ofs << SerializeDocumentToRawData(doc);
    return true;
}

bool Serializer::LoadFromFile(const std::string& filePath, Document& doc) {
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        return false;
    }
    std::string content((std::istreambuf_iterator<char>(ifs)),
                        std::istreambuf_iterator<char>());
    return DeserializeDocumentFromRawData(content, doc);
}

} // namespace ufmnext::core
