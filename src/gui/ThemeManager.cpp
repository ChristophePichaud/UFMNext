#include "ufmnext/gui/ThemeManager.h"
#include <QPalette>
#include <QStyleFactory>

namespace ufmnext::gui {

QString ThemeManager::GetThemeName(AppTheme theme) {
    switch (theme) {
        case AppTheme::Dark: return "Dark";
        case AppTheme::Light: return "Light";
        case AppTheme::ModernBlue: return "Modern Blue";
        default: return "Default";
    }
}

void ThemeManager::ApplyTheme(AppTheme theme) {
    if (theme == AppTheme::Dark) {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
        darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
        darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::Button, QColor(45, 45, 48));
        darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(0, 122, 204));
        darkPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));
        darkPalette.setColor(QPalette::HighlightedText, Qt::white);
        QApplication::setPalette(darkPalette);
        qApp->setStyleSheet(getDarkStyleSheet());
    } else if (theme == AppTheme::ModernBlue) {
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        QPalette bluePalette;
        bluePalette.setColor(QPalette::Window, QColor(245, 247, 250));
        bluePalette.setColor(QPalette::WindowText, QColor(32, 32, 32));
        bluePalette.setColor(QPalette::Base, Qt::white);
        bluePalette.setColor(QPalette::AlternateBase, QColor(238, 242, 246));
        bluePalette.setColor(QPalette::Text, QColor(32, 32, 32));
        bluePalette.setColor(QPalette::Button, QColor(238, 242, 246));
        bluePalette.setColor(QPalette::ButtonText, QColor(32, 32, 32));
        bluePalette.setColor(QPalette::Highlight, QColor(16, 110, 190));
        bluePalette.setColor(QPalette::HighlightedText, Qt::white);
        QApplication::setPalette(bluePalette);
        qApp->setStyleSheet(getModernBlueStyleSheet());
    } else {
        // Light
        QApplication::setStyle(QStyleFactory::create("Fusion"));
        QPalette lightPalette;
        lightPalette.setColor(QPalette::Window, QColor(240, 240, 240));
        lightPalette.setColor(QPalette::WindowText, QColor(30, 30, 30));
        lightPalette.setColor(QPalette::Base, Qt::white);
        lightPalette.setColor(QPalette::AlternateBase, QColor(247, 247, 247));
        lightPalette.setColor(QPalette::Text, QColor(30, 30, 30));
        lightPalette.setColor(QPalette::Button, QColor(240, 240, 240));
        lightPalette.setColor(QPalette::ButtonText, QColor(30, 30, 30));
        lightPalette.setColor(QPalette::Highlight, QColor(0, 120, 215));
        lightPalette.setColor(QPalette::HighlightedText, Qt::white);
        QApplication::setPalette(lightPalette);
        qApp->setStyleSheet(getLightStyleSheet());
    }
}

QString ThemeManager::getDarkStyleSheet() {
    return R"(
        QMainWindow {
            background-color: #1e1e1e;
        }
        QMenuBar {
            background-color: #2d2d30;
            color: #f1f1f1;
            border-bottom: 1px solid #3f3f46;
            padding: 2px;
        }
        QMenuBar::item:selected {
            background-color: #3e3e42;
            border-radius: 3px;
        }
        QMenu {
            background-color: #252526;
            color: #f1f1f1;
            border: 1px solid #3f3f46;
            padding: 4px;
        }
        QMenu::item:selected {
            background-color: #094771;
            border-radius: 3px;
        }
        QToolBar {
            background-color: #2d2d30;
            border: 1px solid #3f3f46;
            spacing: 4px;
            padding: 3px;
        }
        QToolButton {
            background-color: transparent;
            color: #f1f1f1;
            border: 1px solid transparent;
            border-radius: 3px;
            padding: 4px;
        }
        QToolButton:hover {
            background-color: #3e3e42;
            border: 1px solid #007acc;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #094771;
            border: 1px solid #007acc;
        }
        QMdiArea {
            background-color: #252526;
        }
        QDockWidget {
            color: #f1f1f1;
            font-weight: bold;
        }
        QDockWidget::title {
            background-color: #2d2d30;
            border: 1px solid #3f3f46;
            padding: 4px;
        }
        QStatusBar {
            background-color: #007acc;
            color: white;
            font-weight: 500;
        }
        QSpinBox, QDoubleSpinBox, QComboBox, QLineEdit {
            background-color: #333337;
            color: #f1f1f1;
            border: 1px solid #3f3f46;
            border-radius: 3px;
            padding: 3px 6px;
        }
    )";
}

