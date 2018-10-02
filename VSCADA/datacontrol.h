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
#include "typedefs.h"

#include "datamonitor.h"
class DataMonitor;
#include "db_engine.h"
class DB_Engine;
#include "iocontrol.h"
class iocontrol;
#include "glv_thread.h"
class GLV_Thread;
#include "tsi_thread.h"
class TSI_Thread;
#include "tsv_thread.h"
class TSV_Thread;
#include "cool_thread.h"
class COOL_Thread;

#define OFF 0
#define IDLE_MODE 1
#define DRIVE_MODE 2

using namespace std;

class DataControl
{
public:

    // Member function declarations
    DataControl(DataMonitor * mtr, DB_Engine * db, iocontrol * io, GLV_Thread * glvTh,
                TSI_Thread * tsiTh, TSV_Thread * tsvTh, COOL_Thread * coolTh);
    ~DataControl();

    int collectData();
    void setMode(int md);
    int set_GLV_rate(int rate);
    int set_TSI_rate(int rate);
    int set_TSV_rate(int rate);
    int set_COOL_rate(int rate);
    int write_to_CAN(controlSpec spec);
    int write_to_USB(controlSpec spec);
    int write_to_GPIO(controlSpec spec);
    int write_to_DYNO(controlSpec spec);
    int write_to_databaase(datapoint dp);
    int change_system_state(int newState);
    int init_GLV_sensors(vector<meta> vctr);
    int init_TSI_sensors(vector<meta> vctr);
    int init_TSV_sensors(vector<meta> vctr);
    int init_COOL_sensors(vector<meta> vctr);
    void init_meta_vector(vector<meta> vctr);
    int change_sampling_rate(controlSpec spec, int rate);

    // active submodule pointers
    DataMonitor * monitor;
    iocontrol * ioControl;
    DB_Engine * dbase;
    GLV_Thread * glv_thread;
    TSI_Thread * tsi_thread;
    TSV_Thread * tsv_thread;
    COOL_Thread * cool_thread;

    // overall system mode
    int mode;

};
#endif // DATACONTROL_H
