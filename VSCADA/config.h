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
#include "canbus_interface.h"
#include "subsystemthread.h"
#include <rapidxml/rapidxml_utils.hpp>
#include <QtXml/QtXml>

class DataControl;
class DataMonitor;
class iocontrol;
class DB_Engine;
class SubsystemThread;

#define CONFIG_PRINTOUTS

using namespace std;
using namespace rapidxml;

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

    // submodule declarations
    DataControl * dataCtrl;
    DataMonitor * dataMtr;
    DB_Engine * dbase;
    iocontrol * ioControl;
    SubsystemThread * genericThread;
    canbus_interface * canInterface;
    std::vector<SubsystemThread *> subsystems;
};

#endif // CONFIG_H
