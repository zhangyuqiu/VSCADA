#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#define DYNO 0
#define CAR 1
#define DASH_DISP 0
#define BACK_DISP 1
#define GLV 0
#define TSI 1
#define TSV 2
#define COOLING 3

typedef struct{
    int sensorIndex;
    std::string sensorName;
    double minimum;
    double maximum;
    std::string subsystem;
    std::string unit;
    int checkRate;
    int maxRxnCode;
    int minRxnCode;
    int normRxnCode;
    int primAddress;
    int auxAddress;
    int offset;
    int main;
    int i2cAddress;
    int gpioPin;
    int val;
    double calConst;
    double calVal;
    void calData(){
        calVal = static_cast<double>(val)*calConst;
    }
    void updateVal(int newVal)
      {
        val = newVal;
      }
}meta;

typedef struct{
    int sensorIndex;
    int value;
    bool monitored;
    bool displayed;
    int primAddress;
    int auxAddress;
    int offset;
    int gpioPin;
    std::string timestamp;
}datapoint;

typedef struct{
    std::string name;
    std::string type;
    bool slider;
    bool button;
    int primAddress;
    int auxAddress;
    int offset;
    int gpiopin;
    int maxslider;
    int minslider;
    int pressVal;
    int releaseVal;
}controlSpec;

typedef struct{
    int responseIndex;
    int displayTarget;
    std::string msg;
    int canValue;
    int gpioValue;
    int defVal;
    int primAddress;
    int auxAddress;
    int offset;
    int gpioPin;
    int gpioPair;
}response;

typedef struct{
    std::string logicName;
    meta * sensor1;
    meta * sensor2;
    double val1;
    double val2;
    response rsp;
}logic;

typedef struct{
    std::string name;
    int primAddress;
    int auxAddress;
    int offset;
    int value;
    bool active;
}system_state;

typedef struct{
    int primAddress;
    int auxAddress;
    int offset;
    std::string name;
    std::vector<system_state *> states;
}statemachine;

#endif // TYPEDEFS_H
