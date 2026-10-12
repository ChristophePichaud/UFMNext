#include "ufmnext/gui/MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("UFMNext");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("ChristophePichaud");

    ufmnext::gui::MainWindow window;
    window.show();

    return app.exec();
}
