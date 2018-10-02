#ifndef COOL_THREAD_H
#define COOL_THREAD_H

#include <QThread>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "typedefs.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class COOL_Thread{

public:
    DataMonitor * monitor;                          // pointer to a datamonitor object

    COOL_Thread(DataMonitor * mtr){ monitor = mtr;} // class object constructor
    virtual ~COOL_Thread(){}                        // class object destructor

    bool running = true;                             // to control running of collection thread
    int COOL_rate = 0;                               // sampling rate of cooling system
    std::vector<meta> COOLSensorMeta;                // cooling sensor metadata


    /** Reads Specified Cooling Sensor Data*/
    datapoint * read_CAN(meta sensor)
    {
       // should call a method from IO control to read cooling sensor data
    }

    /** Returns true if the thread was successfully started, false if there was an error starting the thread */
    bool StartInternalThread()
    {
       return (pthread_create(&_thread, NULL, InternalCOOLThreadEntryFunc, this) == 0);
    }

    /** Will not return until the internal thread has exited. If exists, waits until thread has completed */
    void WaitForInternalThreadToExit()
    {
       (void) pthread_join(_thread, NULL);
    }


protected:
    /** Active cooling data collection method */
    virtual void InternalThreadEntry(){
        while(running){
            // go through all collected cooling data
            cout << "COOLING Data Collected" << endl;
            sleep(COOL_rate);                           //wait to achieve sampling rate
        }
    }

private:
    /** Links the member function to ordinary space */
    static void * InternalCOOLThreadEntryFunc(void * This) {((COOL_Thread *)This)->InternalThreadEntry(); return NULL;}

    pthread_t _thread;
};

#endif // COOL_THREAD_H
