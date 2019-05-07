#include "datacontrol.h"

/**
 * @brief DataControl::DataControl class control
 * @param gpio : GPIO interface module
 * @param can : CAN interface module
 * @param usb : USB7204 interface module
 * @param db : SQLITE database engine
 * @param threads : subsystem threads
 * @param stts : system statuses
 * @param FSM : system finite state machines
 * @param mode : system mode
 * @param ctrlSpecs : control specifications
 * @param sensors : system sensors
 * @param rsp : system responses
 */
DataControl::DataControl(gpio_interface * gpio, canbus_interface * can, usb7402_interface * usb, DB_Engine * db,
                         map<string, Group *> subMap, vector<system_state *> stts, vector<statemachine *> FSM,
                         int mode, vector<controlSpec *> ctrlSpecs, map<int, response> rspMap,
                         vector<bootloader> bootArgs, map<uint32_t, vector<meta *>*> canMap, bootloader boot,
                         vector<canItem> cSyncs, vector<i2cItem> iSyncs, vector<gpioItem> gSyncs,
                         map<int,recordwindow*>recSenWins, map<int, recordwindow *> recStateWins, map<int, meta*> sensMap){

    // set mode parameters
    systemMode = mode;
    if(mode == 1){
        modeName = "DYNO";
    } else if (mode == 0){
        modeName = "CAR";
    }

    // assign global objects
    dbase = db;
    FSMs = FSM;
    usb7204 = usb;
    states = stts;
    canInterface = can;
    responseMap = rspMap;
    subsystemMap = subMap;
    gpioInterface = gpio;
    controlSpecs = ctrlSpecs;
    bootConfigs = bootArgs;
    systemTimer = new QTime;
    canSensorGroup = canMap;
    bootCmds = boot;
    recordSensorMap = recSenWins;
    recordStateMap = recStateWins;
    sensorMap = sensMap;

    vector<string> cols;
    cols.push_back("recordindex");
    vector<string> result = dbase->get_row_values("system_info",cols,1);
    if (result.size() > 0) {
        sessionNumber = stoi(result.at(0));
    }
    cout << "Record Index: " << sessionNumber << endl;
    startSystemTimer();
    
    //signal-slot connections
    map<string, Group *>::iterator it;

    for (uint i = 0; i < cSyncs.size(); i++){
        syncTimer = new QTimer;
        connect(syncTimer, SIGNAL(timeout()), this, SLOT(canSyncSlot()));
        canSyncTimers.insert(make_pair(i, syncTimer));
        canSyncs.insert(make_pair(i,cSyncs.at(i)));
        syncTimer->start(cSyncs.at(i).rate_ms);
    }

    for (uint i = 0; i < iSyncs.size(); i++){
        syncTimer = new QTimer;
        connect(syncTimer, SIGNAL(timeout()), this, SLOT(i2cSyncSlot()));
        i2cSyncTimers.insert(make_pair(i, syncTimer));
        i2cSyncs.insert(make_pair(i,iSyncs.at(i)));
        syncTimer->start(iSyncs.at(i).rate_ms);
    }

    for (uint i = 0; i < gSyncs.size(); i++){
        syncTimer = new QTimer;
        connect(syncTimer, SIGNAL(timeout()), this, SLOT(gpioSyncSlot()));
        gpioSyncTimers.insert(make_pair(i, syncTimer));
        gpioSyncs.insert(make_pair(i,gSyncs.at(i)));
        syncTimer->start(gSyncs.at(i).rate_ms);
    }

    for ( it = subsystemMap.begin(); it != subsystemMap.end(); it++ ){
        connect(it->second, SIGNAL(initiateRxn(int)), this,SLOT(executeRxn(int)));
        it->second->setSystemTimer(systemTimer);
    }

    watchdogTimer = new QTimer;
    connect(watchdogTimer, SIGNAL(timeout()), this, SLOT(feedWatchdog()));
    watchdogTimer->start((WATCHDOG_PERIOD/2)*1000);

    connect(this, SIGNAL(pushGPIOData(int,int)), gpioInterface,SLOT(GPIOWrite(int,int)));
    connect(this, SIGNAL(sendI2CData(int,int)), gpioInterface,SLOT(i2cWrite(int,int)));
    connect(this, SIGNAL(deactivateState(system_state *)), this,SLOT(deactivateLog(system_state *)));
    connect(this, SIGNAL(sendToUSB7204(uint8_t, float, bool*)), usb7204, SLOT(writeUSBData(uint8_t, float, bool*)));
    connect(this, SIGNAL(sendCANData(int, uint64_t)), canInterface, SLOT(sendData(int, uint64_t)));
    connect(this, SIGNAL(sendCANDataByte(int, uint64_t,int)), canInterface, SLOT(sendDataByte(int, uint64_t,int)));
    connect(canInterface, SIGNAL(process_can_data(uint32_t,uint64_t)), this, SLOT(receive_can_data(uint32_t,uint64_t)));
    connect(usb7204, SIGNAL(sensorValueChanged(meta*)), this, SLOT(receive_sensor_data(meta*)));
    connect(gpioInterface, SIGNAL(sensorValueChanged(meta*)), this, SLOT(receive_sensor_data(meta*)));
    cout << "DATA CONTROL CONFIGURED" << endl;
}

