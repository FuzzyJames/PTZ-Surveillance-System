/// @file GUI Application Entry Point
/// Launches the Qt6-based Windows surveillance GUI.

#include "MainWindow.h"
#include "surveillance/config.h"
#include "surveillance/logging.h"

#include <QApplication>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("Surveillance System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("SurveillanceProject");

    surveillance::logging::init("gui");
    LOG_INFO("Surveillance GUI starting...");

    MainWindow window;
    window.setWindowTitle("PTZ Camera Surveillance System");
    window.resize(1280, 800);
    window.show();

    LOG_INFO("GUI ready.");
    return app.exec();
}
