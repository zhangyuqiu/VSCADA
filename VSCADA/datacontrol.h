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
#include <bitset>
#include <iomanip>
#include "db_engine.h"
#include "typedefs.h"
class iocontrol;
#include "subsystemthread.h"
class SubsystemThread;
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
    DataControl(DataMonitor * mtr, gpio_interface *gpio, canbus_interface *can, usb7402_interface * usb, DB_Engine * db, vector<SubsystemThread *> threads, vector<system_state *> stts, vector<statemachine *> FSMs, int mode, vector<controlSpec *> ctrlSpecs, vector<meta *> sensors, vector<response> rsp);
    ~DataControl();

    int collectData();
    void setMode(int md);
    int write_to_CAN(controlSpec spec);
    int write_to_USB(controlSpec spec);
    int write_to_GPIO(controlSpec spec);
    int write_to_DYNO(controlSpec spec);
    int write_to_databaase(datapoint dp);
    int change_system_state(system_state * newState);
    uint32_t isolateData64(uint auxAddress, uint offset, uint64_t data);
    uint64_t LSBto64Spec(uint auxAddress, uint offset, uint64_t data);
    void init_meta_vector(vector<meta> vctr);
    void logMsg(string msg);
    int change_sampling_rate(controlSpec spec, int rate);
    vector<controlSpec *> get_control_specs();
    string get_curr_time();
    void saveSession(string name);

    // active submodule pointers
    DataMonitor * monitor;
    DB_Engine * dbase;
    gpio_interface * gpioInterface;
    canbus_interface * canInterface;
    usb7402_interface * usb7204;
    vector<response> responseVector;
    vector<system_state *> states;
    vector<SubsystemThread *> subsystems;
    vector<statemachine *> FSMs;
    vector<controlSpec *> controlSpecs;
    vector<meta *> sensorVector;

    // overall system mode
    string currState;
    int systemMode;
    string modeName;

public slots:
    void executeRxn(int rxnCode);
    void deactivateLog(system_state * prevstate);
    void receive_can_data(uint32_t addr, uint64_t arr);
    void receive_control_val(int data, controlSpec * spec);
signals:
    void pushGPIOData(response rsp);
    void sendCANData(int address, uint64_t data);
    void sendToUSB7204(uint8_t channel, float voltage);
    void deactivateState(system_state * prevstate);
    void activateState(system_state * newState);
    void updateFSM(statemachine * currFSM);
    void pushMessage(string msg);
};
#endif // DATACONTROL_H
