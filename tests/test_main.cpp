#include "ufmnext/core/Point.h"
#include "ufmnext/core/Rect.h"
#include "ufmnext/core/Color.h"
#include "ufmnext/core/IDrawingContext.h"
#include "ufmnext/core/SquareShape.h"
#include "ufmnext/core/RoundRectangleShape.h"
#include "ufmnext/core/CircleShape.h"
#include "ufmnext/core/OvalShape.h"
#include "ufmnext/core/LineShape.h"
#include "ufmnext/core/StarShape.h"
#include "ufmnext/core/ArrowShape.h"
#include "ufmnext/core/Tracker.h"
#include "ufmnext/core/Document.h"
#include "ufmnext/core/Serializer.h"

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>

#define TEST_ASSERT(cond) \
    do { \
        if (!(cond)) { \
            std::cerr << "Assertion failed: " #cond << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::abort(); \
        } \
    } while (0)

using namespace ufmnext::core;

// Mock drawing context to verify ISO C++ rendering without Qt dependency
class MockDrawingContext : public IDrawingContext {
public:
    int rectDrawCount{0};
    int roundedRectDrawCount{0};
    int circleDrawCount{0};
    int ellipseDrawCount{0};
    int lineDrawCount{0};
    int polygonDrawCount{0};
    int textDrawCount{0};

    void DrawRect(const Rect&, const Color&, double, bool, const Color&) override {
        ++rectDrawCount;
    }

    void DrawRoundedRect(const Rect&, double, double, const Color&, double, bool, const Color&) override {
        ++roundedRectDrawCount;
    }

    void DrawCircle(const Point&, double, const Color&, double, bool, const Color&) override {
        ++circleDrawCount;
    }

    void DrawEllipse(const Rect&, const Color&, double, bool, const Color&) override {
        ++ellipseDrawCount;
    }

    void DrawLine(const Point&, const Point&, const Color&, double, LineCapStyle, LineCapStyle) override {
        ++lineDrawCount;
    }

    void DrawPolygon(const std::vector<Point>&, const Color&, double, bool, const Color&) override {
        ++polygonDrawCount;
    }

    void DrawText(const Rect&, const std::string&, const Color&, double, TextAlignment) override {
        ++textDrawCount;
    }
};

void TestShapeProperties() {
    std::cout << "Running TestShapeProperties..." << std::endl;

    SquareShape sq(10.0, 20.0, 100.0, 150.0);
    sq.SetId(42);
    sq.SetColor(Color::Red());
    sq.SetOutlineColor(Color::Blue());
    sq.SetFillColor(Color::Green());
    sq.SetFillEnabled(true);
    sq.SetVisible(true);
    sq.SetText("Office Box");
    sq.SetFontSize(14.0);
    sq.SetFontColor(Color::DarkGray());
    sq.SetStrokeWidth(3.0);

    TEST_ASSERT(sq.GetId() == 42);
    TEST_ASSERT(sq.GetX() == 10.0);
    TEST_ASSERT(sq.GetY() == 20.0);
    TEST_ASSERT(sq.GetWidth() == 100.0);
    TEST_ASSERT(sq.GetHeight() == 150.0);
    TEST_ASSERT(sq.GetLength() == 150.0);
    TEST_ASSERT(sq.GetColor() == Color::Red());
    TEST_ASSERT(sq.GetOutlineColor() == Color::Blue());
    TEST_ASSERT(sq.GetFillColor() == Color::Green());
    TEST_ASSERT(sq.IsFillEnabled() == true);
    TEST_ASSERT(sq.IsVisible() == true);
    TEST_ASSERT(sq.GetText() == "Office Box");
    TEST_ASSERT(sq.GetFontSize() == 14.0);
    TEST_ASSERT(sq.GetFontColor() == Color::DarkGray());
    TEST_ASSERT(sq.GetStrokeWidth() == 3.0);

    // Test clone
    auto clone = sq.Clone();
    TEST_ASSERT(clone != nullptr);
    TEST_ASSERT(clone->GetId() == 42);
    TEST_ASSERT(clone->GetText() == "Office Box");
    TEST_ASSERT(clone->GetType() == ShapeType::Square);

    std::cout << "TestShapeProperties PASSED!" << std::endl;
}

