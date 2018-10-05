#include "mainwindow.h"
#include <QApplication>
#include "typedefs.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(70,70,70));
    palette.setColor(QPalette::WindowText, QColor(255,255,255));
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(70,70,70));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::yellow);
    palette.setColor(QPalette::Button, QColor(70,70,70));
    palette.setColor(QPalette::ButtonText, QColor(255,255,255));
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);
    QApplication::setPalette(palette);

    MainWindow w;
    w.setWindowState((w.windowState()^Qt::WindowFullScreen));
    datapoint d;
//    w.update(d);
    w.show();

    return a.exec();
}
