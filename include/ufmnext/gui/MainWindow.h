#pragma once

#include "ufmnext/gui/MdiChild.h"
#include "ufmnext/gui/ShapeToolboxWidget.h"
#include "ufmnext/gui/ThemeManager.h"

#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QDockWidget>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

namespace ufmnext::gui {

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void newFile();
    void openFile();
    void saveFile();
    void saveAsRawData();
    void exportPng();
    void printFile();
    void printPreview();
    void deleteSelectedShape();
    void setPaperSizeDialog();
    void updateMenusAndToolbars();
    void updateStatusBarCoordinates(const core::Point& pt);
    void onToolChanged(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap);
    void applyTheme(AppTheme theme);
    void about();

private:
    void createActions();
    void createMenus();
    void createToolbars();
    void createToolboxDock();
    void createStatusBar();

    MdiChild* activeMdiChild() const;
    DrawingCanvas* activeCanvas() const;
    MdiChild* createMdiChild();

    QMdiArea* m_mdiArea{nullptr};
    ShapeToolboxWidget* m_toolbox{nullptr};
    QDockWidget* m_toolboxDock{nullptr};

    // Styling widgets in toolbar
    QPushButton* m_btnOutlineColor{nullptr};
    QPushButton* m_btnFillColor{nullptr};
    QCheckBox* m_chkFillEnabled{nullptr};
    QDoubleSpinBox* m_spnStrokeWidth{nullptr};
    QLineEdit* m_txtShapeText{nullptr};
    QDoubleSpinBox* m_spnFontSize{nullptr};
    QPushButton* m_btnFontColor{nullptr};
    QComboBox* m_cmbZoom{nullptr};

    // Status bar labels
    QLabel* m_lblCoordinates{nullptr};
    QLabel* m_lblStatusTool{nullptr};
    QLabel* m_lblShapeCount{nullptr};

    // Actions
    QAction* m_actNew{nullptr};
    QAction* m_actOpen{nullptr};
    QAction* m_actSave{nullptr};
    QAction* m_actSaveAs{nullptr};
    QAction* m_actExportPng{nullptr};
    QAction* m_actPrint{nullptr};
    QAction* m_actPrintPreview{nullptr};
    QAction* m_actExit{nullptr};

    QAction* m_actDelete{nullptr};
    QAction* m_actCut{nullptr};
    QAction* m_actCopy{nullptr};
    QAction* m_actPaste{nullptr};
    QAction* m_actSelectTool{nullptr};
    QAction* m_actBringForward{nullptr};
    QAction* m_actSendBackward{nullptr};
    QAction* m_actPaperSize{nullptr};

    QAction* m_actZoomIn{nullptr};
    QAction* m_actZoomOut{nullptr};
    QAction* m_actZoom100{nullptr};
    QAction* m_actZoomToFit{nullptr};
    QAction* m_actGrid{nullptr};

    // Theme actions
    QAction* m_actThemeDark{nullptr};
    QAction* m_actThemeLight{nullptr};
    QAction* m_actThemeBlue{nullptr};

    // Current styling defaults
    core::Color m_curOutlineColor{core::Color::DarkGray()};
    core::Color m_curFillColor{core::Color(204, 229, 255, 255)};
    core::Color m_curFontColor{core::Color::Black()};
};

} // namespace ufmnext::gui
