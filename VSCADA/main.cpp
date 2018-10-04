#include "mainwindow.h"
#include <QApplication>
#include "typedefs.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowState((w.windowState()^Qt::WindowFullScreen));
    datapoint d;
    w.update(d);
    w.show();

    return a.exec();
}
