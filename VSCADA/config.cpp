#include "config.h"

Config::Config(){
}

Config::~Config(){

}

bool Config::read_config_file_data(string configFile){

    vector<vector<meta *>> sensorVector;
    vector<meta> allSensors;
    vector<response> allResponses;
    vector<int> minrates;

    qDebug("Inside the PARSE Slot");
        QDomDocument doc;
        QFile f("config.xml");
        if(!f.open(QIODevice::ReadOnly))
        {
            qDebug("Error While Reading the File");
        }

        doc.setContent(&f);
        f.close();
        qDebug("File was closed Successfully");


    // print out the element names of all elements that are direct children
    // of the outermost element.
    QDomNodeList subsystemNodes = doc.elementsByTagName("subsystem");

    cout << "Number of subsystems: " << subsystemNodes.size() << endl;
    for(int i =0; i < static_cast<int>(subsystemNodes.size()); i++){
        int minrate = 0;
        int maxrate = 0;
        string subSystemId;
        vector<meta *> sensors;
        cout << "subsystem: " << qPrintable(subsystemNodes.at(i).firstChild().firstChild().nodeValue()) << endl;

        //get subsystem characteristics: subsystemId, minrate and maxrate
        QDomNodeList subsystemXteristics = subsystemNodes.at(i).childNodes();
        for (int j = 0; j < subsystemXteristics.size(); j++){
            if (subsystemXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                subSystemId = subsystemXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if(subsystemXteristics.at(j).nodeName().toStdString().compare("minrate") == 0){
                minrate = stoi(subsystemXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (subsystemXteristics.at(j).nodeName().toStdString().compare("maxrate") == 0){
                maxrate = stoi(subsystemXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (subsystemXteristics.at(j).nodeName().toStdString().compare("sensors") == 0){
                QDomNodeList sensorsList = subsystemXteristics.at(j).childNodes();
                for (int k = 0; k < sensorsList.size(); k++){
                    meta thisSensor;
                    storedSensor = new meta;
                    storedSensor->subsystem = subSystemId;
                    QDomNode sensor = sensorsList.at(k);
                    cout << "Sensor Name: " << sensor.firstChild().firstChild().nodeValue().toStdString() << endl;
                    QDomNodeList attributeList = sensor.childNodes();
                    for (int m = 0; m < attributeList.size(); m++){
                        if(attributeList.at(m).nodeName().toStdString().compare("name") == 0){
//                            thisSensor.sensorName = attributeList.at(m).firstChild().nodeValue().toStdString();
                            storedSensor->sensorName = attributeList.at(m).firstChild().nodeValue().toStdString();
                        } else if (attributeList.at(m).nodeName().toStdString().compare("canaddress") == 0){
//                            thisSensor.canAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->canAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minimum") == 0){
//                            thisSensor.minimum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->minimum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maximum") == 0){
//                            thisSensor.maximum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->maximum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minreaction") == 0){
//                            thisSensor.minRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->minRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maxreaction") == 0){
//                            thisSensor.maxRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->maxRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("checkrate") == 0){
//                            thisSensor.canAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            storedSensor->checkRate = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        }
                        storedSensor->val = 0;
                    }
                    storedSensors.push_back(storedSensor);
                    sensors.push_back(storedSensor);
                    allSensors.push_back(thisSensor);
                }

                cout << "All Sensors: " << sensors.size() << endl;
                for (int n = 0; n < sensors.size(); n++){
                    cout << "Sensor Name: " << storedSensors.at(n)->sensorName << endl;
                    cout << "Sensor Max: " << storedSensors.at(n)->maximum << endl;
                    cout << "Sensor Min: " << storedSensors.at(n)->minimum << endl;
                    cout << "Sensor MaxRxn: " << storedSensors.at(n)->maxRxnCode << endl;
                    cout << "Sensor MinRxn: " << storedSensors.at(n)->minRxnCode << endl;
                }

            }
        }
        cout << "subsystem ID: " << subSystemId << endl;
        cout << "subsystem min: " << minrate << endl;
        cout << "subsystem max: " << maxrate << endl;
        SubsystemThread * thread = new SubsystemThread(sensors,subSystemId);
        subsystems.push_back(thread);
        sensorVector.push_back(sensors);
        minrates.push_back(minrate);

    }

    //********************************************************//
    //              PREPARE DATABASE INIT SCRIPT
    //********************************************************//

    ofstream dbScript;
    dbScript.open ("script.sql");

    // write create universal tables
    dbScript << "create table if not exists system_log(" << endl;
    dbScript << "time char not null," << endl;
    dbScript << "reactionId char not null," << endl;
    dbScript << "message char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists sensors(" << endl;
    dbScript << "sensorIndex char not null," << endl;
    dbScript << "sensorName char not null," << endl;
    dbScript << "subsystem char not null," << endl;
    dbScript << "minThreshold char not null," << endl;
    dbScript << "maxThreshold char not null," << endl;
    dbScript << "maxReactionId char not null," << endl;
    dbScript << "minReactionId char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists reactions(" << endl;
    dbScript << "reactionId char not null," << endl;
    dbScript << "message char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists system_info(" << endl;
    dbScript << "runId char not null," << endl;
    dbScript << "startTime char not null," << endl;
    dbScript << "endTime char not null" << endl;
    dbScript << ");" << endl;

    //create subsystem tables
    for (int i = 0; i < subsystems.size(); i++){
        string scriptTableArg = "create table if not exists " + subsystems.at(i)->subsystemId + "_data(";
        dbScript << scriptTableArg << endl;
        dbScript << "time char not null," << endl;
        dbScript << "sensorindex char not null," << endl;
        dbScript << "sensorName char not null," << endl;
        dbScript << "value char not null" << endl;
        dbScript << ");" << endl;
    }
    dbScript.close();

    //run script
    dbase = new DB_Engine();
    dbase->runScript("script.sql");
    //************************FINISH**************************//


    dataMtr = new DataMonitor(allSensors,allResponses);
    canInterface = new canbus_interface(storedSensors,"STUFF",subsystems);
    for (int i = 0; i < subsystems.size(); i++){
        SubsystemThread * thread = subsystems.at(i);
        thread->setMonitor(dataMtr);
        thread->set_rate(minrates.at(i));
        thread->setDB(dbase);
        subsystems.at(i)->start();
    }
    return true;
}

vector<string> Config::split(string s, string delim){
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
