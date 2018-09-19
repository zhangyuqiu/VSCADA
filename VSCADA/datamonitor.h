#ifndef DATAMONITOR_H
#define DATAMONITOR_H
#include <sqlite3.h>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

class DataMonitor
{
public:
    DataMonitor();
    ~DataMonitor();
    int initThresholds();
    int checkThreshold(int dataPoint);

    typedef struct{
        int minimum;
        int maximum;
        string sensor;
    }threshold;

    vector<threshold> thresholdVector;
};

#endif // DATAMONITOR_H
