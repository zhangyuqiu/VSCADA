#ifndef TSI_THREAD_H
#define TSI_THREAD_H

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

class TSI_Thread : public QObject
{

    Q_OBJECT

public:
    TSI_Thread(DataMonitor * mtr, canbus_interface * can, vector<meta> TSISensors); //class object constructor
    virtual ~TSI_Thread();                                  //class object destructor

    void stop();                                            //stops data collection
    void start();                                           //starts data collection
    void init_TSI_data();                                   //initializes GLV data vector
    vector<int> get_TSI_Data();                             //retrieves GLV Data
    void WaitForInternalThreadToExit();                     //stops code until this thread is destroyed

    QTimer * timer;                                         //timer to control data collection frequency
    DataMonitor * monitor;                                  //pointer to a datamonitor object
    canbus_interface * canInterface;                        //can interface to enable reading from CAN

    int testVal = 0;                                        //dummy data for testing

    int TSI_rate = 0;                                       //sampling rate of tsi subsystem
    vector<int> TSIData;                                    //last tsi data sampled
    std::vector<meta> TSISensorMeta;                        //tsi sensor metadata

protected:
    virtual void TSICollectionTasks();                      //runs thread tasks

private:
    /** Links the member function to ordinary space */
    static void * InternalTSIThreadEntryFunc(void * This) {((TSI_Thread *)This)->TSICollectionTasks(); return NULL;}

    pthread_t _thread;

public slots:
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    void StartInternalThread();
};

#endif // TSI_THREAD_H
