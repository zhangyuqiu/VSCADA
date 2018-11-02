#ifndef DETAILPAGE_H
#define DETAILPAGE_H
#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>
#include <QRegion>
#include <QtWidgets>
#include <qcustomplot.h>
#include <string>
#include "config.h"
#include "typedefs.h"
#include "canbus_interface.h"
#include "subsystemthread.h"


namespace Ui {
class detailPage;
}

class detailPage : public QMainWindow
{
    Q_OBJECT

public:
    explicit detailPage(QWidget *parent = nullptr);
    ~detailPage();
    void drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay);
    void addErrorMessage(QString message);
    void addPoint(int x, int y);
    void exit();
    void setCurrentSystem(int current);

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
    vector<QPushButton *> stateButtons;
    vector<QPushButton *> FSMButtons;
    QString ** systemName;


    QGridLayout * mainLayout;
    QWidget * central;


    QPushButton * stateButton;
    QPushButton * plotButton;
    QPushButton * exitButton;
    QPushButton * indiButton;

    QListWidget * message;
    QCustomPlot * plot;
    QLabel * ctrlLabel;

    int currentSystem;
    int currentSubSystem;


    QTimer * timer;

    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;
    int xinit;
    int yinit;
    int maxSensorRow;
    bool detail;

     QVector<double> gx,gy;

    datapoint data;

    Config * conf;
private slots:
    void update();
    void updateVals();
    void plotGraph();
    void receiveMsg(string msg);
    void deactivateStateMW(system_state * prevstate);
    void activateStateMW(system_state * nextState);

private:
    Ui::detailPage *ui;
};

#endif

