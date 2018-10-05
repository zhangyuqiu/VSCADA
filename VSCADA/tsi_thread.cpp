#include "tsi_thread.h"

TSI_Thread::TSI_Thread(DataMonitor * mtr, vector<meta> TSISensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface();
    TSISensorMeta = TSISensors;
    init_TSI_data();
}

TSI_Thread::~TSI_Thread(){

}

void TSI_Thread::init_TSI_data(){
    for(int i = 0; i < (int)TSISensorMeta.size(); i++){
        TSIData.push_back(0);
    }
}

void TSI_Thread::start(){
    timer->start(TSI_rate);
}

void TSI_Thread::stop(){
    timer->stop();
}

vector<int> TSI_Thread::get_TSI_Data(){
    return TSIData;
}

/**
 * Will not return until the internal thread has exited. If exists, waits until thread has completed
 */
void TSI_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

/**
 * Active cooling data collection method
 */
void TSI_Thread::TSICollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)TSISensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint(TSISensorMeta.at(i).sensorIndex);
        TSIData.at(i) = data.value;
    }
    cout << "TSI Data Collected" << endl;
}

/**
 * Returns true if the thread was successfully started, false if there was an error starting the thread
 */
void TSI_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalTSIThreadEntryFunc, this);
}
