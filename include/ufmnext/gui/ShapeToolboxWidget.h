#pragma once

#include "ufmnext/core/ShapeType.h"
#include <QWidget>
#include <QListWidget>

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
};

class ShapeToolboxWidget : public QWidget {
    Q_OBJECT
public:
    explicit ShapeToolboxWidget(QWidget* parent = nullptr);
    ~ShapeToolboxWidget() override = default;

    void SetActiveTool(core::ShapeType type);
    void SetSelectTool();

signals:
    void toolChanged(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap);

private slots:
    void onItemSelectionChanged();

private:
    void setupUI();
    QIcon createShapeIcon(const ToolItem& tool);

    QListWidget* m_listWidget{nullptr};
    std::vector<ToolItem> m_tools;
};

} // namespace ufmnext::gui
