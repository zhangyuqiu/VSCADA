#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(vector<meta *> sensors, string id, vector<response> respVector){
    msgQueue = new QQueue<string>;
    respCANQueue = new QQueue<response>;
    respGPIOQueue = new QQueue<response>;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    sensorMeta = sensors;
    subsystemId = id;
    responseVector = respVector;
    init_data();
    for(uint i = 0; i < sensors.size(); i++){
        lineEdit = new QLineEdit;
        checkTmr = new QTimer;
        connect(checkTmr, SIGNAL(timeout()), checkTmr, SLOT(stop()));
        connect(checkTmr, SIGNAL(timeout()), this, SLOT(checkTimeout()));
        checkTmr->start(sensors.at(i)->checkRate);
        edits.push_back(lineEdit);
        lineEdit->setStyleSheet("font: 20pt; color: #FFFF00");
        editTimers.push_back(checkTmr);
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
    for (uint i = 0; i < sensorMeta.size(); i++){
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
    for(uint i = 0; i < edits.size(); i++){
        if(sensorMeta.at(i) == sensor){
            int num = sensor->val;
            string val = to_string(num);
            editTimers.at(i)->start(sensor->checkRate);
            edits.at(i)->setText(QString::fromStdString(val));
        }
    }
}

void SubsystemThread::checkTimeout(){
    for(uint i = 0; i < edits.size(); i++){
        if (!editTimers.at(i)->isActive()) edits.at(i)->setStyleSheet("font: 20pt; color: #A9A9A9");
    }
}

void SubsystemThread::checkThresholds(meta * sensor){
    string msg;
    if (sensor->val > sensor->maximum){
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FF0000");
                break;
            }
        }
        msg = get_curr_time() + ": " + sensor->sensorName + " exceeded upper threshold: " + to_string(sensor->maximum);
        emit pushErrMsg(msg);
        initiateRxn(sensor->maxRxnCode);
        enqueueMsg(msg);

    } else if (sensor->val < sensor->minimum){
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #1E90FF");
                break;
            }
        }
        msg = get_curr_time() + ": " + sensor->sensorName + " below lower threshold: " + to_string(sensor->maximum);
        emit pushErrMsg(msg);
        initiateRxn(sensor->minRxnCode);
        enqueueMsg(msg);
    } else {
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FFFF00");
                break;
            }
        }
    }
}

void SubsystemThread::enqueueMsg(string msg){
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("reactionId");
    cols.push_back("message");
    rows.push_back(get_curr_time());
    rows.push_back("console");
    rows.push_back(msg);
    dbase->insert_row("system_log",cols,rows);
    msgQueue->enqueue(msg);
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
   (void) pthread_join(_thread, nullptr);
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
   pthread_create(&_thread, nullptr, InternalThreadEntryFunc, this);
}

int SubsystemThread::initiateRxn(int rxnCode){
    //print to log
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("reactionId");
    cols.push_back("message");
    rows.push_back(get_curr_time());
    rows.push_back(to_string(rxnCode));

    for (uint i = 0; i < responseVector.size(); i++){
        response rsp = responseVector.at(i);
        if (rsp.responseIndex == rxnCode){
            rows.push_back(rsp.msg);
            enqueueMsg(rsp.msg);
            emit pushMessage(rsp.msg);
            if (rsp.canAddress >= 0){
                cout << "sending out can data" << endl;
                respCANQueue->enqueue(rsp);
                emit pushCANItem(rsp);
            }
            if (rsp.gpioPin >= 0){
                respGPIOQueue->enqueue(rsp);
                emit pushGPIOData(rsp);
            }
        }
    }
    dbase->insert_row("system_log",cols,rows);
    return 0;
}

string SubsystemThread::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%X",&now);
    return buf;
}
