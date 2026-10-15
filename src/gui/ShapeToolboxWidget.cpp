#include "ufmnext/gui/ShapeToolboxWidget.h"
#include "ufmnext/gui/QtDrawingContext.h"
#include "ufmnext/core/ShapeFactory.h"
#include "ufmnext/core/PluginApi.h"
#include <QComboBox>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFileInfo>
#include <QLibrary>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QStackedWidget>
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

    auto* selectButton = new QPushButton(tr("Select"), this);
    connect(selectButton, &QPushButton::clicked, this, [this]() {
        clearSelectionsExcept(nullptr);
        emit toolChanged(ToolMode::Select, core::ShapeType::None, core::LineCapStyle::None,
                         core::LineCapStyle::None, QString());
    });
    mainLayout->addWidget(selectButton);

    m_paletteCombo = new QComboBox(this);
    mainLayout->addWidget(m_paletteCombo);

    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack, 1);
    connect(m_paletteCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), m_stack,
            &QStackedWidget::setCurrentIndex);

    auto* pluginButton = new QPushButton(tr("Load Plugin Palette..."), this);
    connect(pluginButton, &QPushButton::clicked, this, &ShapeToolboxWidget::onLoadPluginClicked);
    mainLayout->addWidget(pluginButton);

    loadBuiltInPalettes();
}

void ShapeToolboxWidget::loadBuiltInPalettes() {
    QStringList dirs{QCoreApplication::applicationDirPath() + "/palettes"};
#ifdef UFMNEXT_PALETTES_SOURCE_DIR
    dirs << QString::fromUtf8(UFMNEXT_PALETTES_SOURCE_DIR);
#endif
    for (const char* file : {"shapes.json", "uml.json", "pictures.json"}) {
        for (const QString& dir : dirs) {
            const QString path = dir + "/" + file;
            core::Palette palette;
            if (QFileInfo::exists(path) && core::PaletteLoader::LoadFromFile(path.toStdString(), palette)) {
                AddPalette(palette);
                break;
            }
        }
    }
}

void ShapeToolboxWidget::AddPalette(const core::Palette& palette) {
    auto* list = new QListWidget(m_stack);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(44, 44));
    list->setGridSize(QSize(68, 68));
    list->setResizeMode(QListView::Adjust);
    list->setUniformItemSizes(true);
    list->setMovement(QListView::Static);
    list->setSpacing(4);
    list->setStyleSheet(
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

    PaletteView view;
    view.list = list;
    for (const auto& pi : palette.items) {
        const auto shape = core::ShapeFactory::Instance().Create(pi.shapeClass);
        if (!shape) continue;
        ToolItem tool;
        tool.name = QString::fromStdString(pi.name);
        tool.mode = ToolMode::CreateShape;
        tool.shapeType = shape->GetType();
        tool.startCap = core::StringToLineCapStyle(pi.startCap);
        tool.endCap = core::StringToLineCapStyle(pi.endCap);
        tool.shapeClass = QString::fromStdString(pi.shapeClass);
        view.tools.push_back(tool);

        auto* item = new QListWidgetItem(createShapeIcon(tool), tool.name, list);
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignBottom);
        item->setToolTip(tool.name);
    }

    connect(list, &QListWidget::itemSelectionChanged, this, &ShapeToolboxWidget::onItemSelectionChanged);

    m_palettes.push_back(std::move(view));
    m_stack->addWidget(list);
    m_paletteCombo->addItem(QString::fromStdString(palette.name));
}

void ShapeToolboxWidget::clearSelectionsExcept(QListWidget* keep) {
    for (auto& pv : m_palettes) {
        if (pv.list != keep) {
            const QSignalBlocker blocker(pv.list);
            pv.list->clearSelection();
            pv.list->setCurrentItem(nullptr);
        }
    }
}

void ShapeToolboxWidget::onLoadPluginClicked() {
    const QString path = QFileDialog::getOpenFileName(
        this, tr("Load Plugin Palette"), QString(), tr("Plugins (*.dll *.so *.dylib);;All files (*)"));
    if (path.isEmpty()) return;

    auto* lib = new QLibrary(path, this);
    if (!lib->load()) {
        QMessageBox::warning(this, tr("Plugin"), tr("Cannot load plugin: %1").arg(lib->errorString()));
        delete lib;
        return;
    }
    const auto registerFn = reinterpret_cast<core::PluginRegisterFn>(lib->resolve(UFMNEXT_PLUGIN_REGISTER_SYMBOL));
    const auto paletteFn = reinterpret_cast<core::PluginPaletteJsonFn>(lib->resolve(UFMNEXT_PLUGIN_PALETTE_SYMBOL));
    if (!registerFn || !paletteFn) {
        QMessageBox::warning(this, tr("Plugin"),
                             tr("Not a UFMNext shape plugin (missing %1 / %2).")
                                 .arg(UFMNEXT_PLUGIN_REGISTER_SYMBOL, UFMNEXT_PLUGIN_PALETTE_SYMBOL));
        lib->unload();
        delete lib;
        return;
    }

    registerFn(core::ShapeFactory::Instance());
    const char* json = paletteFn();
    core::Palette palette;
    if (!json || !core::PaletteLoader::LoadFromString(json, palette)) {
        QMessageBox::warning(this, tr("Plugin"), tr("The plugin palette description is invalid."));
        return; // library stays loaded: registered shapes reference its code
    }
    AddPalette(palette);
    m_paletteCombo->setCurrentIndex(m_paletteCombo->count() - 1);
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
            case core::ShapeType::Custom: {
                auto shape = core::ShapeFactory::Instance().Create(tool.shapeClass.toStdString());
                if (shape) {
                    shape->SetPosition(6, 6);
                    shape->SetWidth(32);
                    shape->SetHeight(32);
                    shape->SetText(std::string());
                    shape->SetStrokeWidth(1.0);
                    QtDrawingContext ctx(&p);
                    shape->Draw(ctx);
                }
                break;
            }
            default:
                break;
        }
    }

    return QIcon(pixmap);
}

void ShapeToolboxWidget::onItemSelectionChanged() {
    auto* list = qobject_cast<QListWidget*>(sender());
    for (const auto& pv : m_palettes) {
        if (pv.list != list) continue;
        const int row = list->currentRow();
        if (row >= 0 && row < static_cast<int>(pv.tools.size())) {
            clearSelectionsExcept(list);
            const auto& tool = pv.tools[row];
            emit toolChanged(tool.mode, tool.shapeType, tool.startCap, tool.endCap, tool.shapeClass);
        }
        return;
    }
}

void ShapeToolboxWidget::SetActiveTool(core::ShapeType type) {
    for (std::size_t p = 0; p < m_palettes.size(); ++p) {
        for (std::size_t i = 0; i < m_palettes[p].tools.size(); ++i) {
            if (m_palettes[p].tools[i].shapeType == type) {
                m_paletteCombo->setCurrentIndex(static_cast<int>(p));
                m_palettes[p].list->setCurrentRow(static_cast<int>(i));
                return;
            }
        }
    }
}

void ShapeToolboxWidget::SetSelectTool() {
    clearSelectionsExcept(nullptr);
    emit toolChanged(ToolMode::Select, core::ShapeType::None, core::LineCapStyle::None,
                     core::LineCapStyle::None, QString());
}

} // namespace ufmnext::gui
