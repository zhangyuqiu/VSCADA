#ifndef DATACONTROL_H
#define DATACONTROL_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stack>
#include "db_engine.h"
#include "typedefs.h"
#include "iocontrol.h"
class iocontrol;
#include "subsystemthread.h"
class SubsystemThread;

#define OFF 0
#define IDLE_MODE 1
#define DRIVE_MODE 2

using namespace std;

class DataControl
{
public:

    // Member function declarations
    DataControl(DataMonitor * mtr, DB_Engine * db, iocontrol * io, vector<SubsystemThread *> threads);
    ~DataControl();

    int collectData();
    void setMode(int md);
    int write_to_CAN(controlSpec spec);
    int write_to_USB(controlSpec spec);
    int write_to_GPIO(controlSpec spec);
    int write_to_DYNO(controlSpec spec);
    int write_to_databaase(datapoint dp);
    int change_system_state(int newState);
    void init_meta_vector(vector<meta> vctr);
    int change_sampling_rate(controlSpec spec, int rate);

    // active submodule pointers
    DataMonitor * monitor;
    iocontrol * ioControl;
    DB_Engine * dbase;

    // overall system mode
    int mode;

};
#endif // DATACONTROL_H
