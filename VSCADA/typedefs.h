#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

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

#endif // TYPEDEFS_H
