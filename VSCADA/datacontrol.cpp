#include "datacontrol.h"

DataControl::DataControl(DataMonitor * mtr, DB_Engine * db, iocontrol * io,GLV_Thread * glvTh,
                         TSI_Thread *tsiTh, TSV_Thread *tsvTh, COOL_Thread *coolTh){
    // assign global objects
    monitor = mtr;
    dbase = db;
    ioControl = io;
    glv_thread = glvTh;
    tsi_thread = tsiTh;
    tsv_thread = tsvTh;
    cool_thread = coolTh;
}

DataControl::~DataControl(){

}

void DataControl::init_meta_vector(vector<meta> vctr){

}

int DataControl::init_GLV_sensors(vector<meta> vctr){
    glv_thread->GLVSensorMeta = vctr;
    return 0;
}

int DataControl::init_TSI_sensors(vector<meta> vctr){
    tsi_thread->TSISensorMeta = vctr;
    return 0;
}

int DataControl::init_TSV_sensors(vector<meta> vctr){
    tsv_thread->TSVSensorMeta = vctr;
    return 0;
}

int DataControl::init_COOL_sensors(vector<meta> vctr){
    cool_thread->COOLSensorMeta = vctr;
    return 0;
}

void DataControl::setMode(int md){
    mode = md;
}

int DataControl::set_GLV_rate(int rate){
    glv_thread->GLV_rate = rate;
    return 0;
}

int DataControl::set_TSI_rate(int rate){
    tsi_thread->TSI_rate = rate;
    return 0;
}

int DataControl::set_TSV_rate(int rate){
    tsv_thread->TSV_rate = rate;
    return 0;
}

int DataControl::set_COOL_rate(int rate){
    cool_thread->COOL_rate = rate;
    return 0;
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

int DataControl::change_system_state(int newState){
    //change state of system
    //update systems on database
    //display change on back screen
    //display change on front screen
    return 0;
}

int DataControl::collectData(){
    glv_thread->StartInternalThread();
    tsi_thread->StartInternalThread();
    tsv_thread->StartInternalThread();
    cool_thread->StartInternalThread();
    glv_thread->WaitForInternalThreadToExit();
    tsi_thread->WaitForInternalThreadToExit();
    tsv_thread->WaitForInternalThreadToExit();
    cool_thread->WaitForInternalThreadToExit();
    return 0;
}
