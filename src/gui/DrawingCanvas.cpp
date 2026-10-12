#include "ufmnext/gui/DrawingCanvas.h"
#include "ufmnext/gui/QtDrawingContext.h"
#include "ufmnext/core/SquareShape.h"
#include "ufmnext/core/RoundRectangleShape.h"
#include "ufmnext/core/CircleShape.h"
#include "ufmnext/core/OvalShape.h"
#include "ufmnext/core/LineShape.h"
#include "ufmnext/core/StarShape.h"
#include "ufmnext/core/ArrowShape.h"

#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QInputDialog>
#include <QPrintPreviewDialog>
#include <cmath>

namespace ufmnext::gui {

DrawingCanvas::DrawingCanvas(std::shared_ptr<core::Document> doc, QWidget* parent)
    : QWidget(parent)
    , m_document(doc ? doc : std::make_shared<core::Document>())
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    updateCanvasGeometry();
}

void DrawingCanvas::SetDocument(std::shared_ptr<core::Document> doc) {
    m_document = doc ? doc : std::make_shared<core::Document>();
    updateCanvasGeometry();
    update();
}

void DrawingCanvas::updateCanvasGeometry() {
    if (!m_document) return;
    const auto paper = m_document->GetPaperSize();
    const int w = static_cast<int>(paper.width * m_zoom + m_margin * 2.0);
    const int h = static_cast<int>(paper.height * m_zoom + m_margin * 2.0);
    setMinimumSize(w, h);
    resize(w, h);
}

QSize DrawingCanvas::sizeHint() const {
    if (!m_document) return QSize(1280, 1480);
    const auto paper = m_document->GetPaperSize();
    return QSize(static_cast<int>(paper.width * m_zoom + m_margin * 2.0),
                 static_cast<int>(paper.height * m_zoom + m_margin * 2.0));
}

void DrawingCanvas::SetPaperSize(double width, double height) {
    if (!m_document) return;
    m_document->SetPaperSize(width, height);
    updateCanvasGeometry();
    update();
    emit documentModified();
}

void DrawingCanvas::SetZoom(double zoom) {
    m_zoom = std::clamp(zoom, 0.1, 5.0);
    updateCanvasGeometry();
    update();
    emit zoomChanged(m_zoom);
}

void DrawingCanvas::ZoomIn() {
    SetZoom(m_zoom * 1.2);
}

void DrawingCanvas::ZoomOut() {
    SetZoom(m_zoom / 1.2);
}

void DrawingCanvas::Zoom100() {
    SetZoom(1.0);
}

void DrawingCanvas::ZoomToFit() {
    if (!m_document || !parentWidget()) return;
    const auto paper = m_document->GetPaperSize();
    const double availW = parentWidget()->width() - m_margin * 2.0;
    const double availH = parentWidget()->height() - m_margin * 2.0;
    if (availW <= 0 || availH <= 0 || paper.width <= 0 || paper.height <= 0) return;

    const double zoomX = availW / paper.width;
    const double zoomY = availH / paper.height;
    SetZoom(std::min(zoomX, zoomY));
}

