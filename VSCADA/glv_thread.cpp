#include "glv_thread.h"

/**
 * @brief GLV_Thread::GLV_Thread - class constructor
 * @param mtr - data monitor object
 * @param GLVSensors - vector of GLV sensors configured
 */
GLV_Thread::GLV_Thread(DataMonitor * mtr, vector<meta> GLVSensors){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    monitor = mtr;
    canInterface = new canbus_interface(GLVSensors);
    GLVSensorMeta = GLVSensors;
    init_GLV_data();
}

/**
 * @brief GLV_Thread::~GLV_Thread - class destructor
 */
GLV_Thread::~GLV_Thread(){

}

/**
 * @brief GLV_Thread::start - starts data collection
 */
void GLV_Thread::start(){
    timer->start(GLV_rate);
}

/**
 * @brief GLV_Thread::stop - stops data collection
 */
void GLV_Thread::stop(){
    timer->stop();
}

/**
 * @brief GLV_Thread::get_GLV_Data - retrieves GLV data
 * @return
 */
vector<int> GLV_Thread::get_GLV_Data(){
    return GLVData;
}

/**
 * @brief GLV_Thread::init_GLV_data - initializes GLV data to 0 on startup
 */
void GLV_Thread::init_GLV_data(){
    for(int i = 0; i < (int)GLVSensorMeta.size(); i++){
        GLVData.push_back(0);
    }
}

/**
 * @brief GLV_Thread::GLVCollectionTasks - performs data collection tasks
 */
void GLV_Thread::GLVCollectionTasks(){
    testVal++;
    for (int i = 0; i < (int)GLVSensorMeta.size(); i++){
//        datapoint data = canInterface->getdatapoint(GLVSensorMeta.at(i).sensorIndex);
//        GLVData.at(i) = data.value;
        GLVData.at(i)++;
    }
    cout << "GLV Data Collected" << endl;
}

/**
 * @brief GLV_Thread::StartInternalThread - launches thread
 */
void GLV_Thread::StartInternalThread(){

    pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
 }

/**
 * @brief GLV_Thread::WaitForInternalThreadToExit - waits until thread is destroyed
 */
void GLV_Thread::WaitForInternalThreadToExit()
{
   (void) pthread_join(_thread, NULL);
}