DataControl::~DataControl(){

}

/**
 * @brief DataControl::startSystemTimer : starts internal clock
 */
void DataControl::startSystemTimer(){
    systemTimer->start();
}

string DataControl::getProgramTime(){
    int timeElapsed = systemTimer->elapsed();
    double time = static_cast<double>(timeElapsed)/1000;
    ostringstream streamObj;
    streamObj << std::fixed;
    streamObj << std::setprecision(3);
    streamObj << time;
    return streamObj.str();
}

void DataControl::feedWatchdog(){
    cout << "Feeding watchDog..." << endl;
    system("echo 0 > watchdog.txt");
}

/**
 * @brief DataControl::init_data - initializes vector of data to 0
 */
void DataControl::bootSubsystem(){
    for (uint i = 0; i < bootCmds.bootCanCmds.size(); i++){
        emit sendCANDataByte(bootCmds.bootCanCmds.at(i).address,bootCmds.bootCanCmds.at(i).data,bootCmds.bootCanCmds.at(i).dataSize);
    }
    for (uint i = 0; i < bootCmds.bootI2cCmds.size(); i++){
        emit pushI2cData(bootCmds.bootI2cCmds.at(i));
    }
    for (uint i = 0; i < bootCmds.bootGPIOCmds.size(); i++){
        emit pushGPIOData(bootCmds.bootGPIOCmds.at(i).pin, bootCmds.bootGPIOCmds.at(i).value);
    }
    string bootMsg = "Boot sequence completed";
    pushMessage(bootMsg);
}

void DataControl::canSyncSlot(){
    QObject * tmr = sender();
    for (auto const &x: canSyncTimers){
        if (x.second == tmr) {
            canItem item = canSyncs[x.first];
            emit sendCANDataByte(item.address,item.data,item.dataSize);
        }
    }
}

void DataControl::i2cSyncSlot(){
    QObject * tmr = sender();
    for (auto const &x: i2cSyncTimers){
        if (x.second == tmr) {
            i2cItem item = i2cSyncs[x.first];
            emit sendI2CData(item.address,item.data);
        }
    }
}

void DataControl::gpioSyncSlot(){
    QObject * tmr = sender();
    for (auto const &x: gpioSyncTimers){
        if (x.second == tmr) {
            gpioItem item = gpioSyncs[x.first];
            emit pushGPIOData(item.pin,item.value);
        }
    }
}

void DataControl::receive_sensor_data(meta * sensor){
    try{
        receiveData(sensor);
        for (uint i = 0; i < sensor->dependencies.size(); i++){
            meta * depSensor = static_cast<meta *>(sensor->dependencies.at(i));
            receiveData(depSensor);
        }
    } catch (...) {
        pushMessage("CRITICAL ERROR: Crash on receiving sensor data");
    }
}

/**
 * @brief DataControl::receive_can_data : gets data from canbus
 * @param addr : CAN address
 * @param data : data transmitted
 */
