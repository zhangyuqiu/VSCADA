#ifndef TSV_THREAD_H
#define TSV_THREAD_H

#include <QThread>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <QTimer>
#include "typedefs.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class TSV_Thread : public QObject
{

    Q_OBJECT
public:
    DataMonitor * monitor;                          // pointer to a datamonitor object
    QTimer * timer;

    TSV_Thread(DataMonitor * mtr){
        timer = new QTimer;
        connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
        monitor = mtr;
    }                                               // class object constructor
    virtual ~TSV_Thread(){}                         // class object destructor

    int testVal = 0;
    bool running = true;                             // to control running of collection thread
    int TSV_rate = 0;                                    // sampling rate of cooling system
    std::vector<meta> TSVSensorMeta;                 // tsv sensor metadata


    /** Reads Specified Cooling Sensor Data*/
    datapoint * read_CAN(meta sensor){
        // should call a method from IO control to read tsi sensor data
    }

    void start(){
        timer->start(TSV_rate);
    }

    void stop(){
        timer->stop();
    }

    /** Will not return until the internal thread has exited. If exists, waits until thread has completed */
    void WaitForInternalThreadToExit()
    {
       (void) pthread_join(_thread, NULL);
    }


protected:
    /** Active tsi data collection method */
    virtual void InternalThreadEntry(){
        testVal++;
        cout << "TSV Data Collected" << endl;
    }


private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((TSV_Thread *)This)->InternalThreadEntry(); return NULL;}

    pthread_t _thread;

public slots:
    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    void StartInternalThread()
    {
       pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
    }
};

#endif // TSV_THREAD_H
