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
#include "db_engine.h"
#include "gpio_interface.h"
#include "canbus_interface.h"
#include "subsystemthread.h"
#include <QtXml/QtXml>
#include "usb7402_interface.h"

class DataControl;
class DataMonitor;
class iocontrol;
class DB_Engine;
class SubsystemThread;

#define CONFIG_PRINT

using namespace std;

class Config
{
public:

    // member function declarations
    Config();
    ~Config();
    bool read_config_file_data();
    bool isInteger(const string & s);
    string get_curr_time();

    //sensor vectors
    meta * storedSensor;
    vector<string> configErrors;
    vector<meta *> storedSensors;
    vector<meta *> canSensors;
    vector<meta *> gpioSensors;
    vector<meta *> usbSensors;
    vector<meta *> i2cSensors;
    system_state * thisState;
    vector<system_state *> sysStates;
    controlSpec * currSpec;
    statemachine * thisFSM;
    vector<statemachine *> FSMs;
    // submodule declarations
    DataControl * dataCtrl;
    DataMonitor * dataMtr;
    DB_Engine * dbase;
    iocontrol * ioControl;
    SubsystemThread * genericThread;
    canbus_interface * canInterface;
    gpio_interface * gpioInterface;
    usb7402_interface * usb7204;
    vector<SubsystemThread *> subsystems;
    vector<controlSpec *> controlSpecs;
    int systemMode;
};

#endif // CONFIG_H
