#include "tsv_thread.h"
TSV_Thread::TSV_Thread(DataMonitor * mtr, vector<meta> TSVSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface(TSVSensors);
    TSVSensorMeta = TSVSensors;
    init_TSV_data();
}
// class object constructor
TSV_Thread::~TSV_Thread(){

}

void TSV_Thread::init_TSV_data(){
    for(int i = 0; i < (int)TSVSensorMeta.size(); i++){
        TSVData.push_back(0);
    }
}

void TSV_Thread::start(){
    timer->start(TSV_rate);
}

void TSV_Thread::stop(){
    timer->stop();
}

vector<int> TSV_Thread::get_TSV_Data(){
    return TSVData;
}

void TSV_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

void TSV_Thread::TSVCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)TSVSensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint(TSVSensorMeta.at(i).sensorIndex);
        TSVData.at(i) = data.value;
    }
}

void TSV_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
}
