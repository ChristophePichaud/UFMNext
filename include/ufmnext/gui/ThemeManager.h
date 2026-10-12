#pragma once

#include <QString>
#include <QApplication>

namespace ufmnext::gui {

enum class AppTheme {
    Dark = 0,
    Light,
    ModernBlue
};

class ThemeManager {
public:
    static void ApplyTheme(AppTheme theme);
    static QString GetThemeName(AppTheme theme);

private:
    static QString getDarkStyleSheet();
    static QString getLightStyleSheet();
    static QString getModernBlueStyleSheet();
};

} // namespace ufmnext::gui
