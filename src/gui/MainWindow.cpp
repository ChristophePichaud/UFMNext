#include "ufmnext/gui/MainWindow.h"
#include "ufmnext/gui/QtDrawingContext.h"

#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QColorDialog>
#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QApplication>
#include <QPainter>
#include <QPrintDialog>


namespace ufmnext::gui {

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    m_mdiArea = new QMdiArea(this);
    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setViewMode(QMdiArea::TabbedView);
    m_mdiArea->setTabsClosable(true);
    m_mdiArea->setTabsMovable(true);
    setCentralWidget(m_mdiArea);

    createActions();
    createMenus();
    createToolbars();
    createToolboxDock();
    createStatusBar();

    connect(m_mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::updateMenusAndToolbars);

    resize(1400, 900);
    setWindowTitle(tr("UFMNext - 2D Drawing Studio (Qt6 / ISO C++17)"));

    // Default theme: Modern Blue
    applyTheme(AppTheme::ModernBlue);

    // Create initial drawing document
    newFile();
}

MdiChild* MainWindow::activeMdiChild() const {
    if (auto* subWindow = m_mdiArea->activeSubWindow()) {
        return qobject_cast<MdiChild*>(subWindow->widget());
    }
    return nullptr;
}

DrawingCanvas* MainWindow::activeCanvas() const {
    auto* child = activeMdiChild();
    return child ? child->GetCanvas() : nullptr;
}

MdiChild* MainWindow::createMdiChild() {
    auto* child = new MdiChild(this);
    m_mdiArea->addSubWindow(child);

    auto* canvas = child->GetCanvas();
    connect(canvas, &DrawingCanvas::mouseMoved, this, &MainWindow::updateStatusBarCoordinates);
    connect(canvas, &DrawingCanvas::selectionChanged, this, &MainWindow::updateMenusAndToolbars);
    connect(canvas, &DrawingCanvas::documentModified, this, &MainWindow::updateMenusAndToolbars);

    return child;
}

