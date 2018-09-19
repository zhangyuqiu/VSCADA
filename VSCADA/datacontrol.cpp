#include "datacontrol.h"

DataControl::DataControl()
{
    //constructor
}

DataControl::~DataControl()
{
    //destructor
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
