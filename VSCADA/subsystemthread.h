#ifndef SUBSYSTEMTHREAD_H
#define SUBSYSTEMTHREAD_H

#include <QThread>
#include <QObject>
#include <QLineEdit>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <QTimer>
#include "typedefs.h"
#include "canbus_interface.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class SubsystemThread : public QObject
{
    Q_OBJECT
public:
    SubsystemThread(vector<meta> sensors, string id);   //class object destructor
    virtual ~SubsystemThread();                                     //class object destructor

    void stop();                                                //stops data collection
    void start();                                               //starts data collection
    void init_data();                                           //retrieves GLV Data
    void set_rate(int newRate);                                            //sets rate at which data is checked
    void updateEdits();
    void setCAN(canbus_interface * canInt);
    void setMonitor(DataMonitor * mtr);
    vector<meta> get_metadata();
    vector<int> get_data();                                     //initializes GLV data vector
    void WaitForInternalThreadToExit();                         //stops code until this thread is destroyed

    QTimer * timer;                                             //timer to implement sampling frequency
    DataMonitor * monitor;                                      //pointer to a datamonitor object
    canbus_interface * canInterface;                            //canInterface to provide read access to CAN

    int testVal = 0;                                            //dummy variable for testing

    string subsystemId;
    int checkRate = 0;                                          //sampling rate of cooling system
    bool running = true;                                        //to control running of collection thread
    vector<int> rawData;                                       //cooling sensor data
    vector<meta> sensorMeta;                           //cooling sensor metadata
    vector<QLineEdit *> edits;

protected:
    virtual void subsystemCollectionTasks();                      //runs collection tasks

private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((SubsystemThread *)This)->subsystemCollectionTasks(); return NULL;}

    pthread_t _thread;

public slots:
    void StartInternalThread();                                 //starts thread
};

#endif // SUBSYSTEMTHREAD_H