void MainWindow::createActions() {
    m_actNew = new QAction(QIcon::fromTheme("document-new"), tr("&New"), this);
    m_actNew->setShortcut(QKeySequence::New);
    connect(m_actNew, &QAction::triggered, this, &MainWindow::newFile);

    m_actOpen = new QAction(QIcon::fromTheme("document-open"), tr("&Open Raw Data..."), this);
    m_actOpen->setShortcut(QKeySequence::Open);
    connect(m_actOpen, &QAction::triggered, this, &MainWindow::openFile);

    m_actSave = new QAction(QIcon::fromTheme("document-save"), tr("&Save"), this);
    m_actSave->setShortcut(QKeySequence::Save);
    connect(m_actSave, &QAction::triggered, this, &MainWindow::saveFile);

    m_actSaveAs = new QAction(QIcon::fromTheme("document-save-as"), tr("Save &As Raw Data..."), this);
    connect(m_actSaveAs, &QAction::triggered, this, &MainWindow::saveAsRawData);

    m_actExportPng = new QAction(QIcon::fromTheme("image-x-generic"), tr("&Export Diagram as PNG..."), this);
    connect(m_actExportPng, &QAction::triggered, this, &MainWindow::exportPng);

    m_actPrint = new QAction(QIcon::fromTheme("document-print"), tr("&Print..."), this);
    m_actPrint->setShortcut(QKeySequence::Print);
    connect(m_actPrint, &QAction::triggered, this, &MainWindow::printFile);

    m_actPrintPreview = new QAction(QIcon::fromTheme("document-print-preview"), tr("Print Pre&view..."), this);
    connect(m_actPrintPreview, &QAction::triggered, this, &MainWindow::printPreview);

    m_actExit = new QAction(tr("E&xit"), this);
    m_actExit->setShortcut(QKeySequence::Quit);
    connect(m_actExit, &QAction::triggered, qApp, &QApplication::closeAllWindows);

    m_actDelete = new QAction(QIcon::fromTheme("edit-delete"), tr("&Delete"), this);
    m_actDelete->setShortcut(QKeySequence::Delete);
    connect(m_actDelete, &QAction::triggered, this, &MainWindow::deleteSelectedShape);

    m_actBringForward = new QAction(tr("Bring &Forward"), this);
    connect(m_actBringForward, &QAction::triggered, this, [this]() {
        auto* canvas = activeCanvas();
        if (canvas) {
            auto primary = canvas->GetDocument()->GetPrimarySelectedShape();
            if (primary) {
                canvas->GetDocument()->BringForward(primary->GetId());
                canvas->update();
            }
        }
    });

    m_actSendBackward = new QAction(tr("Send &Backward"), this);
    connect(m_actSendBackward, &QAction::triggered, this, [this]() {
        auto* canvas = activeCanvas();
        if (canvas) {
            auto primary = canvas->GetDocument()->GetPrimarySelectedShape();
            if (primary) {
                canvas->GetDocument()->SendBackward(primary->GetId());
                canvas->update();
            }
        }
    });

    m_actPaperSize = new QAction(tr("&Paper Size Resolution..."), this);
    connect(m_actPaperSize, &QAction::triggered, this, &MainWindow::setPaperSizeDialog);

    m_actZoomIn = new QAction(QIcon::fromTheme("zoom-in"), tr("Zoom &In"), this);
    m_actZoomIn->setShortcut(QKeySequence::ZoomIn);
    connect(m_actZoomIn, &QAction::triggered, this, [this]() {
        if (auto* c = activeCanvas()) c->ZoomIn();
    });

    m_actZoomOut = new QAction(QIcon::fromTheme("zoom-out"), tr("Zoom &Out"), this);
    m_actZoomOut->setShortcut(QKeySequence::ZoomOut);
    connect(m_actZoomOut, &QAction::triggered, this, [this]() {
        if (auto* c = activeCanvas()) c->ZoomOut();
    });

    m_actZoom100 = new QAction(tr("Zoom &100%"), this);
    m_actZoom100->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    connect(m_actZoom100, &QAction::triggered, this, [this]() {
        if (auto* c = activeCanvas()) c->Zoom100();
    });

    m_actZoomToFit = new QAction(QIcon::fromTheme("zoom-fit-best"), tr("Zoom to &Fit"), this);
    connect(m_actZoomToFit, &QAction::triggered, this, [this]() {
        if (auto* c = activeCanvas()) c->ZoomToFit();
    });

    m_actGrid = new QAction(tr("Show &Grid"), this);
    m_actGrid->setCheckable(true);
    m_actGrid->setChecked(true);
    connect(m_actGrid, &QAction::toggled, this, [this](bool checked) {
        if (auto* c = activeCanvas()) c->SetGridVisible(checked);
    });

    // Themes
    m_actThemeDark = new QAction(tr("Dark Theme"), this);
    connect(m_actThemeDark, &QAction::triggered, this, [this]() { applyTheme(AppTheme::Dark); });

    m_actThemeLight = new QAction(tr("Light Theme"), this);
    connect(m_actThemeLight, &QAction::triggered, this, [this]() { applyTheme(AppTheme::Light); });

    m_actThemeBlue = new QAction(tr("Modern Blue (Office)"), this);
    connect(m_actThemeBlue, &QAction::triggered, this, [this]() { applyTheme(AppTheme::ModernBlue); });
}

