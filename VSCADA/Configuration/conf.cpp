#include "conf.h"

conf::conf(){

}

conf::~conf(){

}

bool conf::read_config_file_data(string configFile){
    ifstream inFile;
    inFile.open(configFile);
    vector<string> splitLine;
    vector<meta> sensorMetaData;
    vector<response> responseVector;
    cout << "Opening file" << endl;
    if(!inFile){
        string e =  "Unable to open file   " + configFile;
        cerr << e << endl;
        return false;
    }

    system_states * states = new system_states;

    string fileName = configFile;
    string line;

    while(getline(inFile,line)){
        splitLine = split(line,",");
start_comp:
        string arg = splitLine.at(0);
        if (arg.compare("$system") == 0){

            //get system mode
            getline(inFile,line);
            splitLine = split(line,",");
            if(splitLine.at(0).compare("car") == 0){
                states->mode = CAR;
            }else if(splitLine.at(0).compare("dyno") == 0){
                states->mode = DYNO;
            }else{
                // error in config file
                cout << "Error In Config File: system" << endl;
            }

            //get GLV min and max sampling rates
            getline(inFile,line);
            splitLine = split(line,",");
            if((int)splitLine.size() > 1){
                string bufRate = splitLine.at(0);
                states->GLV_max = (uint32_t)stoi(bufRate);
                bufRate = splitLine.at(1);
                states->GLV_min = (uint32_t)stoi(bufRate);
            } else {
                // error in config file
                cout << "Error In Config File: GLV Rates" << endl;
            }

            //get TSI min and max sampling rates
            getline(inFile,line);
            splitLine = split(line,",");
            if((int)splitLine.size() > 1){
                string bufRate = splitLine.at(0);
                states->TSI_max = (uint32_t)stoi(bufRate);
                bufRate = splitLine.at(1);
                states->TSI_min = (uint32_t)stoi(bufRate);
            } else {
                // error in config file
                cout << "Error In Config File: TSI Rates" << endl;
            }

            //get TSV min and max sampling rates
            getline(inFile,line);
            splitLine = split(line,",");
            if((int)splitLine.size() > 1){
                string bufRate = splitLine.at(0);
                states->TSV_max = (uint32_t)stoi(bufRate);
                bufRate = splitLine.at(1);
                states->TSV_min = (uint32_t)stoi(bufRate);
            } else {
                // error in config file
                cout << "Error In Config File: TSV Rates" << endl;
            }

            //get COOLING min and max sampling rates
            getline(inFile,line);
            splitLine = split(line,",");
            if((int)splitLine.size() > 1){
                string bufRate = splitLine.at(0);
                states->COOL_max = (uint32_t)stoi(bufRate);
                bufRate = splitLine.at(1);
                states->COOL_min = (uint32_t)stoi(bufRate);
            } else {
                // error in config file
                cout << "Error In Config File: COOLING Rates" << endl;
            }
#ifdef CONFIG_PRINTOUTS
            cout << "Mode: " << states->mode << " CAR" << endl;
            cout << "GLV max rate: " << states->GLV_max << endl;
            cout << "GLV min rate: " << states->GLV_min << endl;
            cout << "TSI max rate: " << states->TSI_max << endl;
            cout << "TSI min rate: " << states->TSI_min << endl;
            cout << "TSV max rate: " << states->TSV_max << endl;
            cout << "TSV min rate: " << states->TSV_min << endl;
            cout << "COOLING max rate: " << states->COOL_max << endl;
            cout << "COOLING min rate: " << states->COOL_min << endl;
            cout << endl;
#endif
        } else if (arg.compare("$sensors") == 0){

            // record configured sensors
            while(getline(inFile,line)){
                splitLine = split(line,",");
                arg = splitLine.at(0);
                if(arg[0] == '$') goto start_comp;
                meta bufMeta;
                bufMeta.sensorIndex = (uint32_t)stoi(splitLine.at(0));

                if (splitLine.at(2).compare("GLV") == 0) bufMeta.subsystem = GLV;
                else if (splitLine.at(2).compare("TSI") == 0) bufMeta.subsystem = TSI;
                else if (splitLine.at(2).compare("TSV") == 0) bufMeta.subsystem = TSV;
                else if (splitLine.at(2).compare("COOLING") == 0) bufMeta.subsystem = COOLING;
                else {
                    //error in config file
                    cout << "Error in config file: subsystem" << endl;
                }

                bufMeta.defSamplingRate = (uint32_t)stoi(splitLine.at(3));
                bufMeta.minimum = (uint32_t)stoi(splitLine.at(4));
                bufMeta.maximum = (uint32_t)stoi(splitLine.at(5));
                bufMeta.minRxnCode = (uint32_t)stoi(splitLine.at(6));
                bufMeta.maxRxnCode = (uint32_t)stoi(splitLine.at(7));
                sensorMetaData.push_back(bufMeta);

#ifdef CONFIG_PRINTOUTS
                cout << "Sensor index: " << bufMeta.sensorIndex << endl;
                cout << "Sensor subsystem: " << bufMeta.subsystem << endl;
                cout << "Sensor default sampling rate: " << bufMeta.defSamplingRate << endl;
                cout << "Sensor lower threshold: " << bufMeta.minimum << endl;
                cout << "Sensor upper threshold: " << bufMeta.maximum << endl;
                cout << "Sensor min reaction code: " << bufMeta.minRxnCode << endl;
                cout << "Sensor max reaction code: " << bufMeta.maxRxnCode << endl;
                cout << endl << "Sensor Recorded" << endl << endl;
#endif
            }
        } else if (arg.compare("$responses") == 0){

            // record configured responses
            while(getline(inFile,line)){
                splitLine = split(line,",");
                arg = splitLine.at(0);
                if(arg[0] == '$') goto start_comp;
                response bufResponse;
                bufResponse.responseIndex = stoi(splitLine.at(0));
                bufResponse.canAddress = stoi(splitLine.at(1));
                bufResponse.gpioPin = stoi(splitLine.at(2));
                if(splitLine.at(3).compare("dash_disp") == 0){
                    bufResponse.displayTarget = DASH_DISP;
                } else if (splitLine.at(3).compare("back_disp") == 0){
                    bufResponse.displayTarget = BACK_DISP;
                } else {
                    //error in config file
                    cout << "Error in config file: response section" << endl;
                }
                bufResponse.value = stoi(splitLine.at(4));
                responseVector.push_back(bufResponse);

#ifdef CONFIG_PRINTOUTS
                cout << "Response index: " << bufResponse.responseIndex << endl;
                cout << "Response CAN address: " << bufResponse.canAddress << endl;
                cout << "Response GPIO pin: " << bufResponse.responseIndex << endl;
                cout << "Response display target: " << bufResponse.displayTarget << endl;
                cout << "Response value: " << bufResponse.value << endl;
                cout << endl << "Response Recorded" << endl << endl;
#endif
            }
        }
    }
    inFile.close();

    return true;
}

vector<string> conf::split(string s, string delim){

    vector<string> splittedString;
    int startIndex = 0;
    int endIndex = 0;
    while((endIndex = s.find(delim,startIndex)) < s.size()){
        string val = s.substr(startIndex,endIndex - startIndex);
        splittedString.push_back(val);
        startIndex = endIndex + delim.size();
    }
    if(startIndex < s.size()){
        string val = s.substr(startIndex);
        splittedString.push_back(val);
    }
    return splittedString;

}

