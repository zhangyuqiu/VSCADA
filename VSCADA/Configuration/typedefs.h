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
    uint32_t sensorIndex;
    uint32_t minimum;
    uint32_t maximum;
    uint32_t subsystem;
    uint32_t defSamplingRate;
    uint32_t maxRxnCode;
    uint32_t minRxnCode;
}meta;

typedef struct{
    uint32_t sensorIndex;
    uint32_t value;
    bool monitored;
    bool displayed;
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
    uint32_t mode;
    uint32_t GLV_max;
    uint32_t GLV_min;
    uint32_t TSI_max;
    uint32_t TSI_min;
    uint32_t TSV_max;
    uint32_t TSV_min;
    uint32_t COOL_max;
    uint32_t COOL_min;
}system_states;

#endif // TYPEDEFS_H