void MainWindow::createMenus() {
    auto* menuFile = menuBar()->addMenu(tr("&File"));
    menuFile->addAction(m_actNew);
    menuFile->addAction(m_actOpen);
    menuFile->addAction(m_actSave);
    menuFile->addAction(m_actSaveAs);
    menuFile->addSeparator();
    menuFile->addAction(m_actExportPng);
    menuFile->addSeparator();
    menuFile->addAction(m_actPrintPreview);
    menuFile->addAction(m_actPrint);
    menuFile->addSeparator();
    menuFile->addAction(m_actExit);

    auto* menuEdit = menuBar()->addMenu(tr("&Edit"));
    menuEdit->addAction(m_actDelete);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actBringForward);
    menuEdit->addAction(m_actSendBackward);
    menuEdit->addSeparator();
    menuEdit->addAction(m_actPaperSize);

    auto* menuView = menuBar()->addMenu(tr("&View"));
    menuView->addAction(m_actZoomIn);
    menuView->addAction(m_actZoomOut);
    menuView->addAction(m_actZoom100);
    menuView->addAction(m_actZoomToFit);
    menuView->addSeparator();
    menuView->addAction(m_actGrid);
    menuView->addSeparator();

    auto* menuTheme = menuView->addMenu(tr("&Themes"));
    menuTheme->addAction(m_actThemeBlue);
    menuTheme->addAction(m_actThemeLight);
    menuTheme->addAction(m_actThemeDark);

    auto* menuWindow = menuBar()->addMenu(tr("&Window"));
    menuWindow->addAction(tr("&Cascade"), m_mdiArea, &QMdiArea::cascadeSubWindows);
    menuWindow->addAction(tr("&Tile"), m_mdiArea, &QMdiArea::tileSubWindows);
    menuWindow->addSeparator();
    menuWindow->addAction(tr("Close &All"), m_mdiArea, &QMdiArea::closeAllSubWindows);

    auto* menuHelp = menuBar()->addMenu(tr("&Help"));
    menuHelp->addAction(tr("&About UFMNext..."), this, &MainWindow::about);
}

static QIcon createColorIcon(const core::Color& c, int size = 16) {
    QPixmap pix(size, size);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setPen(QColor(100, 100, 100));
    p.setBrush(QtDrawingContext::ToQColor(c));
    p.drawRect(0, 0, size - 1, size - 1);
    return QIcon(pix);
}

