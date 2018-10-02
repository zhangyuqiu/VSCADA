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

    // submodule declarations
    DataControl * dataCtrl;
    DataMonitor * dataMtr;
    DB_Engine * dbase;
    iocontrol * ioControl;
    GLV_Thread * glv_thread;
    TSI_Thread * tsi_thread;
    TSV_Thread * tsv_thread;
    COOL_Thread * cool_thread;

};

#endif // CONFIG_H