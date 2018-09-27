#ifndef DATAMONITOR_H
#define DATAMONITOR_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "typedefs.h"

#define GLV 0
#define TSI 1
#define TSV 2
#define COOLING 3
#define SENSOR_DATA_SIZE 6

using namespace std;

class DataMonitor
{
public:
    DataMonitor();
    ~DataMonitor();
    int initThresholds(vector<vector<string>> config_info);
    int checkThreshold(datapoint * pt);
    int initiateRxn(uint32_t rxnCode);

    vector<meta> metaVector;
};

#endif // DATAMONITOR_H
