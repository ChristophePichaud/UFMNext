#include "ufmnext/gui/ShapeToolboxWidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <cmath>

namespace ufmnext::gui {

ShapeToolboxWidget::ShapeToolboxWidget(QWidget* parent)
    : QWidget(parent)
{
    setupUI();
}

void ShapeToolboxWidget::setupUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(6, 6, 6, 6);
    mainLayout->setSpacing(6);

    auto* header = new QLabel(tr("Shapes Palette"), this);
    header->setStyleSheet("font-weight: bold; font-size: 13px; padding: 4px;");
    mainLayout->addWidget(header);

    m_listWidget = new QListWidget(this);
    m_listWidget->setViewMode(QListView::IconMode);
    m_listWidget->setIconSize(QSize(44, 44));
    m_listWidget->setGridSize(QSize(68, 68));
    m_listWidget->setResizeMode(QListView::Adjust);
    m_listWidget->setUniformItemSizes(true);
    m_listWidget->setMovement(QListView::Static);
    m_listWidget->setSpacing(4);
    m_listWidget->setStyleSheet(
        "QListWidget {"
        "  border: 1px solid #c0c0c0;"
        "  border-radius: 4px;"
        "  background: #fdfdfd;"
        "}"
        "QListWidget::item {"
        "  border: 1px solid transparent;"
        "  border-radius: 4px;"
        "  padding: 2px;"
        "}"
        "QListWidget::item:hover {"
        "  background: #e5f1fb;"
        "  border: 1px solid #70c0e7;"
        "}"
        "QListWidget::item:selected {"
        "  background: #cce8ff;"
        "  border: 1px solid #99d1ff;"
        "}"
    );

    // Populate tools
    m_tools = {
        {tr("Select"), ToolMode::Select, core::ShapeType::None, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Square"), ToolMode::CreateShape, core::ShapeType::Square, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Round"), ToolMode::CreateShape, core::ShapeType::RoundRectangle, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Circle"), ToolMode::CreateShape, core::ShapeType::Circle, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Oval"), ToolMode::CreateShape, core::ShapeType::Oval, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Line"), ToolMode::CreateShape, core::ShapeType::Line, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Arrow Line"), ToolMode::CreateShape, core::ShapeType::Line, core::LineCapStyle::None, core::LineCapStyle::Arrow},
        {tr("Dot Line"), ToolMode::CreateShape, core::ShapeType::Line, core::LineCapStyle::Dot, core::LineCapStyle::Arrow},
        {tr("Star"), ToolMode::CreateShape, core::ShapeType::Star, core::LineCapStyle::None, core::LineCapStyle::None},
        {tr("Arrow"), ToolMode::CreateShape, core::ShapeType::Arrow, core::LineCapStyle::None, core::LineCapStyle::None}
    };

    for (const auto& tool : m_tools) {
        auto* item = new QListWidgetItem(createShapeIcon(tool), tool.name, m_listWidget);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        item->setToolTip(tool.name);
    }

    mainLayout->addWidget(m_listWidget);

    connect(m_listWidget, &QListWidget::itemSelectionChanged, this, &ShapeToolboxWidget::onItemSelectionChanged);

    // Select first tool (Select)
    if (m_listWidget->count() > 0) {
        m_listWidget->setCurrentRow(0);
    }
}

QIcon ShapeToolboxWidget::createShapeIcon(const ToolItem& tool) {
    QPixmap pixmap(44, 44);
    pixmap.fill(Qt::transparent);

    QPainter p(&pixmap);
    p.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(QColor(40, 90, 160), 2.0);
    QBrush brush(QColor(210, 230, 255));
    p.setPen(pen);
    p.setBrush(brush);

    if (tool.mode == ToolMode::Select) {
        // Draw pointer cursor icon
        QPolygonF pointer;
        pointer << QPointF(8, 6) << QPointF(8, 34) << QPointF(16, 26) << QPointF(24, 34)
                << QPointF(28, 30) << QPointF(20, 22) << QPointF(30, 22);
        p.setBrush(QColor(60, 60, 60));
        p.setPen(QPen(Qt::white, 1.5));
        p.drawPolygon(pointer);
    } else {
        switch (tool.shapeType) {
            case core::ShapeType::Square:
                p.drawRect(8, 8, 28, 28);
                break;
            case core::ShapeType::RoundRectangle:
                p.drawRoundedRect(8, 8, 28, 28, 6, 6);
                break;
            case core::ShapeType::Circle:
                p.drawEllipse(8, 8, 28, 28);
                break;
            case core::ShapeType::Oval:
                p.drawEllipse(6, 12, 32, 20);
                break;
            case core::ShapeType::Line: {
                p.setPen(QPen(QColor(40, 90, 160), 2.5));
                p.drawLine(8, 36, 36, 8);
                if (tool.startCap == core::LineCapStyle::Dot) {
                    p.setBrush(QColor(40, 90, 160));
                    p.drawEllipse(QPointF(8, 36), 3, 3);
                }
                if (tool.endCap == core::LineCapStyle::Arrow) {
                    QPolygonF arrow;
                    arrow << QPointF(36, 8) << QPointF(26, 10) << QPointF(34, 18);
                    p.setBrush(QColor(40, 90, 160));
                    p.drawPolygon(arrow);
                }
                break;
            }
            case core::ShapeType::Star: {
                QPolygonF star;
                const double cx = 22, cy = 22, rOut = 16, rIn = 7;
                for (int i = 0; i < 10; ++i) {
                    const double a = -M_PI * 0.5 + i * (M_PI / 5.0);
                    const double r = (i % 2 == 0) ? rOut : rIn;
                    star << QPointF(cx + r * std::cos(a), cy + r * std::sin(a));
                }
                p.drawPolygon(star);
                break;
            }
            case core::ShapeType::Arrow: {
                QPolygonF arrow;
                arrow << QPointF(6, 16) << QPointF(24, 16) << QPointF(24, 10)
                      << QPointF(38, 22) << QPointF(24, 34) << QPointF(24, 28)
                      << QPointF(6, 28);
                p.drawPolygon(arrow);
                break;
            }
            default:
                break;
        }
    }

    return QIcon(pixmap);
}

void ShapeToolboxWidget::onItemSelectionChanged() {
    int row = m_listWidget->currentRow();
    if (row >= 0 && row < static_cast<int>(m_tools.size())) {
        const auto& tool = m_tools[row];
        emit toolChanged(tool.mode, tool.shapeType, tool.startCap, tool.endCap);
    }
}

void ShapeToolboxWidget::SetActiveTool(core::ShapeType type) {
    for (size_t i = 0; i < m_tools.size(); ++i) {
        if (m_tools[i].shapeType == type) {
            m_listWidget->setCurrentRow(static_cast<int>(i));
            return;
        }
    }
}

void ShapeToolboxWidget::SetSelectTool() {
    m_listWidget->setCurrentRow(0);
}

} // namespace ufmnext::gui
