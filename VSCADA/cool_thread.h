#ifndef COOL_THREAD_H
#define COOL_THREAD_H

#include <QThread>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <QTimer>
#include "typedefs.h"
#include "canbus_interface.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class COOL_Thread : public QObject
{
    Q_OBJECT
public:
    COOL_Thread(DataMonitor * mtr, canbus_interface * can, vector<meta> COOLSensors);   //class object destructor
    virtual ~COOL_Thread();                                     //class object destructor

    void stop();                                                //stops data collection
    void start();                                               //starts data collection
    void init_COOL_data();                                      //retrieves GLV Data
    vector<int> get_COOL_data();                                //initializes GLV data vector
    void WaitForInternalThreadToExit();                         //stops code until this thread is destroyed

    QTimer * timer;                                             //timer to implement sampling frequency
    DataMonitor * monitor;                                      //pointer to a datamonitor object
    canbus_interface * canInterface;                            //canInterface to provide read access to CAN

    int testVal = 0;                                            //dummy variable for testing

    int COOL_rate = 0;                                          //sampling rate of cooling system
    bool running = true;                                        //to control running of collection thread
    vector<int> COOLData;                                       //cooling sensor data
    std::vector<meta> COOLSensorMeta;                           //cooling sensor metadata

protected:
    virtual void COOLINGCollectionTasks();                      //runs collection tasks

private:
    /** Links the member function to ordinary space */
    static void * InternalCOOLThreadEntryFunc(void * This) {((COOL_Thread *)This)->COOLINGCollectionTasks(); return NULL;}

    pthread_t _thread;

public slots:
    void StartInternalThread();                                 //starts thread
};

#endif // COOL_THREAD_H
