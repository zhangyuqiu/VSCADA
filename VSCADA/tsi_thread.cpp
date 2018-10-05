#include "tsi_thread.h"

/**
 * @brief TSI_Thread::TSI_Thread - class constructor
 * @param mtr - data monitor object
 * @param TSISensors - vector of TSI sensors as configured
 */
TSI_Thread::TSI_Thread(DataMonitor * mtr, vector<meta> TSISensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface(TSISensors);
    TSISensorMeta = TSISensors;
    init_TSI_data();
}

/**
 * @brief TSI_Thread::~TSI_Thread - class destructor
 */
TSI_Thread::~TSI_Thread(){

}

/**
 * @brief TSI_Thread::init_TSI_data - initializes tsi data to 0 on startup
 */
void TSI_Thread::init_TSI_data(){
    for(int i = 0; i < (int)TSISensorMeta.size(); i++){
        TSIData.push_back(0);
    }
}

/**
 * @brief TSI_Thread::start - starts data collection
 */
void TSI_Thread::start(){
    timer->start(TSI_rate);
}

/**
 * @brief TSI_Thread::stop - stops data collection
 */
void TSI_Thread::stop(){
    timer->stop();
}

/**
 * @brief TSI_Thread::get_TSI_Data - retrieves tsi data
 * @return
 */
vector<int> TSI_Thread::get_TSI_Data(){
    return TSIData;
}

/**
 * @brief TSI_Thread::WaitForInternalThreadToExit - waits until thread is destroyed
 */
void TSI_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

/**
 * @brief TSI_Thread::TSICollectionTasks - runs data collection tasks
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
 * @brief TSI_Thread::StartInternalThread - launches thread
 */
void TSI_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalTSIThreadEntryFunc, this);
}
