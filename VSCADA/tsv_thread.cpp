#include "tsv_thread.h"

/**
 * @brief TSV_Thread::TSV_Thread - class constructor
 * @param mtr - data monitor object
 * @param TSVSensors - vector of tsv sensors as configured
 */
TSV_Thread::TSV_Thread(DataMonitor * mtr, vector<meta> TSVSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface(TSVSensors);
    TSVSensorMeta = TSVSensors;
    init_TSV_data();
}

/**
 * @brief TSV_Thread::~TSV_Thread - class destructor
 */
TSV_Thread::~TSV_Thread(){

}

/**
 * @brief TSV_Thread::init_TSV_data - initializes tsv data to 0 on startup
 */
void TSV_Thread::init_TSV_data(){
    for(int i = 0; i < (int)TSVSensorMeta.size(); i++){
        TSVData.push_back(0);
    }
}

/**
 * @brief TSV_Thread::start - starts data collection
 */
void TSV_Thread::start(){
    timer->start(TSV_rate);
}

/**
 * @brief TSV_Thread::stop - stops data collection
 */
void TSV_Thread::stop(){
    timer->stop();
}

/**
 * @brief TSV_Thread::get_TSV_Data - retrieves TSV data
 * @return
 */
vector<int> TSV_Thread::get_TSV_Data(){
    return TSVData;
}

/**
 * @brief TSV_Thread::WaitForInternalThreadToExit - waits until thread is destroyed
 */
void TSV_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

/**
 * @brief TSV_Thread::TSVCollectionTasks - runs data collection tasks
 */
void TSV_Thread::TSVCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)TSVSensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint(TSVSensorMeta.at(i).sensorIndex);
        TSVData.at(i) = data.value;
    }
}

/**
 * @brief TSV_Thread::StartInternalThread - launches thread
 */
void TSV_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
}