void DrawingCanvas::SetTool(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap) {
    m_toolMode = mode;
    m_createShapeType = type;
    m_createStartCap = startCap;
    m_createEndCap = endCap;
    if (mode == ToolMode::CreateShape) {
        setCursor(Qt::CrossCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
}

void DrawingCanvas::SetCurrentOutlineColor(const core::Color& c) {
    m_curOutlineColor = c;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetOutlineColor(c);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentFillColor(const core::Color& c) {
    m_curFillColor = c;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetFillColor(c);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentFillEnabled(bool enabled) {
    m_curFillEnabled = enabled;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetFillEnabled(enabled);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentStrokeWidth(double w) {
    m_curStrokeWidth = w;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetStrokeWidth(w);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentFontSize(double size) {
    m_curFontSize = size;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetFontSize(size);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentFontColor(const core::Color& c) {
    m_curFontColor = c;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetFontColor(c);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetCurrentText(const std::string& text) {
    m_curText = text;
    auto primary = m_document->GetPrimarySelectedShape();
    if (primary) {
        primary->SetText(text);
        update();
        emit documentModified();
    }
}

void DrawingCanvas::SetGridVisible(bool visible) {
    m_showGrid = visible;
    update();
}

QPointF DrawingCanvas::DocToWidget(const core::Point& pt) const {
    return QPointF(m_margin + pt.x * m_zoom, m_margin + pt.y * m_zoom);
}

core::Point DrawingCanvas::WidgetToDoc(const QPointF& pt) const {
    return core::Point((pt.x() - m_margin) / m_zoom, (pt.y() - m_margin) / m_zoom);
}

void DrawingCanvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Background workspace color
    painter.fillRect(rect(), palette().window());

    // Draw paper boundary with drop shadow
    drawPaper(painter);

    if (!m_document) return;

    // Fast rendering: calculate visible viewport in document coordinates
    const QRect clipRect = event->rect();
    const core::Point docTopLeft = WidgetToDoc(clipRect.topLeft());
    const core::Point docBottomRight = WidgetToDoc(clipRect.bottomRight());
    const core::Rect visibleDocRect(docTopLeft.x,
                                    docTopLeft.y,
                                    docBottomRight.x - docTopLeft.x,
                                    docBottomRight.y - docTopLeft.y);

    painter.save();
    painter.translate(m_margin, m_margin);
    painter.scale(m_zoom, m_zoom);

    // Clip to paper boundary
    const auto paper = m_document->GetPaperSize();
    painter.setClipRect(QRectF(0, 0, paper.width, paper.height));

    if (m_showGrid) {
        drawGrid(painter);
    }

    // Fast rendering: only render shapes that intersect the visible viewport
    const auto visibleShapes = m_document->GetVisibleShapes(visibleDocRect);
    QtDrawingContext ctx(&painter);
    for (const auto& shape : visibleShapes) {
        if (shape) {
            shape->Draw(ctx);
        }
    }

    // Draw tracker / selection for selected shapes
    for (const auto& shape : visibleShapes) {
        if (shape && shape->IsSelected()) {
            drawTracker(painter, *shape);
            drawConnectionPoints(painter, *shape);
        }
    }

    // Draw snap indicator if active
    if (m_currentSnapResult.snapped) {
        drawSnapIndicator(painter, m_currentSnapResult.anchorPosition);
    }

    painter.restore();
}

void DrawingCanvas::drawPaper(QPainter& painter) {
    if (!m_document) return;
    const auto paper = m_document->GetPaperSize();
    const QRectF paperRect(m_margin, m_margin, paper.width * m_zoom, paper.height * m_zoom);

    // Drop shadow
    painter.fillRect(paperRect.translated(4, 4), QColor(0, 0, 0, 40));

    // White sheet
    painter.fillRect(paperRect, Qt::white);
    painter.setPen(QPen(QColor(180, 180, 180), 1.0));
    painter.drawRect(paperRect);
}

void DrawingCanvas::drawGrid(QPainter& painter) {
    if (!m_document) return;
    const auto paper = m_document->GetPaperSize();

    painter.setPen(QPen(QColor(230, 230, 230), 1.0, Qt::DotLine));
    for (double x = m_gridSpacing; x < paper.width; x += m_gridSpacing) {
        painter.drawLine(QPointF(x, 0), QPointF(x, paper.height));
    }
    for (double y = m_gridSpacing; y < paper.height; y += m_gridSpacing) {
        painter.drawLine(QPointF(0, y), QPointF(paper.width, y));
    }
}

void DrawingCanvas::drawTracker(QPainter& painter, const core::Shape& shape) {
    const auto handles = core::Tracker::GetHandles(shape, core::Tracker::DefaultHandleSize / m_zoom);

    // Draw dashed bounding box
    const core::Rect b = shape.GetBoundingBox();
    painter.setPen(QPen(QColor(0, 120, 215), 1.0 / m_zoom, Qt::DashLine));
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRectF(b.x, b.y, b.width, b.height));

    // Draw handles
    for (const auto& h : handles) {
        painter.setPen(QPen(QColor(0, 120, 215), 1.0 / m_zoom));
        painter.setBrush(Qt::white);
        painter.drawRect(QRectF(h.rect.x, h.rect.y, h.rect.width, h.rect.height));
    }
}

void DrawingCanvas::drawConnectionPoints(QPainter& painter, const core::Shape& shape) {
    const auto cpList = shape.GetConnectionPoints();
    const double r = 4.0 / m_zoom;

    painter.setPen(QPen(QColor(40, 167, 69), 1.0 / m_zoom));
    painter.setBrush(QColor(40, 167, 69, 180));

    for (const auto& cp : cpList) {
        painter.drawEllipse(QPointF(cp.position.x, cp.position.y), r, r);
    }
}

void DrawingCanvas::drawSnapIndicator(QPainter& painter, const core::Point& pt) {
    const double r = 7.0 / m_zoom;
    painter.setPen(QPen(QColor(220, 53, 69), 2.0 / m_zoom));
    painter.setBrush(QColor(255, 193, 7, 180));
    painter.drawEllipse(QPointF(pt.x, pt.y), r, r);
}

void DrawingCanvas::mousePressEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton || !m_document) return;

    const core::Point docPt = WidgetToDoc(event->position());
    m_dragStartDocPt = docPt;
    m_lastDocPt = docPt;

    if (m_toolMode == ToolMode::Select) {
        auto primary = m_document->GetPrimarySelectedShape();
        core::HandleId handle = core::HandleId::None;

        if (primary) {
            handle = core::Tracker::HitTestHandle(*primary, docPt, core::Tracker::DefaultHandleSize / m_zoom);
        }

        if (handle != core::HandleId::None && handle != core::HandleId::Body) {
            // Started resizing via tracker handle
            m_dragMode = DragMode::Resize;
            m_activeHandle = handle;
            m_activeShape = primary;
            m_originalBounds = primary->GetBoundingBox();
        } else {
            // Hit test shapes
            auto hitShape = m_document->FindShapeAt(docPt, 5.0 / m_zoom);
            if (hitShape) {
                m_document->SelectShape(hitShape->GetId(), !(event->modifiers() & Qt::ControlModifier));
                m_dragMode = DragMode::Move;
                m_activeShape = hitShape;
                emit selectionChanged();
            } else {
                m_document->ClearSelection();
                m_dragMode = DragMode::None;
                emit selectionChanged();
            }
        }
    } else if (m_toolMode == ToolMode::CreateShape) {
        // Create new shape
        std::shared_ptr<core::Shape> newShape;
        switch (m_createShapeType) {
            case core::ShapeType::Square:
                newShape = std::make_shared<core::SquareShape>(docPt.x, docPt.y, 10, 10);
                break;
            case core::ShapeType::RoundRectangle:
                newShape = std::make_shared<core::RoundRectangleShape>(docPt.x, docPt.y, 10, 10);
                break;
            case core::ShapeType::Circle:
                newShape = std::make_shared<core::CircleShape>(docPt.x + 5, docPt.y + 5, 5);
                break;
            case core::ShapeType::Oval:
                newShape = std::make_shared<core::OvalShape>(docPt.x, docPt.y, 10, 10);
                break;
            case core::ShapeType::Line: {
                auto line = std::make_shared<core::LineShape>(docPt, docPt);
                line->SetStartCap(m_createStartCap);
                line->SetEndCap(m_createEndCap);

                // Snap start anchor
                auto snap = core::Tracker::FindSnapAnchor(docPt, m_document->GetShapes(), 0, 16.0 / m_zoom);
                if (snap.snapped) {
                    line->SetStartPoint(snap.anchorPosition);
                    line->SetStartConnectedShapeId(snap.targetShapeId, snap.anchorIndex);
                }
                newShape = line;
                break;
            }
            case core::ShapeType::Star:
                newShape = std::make_shared<core::StarShape>(docPt.x, docPt.y, 10, 10);
                break;
            case core::ShapeType::Arrow:
                newShape = std::make_shared<core::ArrowShape>(docPt.x, docPt.y, 10, 10);
                break;
            default:
                break;
        }

        if (newShape) {
            newShape->SetOutlineColor(m_curOutlineColor);
            newShape->SetFillColor(m_curFillColor);
            newShape->SetFillEnabled(m_curFillEnabled);
            newShape->SetStrokeWidth(m_curStrokeWidth);
            newShape->SetFontSize(m_curFontSize);
            newShape->SetFontColor(m_curFontColor);
            newShape->SetText(m_curText);

            m_document->AddShape(newShape);
            m_document->SelectShape(newShape->GetId(), true);
            m_activeShape = newShape;
            m_dragMode = DragMode::Create;
            emit selectionChanged();
        }
    }

    update();
}

void DrawingCanvas::mouseMoveEvent(QMouseEvent* event) {
    const core::Point docPt = WidgetToDoc(event->position());
    emit mouseMoved(docPt);

    if (m_dragMode == DragMode::Move && m_activeShape) {
        const core::Point delta = docPt - m_lastDocPt;
        m_activeShape->MoveBy(delta);
        m_document->UpdateConnectedLines();
        m_document->SetModified(true);
        m_lastDocPt = docPt;
        update();
        emit documentModified();
    } else if (m_dragMode == DragMode::Resize && m_activeShape) {
        core::Tracker::ResizeShape(*m_activeShape, m_activeHandle, docPt, m_originalBounds);
        m_document->UpdateConnectedLines();
        m_document->SetModified(true);
        update();
        emit documentModified();
    } else if (m_dragMode == DragMode::Create && m_activeShape) {
        if (m_activeShape->GetType() == core::ShapeType::Line) {
            auto* line = dynamic_cast<core::LineShape*>(m_activeShape.get());
            if (line) {
                // Check snap anchor for line endpoint
                m_currentSnapResult = core::Tracker::FindSnapAnchor(docPt, m_document->GetShapes(), line->GetId(), 16.0 / m_zoom);
                if (m_currentSnapResult.snapped) {
                    line->SetEndPoint(m_currentSnapResult.anchorPosition);
                } else {
                    line->SetEndPoint(docPt);
                }
            }
        } else {
            const double w = docPt.x - m_dragStartDocPt.x;
            const double h = docPt.y - m_dragStartDocPt.y;
            m_activeShape->SetWidth(std::abs(w));
            m_activeShape->SetHeight(std::abs(h));
            m_activeShape->SetPosition(std::min(m_dragStartDocPt.x, docPt.x),
                                       std::min(m_dragStartDocPt.y, docPt.y));
        }
        m_document->SetModified(true);
        update();
        emit documentModified();
    } else {
        // Hover cursor update when in Select mode
        if (m_toolMode == ToolMode::Select) {
            auto primary = m_document->GetPrimarySelectedShape();
            if (primary) {
                core::HandleId h = core::Tracker::HitTestHandle(*primary, docPt, core::Tracker::DefaultHandleSize / m_zoom);
                switch (h) {
                    case core::HandleId::TopLeft:
                    case core::HandleId::BottomRight:
                        setCursor(Qt::SizeFDiagCursor); break;
                    case core::HandleId::TopRight:
                    case core::HandleId::BottomLeft:
                        setCursor(Qt::SizeBDiagCursor); break;
                    case core::HandleId::TopCenter:
                    case core::HandleId::BottomCenter:
                        setCursor(Qt::SizeVerCursor); break;
                    case core::HandleId::MiddleLeft:
                    case core::HandleId::MiddleRight:
                        setCursor(Qt::SizeHorCursor); break;
                    case core::HandleId::Body:
                        setCursor(Qt::SizeAllCursor); break;
                    default:
                        setCursor(Qt::ArrowCursor); break;
                }
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }
}

void DrawingCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;

    if (m_dragMode == DragMode::Create && m_activeShape) {
        if (m_activeShape->GetType() == core::ShapeType::Line) {
            auto* line = dynamic_cast<core::LineShape*>(m_activeShape.get());
            if (line && m_currentSnapResult.snapped) {
                line->SetEndConnectedShapeId(m_currentSnapResult.targetShapeId, m_currentSnapResult.anchorIndex);
            }
        }
    }

    m_currentSnapResult.snapped = false;
    m_dragMode = DragMode::None;
    m_activeHandle = core::HandleId::None;
    update();
}

void DrawingCanvas::mouseDoubleClickEvent(QMouseEvent* event) {
    const core::Point docPt = WidgetToDoc(event->position());
    auto hitShape = m_document->FindShapeAt(docPt, 5.0 / m_zoom);
    if (hitShape) {
        bool ok = false;
        QString text = QInputDialog::getText(this, tr("Edit Shape Text"), tr("Text:"),
                                             QLineEdit::Normal, QString::fromStdString(hitShape->GetText()), &ok);
        if (ok) {
            hitShape->SetText(text.toStdString());
            m_document->SetModified(true);
            update();
            emit documentModified();
        }
    }
}

void DrawingCanvas::wheelEvent(QWheelEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        const double numDegrees = event->angleDelta().y() / 8.0;
        const double numSteps = numDegrees / 15.0;
        const double factor = std::pow(1.15, numSteps);
        SetZoom(m_zoom * factor);
        event->accept();
    } else {
        QWidget::wheelEvent(event);
    }
}

bool DrawingCanvas::ExportToPng(const QString& filePath) {
    if (!m_document) return false;
    const auto paper = m_document->GetPaperSize();

    QImage image(static_cast<int>(paper.width), static_cast<int>(paper.height), QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    QtDrawingContext ctx(&painter);
    for (const auto& shape : m_document->GetShapes()) {
        if (shape && shape->IsVisible()) {
            shape->Draw(ctx);
        }
    }

    return image.save(filePath, "PNG");
}

void DrawingCanvas::Print(QPrinter* printer) {
    if (!m_document || !printer) return;
    const auto paper = m_document->GetPaperSize();

    QPainter painter(printer);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect pageRect = printer->pageRect(QPrinter::DevicePixel).toRect();
    const double scaleX = static_cast<double>(pageRect.width()) / paper.width;
    const double scaleY = static_cast<double>(pageRect.height()) / paper.height;
    const double scale = std::min(scaleX, scaleY);

    painter.scale(scale, scale);

    QtDrawingContext ctx(&painter);
    for (const auto& shape : m_document->GetShapes()) {
        if (shape && shape->IsVisible()) {
            shape->Draw(ctx);
        }
    }
}

void DrawingCanvas::ShowPrintPreview() {
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Portrait);

    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, &QPrintPreviewDialog::paintRequested, this, [this](QPrinter* p) {
        this->Print(p);
    });
    preview.exec();
}

} // namespace ufmnext::gui
