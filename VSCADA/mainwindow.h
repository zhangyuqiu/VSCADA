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
    void updateData(vector <int> glvVector, vector <int> tsiVector, vector <int> tsvVector, vector <int> coolingVector);
    void exit();
    void initMetadata(vector<meta> glv, vector<meta> tsi, vector<meta> tsv, vector<meta> cooling);

    vector<meta> GLV_meta;
    vector<meta> TSI_meta;
    vector<meta> TSV_meta;
    vector<meta> COOLING_meta;

    vector<QLineEdit *> GLVEdits;
    vector<QLineEdit *> TSIEdits;
    vector<QLineEdit *> TSVEdits;
    vector<QLineEdit *> COOLINGEdits;


    QGridLayout * mainLayout;
    QWidget * central;


    QPushButton * plotButton;
    QPushButton * exitButton;

    QListWidget * message;
    QCustomPlot * plot;



    QTimer * timer;

    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;
    int xinit;
    int yinit;

    datapoint data;

    Config * conf;
private slots:
    void plotGraph();
    void update();
    void updateVals();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

