#ifndef DATAMONITOR_H
#define DATAMONITOR_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include "typedefs.h"

#define SENSOR_DATA_SIZE 6

using namespace std;

class DataMonitor
{
public:

    // Member function declarations
    DataMonitor(vector<meta> sensorMetaData, vector<response> responseData);
    ~DataMonitor();
    int initThresholds(vector<vector<string>> config_info);
    int checkThreshold(datapoint * pt);
    int initiateRxn(uint32_t rxnCode);
    int setMode(int md);

    // global vectors
    vector<meta> metaVector;
    vector<response> responseVector;

    // Overall system mode
    int mode;

};

#endif // DATAMONITOR_H
