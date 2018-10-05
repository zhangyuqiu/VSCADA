#include "cool_thread.h"

/**
 * @brief COOL_Thread::COOL_Thread - class constructor
 * @param mtr - data monitor module
 * @param COOLSensors - vector of COOLING systems sensors configured
 */
COOL_Thread::COOL_Thread(DataMonitor * mtr, vector<meta> COOLSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface(COOLSensors);
    COOLSensorMeta = COOLSensors;
    init_COOL_data();
}

/**
 * @brief COOL_Thread::~COOL_Thread - class destructor
 */
COOL_Thread::~COOL_Thread(){

}

/**
 * @brief COOL_Thread::init_COOL_data - initializes vector of data to 0
 */
void COOL_Thread::init_COOL_data(){
    for(int i = 0; i < (int)COOLSensorMeta.size(); i++){
        COOLData.push_back(0);
    }
}

/**
 * @brief COOL_Thread::start - starts cooling system data collection thread
 */
void COOL_Thread::start(){
    timer->start(COOL_rate);
}

/**
 * @brief COOL_Thread::stop  - stops cooling system data collection thread
 */
void COOL_Thread::stop(){
    timer->stop();
}

/**
 * @brief COOL_Thread::init_COOL_data - retrieves COOLING system data
 */
vector<int> COOL_Thread::get_COOL_data(){
    return COOLData;
}

/**
 * @brief COOL_Thread::WaitForInternalThreadToExit - waits until thread finishes executing
 */
void COOL_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

/**
 * @brief COOL_Thread::COOLINGCollectionTasks - performs tasks for data collection
 */
void COOL_Thread::COOLINGCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)COOLSensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint_canadd(COOLSensorMeta.at(i).canAddress);
        COOLData.at(i) = data.value;
    }
    cout << "COOLING Data Collected" << endl;
}

/**
 * @brief COOL_Thread::StartInternalThread - launches thread
 */
void COOL_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalCOOLThreadEntryFunc, this);
}