void TestDerivedShapesAndVirtualDraw() {
    std::cout << "Running TestDerivedShapesAndVirtualDraw..." << std::endl;

    MockDrawingContext ctx;

    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<SquareShape>(0, 0, 50, 50));
    shapes.push_back(std::make_unique<RoundRectangleShape>(60, 0, 50, 50, 10.0));
    shapes.push_back(std::make_unique<CircleShape>(150, 25, 25));
    shapes.push_back(std::make_unique<OvalShape>(200, 0, 80, 50));
    shapes.push_back(std::make_unique<LineShape>(0, 100, 100, 200));
    shapes.push_back(std::make_unique<StarShape>(100, 100, 60, 60, 5));
    shapes.push_back(std::make_unique<ArrowShape>(200, 100, 80, 40));

    // Polymorphic Draw calls
    for (const auto& s : shapes) {
        s->Draw(ctx);
    }

    TEST_ASSERT(ctx.rectDrawCount == 1);
    TEST_ASSERT(ctx.roundedRectDrawCount == 1);
    TEST_ASSERT(ctx.circleDrawCount == 1);
    TEST_ASSERT(ctx.ellipseDrawCount == 1);
    TEST_ASSERT(ctx.lineDrawCount == 1);
    TEST_ASSERT(ctx.polygonDrawCount == 2); // Star and Arrow are polygons

    std::cout << "TestDerivedShapesAndVirtualDraw PASSED!" << std::endl;
}

void TestTrackerAndInteractions() {
    std::cout << "Running TestTrackerAndInteractions..." << std::endl;

    SquareShape sq(50.0, 50.0, 100.0, 100.0);
    auto handles = Tracker::GetHandles(sq, 8.0);
    TEST_ASSERT(handles.size() == 8);

    // Hit test top-left handle
    HandleId hTopLeft = Tracker::HitTestHandle(sq, Point(50.0, 50.0), 8.0);
    TEST_ASSERT(hTopLeft == HandleId::TopLeft);

    // Hit test body
    HandleId hBody = Tracker::HitTestHandle(sq, Point(100.0, 100.0), 8.0);
    TEST_ASSERT(hBody == HandleId::Body);

    // Move shape
    Tracker::MoveShape(sq, Point(10.0, 20.0));
    TEST_ASSERT(sq.GetX() == 60.0);
    TEST_ASSERT(sq.GetY() == 70.0);

    // Resize shape via bottom-right handle
    Rect orig = sq.GetBoundingBox();
    Tracker::ResizeShape(sq, HandleId::BottomRight, Point(200.0, 250.0), orig);
    TEST_ASSERT(sq.GetWidth() == 140.0); // 200 - 60
    TEST_ASSERT(sq.GetHeight() == 180.0); // 250 - 70

    std::cout << "TestTrackerAndInteractions PASSED!" << std::endl;
}

void TestLineConnectionAndAnchors() {
    std::cout << "Running TestLineConnectionAndAnchors..." << std::endl;

    Document doc;
    auto sq1 = std::make_shared<SquareShape>(100.0, 100.0, 100.0, 100.0);
    auto sq2 = std::make_shared<SquareShape>(400.0, 100.0, 100.0, 100.0);
    uint64_t id1 = doc.AddShape(sq1);
    uint64_t id2 = doc.AddShape(sq2);

    // Check connection points
    auto cpList1 = sq1->GetConnectionPoints();
    TEST_ASSERT(cpList1.size() == 5); // Top, Right, Bottom, Left, Center

    // Right anchor of sq1 is at (200, 150)
    Point rightAnchor1 = cpList1[1].position;
    TEST_ASSERT(std::abs(rightAnchor1.x - 200.0) < 1e-4);
    TEST_ASSERT(std::abs(rightAnchor1.y - 150.0) < 1e-4);

    // Left anchor of sq2 is at (400, 150)
    auto cpList2 = sq2->GetConnectionPoints();
    Point leftAnchor2 = cpList2[3].position;
    TEST_ASSERT(std::abs(leftAnchor2.x - 400.0) < 1e-4);
    TEST_ASSERT(std::abs(leftAnchor2.y - 150.0) < 1e-4);

    // Snap test
    auto snap1 = Tracker::FindSnapAnchor(Point(198.0, 151.0), doc.GetShapes(), 0, 15.0);
    TEST_ASSERT(snap1.snapped == true);
    TEST_ASSERT(snap1.targetShapeId == id1);
    TEST_ASSERT(snap1.anchorIndex == 1);

    // Create line connector connected to sq1 right anchor and sq2 left anchor
    auto line = std::make_shared<LineShape>(rightAnchor1, leftAnchor2);
    line->SetStartCap(LineCapStyle::Dot);
    line->SetEndCap(LineCapStyle::Arrow);
    line->SetStartConnectedShapeId(id1, 1);
    line->SetEndConnectedShapeId(id2, 3);
    doc.AddShape(line);

    TEST_ASSERT(line->GetStartPoint() == rightAnchor1);
    TEST_ASSERT(line->GetEndPoint() == leftAnchor2);

    // Now move sq1 by (+50, +30)
    sq1->MoveBy(Point(50.0, 30.0));
    doc.UpdateConnectedLines();

    // Verify line start point automatically moved with sq1!
    Point newExpectedStart(250.0, 180.0);
    TEST_ASSERT(line->GetStartPoint() == newExpectedStart);
    TEST_ASSERT(line->GetEndPoint() == leftAnchor2);

    std::cout << "TestLineConnectionAndAnchors PASSED!" << std::endl;
}

