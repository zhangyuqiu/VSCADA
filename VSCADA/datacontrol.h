#ifndef DATACONTROL_H
#define DATACONTROL_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

#define OFF 0
#define IDLE_MODE 1
#define DRIVE_MODE 2

using namespace std;

class DataControl
{
public:
    DataControl();
    ~DataControl();

    typedef struct{
        string sensorName;
        string USB_path;
        int CAN_address;
        int GPIO_pin;
        int samplingRate;
    }controlSpec;

    int write_to_CAN(controlSpec spec);
    int write_to_USB(controlSpec spec);
    int write_to_GPIO(controlSpec spec);
    int write_to_DYNO(controlSpec spec);
    int change_system_state(int newState);
    int change_sampling_rate(controlSpec spec, int rate);

    typedef int state;
    state currState = 0;
    vector<controlSpec> specVector;
};

#endif // DATACONTROL_H