void MainWindow::createToolbars() {
    // Standard File Toolbar
    auto* tbFile = addToolBar(tr("Standard"));
    tbFile->setObjectName("StandardToolBar");
    tbFile->addAction(m_actNew);
    tbFile->addAction(m_actOpen);
    tbFile->addAction(m_actSave);
    tbFile->addAction(m_actExportPng);
    tbFile->addSeparator();
    tbFile->addAction(m_actPrintPreview);
    tbFile->addAction(m_actPrint);

    // Edit Toolbar
    auto* tbEdit = addToolBar(tr("Edit"));
    tbEdit->setObjectName("EditToolBar");
    tbEdit->addAction(m_actDelete);
    tbEdit->addAction(m_actBringForward);
    tbEdit->addAction(m_actSendBackward);

    // Zoom Toolbar
    auto* tbZoom = addToolBar(tr("Zoom"));
    tbZoom->setObjectName("ZoomToolBar");
    tbZoom->addAction(m_actZoomIn);
    tbZoom->addAction(m_actZoomOut);
    tbZoom->addAction(m_actZoom100);
    tbZoom->addAction(m_actZoomToFit);

    m_cmbZoom = new QComboBox(this);
    m_cmbZoom->addItems({"25%", "50%", "75%", "100%", "125%", "150%", "200%", "300%"});
    m_cmbZoom->setCurrentText("100%");
    connect(m_cmbZoom, &QComboBox::currentTextChanged, this, [this](const QString& text) {
        if (auto* c = activeCanvas()) {
            double z = text.left(text.length() - 1).toDouble() / 100.0;
            if (z > 0) c->SetZoom(z);
        }
    });
    tbZoom->addWidget(m_cmbZoom);

    // Style Toolbar
    addToolBarBreak();
    auto* tbStyle = addToolBar(tr("Shape Properties"));
    tbStyle->setObjectName("StyleToolBar");

    // Outline color
    m_btnOutlineColor = new QPushButton(createColorIcon(m_curOutlineColor), tr(" Outline"), this);
    connect(m_btnOutlineColor, &QPushButton::clicked, this, [this]() {
        QColor chosen = QColorDialog::getColor(QtDrawingContext::ToQColor(m_curOutlineColor), this, tr("Select Outline Color"));
        if (chosen.isValid()) {
            m_curOutlineColor = QtDrawingContext::FromQColor(chosen);
            m_btnOutlineColor->setIcon(createColorIcon(m_curOutlineColor));
            if (auto* c = activeCanvas()) c->SetCurrentOutlineColor(m_curOutlineColor);
        }
    });
    tbStyle->addWidget(m_btnOutlineColor);

    // Fill color
    m_btnFillColor = new QPushButton(createColorIcon(m_curFillColor), tr(" Fill"), this);
    connect(m_btnFillColor, &QPushButton::clicked, this, [this]() {
        QColor chosen = QColorDialog::getColor(QtDrawingContext::ToQColor(m_curFillColor), this, tr("Select Fill Color"));
        if (chosen.isValid()) {
            m_curFillColor = QtDrawingContext::FromQColor(chosen);
            m_btnFillColor->setIcon(createColorIcon(m_curFillColor));
            if (auto* c = activeCanvas()) c->SetCurrentFillColor(m_curFillColor);
        }
    });
    tbStyle->addWidget(m_btnFillColor);

    // Fill enabled
    m_chkFillEnabled = new QCheckBox(tr("Fill Enabled"), this);
    m_chkFillEnabled->setChecked(true);
    connect(m_chkFillEnabled, &QCheckBox::toggled, this, [this](bool checked) {
        if (auto* c = activeCanvas()) c->SetCurrentFillEnabled(checked);
    });
    tbStyle->addWidget(m_chkFillEnabled);

    // Stroke width
    tbStyle->addWidget(new QLabel(tr(" Stroke: "), this));
    m_spnStrokeWidth = new QDoubleSpinBox(this);
    m_spnStrokeWidth->setRange(0.5, 30.0);
    m_spnStrokeWidth->setValue(2.0);
    m_spnStrokeWidth->setSingleStep(0.5);
    connect(m_spnStrokeWidth, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (auto* c = activeCanvas()) c->SetCurrentStrokeWidth(val);
    });
    tbStyle->addWidget(m_spnStrokeWidth);

    tbStyle->addSeparator();

    // Shape text
    tbStyle->addWidget(new QLabel(tr(" Text: "), this));
    m_txtShapeText = new QLineEdit(this);
    m_txtShapeText->setMaximumWidth(150);
    connect(m_txtShapeText, &QLineEdit::textChanged, this, [this](const QString& txt) {
        if (auto* c = activeCanvas()) c->SetCurrentText(txt.toStdString());
    });
    tbStyle->addWidget(m_txtShapeText);

    // Font size
    tbStyle->addWidget(new QLabel(tr(" Size: "), this));
    m_spnFontSize = new QDoubleSpinBox(this);
    m_spnFontSize->setRange(6.0, 72.0);
    m_spnFontSize->setValue(14.0);
    connect(m_spnFontSize, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, [this](double val) {
        if (auto* c = activeCanvas()) c->SetCurrentFontSize(val);
    });
    tbStyle->addWidget(m_spnFontSize);

    // Font color
    m_btnFontColor = new QPushButton(createColorIcon(m_curFontColor), tr(" Font Color"), this);
    connect(m_btnFontColor, &QPushButton::clicked, this, [this]() {
        QColor chosen = QColorDialog::getColor(QtDrawingContext::ToQColor(m_curFontColor), this, tr("Select Font Color"));
        if (chosen.isValid()) {
            m_curFontColor = QtDrawingContext::FromQColor(chosen);
            m_btnFontColor->setIcon(createColorIcon(m_curFontColor));
            if (auto* c = activeCanvas()) c->SetCurrentFontColor(m_curFontColor);
        }
    });
    tbStyle->addWidget(m_btnFontColor);
}

void MainWindow::createToolboxDock() {
    m_toolboxDock = new QDockWidget(tr("Drawing Shapes"), this);
    m_toolboxDock->setObjectName("ShapesToolboxDock");
    m_toolboxDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_toolbox = new ShapeToolboxWidget(m_toolboxDock);
    m_toolboxDock->setWidget(m_toolbox);

    connect(m_toolbox, &ShapeToolboxWidget::toolChanged, this, &MainWindow::onToolChanged);

    addDockWidget(Qt::LeftDockWidgetArea, m_toolboxDock);
}

