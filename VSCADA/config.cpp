#include "config.h"

Config::Config(){
}

Config::~Config(){

}

bool Config::read_config_file_data(){

    vector<vector<meta *>> sensorVector;
    vector<meta> allSensors;
    vector<response> allResponses;
    vector<int> minrates;
    vector<logic> logicVector;
//    vector<statemachine> FSMs;

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

     QDomDocument respDoc;
     QFile fl("config_responses.xml");
     if(!fl.open(QIODevice::ReadOnly))
     {
         qDebug("Error While Reading the File");
     }

     respDoc.setContent(&fl);
     fl.close();

     QDomDocument logicDoc;
     QFile fl2("config_logic.xml");
     if(!fl2.open(QIODevice::ReadOnly))
     {
         qDebug("Error While Reading the File");
     }

     logicDoc.setContent(&fl2);
     fl2.close();

    QDomNodeList mode = logicDoc.elementsByTagName("mode");
    QDomNodeList systemControls = doc.elementsByTagName("systemcontrols");
    QDomNodeList responseNodes = respDoc.elementsByTagName("response");
    QDomNodeList subsystemNodes = doc.elementsByTagName("subsystem");
    QDomNodeList systemStates = doc.elementsByTagName("systemstate");
    QDomNodeList stateMachines = doc.elementsByTagName("statemachine");
    QDomNodeList systemLogic = logicDoc.elementsByTagName("logic");

    cout << "Number of responses: " << responseNodes.size() << endl;
    cout << "Number of system controls: " << systemControls.size() << endl;
    cout << "Number of subsystems: " << subsystemNodes.size() << endl;
    cout << "Number of logic configurations: " << systemLogic.size() << endl;
    cout << "Number of system states: " << systemStates.size() << endl;
    cout << "Number of state machines: " << stateMachines.size() << endl;

    for (int i = 0; i < mode.size(); i++){
        if(mode.at(i).firstChild().nodeValue().toStdString().compare("0") == 0){
            systemMode = DYNO;
        } else if(mode.at(i).firstChild().nodeValue().toStdString().compare("1") == 0){
            systemMode = CAR;
        }
    }

    for (int i = 0; i < systemControls.size(); i++){
        QDomNodeList ctrlSpecs = systemControls.at(i).childNodes();
        for (int j = 0; j < ctrlSpecs.size(); j++){
            currSpec = new controlSpec;
            currSpec->button = false;
            currSpec->slider = false;
            currSpec->textField = false;
            currSpec->minslider = -1;
            currSpec->maxslider = -1;
            currSpec->pressVal = -1;
            currSpec->releaseVal = -1;
            currSpec->primAddress = -1;
            currSpec->auxAddress = -1;
            currSpec->offset = -1;

            QDomNodeList controlXteristics = ctrlSpecs.at(j).childNodes();
            for (int k = 0; k < controlXteristics.size(); k++){
                if (controlXteristics.at(k).nodeName().toStdString().compare("name") == 0){
                    currSpec->name = controlXteristics.at(k).firstChild().nodeValue().toStdString();
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("primaddress") == 0){
                    currSpec->primAddress = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("auxaddress") == 0){
                    currSpec->auxAddress = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("offset") == 0){
                    currSpec->offset = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("gpiopin") == 0){
                    currSpec->gpiopin = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                }else if (controlXteristics.at(k).nodeName().toStdString().compare("minrange") == 0){
                    currSpec->minslider = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("maxrange") == 0){
                    currSpec->maxslider = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("type") == 0){
                    if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("button") == 0){
                        currSpec->button = true;
                    } else if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("slider") == 0){
                        currSpec->slider = true;
                    } else if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("textfield") == 0){
                        currSpec->textField = true;
                    }
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("pressvalue") == 0){
                    currSpec->pressVal = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("releasevalue") == 0){
                    currSpec->releaseVal = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                }
            }
            controlSpecs.push_back(currSpec);
        }
    }

    for (int i = 0; i < stateMachines.size(); i++){
        thisFSM = new statemachine;
        QDomNodeList machineXteristics = stateMachines.at(i).childNodes();
        for (int j = 0; j < machineXteristics.size(); j++){
            if (machineXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisFSM->name = machineXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                thisFSM->primAddress = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                thisFSM->auxAddress = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                thisFSM->offset = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
            }else if (machineXteristics.at(j).nodeName().toStdString().compare("state") == 0){
                QDomNodeList stateXteristics = machineXteristics.at(j).childNodes();
                thisState = new system_state;
                thisState->primAddress = thisFSM->primAddress;
                thisState->auxAddress = thisFSM->auxAddress;
                thisState->offset = thisFSM->offset;
                for (int k = 0; k < stateXteristics.size(); k++){
                    if (stateXteristics.at(k).nodeName().toStdString().compare("name") == 0){
                        thisState->name = stateXteristics.at(k).firstChild().nodeValue().toStdString();
                    } else if (stateXteristics.at(k).nodeName().toStdString().compare("value") == 0){
                        thisState->value = stoi(stateXteristics.at(k).firstChild().nodeValue().toStdString());
                    }
                }
                thisFSM->states.push_back(thisState);
            }
        }
        FSMs.push_back(thisFSM);
    }

    for (int i = 0; i < FSMs.size(); i++){
        cout << "state machine states: " << FSMs.at(i)->states.size() << endl;
        cout << "state machine name: " << FSMs.at(i)->name << endl;
        cout << "state machine address: " << FSMs.at(i)->primAddress << endl;
    }

    for (int i = 0; i < systemStates.size(); i++){
        thisState = new system_state;
        QDomNodeList stateXteristics = systemStates.at(i).childNodes();
        for (int j = 0; j < stateXteristics.size(); j++){
            if (stateXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisState->name = stateXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (stateXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                thisState->primAddress = stoi(stateXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (stateXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                thisState->auxAddress = stoi(stateXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (stateXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                thisState->offset = stoi(stateXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (stateXteristics.at(j).nodeName().toStdString().compare("value") == 0){
                thisState->value = stoi(stateXteristics.at(j).firstChild().nodeValue().toStdString());
            }
        }
        sysStates.push_back(thisState);
    }

    for (int i = 0; i < responseNodes.size(); i++){
        QDomNodeList responseXteristics = responseNodes.at(i).childNodes();
        response thisRsp;
        thisRsp.primAddress = -1;
        thisRsp.auxAddress = -1;
        thisRsp.offset = -1;
        thisRsp.gpioPin = -1;
        thisRsp.canValue = -1;
        thisRsp.gpioValue = -1;
        for (int j = 0; j < responseXteristics.size(); j++){
            if (responseXteristics.at(j).nodeName().toStdString().compare("id") == 0){
                thisRsp.responseIndex = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("description") == 0){
                thisRsp.msg = responseXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                thisRsp.primAddress = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                thisRsp.auxAddress = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                thisRsp.offset = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("gpiopin") == 0){
                thisRsp.gpioPin = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("gpioval") == 0){
                thisRsp.gpioValue = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("canval") == 0){
                thisRsp.canValue = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
            }
        }
        allResponses.push_back(thisRsp);
    }

    for (uint i = 0; i < allResponses.size(); i++){
        cout << "Response ID: " << allResponses.at(i).responseIndex << endl;
        cout << "description: " << allResponses.at(i).msg << endl;
        cout << "can prim address: " << allResponses.at(i).primAddress << endl;
        cout << "can aux address: " << allResponses.at(i).auxAddress << endl;
        cout << "can offset: " << allResponses.at(i).offset << endl;
        cout << "can value: " << allResponses.at(i).canValue << endl;
        cout << "gpio pin: " << allResponses.at(i).gpioPin << endl;
        cout << "gpio value: " << allResponses.at(i).gpioValue << endl << endl;
    }

    for (int i = 0; i < static_cast<int>(subsystemNodes.size()); i++){
        int minrate = 0;
        int maxrate = 0;
        string subSystemId;
        vector<meta *> sensors;
        vector<meta *> mainMeta;
        cout << endl << "subsystem: " << qPrintable(subsystemNodes.at(i).firstChild().firstChild().nodeValue()) << endl;
        cout << "subsystem ID: " << subSystemId << endl;
        cout << "subsystem min: " << minrate << endl;
        cout << "subsystem max: " << maxrate << endl;

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
                    storedSensor->val = 0;
                    storedSensor->calVal = 0;
                    storedSensor->main = 0;
                    storedSensor->sensorIndex = -1;
                    storedSensor->minimum = -1;
                    storedSensor->maximum = -1;
                    storedSensor->checkRate = -1;
                    storedSensor->maxRxnCode = -1;
                    storedSensor->minRxnCode = -1;
                    storedSensor->normRxnCode = -1;
                    storedSensor->primAddress = -1;
                    storedSensor->auxAddress = -1;
                    storedSensor->offset = -1;
                    storedSensor->i2cAddress = -1;
                    storedSensor->gpioPin = -1;
                    storedSensor->calConst = -1;
                    storedSensor->subsystem = subSystemId;
                    QDomNode sensor = sensorsList.at(k);
                    QDomNodeList attributeList = sensor.childNodes();
                    for (int m = 0; m < attributeList.size(); m++){
                        if(attributeList.at(m).nodeName().toStdString().compare("name") == 0){
                            storedSensor->sensorName = attributeList.at(m).firstChild().nodeValue().toStdString();
                        } else if(attributeList.at(m).nodeName().toStdString().compare("unit") == 0){
                            storedSensor->unit = attributeList.at(m).firstChild().nodeValue().toStdString();
                        } else if (attributeList.at(m).nodeName().toStdString().compare("id") == 0){
                            storedSensor->sensorIndex = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("primaddress") == 0){
                            storedSensor->primAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            canSensors.push_back(storedSensor);
                        } else if (attributeList.at(m).nodeName().toStdString().compare("auxaddress") == 0){
                            storedSensor->auxAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("offset") == 0){
                            storedSensor->offset = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minimum") == 0){
                            storedSensor->minimum = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("main") == 0){
                            storedSensor->main = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maximum") == 0){
                            storedSensor->maximum = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minreaction") == 0){
                            storedSensor->minRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maxreaction") == 0){
                            storedSensor->maxRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("normreaction") == 0){
                            storedSensor->normRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("checkrate") == 0){
                            storedSensor->checkRate = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("multiplier") == 0){
                            storedSensor->calConst = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("gpiopin") == 0){
                            storedSensor->gpioPin = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            gpioSensors.push_back(storedSensor);
                        } else if (attributeList.at(m).nodeName().toStdString().compare("i2caddress") == 0){
                            storedSensor->i2cAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            i2cSensors.push_back(storedSensor);
                        }
                    }
                    storedSensors.push_back(storedSensor);
                    sensors.push_back(storedSensor);
                    allSensors.push_back(thisSensor);
                    if (storedSensor->main == 1){
                        mainMeta.push_back(storedSensor);
                    }
                }

            }
        }
        SubsystemThread * thread = new SubsystemThread(sensors,subSystemId,allResponses,logicVector,mainMeta);
        subsystems.push_back(thread);
        sensorVector.push_back(sensors);
        minrates.push_back(minrate);
    }

    for (int i = 0; i < static_cast<int>(systemLogic.size()); i++){
        QDomNodeList logicConfigs = systemLogic.at(i).childNodes();
        for (int j = 0; j < logicConfigs.size(); j++){
            logic thisLogic;
            if (logicConfigs.at(j).nodeName().toStdString().compare("sensorid1") == 0){
                int id = stoi(logicConfigs.at(j).firstChild().nodeValue().toStdString());
                for (uint k = 0; k < storedSensors.size(); k++){
                    if (storedSensors.at(k)->sensorIndex == id){
                        thisLogic.sensor1 = storedSensors.at(k);
                    }
                }
            } else if (logicConfigs.at(j).nodeName().toStdString().compare("sensorid2") == 0){
                int id = stoi(logicConfigs.at(j).firstChild().nodeValue().toStdString());
                for (uint k = 0; k < storedSensors.size(); k++){
                    if (storedSensors.at(k)->sensorIndex == id){
                        thisLogic.sensor2 = storedSensors.at(k);
                    }
                }
            } else if (logicConfigs.at(j).nodeName().toStdString().compare("state") == 0){
                QDomNodeList stateNodes = logicConfigs.at(j).childNodes();
                for (int k = 0; k < stateNodes.size(); k++){
                    if (stateNodes.at(k).nodeName().toStdString().compare("val1") == 0){
                        thisLogic.val1 = stod(stateNodes.at(k).firstChild().nodeValue().toStdString());
                    } else if (stateNodes.at(k).nodeName().toStdString().compare("val2") == 0){
                        thisLogic.val2 = stod(stateNodes.at(k).firstChild().nodeValue().toStdString());
                    } else if (stateNodes.at(k).nodeName().toStdString().compare("name") == 0){
                        thisLogic.logicName = stateNodes.at(k).firstChild().nodeValue().toStdString();
                    } else if (stateNodes.at(k).nodeName().toStdString().compare("reactionid") == 0){
                        int rxnVal = stoi(stateNodes.at(k).firstChild().nodeValue().toStdString());
                        for (uint m = 0; m < allResponses.size(); m++){
                            if (allResponses.at(m).responseIndex == rxnVal){
                                thisLogic.rsp = allResponses.at(m);
                                break;
                            }
                            if (m == allResponses.size()-1){
                                cout << "Error: Could not find response" << endl;
                            }
                        }
                    }
                }
                logicVector.push_back(thisLogic);
            }
        }
    }

    cout << "Logic Configured: " << endl;
    for (uint i = 0; i < logicVector.size(); i++){
        cout << "Logic Name: " << logicVector.at(i).logicName << endl;
        cout << "Logic Sensor1: " << logicVector.at(i).sensor1->sensorName << " value: " << logicVector.at(i).val1 << endl;
        cout << "Logic Sensor2: " << logicVector.at(i).sensor2->sensorName << " value: " << logicVector.at(i).val2 << endl;
        cout << "Logic response: " << logicVector.at(i).rsp.msg << endl << endl;
    }

    //********************************************************//
    //              PREPARE DATABASE INIT SCRIPT              //
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
    dbScript << "minReactionId char not null," << endl;
    dbScript << "calConstant char not null" << endl;
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

    dbScript << "create table if not exists system_states(" << endl;
    dbScript << "time char not null," << endl;
    dbScript << "state char not null," << endl;
    dbScript << "message char not null" << endl;
    dbScript << ");" << endl;

    //create subsystem tables
    for (uint i = 0; i < subsystems.size(); i++){
        string scriptTableArg = "create table if not exists " + subsystems.at(i)->subsystemId + "_rawdata(";
        dbScript << scriptTableArg << endl;
        dbScript << "time char not null," << endl;
        dbScript << "sensorindex char not null," << endl;
        dbScript << "sensorName char not null," << endl;
        dbScript << "value char not null" << endl;
        dbScript << ");" << endl;
    }

    for (uint i = 0; i < subsystems.size(); i++){
        string scriptTableArg = "create table if not exists " + subsystems.at(i)->subsystemId + "_caldata(";
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
    //*************************FINISH**************************//

    cout << "All Sensors: " << storedSensors.size() << endl;
    vector<string> cols;
    cols.push_back("sensorIndex");
    cols.push_back("sensorName");
    cols.push_back("subsystem");
    cols.push_back("minThreshold");
    cols.push_back("maxThreshold");
    cols.push_back("maxReactionId");
    cols.push_back("minReactionId");
    cols.push_back("calConstant");
    vector<string> rows;
    for (uint n = 0; n < storedSensors.size(); n++){
        rows.clear();
        rows.push_back(to_string(storedSensors.at(n)->sensorIndex));
        rows.push_back(storedSensors.at(n)->sensorName);
        rows.push_back(storedSensors.at(n)->subsystem);
        rows.push_back(to_string(storedSensors.at(n)->minimum));
        rows.push_back(to_string(storedSensors.at(n)->maximum));
        rows.push_back(to_string(storedSensors.at(n)->maxRxnCode));
        rows.push_back(to_string(storedSensors.at(n)->minRxnCode));
        rows.push_back(to_string(storedSensors.at(n)->calConst));
        dbase->insert_row("sensors",cols,rows);
    }
    dataMtr = new DataMonitor(allSensors,allResponses);
    dataCtrl = new DataControl(dataMtr,dbase,subsystems,sysStates,FSMs,systemMode,controlSpecs,storedSensors);
    gpioInterface = new gpio_interface(gpioSensors,i2cSensors,allResponses,subsystems);
    canInterface = new canbus_interface(storedSensors,"STUFF",subsystems,sysStates,dataCtrl,FSMs);
    for (uint i = 0; i < subsystems.size(); i++){
        SubsystemThread * thread = subsystems.at(i);
        thread->setMonitor(dataMtr);
        thread->set_rate(minrates.at(i));
        thread->setDB(dbase);
        subsystems.at(i)->start();
    }
    gpioInterface->startGPIOCheck();
    cout << "Returning from config" << endl;
    return true;
}
