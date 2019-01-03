﻿#include "subsystemthread.h"

/**
 * @brief SubsystemThread::SubsystemThread - class constructor
 * @param mtr - data monitor module
 * @param sensors - vector of subsystem sensors configured
 */
SubsystemThread::SubsystemThread(vector<meta *> sensors, string id, vector<response> respVector, vector<logic *> lVector, vector<meta *> mainMeta, vector<canItem> broadCast, bootloader boot){
    msgQueue = new QQueue<string>;
    timer = new QTimer;
    error=false;
    sensorMeta = sensors;
    subsystemId = id;
    responseVector = respVector;
    logicVector = lVector;
    mainSensorVector = mainMeta;
    broadCastVector = broadCast;
    bootCmds = boot;
    connect(timer, SIGNAL(timeout()), this, SLOT(subsystemCollectionTasks()));
}

/**
 * @brief SubsystemThread::~SubsystemThread - class destructor
 */
SubsystemThread::~SubsystemThread(){

}

void SubsystemThread::setSystemTimer(QTime *timer){
    systemTimer = timer;
}

string SubsystemThread::getProgramTime(){
    int timeElapsed = systemTimer->elapsed();
    double time = static_cast<double>(timeElapsed)/1000;
    ostringstream streamObj;
    streamObj << std::fixed;
    streamObj << std::setprecision(3);
    streamObj << time;
    return streamObj.str();
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

    rows.push_back(getProgramTime());
    rows.push_back(currSensor->sensorName);
    rows.push_back(currSensor->sensorName);
    rows.push_back(to_string(currSensor->val));
    dbase->insert_row(rawTable,cols,rows);

    rows.clear();
    rows.push_back(getProgramTime());
    rows.push_back(to_string(currSensor->sensorIndex));
    rows.push_back(currSensor->sensorName);
    rows.push_back(to_string(currSensor->calVal));
    dbase->insert_row(calTable,cols,rows);
    return;
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
void SubsystemThread::bootSubsystem(){
    timer->stop();
    for (uint i = 0; i < bootCmds.bootCanCmds.size(); i++){
        emit sendCANData(bootCmds.bootCanCmds.at(i).address,bootCmds.bootCanCmds.at(i).data,bootCmds.bootCanCmds.at(i).dataSize);
    }
    for (uint i = 0; i < bootCmds.bootI2cCmds.size(); i++){
        emit pushI2cData(bootCmds.bootI2cCmds.at(i));
    }
    for (uint i = 0; i < bootCmds.bootGPIOCmds.size(); i++){
        emit pushGPIOData(bootCmds.bootGPIOCmds.at(i).pin, bootCmds.bootGPIOCmds.at(i).value);
    }
    timer->start();
}

/**
 * @brief SubsystemThread::setMonitor - sets monitor object for class
 * @param mtr
 */
void SubsystemThread::setMonitor(DataMonitor * mtr){
    monitor = mtr;
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
    error = false;
    string msg;
    if (sensor->calVal >= sensor->maximum){
        if (sensor->state != 1){
            sensor->state = 1;
            emit updateEditColor("red",sensor);
            error=true;
            msg = getProgramTime() + ": " + sensor->sensorName + " exceeded upper threshold: " + to_string(sensor->maximum);
            emit pushErrMsg(msg);
            emit initiateRxn(sensor->maxRxnCode);
            logMsg(msg);
        }
    } else if (sensor->calVal <= sensor->minimum){
        if (sensor->state != -1){
            sensor->state = -1;
            emit updateEditColor("blue",sensor);
            error=true;
            msg = getProgramTime() + ": " + sensor->sensorName + " below lower threshold: " + to_string(sensor->minimum);
            emit pushErrMsg(msg);
            emit initiateRxn(sensor->minRxnCode);
            logMsg(msg);
        }
    } else {
        if (sensor->state != 0){
            sensor->state = 0;
            emit updateEditColor("yellow",sensor);
            emit initiateRxn(sensor->normRxnCode);
        }
    }
}

/**
 * @brief SubsystemThread::calibrateData : calibrates sensor data
 * @param currSensor
 */
void SubsystemThread::calibrateData(meta * currSensor){
    currSensor->calData();
    double data = 0;
    vector<poly> pol = currSensor->calPolynomial;
    if (pol.size() > 0){
        for (uint i = 0; i < pol.size(); i++){
            data += pol.at(i).coefficient*pow(currSensor->val,pol.at(i).exponent);
        }
        currSensor->calVal = data;
    }
}

/**
 * @brief SubsystemThread::receiveData : receive sensor data
 * @param currSensor
 */
void SubsystemThread::receiveData(meta * currSensor){
    for (uint i = 0; i < sensorMeta.size(); i++){
        if (sensorMeta.at(i) == currSensor){
            calibrateData(currSensor);
            checkThresholds(currSensor);
            emit updateDisplay(currSensor);
            checkLogic(currSensor);
            logData(currSensor);
            emit valueChanged(currSensor);
        }
    }
}

/**
 * @brief SubsystemThread::checkLogic : check configured logic
 * @param currSensor
 */
void SubsystemThread::checkLogic(meta * currSensor){
    meta * otherSensor;
    for(uint i = 0; i < logicVector.size(); i++){
        logic * currLogic = logicVector.at(i);
        if (currSensor->sensorIndex == currLogic->sensorId1){
            for (int j = 0; j < sensorMeta.size(); j++){
                if (sensorMeta.at(j)->sensorIndex == currLogic->sensorId2){
                    otherSensor = sensorMeta.at(j);
                }
            }

            if (abs(currSensor->calVal - currLogic->val1) < 0.01 && abs(otherSensor->calVal - currLogic->val2) < 0.01){
                    emit initiateRxn(currLogic->rsp);
                    currLogic->active = true;
                return;
            }
            currLogic->active = false;
        } else if (currSensor->sensorIndex == currLogic->sensorId2){
            for (int j = 0; j < sensorMeta.size(); j++){
                if (sensorMeta.at(j)->sensorIndex == currLogic->sensorId1){
                    otherSensor = sensorMeta.at(j);
                }
            }
            if (abs(currSensor->calVal - currLogic->val2) < 0.01 && abs(otherSensor->calVal - currLogic->val1) < 0.01){
                emit initiateRxn(currLogic->rsp);
                currLogic->active = true;
                return;
            }
            currLogic->active = false;
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
    rows.push_back(getProgramTime());
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
 * @brief SubsystemThread::subsystemCollectionTasks - performs tasks for data collection
 */
void SubsystemThread::subsystemCollectionTasks(){
    for (uint i = 0; i < broadCastVector.size(); i++){
        emit sendCANData(broadCastVector.at(i).address,broadCastVector.at(i).data,broadCastVector.at(i).dataSize);
    }
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
