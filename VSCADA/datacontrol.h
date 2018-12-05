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
#include "subsystemthread.h"
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
                vector<SubsystemThread *> threads, vector<system_state *> stts, vector<statemachine *> FSMs,
                int mode, vector<controlSpec *> ctrlSpecs, vector<meta *> sensors, vector<response> rsp, vector<bootloader> bootArgs);
    ~DataControl();

    void setMode(int md);
    string get_curr_time();
    void logMsg(string msg);
    void startSystemTimer();
    string getProgramTime();
    void saveSession(string name);
//    void sendBootConfig(bootloader bl);
    int change_sampling_rate(int rate);
    vector<controlSpec *> get_control_specs();
    void init_meta_vector(vector<meta> vctr);
    int change_system_state(system_state * newState);
    uint64_t LSBto64Spec(uint auxAddress, uint offset, uint64_t data);
    uint32_t isolateData64(uint auxAddress, uint offset, uint64_t data, int endianness);

    // active submodule pointers
    DB_Engine * dbase;
    DataMonitor * monitor;
    usb7402_interface * usb7204;
    vector<statemachine *> FSMs;
    vector<meta *> sensorVector;
    QTime * systemTimer;
    vector<system_state *> states;
    gpio_interface * gpioInterface;
    canbus_interface * canInterface;
    vector<response> responseVector;
    vector<controlSpec *> controlSpecs;
    vector<SubsystemThread *> subsystems;
    vector<bootloader> bootConfigs;
    // overall system mode
    int systemMode;
    string modeName;
    string currState;

public slots:
    void receive_sensor_data(meta * sensor);
    void executeRxn(int responseIndex);
    void deactivateLog(system_state * prevstate);
    void receive_can_data(uint32_t addr, uint64_t arr);
    void receive_control_val(int data, controlSpec * spec);
    void passCANData(int address, uint64_t data, int size);
    void passI2cData(uint32_t data);
    void passGPIOData(int pin, int value);
signals:
    void pushMessage(string msg);
    void pushGPIOData(int pin, int value);
    void pushI2cData(uint32_t value);
    void updateFSM(statemachine * currFSM);
    void activateState(system_state * newState);
    void sendCANData(int address, uint64_t data);
    void sendCANDataByte(int address, uint64_t data, int size);
    void deactivateState(system_state * prevstate);
    void sendToUSB7204(uint8_t channel, float voltage, bool*);
    void updateEdits(meta *);
};
#endif // DATACONTROL_H
