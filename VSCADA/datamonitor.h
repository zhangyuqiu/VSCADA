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
    int initiateRxn(int rxnCode, meta * sensor);
    int setMode(int md);
    string get_curr_time();

    // global vectors
    vector<meta> metaVector;
    vector<response> responseVector;

    // Overall system mode
    int mode;

};

#endif // DATAMONITOR_H