void DataControl::receive_can_data(uint32_t addr, uint64_t data){
    try{
        bool print = false;
        string msg;
        //check whether address matches any state machine address
        for (uint i = 0; i < FSMs.size(); i++){
            statemachine * currFSM = FSMs.at(i);
            QCoreApplication::processEvents();
            if (currFSM->primAddress == static_cast<int>(addr)){
                for (uint j = 0; j < currFSM->states.size(); j++){
                    QCoreApplication::processEvents();
                    system_state * currState = currFSM->states.at(j);
                    if(currState->value == static_cast<int>(isolateData64(static_cast<uint>(currState->auxAddress),static_cast<uint>(currState->offset),data,currState->endianness))){
                        for (const auto &x: recordStateMap){
                            if (x.second->triggerFSM.compare(currFSM->name) == 0 && x.second->triggerState.compare(currState->name) == 0 && !x.second->active){
                                checkStateRecordTriggers(x.second);
                            } else if (x.second->triggerFSM.compare(currFSM->name) == 0 && x.second->triggerState.compare(currState->name) != 0 && x.second->active){
                                checkStateRecordTriggers(x.second);
                            }
                        }
                        change_system_state(currState);
                    } else if (currState->active){
                        deactivateLog(currState);
                    }
                }

                for (uint j = 0; j < currFSM->conditions.size(); j++){
                    condition * currCondition = currFSM->conditions.at(j);
                    if (currCondition->value != static_cast<int>(isolateData64(static_cast<uint>(currCondition->auxAddress),static_cast<uint>(currCondition->offset),data,currFSM->endianness))){
                        currCondition->value = static_cast<int>(isolateData64(static_cast<uint>(currCondition->auxAddress),static_cast<uint>(currCondition->offset),data,currFSM->endianness));
                        print = true;
                    }
                    msg += currCondition->name + "->" + to_string(currCondition->value) ;
                }

                if (print){
                    emit pushMessage(msg);
                }

                emit updateFSM(currFSM);
            }
        }

        //check whether address matches any status address
        for (uint i = 0; i < states.size(); i++){
            QCoreApplication::processEvents();
            if(states.at(i)->primAddress == addr && states.at(i)->value == isolateData64(states.at(i)->auxAddress,states.at(i)->offset,data,states.at(i)->endianness)){
                change_system_state(states.at(i));
            } else if (states.at(i)->primAddress == static_cast<int>(addr)){
                emit deactivateState(states.at(i));
            }
        }

        //check whether address matches any sensor address
        if ( canSensorGroup.find(addr) == canSensorGroup.end() ) {
            // not found
        } else {
          vector<meta *>* specSensors = canSensorGroup.at(addr);
          for (uint i = 0; i < specSensors->size(); i++){
              QCoreApplication::processEvents();
              meta * currSensor = specSensors->at(i);
              currSensor->val = static_cast<int>(isolateData64(currSensor->auxAddress,currSensor->offset,data,currSensor->endianness));
              receiveData(currSensor);
              QCoreApplication::processEvents();
              for (uint i = 0; i < currSensor->dependencies.size(); i++){
                  meta * depSensor = static_cast<meta *>(currSensor->dependencies.at(i));
                  receiveData(depSensor);
              }
          }
        }
    } catch (...) {
        cout << "CRITICAL ERROR: Crash on receiving CAN data" << endl;
        pushMessage("CRITICAL ERROR: Crash on receiving CAN data");
    }
}

/**
 * @brief DataControl::isolateData64 isolate bits of data as specified
 * @param auxAddress : starting bit - LSB -> 63, MSB -> 0
 * @param offset : number of bits in field
 * @param data : bitstream (64) to be isolated
 * @return 32 bit result
 */
uint32_t DataControl::isolateData64(uint auxAddress, uint offset, uint64_t data, int endianness){
    if (auxAddress > 63 || offset > 64) return 0;
    uint lastAddr = sizeof (data)*8 - offset;
    data = data << auxAddress;
    data = data >> lastAddr;
    uint64_t endianData = 0;
    //stringstream s;
    //s << showbase << internal << setfill('0');
    //cout << "Aux: " << auxAddress << " offset: " << offset << " data: " << data << " endianness: " << endianness << endl;

    if (endianness == 0){
        if (offset > 8){
            int cnt = offset/8;
            uint64_t filter = 0xff;
            for (int i = 0; i < cnt; i++){
                endianData = endianData << 8;
                uint64_t buf = (data >> i*8) & filter;
                endianData = endianData | buf;
            }
            data = endianData;
        }
    }
    //s << "Final Data: " << std::hex << setw(16) << data;
    //cout << s.str() << endl;

    return static_cast<uint32_t>(data);
}

