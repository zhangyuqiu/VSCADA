#include "datacontrol.h"

DataControl::DataControl(DataMonitor * mtr, DB_Engine * db, vector<SubsystemThread *> threads, vector<system_state> stts){
    // assign global objects
    monitor = mtr;
    dbase = db;
    states = stts;
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

void DataControl::check_system_state(int canAddress, int value){

}

int DataControl::change_system_state(system_state newState){
    //change state of system
    currState = newState.name;
    //update systems on database

    //display change on back screen
    //display change on front screen
    return 0;
}

int DataControl::collectData(){
    return 0;
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
