#include "mainwindow.h"
#include <QApplication>
#include "typedefs.h"
#include "config.h"
#include "dashboard.h"
#include "postprocess.h"

int main(int argc, char *argv[])
{
//    qputenv("QT_IM_MODULE",QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);

    QApplication::setStyle(QStyleFactory::create("fusion"));

    QPalette palette;
//    palette.setColor(QPalette::Window, QColor(139,69,19));
//    palette.setColor(QPalette::Window, QColor(96,0,128));
    palette.setColor(QPalette::Window, QColor(0,32,128));
//    palette.setColor(QPalette::Window, QColor(128,0,32));
//    palette.setColor(QPalette::Window, QColor(70,70,70));
    palette.setColor(QPalette::WindowText, QColor(255,255,255));
    palette.setColor(QPalette::Base, QColor(15,15,15));
    palette.setColor(QPalette::AlternateBase, QColor(180,0,130));
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::yellow);
//    palette.setColor(QPalette::Button, QColor(139,69,19));
//    palette.setColor(QPalette::Button, QColor(96,0,128));
    palette.setColor(QPalette::Button, QColor(0,32,128));
//    palette.setColor(QPalette::Button, QColor(128,0,32));
//    palette.setColor(QPalette::Button, QColor(70,70,70));
    palette.setColor(QPalette::ButtonText, QColor(255,255,255));
    palette.setColor(QPalette::BrightText, Qt::red);

    palette.setColor(QPalette::Highlight, QColor(142,45,197).lighter());
    palette.setColor(QPalette::HighlightedText, Qt::black);

    QApplication::setPalette(palette);

    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

//    QPixmap pixmap("team_pic.jpg");
//    pixmap = pixmap.scaledToHeight(height);
//    QWidget splashWidget;
//    QMovie *movie = new QMovie("load_gear.gif");
//    QLabel *processLabel = new QLabel(movie);
//    processLabel->setMovie(movie);
//    movie->start();

//    pixmap.scaledToWidth(width);
//    QSplashScreen splash(processLabel);
//    splash.show();
//    a.processEvents();
//    sleep(2);
    MainWindow * w = new MainWindow;
    dashboard * d= new dashboard;
    //postProcess * p = new postProcess;
    //d->setWindowState((w->windowState()^Qt::WindowFullScreen));
        //    w->setWindowState((w->windowState()^Qt::WindowFullScreen));
        //    w->show();
    w->showFullScreen();
//    w->centralWidget()->releaseKeyboard();
//    p->setWindowState((w->windowState()^Qt::WindowFullScreen));
//    p->show();

//    QQuickView view(QString("qrc:/%2").arg(MAIN_QML));
//    view.setResizeMode(QQuickView::SizeRootObjectToView);
//    view.show();
    return a.exec();
}