void MainWindow::createStatusBar() {
    auto* sb = statusBar();
    m_lblStatusTool = new QLabel(tr("Tool: Select"), this);
    m_lblCoordinates = new QLabel(tr("X: 0, Y: 0"), this);
    m_lblShapeCount = new QLabel(tr("Shapes: 0"), this);

    sb->addPermanentWidget(m_lblStatusTool);
    sb->addPermanentWidget(m_lblShapeCount);
    sb->addPermanentWidget(m_lblCoordinates);
    sb->showMessage(tr("Ready"), 2000);
}

void MainWindow::onToolChanged(ToolMode mode, core::ShapeType type, core::LineCapStyle startCap, core::LineCapStyle endCap) {
    if (mode == ToolMode::Select) {
        m_lblStatusTool->setText(tr("Tool: Select"));
    } else {
        m_lblStatusTool->setText(tr("Tool: %1").arg(core::ShapeTypeToString(type)));
    }

    if (auto* c = activeCanvas()) {
        c->SetTool(mode, type, startCap, endCap);
    }
}

void MainWindow::updateStatusBarCoordinates(const core::Point& pt) {
    m_lblCoordinates->setText(tr("X: %1, Y: %2").arg(static_cast<int>(pt.x)).arg(static_cast<int>(pt.y)));
}

void MainWindow::updateMenusAndToolbars() {
    auto* canvas = activeCanvas();
    const bool hasChild = (canvas != nullptr);

    m_actSave->setEnabled(hasChild);
    m_actSaveAs->setEnabled(hasChild);
    m_actExportPng->setEnabled(hasChild);
    m_actPrint->setEnabled(hasChild);
    m_actPrintPreview->setEnabled(hasChild);
    m_actPaperSize->setEnabled(hasChild);

    if (canvas && canvas->GetDocument()) {
        auto doc = canvas->GetDocument();
        m_lblShapeCount->setText(tr("Shapes: %1").arg(doc->GetShapes().size()));
        auto primary = doc->GetPrimarySelectedShape();
        m_actDelete->setEnabled(primary != nullptr);
        m_actBringForward->setEnabled(primary != nullptr);
        m_actSendBackward->setEnabled(primary != nullptr);

        if (primary) {
            m_curOutlineColor = primary->GetOutlineColor();
            m_curFillColor = primary->GetFillColor();
            m_btnOutlineColor->setIcon(createColorIcon(m_curOutlineColor));
            m_btnFillColor->setIcon(createColorIcon(m_curFillColor));
            m_chkFillEnabled->setChecked(primary->IsFillEnabled());
            m_spnStrokeWidth->setValue(primary->GetStrokeWidth());
            m_txtShapeText->setText(QString::fromStdString(primary->GetText()));
            m_spnFontSize->setValue(primary->GetFontSize());
            m_curFontColor = primary->GetFontColor();
            m_btnFontColor->setIcon(createColorIcon(m_curFontColor));
        }
    } else {
        m_lblShapeCount->setText(tr("Shapes: 0"));
        m_actDelete->setEnabled(false);
        m_actBringForward->setEnabled(false);
        m_actSendBackward->setEnabled(false);
    }
}

void MainWindow::newFile() {
    auto* child = createMdiChild();
    child->NewFile();
    child->show();
    updateMenusAndToolbars();
}

void MainWindow::openFile() {
    QString path = QFileDialog::getOpenFileName(this, tr("Open Drawing Raw Data"),
                                                QString(), tr("UFM Raw Data (*.ufm);;All Files (*)"));
    if (path.isEmpty()) return;

    auto* child = createMdiChild();
    if (child->LoadFile(path)) {
        child->show();
        statusBar()->showMessage(tr("Loaded %1").arg(path), 3000);
    } else {
        child->close();
        QMessageBox::critical(this, tr("Error"), tr("Failed to load raw data file: %1").arg(path));
    }
    updateMenusAndToolbars();
}

void MainWindow::saveFile() {
    auto* child = activeMdiChild();
    if (!child) return;

    if (child->CurrentFilePath().isEmpty() || child->CurrentFilePath().startsWith("Drawing ")) {
        saveAsRawData();
    } else {
        if (child->SaveFile(child->CurrentFilePath())) {
            statusBar()->showMessage(tr("Saved %1").arg(child->CurrentFilePath()), 3000);
        }
    }
}

