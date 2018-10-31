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
            metaItem.checkRate = (uint32_t)stoi(configItem.at(1));
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
//    if (pt->value > metaVector.at(pt->sensorIndex).maximum){
//        initiateRxn(metaVector.at(pt->sensorIndex).maxRxnCode);
//        pt->monitored = true;
//        return 1;
//    } else if (pt->value < metaVector.at(pt->sensorIndex).minimum){
//        initiateRxn(metaVector.at(pt->sensorIndex).minRxnCode,);
//        pt->monitored = true;
//        return 1;
//    }
//    pt->monitored = true;
//    return 0;
}

int DataMonitor::initiateRxn(int rxnCode, meta *sensor){
    //print to log
    vector<string> cols;
    vector<string> rows;
    cols.push_back("time");
    cols.push_back("reactionId");
    cols.push_back("message");
    rows.push_back(get_curr_time());
    rows.push_back(to_string(rxnCode));
    for (int i = 0; i < responseVector.size(); i++){
        response rsp = responseVector.at(i);
        if (rsp.responseIndex == rxnCode){
            rows.push_back(rsp.msg);
            if (rsp.primAddress >= 0){

            }
        }
    }

    return 0;
}

string DataMonitor::get_curr_time(){
    time_t t = time(0);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%X",&now);
    return buf;
}
