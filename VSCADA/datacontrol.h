#ifndef DATACONTROL_H
#define DATACONTROL_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stack>
#include <bitset>
#include <iomanip>
#include <map>
#include "db_engine.h"
#include "typedefs.h"
#include "group.h"
#include "usb7402_interface.h"
#include "gpio_interface.h"
#include "canbus_interface.h"

#define OFF 0
#define IDLE_MODE 1
#define DRIVE_MODE 2

using namespace std;

class DataControl : public QObject
{
    Q_OBJECT
public:
    // Member function declarations
    DataControl(gpio_interface *gpio, canbus_interface *can, usb7402_interface * usb, DB_Engine * db,
                map<string, Group *> subMap, vector<system_state *> stts, vector<statemachine *> FSMs,
                int mode, vector<controlSpec *> ctrlSpecs, map<int,response> rspMap, vector<bootloader> bootArgs,
                map<uint32_t, vector<meta *> *> canMap, bootloader boot, vector<canItem> cSyncs, vector<i2cItem> iSyncs,
                vector<gpioItem> gSyncs, map<int, recordwindow *> recSenWins, map<int, recordwindow *> recStateWins, map<int, meta *> sensMap);
    ~DataControl();

    void setMode(int md);
    string get_curr_time();
    string get_curr_date();
    void startSystemTimer();
    string getProgramTime();
    void saveSession(string name);
    vector<controlSpec *> get_control_specs();
    void init_meta_vector(vector<meta> vctr);
    int change_system_state(system_state * newState);
    uint64_t LSBto64Spec(uint auxAddress, uint offset, uint64_t data);
    uint32_t isolateData64(uint auxAddress, uint offset, uint64_t data, int endianness);
    void receiveData(meta * currSensor);
    void calibrateData(meta * currSensor);
    void checkThresholds(meta * sensor);
    void logData(meta * currSensor);
    void checkSensorRecordTriggers(meta * currSensor);
    void checkStateRecordTriggers(recordwindow *recWindow);
    void incrementSessionNumber();
    string removeSpaces(string &str);
    void save_all_data();

    // active submodule pointers
    DB_Engine * dbase;
    DB_Engine * customDB;
    DataMonitor * monitor;
    usb7402_interface * usb7204;
    vector<statemachine *> FSMs;
    map<int,meta *> sensorMap;
    QTime * systemTimer;
    vector<system_state *> states;
    gpio_interface * gpioInterface;
    canbus_interface * canInterface;
    map<int,response> responseMap;
    map<int,recordwindow *> recordSensorMap;
    map<int,recordwindow *> recordStateMap;
    map<int,DB_Engine*> recordDBMap;
    map<int,string> recordColStrings;
    vector<controlSpec *> controlSpecs;
    map<string, Group *> subsystemMap;
    vector<bootloader> bootConfigs;
    map<uint32_t,vector<meta *> *> canSensorGroup;
    map<int, canItem> canSyncs;
    map<int, i2cItem> i2cSyncs;
    map<int, gpioItem> gpioSyncs;

    QTimer * syncTimer;
    QTimer * watchdogTimer;
    map<int, QTimer *> canSyncTimers;
    map<int, QTimer *> i2cSyncTimers;
    map<int, QTimer *> gpioSyncTimers;


    // overall system status info
    int systemMode;
    int sessionNumber = 0;
    string modeName;
    string currState;
    bootloader bootCmds;

public slots:
    void bootSubsystem();
    void receive_sensor_data(meta * sensor);
    void executeRxn(int responseIndex);
    void deactivateLog(system_state * prevstate);
    void receive_can_data(uint32_t addr, uint64_t arr);
    void receive_control_val(int data, controlSpec * spec);
    void canSyncSlot();
    void i2cSyncSlot();
    void gpioSyncSlot();
    void feedWatchdog();
signals:
    void sendI2CData(int address, int data);
    void pushI2cData(uint32_t value);
    void pushMessage(string msg);
    void pushGPIOData(int pin, int value);
    void updateFSM(statemachine * currFSM);
    void activateState(system_state * newState);
    void sendCANData(int address, uint64_t data);
    void sendCANDataByte(int address, uint64_t data, int size);
    void deactivateState(system_state * prevstate);
    void sendToUSB7204(uint8_t channel, float voltage, bool*);
    void updateEdits(meta *);
    void updateDisplay(meta * sensor);
//    void logData(meta * currSensor);
    void updateEditColor(string color, meta *sensor);
};
#endif // DATACONTROL_H
