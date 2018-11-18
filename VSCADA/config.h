#ifndef CONFIG_H
#define CONFIG_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include "typedefs.h"
#include "datacontrol.h"
#include "db_engine.h"
#include "gpio_interface.h"
#include "canbus_interface.h"
#include "subsystemthread.h"
#include <QtXml/QtXml>
#include "usb7402_interface.h"


#define CONFIG_PRINT

using namespace std;

class Config
{
public:

    // member function declarations
    Config();
    ~Config();
    string get_curr_time();
    bool read_config_file_data();
    bool isInteger(const string & s);
    string removeSpaces(string &str);

    //system mode : CAR or DYNO
    int systemMode;

    //dummy variables
    meta * storedSensor;
    controlSpec * currSpec;
    statemachine * thisFSM;
    system_state * thisState;

    //data vectors
    vector<meta *> canSensors;
    vector<meta *> usbSensors;
    vector<meta *> i2cSensors;
    vector<meta *> gpioSensors;
    vector<string> configErrors;
    vector<meta *> storedSensors;
    vector<meta *> mainSensors;

    vector<statemachine *> FSMs;
    vector<system_state *> sysStates;
    vector<controlSpec *> controlSpecs;

    // submodule declarations
    DB_Engine * dbase;
    DataControl * dataCtrl;
    usb7402_interface * usb7204;
    gpio_interface * gpioInterface;
    canbus_interface * canInterface;
    vector<SubsystemThread *> subsystems;

};
#endif // CONFIG_H
