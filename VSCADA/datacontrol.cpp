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
                         map<string, SubsystemThread *> subMap, vector<system_state *> stts, vector<statemachine *> FSM,
                         int mode, vector<controlSpec *> ctrlSpecs, vector<meta *> sensors, map<int, response> rspMap,
                         vector<bootloader> bootArgs, map<uint32_t, vector<meta *>*> canMap){

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
    sensorVector = sensors;
    controlSpecs = ctrlSpecs;
    bootConfigs = bootArgs;
    systemTimer = new QTime;
    canSensorGroup = canMap;
    startSystemTimer();

    
    
    //signal-slot connections
    map<string, SubsystemThread *>::iterator it;

    for ( it = subsystemMap.begin(); it != subsystemMap.end(); it++ ){
        connect(it->second, SIGNAL(initiateRxn(int)), this,SLOT(executeRxn(int)));
        it->second->setSystemTimer(systemTimer);
    }

    connect(this, SIGNAL(pushGPIOData(int,int)), gpioInterface,SLOT(GPIOWrite(int,int)));
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

void DataControl::receive_sensor_data(meta * sensor){
    try{
        subsystemMap[sensor->subsystem]->receiveData(sensor);
        for (uint i = 0; i < sensor->dependencies.size(); i++){
            meta * depSensor = static_cast<meta *>(sensor->dependencies.at(i));
            subsystemMap[depSensor->subsystem]->receiveData(depSensor);
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
              cout << "Isolating data: " << addr << endl;
              fflush(stdout);
              currSensor->val = static_cast<int>(isolateData64(currSensor->auxAddress,currSensor->offset,data,currSensor->endianness));
              cout << "Sending to subsystem: " << addr << endl;
              fflush(stdout);
              subsystemMap[currSensor->subsystem]->receiveData(currSensor);
              QCoreApplication::processEvents();
              cout << "Processing dependencies: " << addr << endl;
              fflush(stdout);
              for (uint i = 0; i < currSensor->dependencies.size(); i++){
                  meta * depSensor = static_cast<meta *>(currSensor->dependencies.at(i));
                  subsystemMap[depSensor->subsystem]->receiveData(depSensor);
              }
          }
//          cout << "Done processing CAN data address: " << addr << endl;
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
