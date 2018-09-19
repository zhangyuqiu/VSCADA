#include "datamonitor.h"

DataMonitor::DataMonitor()
{
    //constructor
}

DataMonitor::~DataMonitor(){
    //destructor
}

int DataMonitor::initThresholds(){
    //initialize all thresholds from the database
    //also populates threshold vectors
    return 0;
}

int DataMonitor::checkThreshold(int dataPoint){
    //check whether datapoint is withing thresholds
    return 0;
}
