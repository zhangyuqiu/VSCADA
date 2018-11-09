﻿#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(vector<meta *> sensors, string id, vector<response> respVector, vector<logic> lVector, vector<meta *> mainMeta){
    msgQueue = new QQueue<string>;
    timer = new QTimer;
    error=false;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    sensorMeta = sensors;
    subsystemId = id;
    responseVector = respVector;
    logicVector = lVector;
    mainSensorVector = mainMeta;
    init_data();
    for(uint i = 0; i < sensors.size(); i++){
        lineEdit = new QLineEdit;
        checkTmr = new QTimer;
        connect(checkTmr, SIGNAL(timeout()), checkTmr, SLOT(stop()));
        connect(checkTmr, SIGNAL(timeout()), this, SLOT(checkTimeout()));
        checkTmr->start(sensors.at(i)->checkRate);
        edits.push_back(lineEdit);
        lineEdit->setStyleSheet("font: 20pt; color: #FFFF00");
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->setDisabled(1);
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
    cout << "Subsystem ID: " << subsystemId << endl;
    cout << "Size of sensor vector: " << sensorMeta.size() << endl;
    for (uint i = 0; i < sensorMeta.size(); i++){
        cout << "Checking sensor: " << sensorMeta.at(i)->sensorName << endl;
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
            cout << "Returning" << endl;
            return;
        }
    }
    cout << "Sensor Not Found. System Error" << currSensor->sensorName << endl;
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
            string field = val + " " + sensor->unit;
            edits.at(i)->setText(QString::fromStdString(field));
        }
    }
}

/**
 * @brief SubsystemThread::checkTimeout - checks whether any lineEdit hasn't received updates
 */
void SubsystemThread::checkTimeout(){
    for(uint i = 0; i < edits.size(); i++){
        if (!editTimers.at(i)->isActive()) edits.at(i)->setText("--");
    }
}

/**
 * @brief SubsystemThread::get_mainMeta : retrieves main subsystem sensors
 * @return
 */
vector<meta *> SubsystemThread::get_mainMeta(){
    return mainSensorVector;
}

/**
 * @brief SubsystemThread::checkThresholds - check whether specified sensor data exceeds configured thresholds
 * @param sensor
 */
void SubsystemThread::checkThresholds(meta * sensor){
    cout << "Just checking" << endl;
    error = false;
    string msg;
    if (sensor->calVal > sensor->maximum){
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FF0000");
                break;
            }
        }
        error=true;
        msg = get_curr_time() + ": " + sensor->sensorName + " exceeded upper threshold: " + to_string(sensor->maximum);
        emit pushErrMsg(msg);
        initiateRxn(sensor->maxRxnCode);
        logMsg(msg);

    } else if (sensor->calVal < sensor->minimum){
        for(uint i = 0; i < sensorMeta.size(); i++){
            if (sensorMeta.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #1E90FF");
                break;
            }
        }
        error=true;
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
    cout << "done checking" << endl;
}

/**
 * @brief SubsystemThread::calibrateData : calibrates sensor data
 * @param currSensor
 */
void SubsystemThread::calibrateData(meta * currSensor){
    currSensor->calData();
}

/**
 * @brief SubsystemThread::receiveData : receive sensor data
 * @param currSensor
 */
void SubsystemThread::receiveData(meta * currSensor){
    for (uint i = 0; i < sensorMeta.size(); i++){
        if (sensorMeta.at(i) == currSensor){
            cout << "somewhere here" << endl;
            calibrateData(currSensor);
            checkThresholds(currSensor);
            updateEdits(currSensor);
            checkLogic(currSensor);
            logData(currSensor);
            emit valueChanged();
        }
    }
}

/**
 * @brief SubsystemThread::checkLogic : check configured logic
 * @param currSensor
 */
void SubsystemThread::checkLogic(meta * currSensor){
    for(uint i = 0; i < logicVector.size(); i++){
        logic currLogic = logicVector.at(i);
        if (currSensor == currLogic.sensor1){
            if ((currSensor->calVal - currLogic.val1) < 0.01 && (currLogic.sensor2->calVal - currLogic.val2) < 0.01){
                initiateRxn(currLogic.rsp.responseIndex);
            }
        } else if (currSensor == currLogic.sensor2){
            if ((currSensor->calVal - currLogic.val2) < 0.01 && (currLogic.sensor1->calVal - currLogic.val1) < 0.01){
                initiateRxn(currLogic.rsp.responseIndex);
            }
        }
    }
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
