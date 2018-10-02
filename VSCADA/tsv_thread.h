#ifndef TSV_THREAD_H
#define TSV_THREAD_H

#include <QThread>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "typedefs.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class TSV_Thread
{

public:
    DataMonitor * monitor;                          // pointer to a datamonitor object

    TSV_Thread(DataMonitor * mtr){ monitor = mtr;}  // class object constructor
    virtual ~TSV_Thread(){}                         // class object destructor

    bool running = true;                             // to control running of collection thread
    int TSV_rate = 0;                                    // sampling rate of cooling system
    std::vector<meta> TSVSensorMeta;                 // tsv sensor metadata


    /** Reads Specified Cooling Sensor Data*/
    datapoint * read_CAN(meta sensor){
        // should call a method from IO control to read tsi sensor data
    }

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread()
    {
       return (pthread_create(&_thread, NULL, InternalThreadEntryFunc, this) == 0);
    }

    /** Will not return until the internal thread has exited. If exists, waits until thread has completed */
    void WaitForInternalThreadToExit()
    {
       (void) pthread_join(_thread, NULL);
    }


protected:
    /** Active tsi data collection method */
    virtual void InternalThreadEntry(){
        while(running){
            // go through all collected cooling data
            cout << "TSV Data Collected" << endl;
            sleep(TSV_rate);                            //wait to achieve sampling rate
        }
    }


private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((TSV_Thread *)This)->InternalThreadEntry(); return NULL;}

    pthread_t _thread;
};

#endif // TSV_THREAD_H
