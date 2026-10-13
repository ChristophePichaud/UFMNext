#pragma once

#include "ufmnext/core/Document.h"
#include "ufmnext/core/ShapeType.h"
#include "ufmnext/core/Tracker.h"
#include "ufmnext/gui/ShapeToolboxWidget.h"

#include <QWidget>
#include <QPrinter>
#include <memory>

namespace ufmnext::gui {

enum class DragMode {
    None = 0,
    SelectBox,
    Move,
    Resize,
    Create,
    ConnectLine
};

class DrawingCanvas : public QWidget {
    Q_OBJECT
public:
    explicit DrawingCanvas(std::shared_ptr<core::Document> doc = nullptr, QWidget* parent = nullptr);
    ~DrawingCanvas() override = default;

    std::shared_ptr<core::Document> GetDocument() const noexcept { return m_document; }
    void SetDocument(std::shared_ptr<core::Document> doc);

    // Zoom support
    double GetZoom() const noexcept { return m_zoom; }
    void SetZoom(double zoom);
    void ZoomIn();
    void ZoomOut();
    void Zoom100();
    void ZoomToFit();

    // Active tool
    void SetTool(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap);

    // Active style properties for new shapes or selected shape
    void SetCurrentOutlineColor(const core::Color& c);
    void SetCurrentFillColor(const core::Color& c);
    void SetCurrentFillEnabled(bool enabled);
    void SetCurrentStrokeWidth(double w);
    void SetCurrentFontSize(double size);
    void SetCurrentFontColor(const core::Color& c);
    void SetCurrentText(const std::string& text);

    // Clipboard operations on the selected shapes
    void CopySelection();
    void CutSelection();
    void Paste();
    void DeleteSelection();
    static bool HasClipboard();

    // Export & Print
    bool ExportToPng(const QString& filePath);
    void Print(QPrinter* printer);
    void ShowPrintPreview();

    // Paper size parametrization
    void SetPaperSize(double width, double height);

    // Grid support
    bool IsGridVisible() const noexcept { return m_showGrid; }
    void SetGridVisible(bool visible);

    // Coordinate conversion
    QPointF DocToWidget(const core::Point& pt) const;
    core::Point WidgetToDoc(const QPointF& pt) const;

signals:
    void zoomChanged(double zoom);
    void mouseMoved(const core::Point& docPt);
    void selectionChanged();
    void documentModified();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    QSize sizeHint() const override;

private:
    void updateCanvasGeometry();
    void drawPaper(QPainter& painter);
    void drawGrid(QPainter& painter);
    void drawTracker(QPainter& painter, const core::Shape& shape);
    void drawConnectionPoints(QPainter& painter, const core::Shape& shape);
    void drawSnapIndicator(QPainter& painter, const core::Point& pt);

    std::shared_ptr<core::Document> m_document;

    // View & Zoom
    double m_zoom{1.0};
    double m_margin{40.0};
    bool m_showGrid{true};
    double m_gridSpacing{20.0};

    // Tool state
    ToolMode m_toolMode{ToolMode::Select};
    core::ShapeType m_createShapeType{core::ShapeType::None};
    core::LineCapStyle m_createStartCap{core::LineCapStyle::None};
    core::LineCapStyle m_createEndCap{core::LineCapStyle::None};

    // Style properties
    core::Color m_curOutlineColor{core::Color::DarkGray()};
    core::Color m_curFillColor{core::Color(204, 229, 255, 255)};
    bool m_curFillEnabled{true};
    double m_curStrokeWidth{2.0};
    double m_curFontSize{14.0};
    core::Color m_curFontColor{core::Color::Black()};
    std::string m_curText;

    // Interaction state
    DragMode m_dragMode{DragMode::None};
    core::Point m_dragStartDocPt{0.0, 0.0};
    core::Point m_lastDocPt{0.0, 0.0};
    core::HandleId m_activeHandle{core::HandleId::None};
    core::Rect m_originalBounds{0.0, 0.0, 0.0, 0.0};
    std::shared_ptr<core::Shape> m_activeShape;
    core::SnapResult m_currentSnapResult;
    core::Rect m_selectBox{0.0, 0.0, 0.0, 0.0};
    int m_pasteCount{0};
};

} // namespace ufmnext::gui
