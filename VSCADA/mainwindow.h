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

    QLabel * glvLabel;
    QLabel * tsvLabel;
    QLabel * tsiLabel;

    QLabel * glvLabel1;
    QLabel * tsvLabel1;
    QLabel * tsiLabel1;

    QLabel * glvLabel2;
    QLabel * tsvLabel2;
    QLabel * tsiLabel2;

    QLabel * glvLabel3;
    QLabel * tsvLabel3;
    QLabel * tsiLabel3;

    QLineEdit * glvEdit1;
    QLineEdit * glvEdit2;
    QLineEdit * glvEdit3;

    QLineEdit * tsiEdit1;
    QLineEdit * tsiEdit2;
    QLineEdit * tsiEdit3;

    QLineEdit * tsvEdit11;
    QLineEdit * tsvEdit21;
    QLineEdit * tsvEdit31;

    QLineEdit * tsvEdit12;
    QLineEdit * tsvEdit22;
    QLineEdit * tsvEdit32;

    QLineEdit * tsvEdit13;
    QLineEdit * tsvEdit23;
    QLineEdit * tsvEdit33;

    QLineEdit * tsvEdit14;
    QLineEdit * tsvEdit24;
    QLineEdit * tsvEdit34;

    QPushButton * plotButton;
    QPushButton * glvButton;
    QPushButton * tsiButton;
    QPushButton * tsvButton;
    QPushButton * exitButton;

    QComboBox * glvBox;
    QComboBox * tsiBox;
    QComboBox * tsvBox;

    QListWidget * message;
    QCustomPlot * plot;

    QString dataDisplayGLV1;
    QString dataDisplayGLV2;
    QString dataDisplayGLV3;

    QString dataDisplayTSI1;
    QString dataDisplayTSI2;
    QString dataDisplayTSI3;

    QString dataDisplayTSV11;
    QString dataDisplayTSV21;
    QString dataDisplayTSV31;

    QString dataDisplayTSV12;
    QString dataDisplayTSV22;
    QString dataDisplayTSV32;

    QString dataDisplayTSV13;
    QString dataDisplayTSV23;
    QString dataDisplayTSV33;

    QString dataDisplayTSV14;
    QString dataDisplayTSV24;
    QString dataDisplayTSV34;

    QTimer * timer;

    int editsize;
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

