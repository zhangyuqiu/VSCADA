#ifndef GLV_THREAD_H
#define GLV_THREAD_H
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

class GLV_Thread : public QObject
{

    Q_OBJECT
public:

    GLV_Thread(DataMonitor * mtr, canbus_interface * can, vector<meta> GLVSensors); //class constructor
    virtual ~GLV_Thread();                                  //class destructor

    void stop();                                            //stops data collection
    void start();                                           //starts data collection
    void init_GLV_data();                                   //initializes GLV data vector
    vector<int> get_GLV_Data();                             //retrieves GLV Data
    void WaitForInternalThreadToExit();                     //stops code until this thread is destroyed

    QTimer * timer;                                         //timer to control data collection frequency
    DataMonitor * monitor;                                  //pointer to datamonitor object
    canbus_interface * canInterface;                        //can interface to enable reading from CAN

    int testVal = 0;                                        //dummy data for testing

    int GLV_rate = 0;                                       //sampling rate of GLV sensors
    vector<int> GLVData;                                    //last GLV data sampled
    vector<meta> GLVSensorMeta;                             //GLV sensor metadata

protected:
   virtual void GLVCollectionTasks();                       //runs thread tasks


private:
   /** Links the member function to ordinary space */
   static void * InternalThreadEntryFunc(void * This) {((GLV_Thread *)This)->GLVCollectionTasks(); return NULL;}

   pthread_t _thread;

public slots:
   void StartInternalThread();                              //starts thread
};

#endif // GLV_THREAD_H