QString ThemeManager::getLightStyleSheet() {
    return R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QMenuBar {
            background-color: #ffffff;
            color: #333333;
            border-bottom: 1px solid #e0e0e0;
            padding: 2px;
        }
        QMenuBar::item:selected {
            background-color: #e5f1fb;
            border-radius: 3px;
        }
        QMenu {
            background-color: #ffffff;
            color: #333333;
            border: 1px solid #dcdcdc;
            padding: 4px;
        }
        QMenu::item:selected {
            background-color: #cce8ff;
            border-radius: 3px;
        }
        QToolBar {
            background-color: #fafafa;
            border-bottom: 1px solid #e0e0e0;
            spacing: 4px;
            padding: 3px;
        }
        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 3px;
            padding: 4px;
        }
        QToolButton:hover {
            background-color: #e5f1fb;
            border: 1px solid #70c0e7;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #cce8ff;
            border: 1px solid #0078d4;
        }
        QMdiArea {
            background-color: #e8e8e8;
        }
        QDockWidget::title {
            background-color: #f0f0f0;
            border: 1px solid #dcdcdc;
            padding: 4px;
        }
        QStatusBar {
            background-color: #0078d4;
            color: white;
            font-weight: 500;
        }
        QSpinBox, QDoubleSpinBox, QComboBox, QLineEdit {
            background-color: #ffffff;
            border: 1px solid #c8c8c8;
            border-radius: 3px;
            padding: 3px 6px;
        }
    )";
}

QString ThemeManager::getModernBlueStyleSheet() {
    return R"(
        QMainWindow {
            background-color: #f3f6f9;
        }
        QMenuBar {
            background-color: #004b87;
            color: #ffffff;
            border-bottom: 1px solid #003660;
            padding: 3px;
        }
        QMenuBar::item:selected {
            background-color: #005a9e;
            border-radius: 3px;
        }
        QMenu {
            background-color: #ffffff;
            color: #202020;
            border: 1px solid #c0d1e0;
            padding: 4px;
        }
        QMenu::item:selected {
            background-color: #deecf9;
            color: #004b87;
            border-radius: 3px;
        }
        QToolBar {
            background-color: #ffffff;
            border-bottom: 1px solid #d0dce8;
            spacing: 5px;
            padding: 4px;
        }
        QToolButton {
            background-color: transparent;
            border: 1px solid transparent;
            border-radius: 4px;
            padding: 4px;
        }
        QToolButton:hover {
            background-color: #eff6fc;
            border: 1px solid #106ebe;
        }
        QToolButton:pressed, QToolButton:checked {
            background-color: #deecf9;
            border: 1px solid #005a9e;
        }
        QMdiArea {
            background-color: #e1e9f0;
        }
        QDockWidget::title {
            background-color: #e8f0f8;
            border: 1px solid #c8d8e8;
            padding: 5px;
            color: #004b87;
            font-weight: bold;
        }
        QStatusBar {
            background-color: #004b87;
            color: white;
            font-weight: 500;
        }
        QSpinBox, QDoubleSpinBox, QComboBox, QLineEdit {
            background-color: #ffffff;
            border: 1px solid #b4c8dc;
            border-radius: 3px;
            padding: 3px 6px;
        }
    )";
}

} // namespace ufmnext::gui
