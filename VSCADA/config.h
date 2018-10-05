#ifndef CONFIG_H
#define CONFIG_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "typedefs.h"
#include "datamonitor.h"
#include "datacontrol.h"
#include "iocontrol.h"
#include "db_engine.h"
#include "tsi_thread.h"
#include "tsv_thread.h"
#include "cool_thread.h"
#include "canbus_interface.h"

class DataControl;
class DataMonitor;
class iocontrol;
class DB_Engine;

#define CONFIG_PRINTOUTS

using namespace std;

class Config
{
public:

    // member function declarations
    Config();
    ~Config();
    bool read_config_file_data(string configFile);
    vector<string> csv_split(string str);
    vector<string> split(string s, string delim);

    //sensor vectors
    vector<meta> GLVmeta;
    vector<meta> TSImeta;
    vector<meta> TSVmeta;
    vector<meta> COOLmeta;

    // submodule declarations
    DataControl * dataCtrl;
    DataMonitor * dataMtr;
    DB_Engine * dbase;
    iocontrol * ioControl;
    GLV_Thread * glv_thread;
    TSI_Thread * tsi_thread;
    TSV_Thread * tsv_thread;
    COOL_Thread * cool_thread;
    canbus_interface * canInterface;

};

#endif // CONFIG_H
