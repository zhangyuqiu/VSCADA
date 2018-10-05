#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#define CAR 0
#define DYNO 1
#define DASH_DISP 0
#define BACK_DISP 1
#define GLV 0
#define TSI 1
#define TSV 2
#define COOLING 3

typedef struct{
    int sensorIndex;
    int minimum;
    int maximum;
    int subsystem;
    int defSamplingRate;
    int maxRxnCode;
    int minRxnCode;
}meta;

typedef struct{
    uint32_t sensorIndex;
    uint32_t value;
    bool monitored;
    bool displayed;
    std::string timestamp;
}datapoint;

typedef struct{
    std::string sensorName;
    std::string USB_path;
    int CAN_address;
    int GPIO_pin;
    int samplingRate;
}controlSpec;

typedef struct{
    int responseIndex;
    int canAddress;
    int gpioPin;
    int displayTarget;
    int value;
}response;

typedef struct{
    int mode;
    int GLV_max;
    int GLV_min;
    int TSI_max;
    int TSI_min;
    int TSV_max;
    int TSV_min;
    int COOL_max;
    int COOL_min;
}system_states;

#endif // TYPEDEFS_H
