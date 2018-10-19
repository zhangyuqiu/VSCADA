#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(vector<meta *> sensors, string id, vector<response> respVector){
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

/**
 * @brief SubsystemThread::setDB - sets database object for class
 * @param db
 */
void SubsystemThread::setDB(DB_Engine * db){
    dbase = db;
}

/**
 * @brief SubsystemThread::logData - records specified sensor data in the respective database
 * @param currSensor
 */
void SubsystemThread::logData(meta * currSensor){
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("sensorIndex");
    cols.push_back("sensorName");
    cols.push_back("value");
    vector<string> rows;
    string rawTable = subsystemId + "_rawdata";
    string calTable = subsystemId + "_caldata";
    cout << "Debug dbase write" << endl;
    for (uint i = 0; i < sensorMeta.size(); i++){
        if (sensorMeta.at(i) == currSensor){
            rows.push_back(get_curr_time());
            rows.push_back(currSensor->sensorName);
            rows.push_back(currSensor->sensorName);
            rows.push_back(to_string(currSensor->val));
            dbase->insert_row(rawTable,cols,rows);

            rows.clear();
            rows.push_back(get_curr_time());
            rows.push_back(to_string(currSensor->sensorIndex));
            rows.push_back(currSensor->sensorName);
            rows.push_back(to_string(currSensor->calVal));
            dbase->insert_row(calTable,cols,rows);
            return;
        }
    }
    cout << "Sensor Not Found. System Error" << endl;
}

/**
 * @brief SubsystemThread::get_metadata -
 * @return
 */
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

/**
 * @brief SubsystemThread::setMonitor - sets monitor object for class
 * @param mtr
 */
void SubsystemThread::setMonitor(DataMonitor * mtr){
    monitor = mtr;
}

/**
 * @brief SubsystemThread::updateEdits - updates text edit fields
 */
void SubsystemThread::updateEdits(meta * sensor){
    for(uint i = 0; i < edits.size(); i++){
        if(sensorMeta.at(i) == sensor){
            double num = sensor->calVal;
            ostringstream streamObj;
            streamObj << fixed;
            streamObj << setprecision(2);
            streamObj << num;
            string val = streamObj.str();
            editTimers.at(i)->start(sensor->checkRate);
            edits.at(i)->setText(QString::fromStdString(val));
        }
    }
}

/**
 * @brief SubsystemThread::checkTimeout - checks whether any lineEdit hasn't received updates
 */
void SubsystemThread::checkTimeout(){
    for(uint i = 0; i < edits.size(); i++){
        if (!editTimers.at(i)->isActive()) edits.at(i)->setStyleSheet("color: #A9A9A9");
    }
}

/**
 * @brief SubsystemThread::checkThresholds - check whether specified sensor data exceeds configured thresholds
 * @param sensor
 */
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
        logMsg(msg);

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
        logMsg(msg);
    } else {
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FFFF00");
                break;
            }
        }
        initiateRxn(sensor->normRxnCode);
    }
}

void SubsystemThread::calibrateData(meta * currSensor){
    currSensor->calData();
}

void SubsystemThread::receiveData(meta * currSensor){
    calibrateData(currSensor);
    checkThresholds(currSensor);
    updateEdits(currSensor);
    logData(currSensor);
}

/**
 * @brief SubsystemThread::enqueueMsg - logs message in the database
 * @param msg
 */
void SubsystemThread::logMsg(string msg){
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("reactionId");
    cols.push_back("message");
    rows.push_back(get_curr_time());
    rows.push_back("console");
    rows.push_back(msg);
    dbase->insert_row("system_log",cols,rows);
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

/**
 * @brief SubsystemThread::initiateRxn - execute specified reaction as configured
 * @param rxnCode
 * @return
 */
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
            logMsg(rsp.msg);
            emit pushMessage(rsp.msg);
            if (rsp.canAddress >= 0){
                cout << "sending out can data" << endl;
                emit pushCANItem(rsp);
            }
            if (rsp.gpioPin >= 0){
                emit pushGPIOData(rsp);
            }
        }
    }
    dbase->insert_row("system_log",cols,rows);
    return 0;
}

/**
 * @brief SubsystemThread::get_curr_time - retrieves current operation system time
 * @return
 */
string SubsystemThread::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D_%T",&now);
    return buf;
}
