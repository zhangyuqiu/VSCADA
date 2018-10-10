#include "config.h"

Config::Config(){
}

Config::~Config(){

}

bool Config::read_config_file_data(string configFile){

    vector<vector<meta>> sensorVector;
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
        vector<meta> sensors;
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
                    QDomNode sensor = sensorsList.at(k);
                    QDomNodeList attributeList = sensor.childNodes();
                    for (int m = 0; m < attributeList.size(); m++){
                        if(attributeList.at(m).nodeName().toStdString().compare("name") == 0){
                            thisSensor.sensorName = attributeList.at(m).firstChild().nodeValue().toStdString();
                        } else if (attributeList.at(m).nodeName().toStdString().compare("canaddress") == 0){
                            thisSensor.canAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minimum") == 0){
                            thisSensor.minimum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maximum") == 0){
                            thisSensor.maximum = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minreaction") == 0){
                            thisSensor.minRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maxreaction") == 0){
                            thisSensor.maxRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        }
                    }
                    sensors.push_back(thisSensor);
                    allSensors.push_back(thisSensor);
                }

                for (int n = 0; n < sensors.size(); n++){
                    cout << "Sensor Name: " << sensors.at(n).sensorName << endl;
                    cout << "Sensor Max: " << sensors.at(n).maximum << endl;
                    cout << "Sensor Min: " << sensors.at(n).minimum << endl;
                    cout << "Sensor MaxRxn: " << sensors.at(n).maxRxnCode << endl;
                    cout << "Sensor MinRxn: " << sensors.at(n).minRxnCode << endl;
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

    dataMtr = new DataMonitor(allSensors,allResponses);
    canInterface = new canbus_interface(allSensors,"STUFF");
    for (int i = 0; i < subsystems.size(); i++){
        SubsystemThread * thread = subsystems.at(i);
        thread->setCAN(canInterface);
        thread->setMonitor(dataMtr);
        thread->set_rate(minrates.at(i));
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