void TestFastRenderingViewportCulling() {
    std::cout << "Running TestFastRenderingViewportCulling..." << std::endl;

    Document doc;
    // Add shape inside viewport
    auto s1 = std::make_shared<SquareShape>(100.0, 100.0, 50.0, 50.0);
    // Add shape far outside viewport
    auto s2 = std::make_shared<SquareShape>(2000.0, 3000.0, 50.0, 50.0);
    // Add invisible shape inside viewport
    auto s3 = std::make_shared<SquareShape>(120.0, 120.0, 50.0, 50.0);
    s3->SetVisible(false);

    doc.AddShape(s1);
    doc.AddShape(s2);
    doc.AddShape(s3);

    Rect viewport(0.0, 0.0, 800.0, 600.0);
    auto visibleShapes = doc.GetVisibleShapes(viewport);

    TEST_ASSERT(visibleShapes.size() == 1);
    TEST_ASSERT(visibleShapes[0] == s1);

    std::cout << "TestFastRenderingViewportCulling PASSED!" << std::endl;
}

void TestPaperSizeParametrization() {
    std::cout << "Running TestPaperSizeParametrization..." << std::endl;

    Document doc;
    // Default 1200 x 1400
    TEST_ASSERT(doc.GetPaperSize().width == 1200.0);
    TEST_ASSERT(doc.GetPaperSize().height == 1400.0);

    // Parametrize to custom resolution
    doc.SetPaperSize(1920.0, 1080.0);
    TEST_ASSERT(doc.GetPaperSize().width == 1920.0);
    TEST_ASSERT(doc.GetPaperSize().height == 1080.0);

    std::cout << "TestPaperSizeParametrization PASSED!" << std::endl;
}

void TestSerializationRawData() {
    std::cout << "Running TestSerializationRawData..." << std::endl;

    Document originalDoc(PaperSize(1200.0, 1400.0));

    auto sq = std::make_shared<SquareShape>(50, 50, 120, 80);
    sq->SetText("Hello Raw Data");
    sq->SetColor(Color::Red());
    sq->SetFillColor(Color::Cyan());
    sq->SetFillEnabled(true);
    originalDoc.AddShape(sq);

    auto star = std::make_shared<StarShape>(300, 200, 100, 100, 7);
    star->SetOutlineColor(Color::Magenta());
    originalDoc.AddShape(star);

    auto line = std::make_shared<LineShape>(Point(50, 50), Point(300, 200));
    line->SetStartCap(LineCapStyle::Dot);
    line->SetEndCap(LineCapStyle::Arrow);
    originalDoc.AddShape(line);

    // Save as Raw Data
    std::string rawData = originalDoc.SaveAsRawData();
    TEST_ASSERT(!rawData.empty());

    // Load into new document
    Document loadedDoc;
    bool ok = loadedDoc.LoadFromRawData(rawData);
    TEST_ASSERT(ok);
    TEST_ASSERT(loadedDoc.GetPaperSize().width == 1200.0);
    TEST_ASSERT(loadedDoc.GetPaperSize().height == 1400.0);
    TEST_ASSERT(loadedDoc.GetShapes().size() == 3);

    auto loadedSq = loadedDoc.GetShapes()[0];
    TEST_ASSERT(loadedSq->GetType() == ShapeType::Square);
    TEST_ASSERT(loadedSq->GetText() == "Hello Raw Data");
    TEST_ASSERT(loadedSq->GetFillColor() == Color::Cyan());

    auto loadedStar = dynamic_cast<StarShape*>(loadedDoc.GetShapes()[1].get());
    TEST_ASSERT(loadedStar != nullptr);
    TEST_ASSERT(loadedStar->GetPointsCount() == 7);
    TEST_ASSERT(loadedStar->GetOutlineColor() == Color::Magenta());

    auto loadedLine = dynamic_cast<LineShape*>(loadedDoc.GetShapes()[2].get());
    TEST_ASSERT(loadedLine != nullptr);
    TEST_ASSERT(loadedLine->GetStartCap() == LineCapStyle::Dot);
    TEST_ASSERT(loadedLine->GetEndCap() == LineCapStyle::Arrow);

    std::cout << "TestSerializationRawData PASSED!" << std::endl;
}

int main() {
    std::cout << "=== Running UFMNext Core Unit Tests ===" << std::endl;
    TestShapeProperties();
    TestDerivedShapesAndVirtualDraw();
    TestTrackerAndInteractions();
    TestLineConnectionAndAnchors();
    TestFastRenderingViewportCulling();
    TestPaperSizeParametrization();
    TestSerializationRawData();
    std::cout << "=== All Unit Tests Passed Successfully! ===" << std::endl;
    return 0;
}
