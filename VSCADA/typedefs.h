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
    int exponent;
    double coefficient;
}poly;

typedef struct{
    int main;
    double val;
    int offset;
    int gpioPin;
    int checkRate;
    double calVal;
    int maxRxnCode;
    int minRxnCode;
    int auxAddress;
    int i2cAddress;
    std::vector<uint32_t> i2cConfigs;
    //uint8_t i2cConfigPointer;
    uint8_t i2cReadPointer;
    //uint16_t i2cConfigData;
    int i2cReadDelay;
    int i2cDataField;
    int usbChannel;
    double minimum;
    double maximum;
    double calConst;
    int sensorIndex;
    int normRxnCode;
    int primAddress;
    double calMultiplier;
    std::vector<poly> calPolynomial;

    std::string unit;
    std::string subsystem;
    std::string sensorName;

    void updateVal(int newVal)
    {
        val = newVal;
    }
    void calData(){
        calVal = val*calMultiplier + calConst;
    }
}meta;

typedef struct{
    int offset;
    int gpiopin;
    bool slider;
    bool button;
    int pressVal;
    int maxslider;
    int minslider;
    int releaseVal;
    int usbChannel;
    int auxAddress;
    bool textField;
    int primAddress;
    uint64_t sentVal;
    std::string name;
    std::string type;
    double multiplier;
}controlSpec;

typedef struct{
    int defVal;
    int offset;
    int gpioPin;
    int gpioPair;
    int canValue;
    int gpioValue;
    int auxAddress;
    std::string msg;
    int primAddress;
    int responseIndex;
    int displayTarget;
}response;

typedef struct{
    double val1;
    double val2;
    response rsp;
    meta * sensor1;
    meta * sensor2;
    std::string logicName;
}logic;

typedef struct{
    int value;
    int offset;
    bool active;
    int auxAddress;
    int primAddress;
    std::string name;
}system_state;

typedef struct{
    int offset;
    int auxAddress;
    int primAddress;
    std::string name;
    std::vector<system_state *> states;
}statemachine;

typedef struct{
    int canAddress;
    int trigger;
    std::string displayMsg;
    std::vector<uint64_t> configMsg;
} bootloader;

#endif // TYPEDEFS_H
