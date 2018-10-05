#include "glv_thread.h"

GLV_Thread::GLV_Thread(DataMonitor * mtr, vector<meta> GLVSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface();
    GLVSensorMeta = GLVSensors;
    init_GLV_data();
}

GLV_Thread::~GLV_Thread(){}

void GLV_Thread::start(){
    timer->start(GLV_rate);
}

void GLV_Thread::stop(){
    timer->stop();
}

vector<int> GLV_Thread::get_GLV_Data(){
    return GLVData;
}

void GLV_Thread::init_GLV_data(){
    for(int i = 0; i < (int)GLVSensorMeta.size(); i++){
        GLVData.push_back(0);
    }
}

void GLV_Thread::GLVCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)GLVSensorMeta.size(); i++){
        datapoint data = canInterface->getdatapoint(GLVSensorMeta.at(i).sensorIndex);
        GLVData.at(i) = data.value;
    }
    cout << "GLV Data Collected" << endl;
}

void GLV_Thread::StartInternalThread(){

    pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
 }

void GLV_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}
