#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>
#include <QtWidgets>
#include <qcustomplot.h>
#include <string>
#include "config.h"
#include "typedefs.h"
#include "canbus_interface.h"
#include "subsystemthread.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay);
    void addErrorMessage(QString message);
    void exit();

    vector<meta> GLV_meta;
    vector<meta> TSI_meta;
    vector<meta> TSV_meta;
    vector<meta> COOLING_meta;

    vector<QLineEdit *> GLVEdits;
    vector<QLineEdit *> TSIEdits;
    vector<QLineEdit *> TSVEdits;
    vector<QLineEdit *> COOLINGEdits;

    vector<QComboBox *> systemBox;
    vector<QPushButton *> systemButton;
    QVector< QVector< QString> > systemName;


    QGridLayout * mainLayout;
    QWidget * central;


    QPushButton * plotButton;
    QPushButton * exitButton;

    QListWidget * message;
    QCustomPlot * plot;

    int currentSystem;


    QTimer * timer;

    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;
    int xinit;
    int yinit;

     QVector<double> gx,gy;

    datapoint data;

    Config * conf;
private slots:
    void plotGraph();
    void update();
    void updateVals();
    void receiveMsg(string msg);
    void receiveErrMsg(string msg);
    void getCurrentSystem(int i);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

