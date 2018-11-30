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
                         vector<SubsystemThread *> threads, vector<system_state *> stts, vector<statemachine *> FSM,
                         int mode, vector<controlSpec *> ctrlSpecs, vector<meta *> sensors, vector<response> rsp, vector<bootloader> bootArgs){

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
    responseVector = rsp;
    subsystems = threads;
    gpioInterface = gpio;
    sensorVector = sensors;
    controlSpecs = ctrlSpecs;
    bootConfigs = bootArgs;
    systemTimer = new QTime;
    startSystemTimer();
    //signal-slot connections
    for (uint i = 0 ; i < subsystems.size(); i++){
        connect(subsystems.at(i), SIGNAL(initiateRxn(int)), this,SLOT(executeRxn(int)));
        subsystems.at(i)->setSystemTimer(systemTimer);
    }
    connect(this, SIGNAL(pushGPIOData(int,int)), gpioInterface,SLOT(GPIOWrite(int,int)));
    connect(this, SIGNAL(deactivateState(system_state *)), this,SLOT(deactivateLog(system_state *)));
    connect(this, SIGNAL(sendToUSB7204(uint8_t, float, bool*)), usb7204, SLOT(writeUSBData(uint8_t, float, bool*)));
    connect(this, SIGNAL(sendCANData(int, uint64_t)), canInterface, SLOT(sendData(int, uint64_t)));
    connect(canInterface, SIGNAL(process_can_data(uint32_t,uint64_t)), this, SLOT(receive_can_data(uint32_t,uint64_t)));
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

/**
 * @brief DataControl::change_sampling_rate changes the samplong rates of GPIO and USB interfaces
 * @param rate : new sampling rate
 * @return 0 on success, 0 otherwise
 */
int DataControl::change_sampling_rate(int rate){
    //change sampling rate for specific sensor(s)
    try {
        usb7204->setSamplingRate(rate);
        gpioInterface->setSamplingRate(rate);
        return 1;
    } catch (...) {
        logMsg("ERROR: Smapling Rate not changed");
        return 0;
    }
}

/**
 * @brief DataControl::receive_can_data : gets data from canbus
 * @param addr : CAN address
 * @param data : data transmitted
 */
void DataControl::receive_can_data(uint32_t addr, uint64_t data){
    //check whether address matches any boot arguments
    for (uint i = 0; i < bootConfigs.size(); i++){
        bootloader currBootLoader = bootConfigs.at(i);
        if (currBootLoader.canAddress == static_cast<int>(addr) && currBootLoader.trigger == data){
            sendBootConfig(currBootLoader);
        }
    }

    //check whether address matches any state machine address
    for (uint i = 0; i < FSMs.size(); i++){
        statemachine * currFSM = FSMs.at(i);
        if (currFSM->primAddress == static_cast<int>(addr)){
            for (uint j = 0; j < currFSM->states.size(); j++){
                system_state * currState = currFSM->states.at(j);
                if(currState->value == isolateData64(currState->auxAddress,currState->offset,data)){
                    change_system_state(currState);
                } else if (currState->active){
                    deactivateLog(currState);
                }
            }
            emit updateFSM(currFSM);
        }
    }

    //check whether address matches any status address
    for (uint i = 0; i < states.size(); i++){
        if(states.at(i)->primAddress == addr && states.at(i)->value == isolateData64(states.at(i)->auxAddress,states.at(i)->offset,data)){
            cout << endl << "State found" << endl << endl;
            change_system_state(states.at(i));
        } else if (states.at(i)->primAddress == addr){
            emit deactivateState(states.at(i));
        }
    }

    //check whether address matches any sensor address
    for(uint i = 0; i < sensorVector.size(); i++){
        if(sensorVector.at(i)->primAddress == addr){
            meta * currSensor = sensorVector.at(i);
            if (currSensor->val != isolateData64(currSensor->auxAddress,currSensor->offset,data)) {
                currSensor->val = isolateData64(currSensor->auxAddress,currSensor->offset,data);
                for (uint j = 0; j < subsystems.size(); j++){
                    if (currSensor->subsystem.compare(subsystems.at(j)->subsystemId) == 0){
                        subsystems.at(j)->receiveData(currSensor);
                        break;
                    }
                }
            } else {
                for (uint j = 0; j < subsystems.size(); j++){
                    if (currSensor->subsystem.compare(subsystems.at(j)->subsystemId) == 0){
                        subsystems.at(j)->checkThresholds(currSensor);
                        emit updateEdits(currSensor);
                        break;
                    }
                }
            }
        }
    }
}

/**
 * @brief DataControl::sendBootConfig : sends bootup data to specified address
 * @param bl
 */
void DataControl::sendBootConfig(bootloader bl){
    logMsg(bl.displayMsg);
    for(uint i = 0; i < bl.configMsg.size(); i++){
        stringstream s;
        s << showbase << internal << setfill('0');
        s << "Data " << std::hex << setw(16) << bl.configMsg.at(i) << " sent to address " << bl.canAddress;
        logMsg(s.str());
        emit sendCANData(bl.canAddress,bl.configMsg.at(i));
    }
}

/**
 * @brief DataControl::isolateData64 isolate bits of data as specified
 * @param auxAddress : starting bit - LSB -> 63, MSB -> 0
 * @param offset : number of bits in field
 * @param data : bitstream (64) to be isolated
 * @return 32 bit result
 */
uint32_t DataControl::isolateData64(uint auxAddress, uint offset, uint64_t data){
    if (auxAddress > 63 || offset > 64) return 0;
    uint lastAddr = sizeof (data)*8 - offset;
    data = data << auxAddress;
    data = data >> lastAddr;
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
 * @brief DataControl::change_system_state changes state as specified
 * @param newState : state to be activated
 * @return 1 on success, 0 otherwise
 */
int DataControl::change_system_state(system_state * newState){
    try{
    newState->active = true;
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("state");
    cols.push_back("message");
    vector<string> rows;
    rows.push_back(getProgramTime());
    rows.push_back(newState->name);
    rows.push_back("Entered State");

    //change state of system
    currState = newState->name;

    //update systems on database
    dbase->insert_row("system_states",cols,rows);

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
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("state");
    cols.push_back("message");
    vector<string> rows;
    rows.push_back(getProgramTime());
    rows.push_back(prevstate->name);
    rows.push_back("Exit State");
    dbase->insert_row("system_states",cols,rows);
}

/**
 * @brief DataControl::executeRxn executes specified response
 * @param responseIndex : response identifier
 */
void DataControl::executeRxn(int responseIndex){
    cout << "Reacting: index " << responseIndex << endl;
    //print to log
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("reactionId");
    cols.push_back("message");
    rows.push_back(getProgramTime());
    rows.push_back(to_string(responseIndex));

    for (uint i = 0; i < responseVector.size(); i++){
        response rsp = responseVector.at(i);
        if (rsp.responseIndex == responseIndex){
            rows.push_back(rsp.msg);
            logMsg(rsp.msg);
            if (rsp.primAddress >= 0){
                uint64_t fullData = static_cast<uint64_t>(rsp.canValue);
                fullData = LSBto64Spec(rsp.auxAddress,rsp.offset,fullData);
                emit sendCANData(rsp.primAddress,fullData);
            }
            if (rsp.gpioPin >= 0){
                emit pushGPIOData(rsp.gpioPin,rsp.gpioValue);
            }
        }
    }
    dbase->insert_row("system_log",cols,rows);
}

/**
 * @brief SubsystemThread::enqueueMsg - logs message in the database
 * @param msg
 */
void DataControl::logMsg(string msg){
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("responseid");
    cols.push_back("message");
    rows.push_back(getProgramTime());
    rows.push_back("console");
    rows.push_back(msg);
    dbase->insert_row("system_log",cols,rows);
    emit pushMessage(msg);
}

/**
 * @brief DataControl::receive_control_val : receives control signal to be sent
 * @param data : data to be sent
 * @param spec : specifications of control signal
 */
void DataControl::receive_control_val(int data, controlSpec * spec){
    int addr = spec->primAddress;
    stringstream s;
    if (addr != -1){
        data = data*spec->multiplier;
        uint64_t fullData = static_cast<uint64_t>(data);
        fullData = LSBto64Spec(spec->auxAddress,spec->offset,fullData);
        spec->sentVal = spec->sentVal & ~LSBto64Spec(spec->auxAddress,spec->offset,0xFFFFFFFF);
        spec->sentVal = spec->sentVal | fullData;
        s << showbase << internal << setfill('0');
        s << "Data " << std::hex << setw(16) << fullData << " sent to address " << addr;
        emit sendCANData(addr,spec->sentVal);
        logMsg(s.str());
    } else if (spec->usbChannel != -1){
        float usbData = static_cast<float>(data)*static_cast<float>(spec->multiplier);
        bool success = true;
        emit sendToUSB7204(static_cast<uint8_t>(spec->usbChannel),usbData, &success);
        if (success){
            s << "Value " << usbData << " written to usb out channel " << spec->usbChannel;
            logMsg(s.str());
        }
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
    string systemString = "mv system.db ./savedsessions/";
    systemString += name;
    system(systemString.c_str());
    systemString = "rm system.db";
    system(systemString.c_str());
}

/**
 * @brief SubsystemThread::get_curr_time - retrieves current operation system time
 * @return
 */
string DataControl::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D_%T",&now);
    return buf;
}
