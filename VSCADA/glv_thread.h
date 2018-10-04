#ifndef GLV_THREAD_H
#define GLV_THREAD_H
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

class GLV_Thread : public QObject
{

    Q_OBJECT
public:
    DataMonitor * monitor;                          // pointer to datamonitor object
    QTimer * timer;

    GLV_Thread(DataMonitor * mtr){
        timer = new QTimer;
        connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
        monitor = mtr;
    }  // object class constructor
    virtual ~GLV_Thread(){}                         // object class destructor

    bool running = true;                            // to control running of collection thread
    int GLV_rate = 0;                               // sampling rate of GLV sensors
    std::vector<meta> GLVSensorMeta;                // GLV sensor metadata

    /** Reads Specified Cooling Sensor Data*/
    datapoint * read_CAN(meta sensor){
        // should call a method from IO control to read GLV sensor data
    }

    void start(){
        timer->start(GLV_rate);
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
   /** Active cooling data collection method */
   virtual void InternalThreadEntry(){
//       while(running){
           cout << "GLV Data Collected" << endl;
//           sleep(GLV_rate);
//       }
   }


private:
   /** Links the member function to ordinary space */
   static void * InternalThreadEntryFunc(void * This) {((GLV_Thread *)This)->InternalThreadEntry(); return NULL;}

   pthread_t _thread;

public slots:
   /** Returns true if the thread was successfully started, false if there was an error starting the thread */
   void StartInternalThread();
};

#endif // GLV_THREAD_H
