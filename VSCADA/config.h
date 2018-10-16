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
#include "gpio_interface.h"
#include "canbus_interface.h"
#include "subsystemthread.h"
#include <QtXml/QtXml>

class DataControl;
class DataMonitor;
class iocontrol;
class DB_Engine;
class SubsystemThread;

#define CONFIG_PRINTOUTS

using namespace std;

class Config
{
public:

    // member function declarations
    Config();
    ~Config();
    bool read_config_file_data();

    //sensor vectors
    meta * storedSensor;
    vector<meta *> storedSensors;
    vector<meta *> canSensors;
    vector<meta *> gpioSensors;
    vector<meta *> i2cSensors;
    // submodule declarations
    DataControl * dataCtrl;
    DataMonitor * dataMtr;
    DB_Engine * dbase;
    iocontrol * ioControl;
    SubsystemThread * genericThread;
    canbus_interface * canInterface;
    gpio_interface * gpioInterface;
    std::vector<SubsystemThread *> subsystems;
};

#endif // CONFIG_H