/**
 * @brief DataControl::LSBto64Spec shift specified LSBits to specified field
 * @param auxAddress : starting bit of target field - LSB -> 63, MSB -> 0
 * @param offset : number of bits in field
 * @param data : bitstream (64) to be isolated
 * @return 64 bit result
 */
uint64_t DataControl::LSBto64Spec(uint auxAddress, uint offset, uint64_t data){
    if (auxAddress > 63 || offset > 64) return 0;
    uint lastAddr = sizeof (data)*8 - offset;
    uint firstAddr = sizeof (data)*8 - auxAddress;
    data = data << lastAddr;
    data = data >> firstAddr;
    return data;
}

/**
 * @brief DataControl::receiveData : receive sensor data
 * @param currSensor
 */
void DataControl::receiveData(meta * currSensor){
    calibrateData(currSensor);
    checkSensorRecordTriggers(currSensor);
    checkThresholds(currSensor);
    emit updateDisplay(currSensor);
    logData(currSensor);
}

void DataControl::checkStateRecordTriggers(recordwindow * rec){
    cout << "Checking State Record Triggers..............................." << endl;
    if (!rec->active){
        cout << "Activating Record" << endl;
        rec->active = true;
        ofstream dbScript;
        string scriptName = rec->prefix + "_script.sql";
        dbScript.open (scriptName);
        dbScript << "create table if not exists sensor_data(" << endl;
        string colString;
        for (uint i = 0; i < rec->sensorIds.size(); i++){
            colString += removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName) + ",";
            string scriptLine = removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName) + " char not null,";
            dbScript << scriptLine << endl;
        }
        dbScript << "time char not null" << endl;
        dbScript << ");" << endl;
        dbScript.close();
        colString += "time";
        string dbPath = rec->savePath + rec->prefix + to_string(sessionNumber) + "_data.db";
        cout << "DB Path: " << dbPath << endl;
        customDB = new DB_Engine(dbPath);
        customDB->runScript(scriptName);
        string deleteFileCmd = "rm " + scriptName;
        system(deleteFileCmd.c_str());
        recordDBMap.insert(make_pair(rec->id,customDB));
        recordColStrings.insert(make_pair(rec->id,colString));
    } else if (rec->active){
//        cout << "Deactivating collection" << endl;
        rec->active = false;
        DB_Engine * currDB = recordDBMap[rec->id];
        vector<string> cols;
        ofstream dataFile;
        string fileName = rec->savePath + rec->prefix + get_curr_date() + "_" + to_string(sessionNumber) + "_data.csv";
        dataFile.open(fileName);
        cols.push_back("time");
        dataFile << "time,";
        for (uint i = 0; i < rec->sensorIds.size(); i++){
            cols.push_back(removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName));
            dataFile << sensorMap[rec->sensorIds.at(i)]->sensorName;
            if (i < rec->sensorIds.size()-1) dataFile << ",";
        }
        for (int i = 0; i < currDB->max_rowid("sensor_data"); i++){
            vector<string> vals = currDB->get_row_values("sensor_data",cols,i);
            string dataLine;
            for (uint j = 0; j < vals.size(); j++){
                dataLine += vals.at(j);
                if (j < vals.size()-1) dataLine += ",";
            }
            dataFile << dataLine << endl;
        }
        dataFile.close();
        if (recordDBMap.count(rec->id) > 0) {
//            if (recordDBMap[rec->id] != nullptr) delete recordDBMap[rec->id];
//            recordDBMap.erase(rec->id);
        }
        incrementSessionNumber();
    }
}

