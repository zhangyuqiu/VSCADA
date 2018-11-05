#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>
#include <QtWidgets>
#include <QDialog>
#include <qcustomplot.h>
#include <string>
#include "typedefs.h"
#include "qcgaugewidget.h"




namespace Ui {
class dashboard;
}

class QValueSpacePublisher;

class dashboard : public QMainWindow
{
    Q_OBJECT

public:
    explicit dashboard(QWidget *parent = nullptr);
    ~dashboard();

    QGridLayout * mainLayout;
    QWidget * central;
    QProgressBar * battery;
    QcGaugeWidget * mSpeedGauge;
    QcNeedleItem *mSpeedNeedle;
    QcGaugeWidget * batteryG;
    float speedNum;
    int batteryNum;
    int timeNum;
    QPushButton * stateButton;
    QPushButton * exitButton;




private:
    Ui::dashboard *ui;
};

#endif // DASHBOARD_H
