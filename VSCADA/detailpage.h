#ifndef DETAILPAGE_H
#define DETAILPAGE_H
#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>
#include <QFrame>
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
    int active_dialog(string msg);
    int passive_dialog(string msg);
    string info_dialog(string msg);
    void addPoint(double x, double y);
    void exit();
    void setCurrentSystem(SubsystemThread * subsystem);
    void setConfObject(Config * config);

    SubsystemThread * currentSubSystem;

    vector<QLineEdit *> GLVEdits;
    vector<QLineEdit *> TSIEdits;
    vector<QLineEdit *> TSVEdits;
    vector<QLineEdit *> COOLINGEdits;

    vector<QSlider *> controlSliders;
    vector<QPushButton *> controlButtons;
    vector<QLineEdit *> controlEdits;

    vector<controlSpec *> sliderCtrls;
    vector<controlSpec *> buttonCtrls;
    vector<controlSpec *> editCtrls;

    vector<QComboBox *> systemBox;
    vector<QPushButton *> healthButtons;
    vector<QPushButton *> detailButtons;
    vector<QPushButton *> stateButtons;
    vector<QPushButton *> FSMButtons;
    QString ** systemName;

    vector<QLineEdit *> edits;
    vector<QTimer *> editTimers;
    QLineEdit * lineEdit;
    QTimer * checkTmr;

    QVBoxLayout * mainLayout;
    QWidget * central;

    QGridLayout * bottomLayout = new QGridLayout;
    QComboBox * plotComboBox = new QComboBox;
    QLabel * currLabel;
    QPushButton * stateButton;
    QPushButton * plotButton;
    QPushButton * exitButton;
    QPushButton * indiButton;
    QPushButton * buttonControl;
    QHBoxLayout * controlsLayout;
    QSlider * sliderControl;
    QLineEdit * editControl;
    detailPage * detailWindow;

    QListWidget * message;
    QCustomPlot * plot;
    QLabel * ctrlLabel;


    int currentSystem;

    QTabWidget * tabs;

    QTimer * timer;

    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;
    int xinit;
    int yinit;
    int maxSensorRow;
    bool detail;
    double graphMax = 20;
    double graphMin = 0;

     QVector<double> gx,gy;

//    datapoint data;

    Config * conf;

signals:
    void sendControlValue(int data, controlSpec * spec);
    void openDetailPage(SubsystemThread * thread);
public slots:
    void updateFSM_MW(statemachine * currFSM);
    void deactivateStateMW(system_state * prevstate);
    void activateStateMW(system_state * nextState);
private slots:
    void plotGraph(QString sensorName);
    void update();
    void closeDetailPage();
    void updateVals();
    void updateGraph();
    void receiveMsg(string msg);
    void receiveErrMsg(string msg);
    void getCurrentSystem(int i);
    void sliderValChanged(int value);
    void ctrlButtonPressed();
    void ctrlButtonReleased();
    void editUpdated();
    void shutdownSystem();
    void closeTab(int tabId);
    void updateEdits(meta * sensor);
    void changeEditColor(string color, meta *sensor);
    void checkTimeout();

private:
    Ui::detailPage *ui;
};

#endif

