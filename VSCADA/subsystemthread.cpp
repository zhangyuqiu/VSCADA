#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(DataMonitor * mtr, canbus_interface * can, vector<meta> sensors, string id){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = can;
    sensorMeta = sensors;
    subsystemId = id;
    init_data();
    for(int i = 0; i < sensors.size(); i++){
        QLineEdit * lineEdit = new QLineEdit;
        edits.push_back(lineEdit);
    }
    updateEdits();
}

/**
 * @brief SubsystemThread::~SubsystemThread - class destructor
 */
SubsystemThread::~SubsystemThread(){

}

vector<meta> SubsystemThread::get_metadata(){
    return sensorMeta;
}

/**
 * @brief SubsystemThread::set_rate - changes the current rate at which data is checked
 * @param newRate
 */
void SubsystemThread::set_rate(int newRate){
    checkRate = newRate;
}

/**
 * @brief SubsystemThread::init_data - initializes vector of data to 0
 */
void SubsystemThread::init_data(){
    for(int i = 0; i < (int)sensorMeta.size(); i++){
        rawData.push_back(0);
    }
}

/**
 * @brief SubsystemThread::updateEdits - updates text edit fields
 */
void SubsystemThread::updateEdits(){
    for(int i = 0; i < edits.size(); i++){
        int num = rawData.at(i);
        string val = to_string(num);
        edits.at(i)->setText(QString::fromStdString(val));
        rawData.at(i) = num+1;
    }
}

/**
 * @brief SubsystemThread::start - starts subsystem data collection thread
 */
void SubsystemThread::start(){
    timer->start(checkRate);
}

/**
 * @brief SubsystemThread::stop  - stops subsystem data collection thread
 */
void SubsystemThread::stop(){
    timer->stop();
}

/**
 * @brief SubsystemThread::get_data - retrieves subsystem raw data
 */
vector<int> SubsystemThread::get_data(){
    return rawData;
}

/**
 * @brief SubsystemThread::WaitForInternalThreadToExit - waits until thread finishes executing
 */
void SubsystemThread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

/**
 * @brief SubsystemThread::subsystemCollectionTasks - performs tasks for data collection
 */
void SubsystemThread::subsystemCollectionTasks(){
    testVal++;
//    for (int i = 0; i < (int)sensorMeta.size(); i++){
//        datapoint data = canInterface->getdatapoint_canadd(sensorMeta.at(i).canAddress);
//        rawData.at(i) = data.value;
//    }
    for(int i = 0; i < rawData.size(); i++){
        int num = rawData.at(i);
        rawData.at(i) = num+1;
//        cout << "Number adding: " << rawData.at(i) << endl;
    }
    cout << subsystemId << " Data Collected" << endl;
}

/**
 * @brief SubsystemThread::StartInternalThread - launches thread
 */
void SubsystemThread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
}
