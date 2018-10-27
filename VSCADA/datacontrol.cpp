#include "datacontrol.h"

DataControl::DataControl(DataMonitor * mtr, DB_Engine * db, vector<SubsystemThread *> threads, vector<system_state *> stts, vector<statemachine *> FSM, int mode, vector<controlSpec> ctrlSpecs){
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
    connect(this, SIGNAL(deactivateState(system_state)), this,SLOT(deactivateLog(system_state)));
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

vector<controlSpec> DataControl::get_control_specs(){
    return controlSpecs;
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
