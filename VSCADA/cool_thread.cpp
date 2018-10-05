#include "cool_thread.h"

COOL_Thread::COOL_Thread(DataMonitor * mtr, vector<meta> COOLSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface();
    COOLSensorMeta = COOLSensors;
    init_COOL_data();
}

COOL_Thread::~COOL_Thread(){

}

void COOL_Thread::init_COOL_data(){
    for(int i = 0; i < (int)COOLSensorMeta.size(); i++){
        COOLData.push_back(0);
    }
}

void COOL_Thread::start(){
    timer->start(COOL_rate);
}

void COOL_Thread::stop(){
    timer->stop();
}

void COOL_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}

void COOL_Thread::COOLINGCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)COOLSensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint(COOLSensorMeta.at(i).sensorIndex);
        COOLData.at(i) = data.value;
    }
    cout << "COOLING Data Collected" << endl;
}

void COOL_Thread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalCOOLThreadEntryFunc, this);
}