void DataControl::checkSensorRecordTriggers(meta * currSensor){
    for (const auto &x: recordSensorMap){
        if ( currSensor->sensorIndex == x.second->triggerSensor ) {
            recordwindow * rec = x.second;
            if ((currSensor->calVal >= rec->startVal) && !rec->active){
                rec->active = true;
                ofstream dbScript;
                string scriptName = rec->prefix + "_script.sql";
                dbScript.open (scriptName);
                dbScript << "create table if not exists sensor_data(" << endl;
                string colString;
                for (uint i = 0; i < rec->sensorIds.size(); i++){
                    colString += removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName) + ",";
                    string scriptLine = removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName) + " char not null,";
                    dbScript << scriptLine << endl;
                }
                dbScript << "time char not null" << endl;
                dbScript << ");" << endl;
                dbScript.close();
                colString += "time";
                string dbPath = rec->savePath + rec->prefix + to_string(sessionNumber) + "_data.db";
                cout << "DB Path: " << dbPath << endl;
                customDB = new DB_Engine(dbPath);
                customDB->runScript(scriptName);
                string deleteFileCmd = "rm " + scriptName;
                system(deleteFileCmd.c_str());
                recordDBMap.insert(make_pair(x.first,customDB));
                recordColStrings.insert(make_pair(x.second->id,colString));
            } else if ((currSensor->calVal <= rec->startVal) && rec->active){
//                cout << "Deactivating collection" << endl;
                DB_Engine * currDB = recordDBMap[x.first];
                vector<string> cols;
                ofstream dataFile;
                string fileName = rec->savePath + rec->prefix + get_curr_date() + "_" + to_string(sessionNumber) + "_data.csv";
                dataFile.open(fileName);
                cols.push_back("time");
                dataFile << "time,";
                for (uint i = 0; i < rec->sensorIds.size(); i++){
                    cols.push_back(removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName));
                    dataFile << sensorMap[rec->sensorIds.at(i)]->sensorName;
                    if (i < rec->sensorIds.size()-1) dataFile << ",";
                }
                for (int i = 0; i < currDB->max_rowid("sensor_data"); i++){
                    vector<string> vals = currDB->get_row_values("sensor_data",cols,i);
                    string dataLine;
                    for (uint j = 0; j < vals.size(); j++){
                        dataLine += vals.at(j);
                        if (j < vals.size()-1) dataLine += ",";
                    }
                    dataFile << dataLine << endl;
                }
                dataFile.close();
                if (recordDBMap.count(x.first) > 0) {
//                    if (x.second != nullptr) delete x.second;
//                    recordDBMap.erase(x.first);
                }
                incrementSessionNumber();
                rec->active = false;
            }
        }
    }
}

void DataControl::incrementSessionNumber(){
    sessionNumber++;
    dbase->update_value("system_info","recordindex","rowid", "1", to_string(sessionNumber));
}

/**
 * @brief DataControl::logData - records specified sensor data in the respective database
 * @param currSensor
 */
void DataControl::logData(meta * currSensor){
    for (auto const& x : recordSensorMap){
        string colString;
        string rowString;
        if (x.second->active){
            for (auto const &y: x.second->sensorIds){
                if (currSensor->sensorIndex == y){
                    colString = recordColStrings[x.second->id];
                    for (auto const &z: x.second->sensorIds){
                        rowString += "'" + to_string(sensorMap[z]->calVal) + "',";
                    }
                }
            }
            if (rowString.compare("") != 0){
                rowString += "'" + getProgramTime() + "'";
                recordDBMap[x.first]->insert_row("sensor_data",colString,rowString);
            }
        }
    }
    for (auto const& x : recordStateMap){
        string colString;
        string rowString;
        if (x.second->active){
            for (auto const &y: x.second->sensorIds){
                if (currSensor->sensorIndex == y){
                    colString = recordColStrings[x.second->id];
                    for (auto const &z: x.second->sensorIds){
                        rowString += "'" + to_string(sensorMap[z]->calVal) + "',";
                    }
                }
            }
            if (rowString.compare("") != 0){
                rowString += "'" + getProgramTime() + "'";
                cout << "Column query: " << colString << endl;
                cout << "Row Args: " << rowString << endl;
                recordDBMap[x.first]->insert_row("sensor_data",colString,rowString);
            }
        }
    }
}

/**
 * @brief DataControl::checkThresholds - check whether specified sensor data exceeds configured thresholds
 * @param sensor
 */
