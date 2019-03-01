#include "subsystemthread.h"

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
    string colString = "time,sensorIndex,sensorName,value";
    string rowString = "'" + getProgramTime() + "','" + currSensor->sensorName + "','" + currSensor->sensorName + "','" + to_string(currSensor->val) + "'";
    string rawTable = subsystemId + "_rawdata";
    string calTable = subsystemId + "_caldata";
    dbase->insert_row(rawTable,colString,rowString);
    rowString = "'" + getProgramTime() + "','" + currSensor->sensorName + "','" + currSensor->sensorName + "','" + to_string(currSensor->calVal) + "'";
    dbase->insert_row(calTable,colString,rowString);
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
    string bootMsg = subsystemId + " boot sequence completed";
    logMsg(bootMsg);
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
    string msg;
    if (sensor->calVal >= sensor->maximum){
        if (sensor->state != 1){
            sensor->state = 1;
            emit updateEditColor("red",sensor);
            if (!error){
                error = true;
                emit updateHealth();
            }
            msg = sensor->sensorName + " exceeded upper threshold: " + to_string(sensor->maximum);
            emit pushMessage(msg);
            emit initiateRxn(sensor->maxRxnCode);
            logMsg(msg);
        }
    } else if (sensor->calVal <= sensor->minimum){
        if (sensor->state != -1){
            sensor->state = -1;
            emit updateEditColor("blue",sensor);
            if (!error){
                error = true;
                emit updateHealth();
            }
            msg = sensor->sensorName + " below lower threshold: " + to_string(sensor->minimum);
            emit pushMessage(msg);
            emit initiateRxn(sensor->minRxnCode);
            logMsg(msg);
        }
    } else {
        if (sensor->state != 0){
            sensor->state = 0;
            emit updateEditColor("yellow",sensor);
            emit initiateRxn(sensor->normRxnCode);
            if (error) {
                checkError();
                emit updateHealth();
            }
        }
    }
}

void SubsystemThread::checkError(){
    for (uint i = 0; i < sensorMeta.size(); i++){
        if (sensorMeta.at(i)->state != 0) return;
    }
    error = false;
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
    if (currSensor->senOperator > -1){
        switch(currSensor->senOperator){
        case SUM:   {
                        currSensor->val = 0;
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            currSensor->val += sen->val;
                        }
                        currSensor->calVal = 0;
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            currSensor->calVal += sen->calVal;
                        }
                    }
            break;
        case DIFF:  {
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) currSensor->val = sen->val;
                            else currSensor->val = abs(currSensor->val - sen->val);
                        }
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) currSensor->calVal = sen->calVal;
                            else currSensor->calVal = abs(currSensor->calVal - sen->calVal);
                        }
                    }
            break;
        case MUL:   {
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                          meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                          if (i == 0) currSensor->val = sen->val;
                          else currSensor->val = currSensor->val*sen->val;
                        }
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) {
                                currSensor->calVal = sen->calVal;
                            } else {
                                currSensor->calVal = currSensor->calVal*sen->calVal;
                            }
                        }
                    }
            break;
        case DIV:  {
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) currSensor->val = sen->val;
                            else currSensor->val = currSensor->val/sen->val;
                        }
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) currSensor->calVal = sen->calVal;
                            else currSensor->calVal = currSensor->calVal/sen->calVal;
                        }
                    }
            break;
        case AVG:   {
                        currSensor->val = 0;
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            currSensor->val += sen->val;
                        }
                        currSensor->val = currSensor->val/currSensor->opSensors.size();
                        currSensor->calVal = 0;
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            currSensor->calVal += sen->calVal;
                        }
                        currSensor->calVal = currSensor->calVal/currSensor->opSensors.size();
                    }
            break;
        case MAX:   {
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) {
                                currSensor->calVal = sen->calVal;
                                currSensor->val = sen->val;
                            } else if (sen->calVal > currSensor->calVal) {
                                currSensor->calVal = sen->calVal;
                                currSensor->val = sen->val;
                            }
                        }
        }
            break;
        case MIN:   {
                        for (uint i = 0; i < currSensor->opSensors.size(); i++){
                            meta * sen = static_cast<meta*>(currSensor->opSensors.at(i));
                            if (i == 0) {
                                currSensor->calVal = sen->calVal;
                                currSensor->val = sen->val;
                            } else if (sen->calVal < currSensor->calVal) {
                                currSensor->calVal = sen->calVal;
                                currSensor->val = sen->val;
                            }
                        }
                    }
            break;
        }
    }
}

/**
 * @brief SubsystemThread::receiveData : receive sensor data
 * @param currSensor
 */
void SubsystemThread::receiveData(meta * currSensor){
    cout << "calibrating data: " << currSensor->sensorName << ", " << currSensor->primAddress << endl;
    fflush(stdout);
    calibrateData(currSensor);
    cout << "checking thresholds: " << currSensor->sensorName << ", " << currSensor->primAddress << endl;
    fflush(stdout);
    checkThresholds(currSensor);
    cout << "updating display: " << currSensor->sensorName << ", " << currSensor->primAddress << endl;
    fflush(stdout);
    emit updateDisplay(currSensor);
    cout << "logging data: " << currSensor->sensorName << ", " << currSensor->primAddress << endl;
    fflush(stdout);
    logData(currSensor);
    cout << "done processing sensor data: " << currSensor->sensorName << ", " << currSensor->primAddress << endl;
    fflush(stdout);
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
            for (uint j = 0; j < sensorMeta.size(); j++){
                if (sensorMeta.at(j)->sensorIndex == currLogic->sensorId2){
                    otherSensor = sensorMeta.at(j);
                    if (abs(currSensor->calVal - currLogic->val1) < 0.01 && abs(otherSensor->calVal - currLogic->val2) < 0.01){
                            emit initiateRxn(currLogic->rsp);
                            currLogic->active = true;
                        return;
                    }
                }
            }
            currLogic->active = false;
        } else if (currSensor->sensorIndex == currLogic->sensorId2){
            for (uint j = 0; j < sensorMeta.size(); j++){
                if (sensorMeta.at(j)->sensorIndex == currLogic->sensorId1){
                    otherSensor = sensorMeta.at(j);
                    if (abs(currSensor->calVal - currLogic->val2) < 0.01 && abs(otherSensor->calVal - currLogic->val1) < 0.01){
                        emit initiateRxn(currLogic->rsp);
                        currLogic->active = true;
                        return;
                    }
                }
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
    string colString = "time,reactionId,message";
    string rowString = "'" + getProgramTime() + "','console','" + msg + "'";
    dbase->insert_row("system_log",colString,rowString);
    pushMessage(msg);
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
