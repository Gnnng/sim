#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow w;
    app.setOrganizationName("FourUncles Inc.");
    app.setApplicationName("MIPS Simulator");
    w.show();

    return app.exec();
}