void DataControl::checkThresholds(meta * sensor){
    string msg;
    if (sensor->calVal >= sensor->maximum){
        if (sensor->state != 1){
            sensor->state = 1;
            emit updateEditColor("red",sensor);
            for (auto const &x : sensor->groups){
                if (!subsystemMap[x]->error){
                    subsystemMap[x]->error = true;
                    emit subsystemMap[x]->updateHealth();
                }
            }
            msg = sensor->sensorName + " exceeded upper threshold: " + to_string(sensor->maximum);
            emit pushMessage(msg);
            executeRxn(sensor->maxRxnCode);
            pushMessage(msg);
        }
    } else if (sensor->calVal <= sensor->minimum){
        if (sensor->state != -1){
            sensor->state = -1;
            emit updateEditColor("blue",sensor);
            for (auto const &x : sensor->groups){
                if (!subsystemMap[x]->error){
                    subsystemMap[x]->error = true;
                    emit subsystemMap[x]->updateHealth();
                }
            }
            msg = sensor->sensorName + " below lower threshold: " + to_string(sensor->minimum);
            emit pushMessage(msg);
            executeRxn(sensor->minRxnCode);
            pushMessage(msg);
        }
    } else {
        if (sensor->state != 0){
            sensor->state = 0;
            emit updateEditColor("yellow",sensor);
            executeRxn(sensor->normRxnCode);
            for (auto const &x : sensor->groups){
                if (subsystemMap[x]->error) {
                    subsystemMap[x]->checkError();
                    emit subsystemMap[x]->updateHealth();
                }
            }
        }
    }
}

/**
 * @brief DataControl::calibrateData : calibrates sensor data
 * @param currSensor
 */
void DataControl::calibrateData(meta * currSensor){
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

void DataControl::save_all_data(){
    cout << "all data being saved" << endl;
    for (auto const &x : recordSensorMap){
        recordwindow * rec = x.second;
        if (rec->active){
            rec->active = false;
            DB_Engine * currDB = recordDBMap[x.first];
            vector<string> cols;
            ofstream dataFile;
            string fileName = rec->savePath + rec->prefix + get_curr_date() + "_" + to_string(sessionNumber) + "_data.csv";
            dataFile.open(fileName);
            cols.push_back("time");
            dataFile << "time,";
            for (uint i = 0; i < rec->sensorIds.size(); i++){
                cols.push_back(removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName));
                dataFile << sensorMap[rec->sensorIds.at(i)]->sensorName;
                if (i < rec->sensorIds.size()-1) dataFile << ",";
            }
            for (int i = 0; i < currDB->max_rowid("sensor_data"); i++){
                vector<string> vals = currDB->get_row_values("sensor_data",cols,i);
                string dataLine;
                for (uint j = 0; j < vals.size(); j++){
                    dataLine += vals.at(j);
                    if (j < vals.size()-1) dataLine += ",";
                }
                dataFile << dataLine << endl;
            }
            dataFile.close();
            if (recordDBMap.count(x.first) > 0) {
//                delete recordDBMap[x.first];
//                recordDBMap.erase(x.first);
            }
            incrementSessionNumber();
        }
    }
    for (auto const &x : recordStateMap){
        recordwindow * rec = x.second;
        if (rec->active){
            rec->active = false;
            DB_Engine * currDB = recordDBMap[x.first];
            vector<string> cols;
            ofstream dataFile;
            string fileName = rec->savePath + rec->prefix + get_curr_date() + "_" + to_string(sessionNumber) + "_data.csv";
            dataFile.open(fileName);
            cols.push_back("time");
            dataFile << "time,";
            for (uint i = 0; i < rec->sensorIds.size(); i++){
                cols.push_back(removeSpaces(sensorMap[rec->sensorIds.at(i)]->sensorName));
                dataFile << sensorMap[rec->sensorIds.at(i)]->sensorName;
                if (i < rec->sensorIds.size()-1) dataFile << ",";
            }
            for (int i = 0; i < currDB->max_rowid("sensor_data"); i++){
                vector<string> vals = currDB->get_row_values("sensor_data",cols,i);
                string dataLine;
                for (uint j = 0; j < vals.size(); j++){
                    dataLine += vals.at(j);
                    if (j < vals.size()-1) dataLine += ",";
                }
                dataFile << dataLine << endl;
            }
            dataFile.close();
            if (recordDBMap.count(x.first) > 0) delete recordDBMap[x.first];
            recordDBMap.erase(x.first);
            incrementSessionNumber();
        }
    }
}

/**
 * @brief DataControl::change_system_state changes state as specified
 * @param newState : state to be activated
 * @return 1 on success, 0 otherwise
 */
