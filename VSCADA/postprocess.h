#ifndef POSTPROCESS_H
#define POSTPROCESS_H

#include <QMainWindow>
#include <QApplication>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QObject>
#include <QRegion>
#include<QtSql>
#include <QtWidgets>
#include <qcustomplot.h>
#include <string>
#include "config.h"
#include "typedefs.h"
#include "canbus_interface.h"
#include "subsystemthread.h"

namespace Ui {
class postProcess;
}

class postProcess : public QMainWindow
{
    Q_OBJECT

public:
    explicit postProcess(QWidget *parent = nullptr);
    ~postProcess();

    QGridLayout * mainLayout;
    QWidget * central;
    QLabel * label;
    QString ** systemName;
    Config * conf;
    vector<QString> tableName;
    vector<QString> sensorname;
    vector<QString> subsystem;
    QListView * list;
    QStandardItemModel*  poModel;
    int maxSensorRow;
    QListWidget * message;
    QPushButton * exitButton;
    QPushButton * loadButton;
    QComboBox * dbBox;
    QString currentBase;
    QTableView * dataTable;
    QSqlDatabase mydb;
    QTableWidget* displayTable;
    DB_Engine * db;
    vector<QString> selected;
    QCustomPlot * plot;
    bool raw;
    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;

private slots:
    void addSensor(QStandardItem *poItem);
    void getSensorList();
    void update();
    void loadTable();
    void reload(int no);

private:
    Ui::postProcess *ui;
};

#endif // POSTPROCESS_H
