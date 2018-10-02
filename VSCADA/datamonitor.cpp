#include "datamonitor.h"

DataMonitor::DataMonitor(vector<meta> sensorMetaData, vector<response> responseData)
{
    //constructor
    metaVector = sensorMetaData;
    responseVector = responseData;
}

DataMonitor::~DataMonitor(){
    //destructor
}

int DataMonitor::initThresholds(vector<vector<string>> config_info){
    //initialize all thresholds from the config file
    //also populates threshold vectors
    uint16_t configSize = (uint16_t)config_info.size();
    for (int i = 0; i < configSize; i++){
        vector<string> configItem = config_info.at(i);
        if ((int)(configItem.size()) == SENSOR_DATA_SIZE){
            meta metaItem;
            metaItem.sensorIndex = (uint32_t)stoi(configItem.at(0));
            metaItem.defSamplingRate = (uint32_t)stoi(configItem.at(1));
            metaItem.subsystem = (uint32_t)stoi(configItem.at(2));
            metaItem.minimum = (uint32_t)stoi(configItem.at(3));
            metaItem.maximum = (uint32_t)stoi(configItem.at(4));

            metaVector.push_back(metaItem);
        }
    }
    return 0;
}

int DataMonitor::setMode(int md){
    mode = md;
    return 0;
}

int DataMonitor::checkThreshold(datapoint * pt){
    //check whether datapoint crosses thresholds
    if (pt->value > metaVector.at(pt->sensorIndex).maximum){
        initiateRxn(metaVector.at(pt->sensorIndex).maxRxnCode);
        pt->monitored = true;
        return 1;
    } else if (pt->value < metaVector.at(pt->sensorIndex).minimum){
        initiateRxn(metaVector.at(pt->sensorIndex).minRxnCode);
        pt->monitored = true;
        return 1;
    }
    pt->monitored = true;
    return 0;
}

int DataMonitor::initiateRxn(uint32_t rxnCode){
    // execute configured reactions here
    return 0;
}