int DataControl::change_system_state(system_state * newState){
    try{
    newState->active = true;
    string colString = "time,state,message";
    string rowString = "'" + getProgramTime() + "','" + newState->name + "','Entered State'";

    //change state of system
    currState = newState->name;

    //update systems on database
    dbase->insert_row("system_states",colString,rowString);

    //display change on back and front screen
    emit activateState(newState);
    return 1;
    } catch(...){
        return 0;
    }
}

/**
 * @brief DataControl::deactivateLog records state-exitting to database
 * @param prevstate : state being exitted
 */
void DataControl::deactivateLog(system_state *prevstate){
    prevstate->active = false;
    string colString = "time,state,message";
    string rowString = "'" + getProgramTime() + "','" + prevstate->name + "','Exit State'";
    dbase->insert_row("system_states",colString,rowString);
}

/**
 * @brief DataControl::executeRxn executes specified response
 * @param responseIndex : response identifier
 */
void DataControl::executeRxn(int responseIndex){
    //print to logpushMessage
    try{
        string colString = "time,reactionId,message";
        string rowString = "'" + getProgramTime() + "','" + to_string(responseIndex) + "','" + responseMap[responseIndex].msg + "'";

        response rsp = responseMap[responseIndex];
        if (rsp.primAddress >= 0){
            uint64_t fullData = static_cast<uint64_t>(rsp.canValue);
            fullData = LSBto64Spec(static_cast<uint>(rsp.auxAddress),static_cast<uint>(rsp.offset),fullData);
            emit sendCANData(rsp.primAddress,fullData);
        }
        if (rsp.gpioPin >= 0){
            emit pushGPIOData(rsp.gpioPin,rsp.gpioValue);
        }
        dbase->insert_row("system_log",colString,rowString);
    } catch (...) {
        pushMessage("CRITICAL ERROR: Crash on executing reaction to data");
    }
}

/**
 * @brief DataControl::receive_control_val : receives control signal to be sent
 * @param data : data to be sent
 * @param spec : specifications of control signal
 */
void DataControl::receive_control_val(int data, controlSpec * spec){
    try{
        int addr = spec->primAddress;
        stringstream s;
        if (addr != 1000){
            data = static_cast<int>(data*spec->multiplier);
            uint64_t fullData = static_cast<uint64_t>(data);
            fullData = LSBto64Spec(static_cast<uint>(spec->auxAddress),static_cast<uint>(spec->offset),fullData);
            spec->sentVal = spec->sentVal & ~LSBto64Spec(static_cast<uint>(spec->auxAddress),static_cast<uint>(spec->offset),0xFFFFFFFF);
            spec->sentVal = spec->sentVal | fullData;
            s << showbase << internal << setfill('0');
            s << "Data " << std::hex << setw(16) << fullData << " sent to address " << addr;
            emit sendCANData(addr,spec->sentVal);
            emit pushMessage(s.str());
        } else if (spec->usbChannel != -1){
            float usbData = static_cast<float>(data)*static_cast<float>(spec->multiplier);
            bool success = true;
            emit sendToUSB7204(static_cast<uint8_t>(spec->usbChannel),usbData, &success);
            if (success){
                s << "Value " << usbData << " written to usb out channel " << spec->usbChannel;
                emit pushMessage(s.str());
            }
        }
    } catch (...) {
        pushMessage("CRITICAL ERROR: Crash on receiving control data");
    }
}

/**
 * @brief DataControl::get_control_specs : returns all configured control specs
 * @return
 */
vector<controlSpec *> DataControl::get_control_specs(){
    return controlSpecs;
}

/**
 * @brief DataControl::saveSession : saves database fiel in the savedsessions folder
 * @param name : name of saved file
 */
void DataControl::saveSession(string name){
    string systemString = "mv ./savedsessions/system.db ./savedsessions/";
    systemString += name;
    system(systemString.c_str());
}

/**
 * @brief DataControl::get_curr_time - retrieves current operation system time
 * @return
 */
string DataControl::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D_%T",&now);
    return buf;
}

string DataControl::get_curr_date(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D",&now);
    return buf;
}

/**
 * @brief Function to remove all spaces from a given string
 */
string DataControl::removeSpaces(string &str)
{
    int size = str.length();
    for(int j = 0; j<=size; j++){
        for(int i = 0; i <=j; i++){
            if(str[i] == ' ') str.erase(str.begin() + i);
        }
    }
    return str;
}
