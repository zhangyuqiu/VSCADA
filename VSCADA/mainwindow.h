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




namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void update();
    void drawEdit(QLineEdit * edit, int x, int y);
    void plotGraph();
    void addErrorMessage(QString message);

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

    QComboBox * glvBox;
    QComboBox * tsiBox;
    QComboBox * tsvBox;

    QListWidget * message;
    QCustomPlot * plot;

    int editsize;
    int xinit;
    int yinit;


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
