#ifndef GLV_THREAD_H
#define GLV_THREAD_H
#include <QThread>
#include <QObject>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "typedefs.h"
#include "datamonitor.h"
class DataMonitor;

using namespace std;

class GLV_Thread
{

public:
    DataMonitor * monitor;                          // pointer to datamonitor object

    GLV_Thread(DataMonitor * mtr){ monitor = mtr;}  // object class constructor
    virtual ~GLV_Thread(){}                         // object class destructor

    bool running = true;                            // to control running of collection thread
    int GLV_rate = 0;                               // sampling rate of GLV sensors
    std::vector<meta> GLVSensorMeta;                // GLV sensor metadata

    /** Reads Specified Cooling Sensor Data*/
    datapoint * read_CAN(meta sensor){
        // should call a method from IO control to read GLV sensor data
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
   /** Active cooling data collection method */
   virtual void InternalThreadEntry(){
       while(running){
           cout << "GLV Data Collected" << endl;
           sleep(GLV_rate);
       }
   }


private:
   /** Links the member function to ordinary space */
   static void * InternalThreadEntryFunc(void * This) {((GLV_Thread *)This)->InternalThreadEntry(); return NULL;}

   pthread_t _thread;
};

#endif // GLV_THREAD_H
