#ifndef SUBSYSTEMTHREAD_H
#define SUBSYSTEMTHREAD_H

#include <QThread>
#include <QObject>
#include <QLineEdit>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <QTimer>
#include <QQueue>
#include <iomanip>
#include "typedefs.h"
#include "datamonitor.h"
#include "db_engine.h"

class DataMonitor;

using namespace std;

class SubsystemThread : public QObject
{
    Q_OBJECT
public:
    SubsystemThread(vector<meta *> sensors, string id, vector<response> respVector, vector<logic> lVector, vector<meta *> mainMeta);    //class object destructor
    virtual ~SubsystemThread();                                                         //class object destructor

    void stop();                                    //stops data collection
    void start();                                   //starts data collection
    void init_data();                               //retrieves GLV Data
    vector<int> get_data();                         //initializes GLV data vector
    string get_curr_time();                         //get curent time
    void setDB(DB_Engine * db);                     //sets database object
    void set_rate(int newRate);                     //sets rate at which data is checked
    void logMsg(string msg);                        //enqueue message for display
    vector<meta *> get_metadata();                  //retrieves a configured list of sensors
    void setMonitor(DataMonitor * mtr);             //sets monitor object
    void WaitForInternalThreadToExit();             //stops code until this thread is destroyed
    void calibrateData(meta * currSensor);
    void checkLogic(meta * currSensor);
    vector<meta *> get_mainMeta();

    QTimer * timer;                                 //timer to implement sampling frequency
    DataMonitor * monitor;                          //pointer to a datamonitor object
    DB_Engine * dbase;                              //pointer to database object

    QTimer * checkTmr;                              //timer placeholder for checking update frequencies
    QLineEdit * lineEdit;                           //lineEdif placeholder for sensor-specific line edits

    int checkRate = 0;                              //rate for checking for sensor updates
    string subsystemId;                             //identifies subsystem by name

    vector<meta *> sensorMeta;                      //cooling sensor metadata
    vector<meta *> mainSensorVector;
    vector<QLineEdit *> edits;                      //lineEdits for displaying data
    vector<QLineEdit *> controlEdits;
    vector<QTimer *> editTimers;                    //stores checkTimers
    vector<response> responseVector;                //stores configured responses
    vector<logic> logicVector;

    QQueue<string> * msgQueue;                      //queue to store messages for display
    bool error;
    QQueue<response> * respCANQueue;                //queue for CAN responses
    QQueue<response> * respGPIOQueue;               //queue for gpio responses

    bool running = true;                            //to control running of collection thread
    vector<int> rawData;                            //cooling sensor data

protected:
    virtual void subsystemCollectionTasks();        //runs collection tasks

private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((SubsystemThread *)This)->subsystemCollectionTasks(); return NULL;}

    pthread_t _thread;

public slots:
    void receiveData(meta * currSensor);
    void logData(meta * currSensor);                            //records sensor data in database
    void StartInternalThread();                                 //starts subsystem thread
    void updateEdits(meta *sensor);                             //updates LineEdit displays
    void checkThresholds(meta * sensor);                        //checks whether sensor value is within configured bounds
    void checkTimeout();                                        //check whether we haven't received some data

signals:
    void pushCANItem(response rsp);                             //execute response to CAN
    void pushGPIOData(response rsp);                            //execute response to GPIO
    void pushMessage(string msg);
    void pushErrMsg(string msg);
    void valueChanged();
    void initiateRxn(int rxnCode);                               //execute configured reaction
};

#endif // SUBSYSTEMTHREAD_H
