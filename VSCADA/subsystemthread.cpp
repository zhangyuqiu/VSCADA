#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(vector<meta *> sensors, string id){
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    sensorMeta = sensors;
    subsystemId = id;
    init_data();
    for(int i = 0; i < static_cast<int>(sensors.size()); i++){
        lineEdit = new QLineEdit;
        tmr = new QTimer;
        connect(tmr, SIGNAL(timeout()), tmr, SLOT(stop()));
        connect(tmr, SIGNAL(timeout()), this, SLOT(checkTimeout()));
        tmr->start(sensors.at(i)->checkRate);
        edits.push_back(lineEdit);
        lineEdit->setStyleSheet("font: 20pt; color: #FFFF00");
        editTimers.push_back(tmr);
        updateEdits(sensors.at(i));
    }
}

/**
 * @brief SubsystemThread::~SubsystemThread - class destructor
 */
SubsystemThread::~SubsystemThread(){

}

void SubsystemThread::setDB(DB_Engine * db){
    dbase = db;
}

void SubsystemThread::logData(meta * currSensor){
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("sensorIndex");
    cols.push_back("sensorName");
    cols.push_back("value");
    vector<string> rows;
    string table = subsystemId + "_data";
    cout << "Debug dbase write" << endl;
    for (int i = 0; i < sensorMeta.size(); i++){
        if (sensorMeta.at(i) == currSensor){
            rows.push_back(get_curr_time());
            rows.push_back(currSensor->sensorName);
            rows.push_back(currSensor->sensorName);
            rows.push_back(to_string(currSensor->val));
            dbase->insert_row(table,cols,rows);
            return;
        }
        cout << "Debug dbase write " << i << endl;
    }
    cout << "Sensor Not Found. System Error" << endl;
}

vector<meta *> SubsystemThread::get_metadata(){
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
    for(int i = 0; i < static_cast<int>(sensorMeta.size()); i++){
        rawData.push_back(0);
    }
}

void SubsystemThread::setMonitor(DataMonitor * mtr){
    monitor = mtr;
}

/**
 * @brief SubsystemThread::updateEdits - updates text edit fields
 */
void SubsystemThread::updateEdits(meta * sensor){
    for(int i = 0; i < static_cast<int>(edits.size()); i++){
        if(sensorMeta.at(i) == sensor){
            int num = sensor->val;
            string val = to_string(num);
            editTimers.at(i)->start(sensor->checkRate);
            edits.at(i)->setText(QString::fromStdString(val));
        }
    }
}

void SubsystemThread::checkTimeout(){
    cout << "Edit timers size: " << edits.size() << " " << subsystemId << endl;
//    string example = "Sijui";
    for(int i = 0; i < static_cast<int>(edits.size()); i++){
        edits.at(i)->setStyleSheet("font: 20pt; color: #A9A9A9");
    }
}

void SubsystemThread::checkThresholds(meta * sensor){
    if (sensor->val > sensor->maximum){
        for(int i = 0; i < static_cast<int>(sensorMeta.size()); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("font: 20pt; color: #FF0000");
                break;
            }
        }
        monitor->initiateRxn(sensor->maximum,sensor);
    } else if (sensor->val < sensor->minimum){
        for(int i = 0; i < static_cast<int>(sensorMeta.size()); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("font: 20pt; color: #1E90FF");
                break;
            }
        }
        monitor->initiateRxn(sensor->minimum,sensor);
    } else {
        for(int i = 0; i < static_cast<int>(sensorMeta.size()); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("font: 20pt; color: #FFFF00");
                break;
            }
        }
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

}

/**
 * @brief SubsystemThread::StartInternalThread - launches thread
 */
void SubsystemThread::StartInternalThread()
{
   pthread_create(&_thread, NULL, InternalThreadEntryFunc, this);
}

string SubsystemThread::get_curr_time(){
    time_t t = time(0);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%X",&now);
    return buf;
}