void MainWindow::saveAsRawData() {
    auto* child = activeMdiChild();
    if (!child) return;

    QString path = QFileDialog::getSaveFileName(this, tr("Save As Raw Data"),
                                                child->UserFriendlyCurrentFile(),
                                                tr("UFM Raw Data (*.ufm);;All Files (*)"));
    if (path.isEmpty()) return;

    if (child->SaveFile(path)) {
        statusBar()->showMessage(tr("Saved As Raw Data: %1").arg(path), 3000);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to save file: %1").arg(path));
    }
}

void MainWindow::exportPng() {
    auto* child = activeMdiChild();
    if (!child) return;

    QString path = QFileDialog::getSaveFileName(this, tr("Export Diagram as PNG"),
                                                "diagram.png",
                                                tr("PNG Image (*.png);;All Files (*)"));
    if (path.isEmpty()) return;

    if (child->ExportPng(path)) {
        statusBar()->showMessage(tr("Exported diagram to %1").arg(path), 3000);
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Failed to export PNG: %1").arg(path));
    }
}

void MainWindow::printFile() {
    auto* canvas = activeCanvas();
    if (!canvas) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setPageOrientation(QPageLayout::Portrait);
    QPrintDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted) {
        canvas->Print(&printer);
    }
}

void MainWindow::printPreview() {
    auto* canvas = activeCanvas();
    if (!canvas) return;
    canvas->ShowPrintPreview();
}

void MainWindow::deleteSelectedShape() {
    auto* canvas = activeCanvas();
    if (!canvas || !canvas->GetDocument()) return;

    auto primary = canvas->GetDocument()->GetPrimarySelectedShape();
    if (primary) {
        canvas->GetDocument()->RemoveShape(primary->GetId());
        canvas->update();
        updateMenusAndToolbars();
    }
}

void MainWindow::setPaperSizeDialog() {
    auto* canvas = activeCanvas();
    if (!canvas || !canvas->GetDocument()) return;

    const auto curSize = canvas->GetDocument()->GetPaperSize();

    QDialog dlg(this);
    dlg.setWindowTitle(tr("Paper Size Resolution"));

    auto* form = new QFormLayout(&dlg);
    auto* spW = new QDoubleSpinBox(&dlg);
    spW->setRange(100.0, 10000.0);
    spW->setValue(curSize.width);
    spW->setSuffix(" px");

    auto* spH = new QDoubleSpinBox(&dlg);
    spH->setRange(100.0, 10000.0);
    spH->setValue(curSize.height);
    spH->setSuffix(" px");

    form->addRow(tr("Paper Width:"), spW);
    form->addRow(tr("Paper Height:"), spH);

    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dlg);
    connect(btnBox, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);
    form->addRow(btnBox);

    if (dlg.exec() == QDialog::Accepted) {
        canvas->SetPaperSize(spW->value(), spH->value());
        statusBar()->showMessage(tr("Paper size changed to %1 x %2").arg(spW->value()).arg(spH->value()), 3000);
    }
}

void MainWindow::applyTheme(AppTheme theme) {
    ThemeManager::ApplyTheme(theme);
}

void MainWindow::about() {
    QMessageBox::about(this, tr("About UFMNext 2D Drawing Studio"),
                       tr("<h3>UFMNext 2D Drawing Studio</h3>"
                          "<p>Written in ISO C++ 17 with Qt 6.</p>"
                          "<p>Features:</p>"
                          "<ul>"
                          "<li>Full MDI Architecture with Tabbed / SubWindow support</li>"
                          "<li>Polymorphic Shape hierarchy with virtual Draw()</li>"
                          "<li>Basic Shapes: Square, Round, Circle, Oval, Line, Arrow, Star</li>"
                          "<li>Tracker with 8 resize handles & line connection snapping</li>"
                          "<li>Fast rendering with visible-viewport culling</li>"
                          "<li>Parametrized paper size (default 1200 x 1400)</li>"
                          "<li>PNG diagram export & Print Preview</li>"
                          "<li>Save / Load Raw Data (.ufm)</li>"
                          "<li>Modern Themes: Dark, Light, Modern Blue</li>"
                          "</ul>"));
}

} // namespace ufmnext::gui
