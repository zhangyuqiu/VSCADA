#ifndef TSV_THREAD_H
#define TSV_THREAD_H

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

class TSV_Thread : public QObject
{

    Q_OBJECT
public:

    TSV_Thread(DataMonitor * mtr, canbus_interface * can, vector<meta> TSVSensors); //class object constructor                                          // class object constructor
    virtual ~TSV_Thread();                                  //class object destructor

    void stop();                                            //stops data collection
    void start();                                           //starts data collection
    void init_TSV_data();                                   //initializes TSV data vector
    vector<int> get_TSV_Data();                             //retrieves TSV Data
    void WaitForInternalThreadToExit();                     //stops code until this thread is destroyed

    QTimer * timer;                                         //timer to control data collection frequency
    DataMonitor * monitor;                                  //pointer to datamonitor object
    canbus_interface * canInterface;                        //can interface to enable reading from CAN

    int testVal = 0;                                        //dummy data for testing

    int TSV_rate = 0;                                       //sampling rate of cooling system
    vector<int> TSVData;                                    //most recent tsv sensor data samples
    vector<meta> TSVSensorMeta;                             //tsv sensor metadata

protected:
    virtual void TSVCollectionTasks();                      //runs thread tasks


private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((TSV_Thread *)This)->TSVCollectionTasks(); return NULL;}

    pthread_t _thread;

public slots:
    void StartInternalThread();
};

#endif // TSV_THREAD_H
