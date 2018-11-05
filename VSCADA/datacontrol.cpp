#include "datacontrol.h"

DataControl::DataControl(DataMonitor * mtr, DB_Engine * db, vector<SubsystemThread *> threads,
                         vector<system_state *> stts, vector<statemachine *> FSM, int mode,
                         vector<controlSpec *> ctrlSpecs, vector<meta *> sensors, vector<response> rsp){
    // assign global objects
    systemMode = mode;
    if(mode == 1){
        modeName = "DYNO";
    } else if (mode == 0){
        modeName = "CAR";
    }
    controlSpecs = ctrlSpecs;
    monitor = mtr;
    dbase = db;
    states = stts;
    FSMs = FSM;
    sensorVector = sensors;
    responseVector = rsp;
    subsystems = threads;
    connect(this, SIGNAL(deactivateState(system_state)), this,SLOT(deactivateLog(system_state)));
    for (uint i = 0 ; i < threads.size(); i++){
        connect(threads.at(i), SIGNAL(initiateRxn(int)), this,SLOT(executeRxn(int)));
    }
}

DataControl::~DataControl(){

}

void DataControl::init_meta_vector(vector<meta> vctr){

}

int DataControl::write_to_CAN(controlSpec spec){
    //write to CAN in response to data
    //log this activity to db
    //display change on back screen
    return 0;
}

int DataControl::write_to_USB(controlSpec spec){
    //write to USB in response to data
    //log this activity to db
    //display change on back screen
    return 0;
}

int DataControl::write_to_GPIO(controlSpec spec){
    //write to USB in response to data
    //log this activity to db
    //display change on back screen
    return 0;
}

int DataControl::write_to_DYNO(controlSpec spec){
    //write to DYNO in response to user input or data
    //log this activity to db
    //display write on back screen
    return 0;
}

int DataControl::change_sampling_rate(controlSpec spec, int rate){
    //change sampling rate for specific sensor(s)
    return 0;
}

void DataControl::receive_can_data(uint32_t addr, uint64_t data){

    cout << endl << "Data Control Module" << endl;
    cout << "Data Transmitted: " << data << endl;
    for (uint i = 0; i < FSMs.size(); i++){
        statemachine * currFSM = FSMs.at(i);
        if (currFSM->primAddress == addr){
            cout << endl << "FSM found" << endl << endl;
            for (int j = 0; j < currFSM->states.size(); j++){
                system_state * currState = currFSM->states.at(j);
                if(currState->value == isolateData64(currState->auxAddress,currState->offset,data)){
                    cout << "ACTIVATING " << currState->name << endl;
                    change_system_state(currState);
                } else if (currState->active){
                    cout << "DEACTIVATING " << currState->name << endl;
                    deactivateLog(currState);
                }
            }
            emit updateFSM(currFSM);
        }
    }

    for (uint i = 0; i < states.size(); i++){
        if(states.at(i)->primAddress == addr && states.at(i)->value == isolateData64(states.at(i)->auxAddress,states.at(i)->offset,data)){
            cout << endl << "State found" << endl << endl;
            change_system_state(states.at(i));
        } else if (states.at(i)->primAddress == addr){
            emit deactivateState(states.at(i));
        }
    }

    for(uint i = 0; i < sensorVector.size(); i++){
        if(sensorVector.at(i)->primAddress == addr){
            meta * currSensor = sensorVector.at(i);
            if (currSensor->val != isolateData64(currSensor->auxAddress,currSensor->offset,data)) {
                cout << "Cal Constant: " << currSensor->calConst << endl;
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
                        subsystems.at(j)->updateEdits(currSensor);
                        break;
                    }
                }
            }
        }
    }
}

uint32_t DataControl::isolateData64(uint auxAddress, uint offset, uint64_t data){
    if (auxAddress > 63 || offset > 64) return 0;
    uint lastAddr = sizeof (data)*8 - offset;
    data = data << auxAddress;
    data = data >> lastAddr;
    return static_cast<uint32_t>(data);
}

uint64_t DataControl::LSBto64Spec(uint auxAddress, uint offset, uint64_t data){
    if (auxAddress > 63 || offset > 64) return 0;
    uint lastAddr = sizeof (data)*8 - offset;
    uint firstAddr = sizeof (data)*8 - auxAddress;
    data = data << lastAddr;
    data = data >> firstAddr;
    return data;
}

int DataControl::change_system_state(system_state * newState){
    newState->active = true;
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("state");
    cols.push_back("message");
    vector<string> rows;
    rows.push_back(get_curr_time());
    rows.push_back(newState->name);
    rows.push_back("Entered State");

    //change state of system
    currState = newState->name;

    //update systems on database
    dbase->insert_row("system_states",cols,rows);

    //display change on back and front screen
    emit activateState(newState);
    return 0;
}

void DataControl::deactivateLog(system_state *prevstate){
    prevstate->active = false;
    vector<string> cols;
    cols.push_back("time");
    cols.push_back("state");
    cols.push_back("message");
    vector<string> rows;
    rows.push_back(get_curr_time());
    rows.push_back(prevstate->name);
    rows.push_back("Entered State");
    dbase->insert_row("system_states",cols,rows);
}

int DataControl::collectData(){
    return 0;
}

void DataControl::executeRxn(int rxnCode){
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
            if (rsp.primAddress >= 0){
                cout << "sending out can data" << endl;
                uint64_t fullData = static_cast<uint64_t>(rsp.canValue);
                fullData = LSBto64Spec(rsp.auxAddress,rsp.offset,fullData);
                emit sendCANData(rsp.primAddress,fullData);
            }
            if (rsp.gpioPin >= 0){
                emit pushGPIOData(rsp);
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
    rows.push_back(get_curr_time());
    rows.push_back("console");
    rows.push_back(msg);
    dbase->insert_row("system_log",cols,rows);
}

void DataControl::receive_control_val(int data, controlSpec * spec){
    int addr = spec->primAddress;
    uint64_t fullData = static_cast<uint64_t>(data);
    fullData = LSBto64Spec(spec->auxAddress,spec->offset,fullData);
    spec->sentVal = spec->sentVal & ~LSBto64Spec(spec->auxAddress,spec->offset,0xFFFFFFFF);
    spec->sentVal = spec->sentVal | fullData;
    stringstream s;
    s << showbase << internal << setfill('0');
    s << "Data " << std::hex << setw(16) << fullData << " sent to address " << addr;
    logMsg(s.str());
    emit pushMessage(s.str());
    emit sendCANData(addr,spec->sentVal);
}

vector<controlSpec *> DataControl::get_control_specs(){
    return controlSpecs;
}

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
