#pragma once

#include "ufmnext/core/ShapeType.h"
#include "ufmnext/core/Palette.h"
#include <QWidget>
#include <QListWidget>
#include <QString>
#include <vector>

class QComboBox;
class QStackedWidget;

namespace ufmnext::gui {

enum class ToolMode {
    Select = 0,
    CreateShape
};

struct ToolItem {
    QString name;
    ToolMode mode{ToolMode::Select};
    core::ShapeType shapeType{core::ShapeType::None};
    core::LineCapStyle startCap{core::LineCapStyle::None};
    core::LineCapStyle endCap{core::LineCapStyle::None};
    QString shapeClass; // derived class name resolved by core::ShapeFactory
};

class ShapeToolboxWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShapeToolboxWidget(QWidget* parent = nullptr);
    ~ShapeToolboxWidget() override = default;

    void SetActiveTool(core::ShapeType type);
    void SetSelectTool();

    // Adds a palette (one list per palette, chosen from the combo box)
    void AddPalette(const core::Palette& palette);

signals:
    void toolChanged(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap,
                     const QString& shapeClass);

private slots:
    void onItemSelectionChanged();
    void onLoadPluginClicked();

private:
    struct PaletteView {
        QListWidget* list{nullptr};
        std::vector<ToolItem> tools;
    };

    void setupUI();
    void loadBuiltInPalettes();
    QIcon createShapeIcon(const ToolItem& tool);
    void clearSelectionsExcept(QListWidget* keep);

    QComboBox* m_paletteCombo{nullptr};
    QStackedWidget* m_stack{nullptr};
    std::vector<PaletteView> m_palettes;
};

} // namespace ufmnext::gui
