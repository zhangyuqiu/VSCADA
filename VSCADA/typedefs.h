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
#define TEST 2

#define DASH_DISP 0
#define BACK_DISP 1

#define SUM 0
#define DIFF 1
#define MUL 2
#define DIV 3
#define AVG 4
#define MAX 5
#define MIN 6

typedef struct{
    int exponent;
    double coefficient;
}poly;

typedef struct{
    int main;
    double val;
    uint offset;
    int endianness;
    int gpioPin;
    int checkRate;
    double calVal;
    int maxRxnCode;
    int minRxnCode;
    uint auxAddress;
    int i2cAddress;
    std::vector<uint32_t> i2cConfigs;
    uint8_t i2cReadPointer;
    int i2cReadDelay;
    int i2cDataField;
    int usbChannel;
    double minimum;
    double maximum;
    double calConst;
    int sensorIndex;
    int normRxnCode;

    int lutIndex;
    int senOperator;
    std::vector<std::string> opAliases;
    std::vector<void *> opSensors;
    std::vector<void *> dependencies;
    uint32_t primAddress;
    double calMultiplier;
    int state;
    std::vector<poly> calPolynomial;

    std::string unit;
    std::string subsystem;
    std::string sensorName;
    std::string alias;

    void updateVal(int newVal)
    {
        val = newVal;
    }

    void calData(){
        calVal = static_cast<double>(val)*calMultiplier + calConst;
    }
}meta;

typedef struct{
    uint offset;
    int gpiopin;
    bool slider;
    bool button;
    int pressVal;
    int maxslider;
    int minslider;
    int releaseVal;
    int usbChannel;
    uint auxAddress;
    bool textField;
    int endianness;
    uint primAddress;
    uint64_t sentVal;
    std::string name;
    std::string type;
    double multiplier;
}controlSpec;

typedef struct{
    int defVal;
    uint offset;
    int gpioPin;
    int gpioPair;
    int canValue;
    int gpioValue;
    uint auxAddress;
    std::string msg;
    uint primAddress;
    int responseIndex;
    int displayTarget;
    int endianness;
}response;

typedef struct{
    double val1;
    double val2;
    int rsp;
    bool active;
    int sensorId1;
    int sensorId2;
    std::string logicName;
}logic;

typedef struct{
    int value;
    uint offset;
    bool active;
    uint auxAddress;
    uint primAddress;
    std::string name;
    int endianness;
}system_state;

typedef struct{
    std::string name;
    uint auxAddress;
    uint offset;
    int value;
}condition;

typedef struct{
    uint offset;
    uint auxAddress;
    uint primAddress;
    int endianness;
    std::string name;
    std::vector<system_state *> states;
    std::vector<condition *> conditions;
}statemachine;

typedef struct{
    int address;
    uint64_t data;
    int dataSize;
}canItem;

typedef struct{
    int pin;
    int value;
    int mode;
}gpioItem;

typedef struct{
    std::vector<canItem> bootCanCmds;
    std::vector<uint32_t> bootI2cCmds;
    std::vector<gpioItem> bootGPIOCmds;
} bootloader;

#endif // TYPEDEFS_H
