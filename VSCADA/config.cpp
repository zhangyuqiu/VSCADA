#include "config.h"

Config::Config(){
}

Config::~Config(){

}

bool Config::read_config_file_data(){
//    funct();
    //local declarations
    vector<vector<meta *>> sensorVector;
    vector<meta> allSensors;
    vector<response> allResponses;
    vector<int> minrates;
    vector<logic> logicVector;

    //************************************//
    //*****extract file to DOM object*****//
    //************************************//

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


    //************************************************//
    //*****extract individual types from xml file*****//
    //************************************************//

    QDomNodeList mode = doc.elementsByTagName("mode");
    QDomNodeList systemControls = doc.elementsByTagName("systemcontrols");
    QDomNodeList responseNodes = doc.elementsByTagName("response");
    QDomNodeList subsystemNodes = doc.elementsByTagName("subsystem");
    QDomNodeList systemStatuses = doc.elementsByTagName("systemstatus");
    QDomNodeList stateMachines = doc.elementsByTagName("statemachine");
    QDomNodeList systemLogic = doc.elementsByTagName("logic");

#ifdef CONFIG_PRINT
    cout << "Number of responses: " << responseNodes.size() << endl;
    cout << "Number of system controls: " << systemControls.size() << endl;
    cout << "Number of subsystems: " << subsystemNodes.size() << endl;
    cout << "Number of logic configurations: " << systemLogic.size() << endl;
    cout << "Number of system states: " << systemStatuses.size() << endl;
    cout << "Number of state machines: " << stateMachines.size() << endl;
#endif

    //*************************//
    //*****get system mode*****//
    //*************************//

    for (int i = 0; i < mode.size(); i++){
        QDomNodeList modeXteristics = mode.at(i).childNodes();
        for (int j = 0; j < modeXteristics.size(); j++){
            if (modeXteristics.at(j).nodeName().toStdString().compare("type") == 0){
                if(modeXteristics.at(j).firstChild().nodeValue().toStdString().compare("0") == 0){
                    systemMode = DYNO;
                } else if(mode.at(i).firstChild().nodeValue().toStdString().compare("1") == 0){
                    systemMode = CAR;
                }
            }
        }
    }

    //**************************************************//
    //*****for DYNO mode, process all control items*****//
    //**************************************************//
    if (systemMode == DYNO){
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
                currSpec->usbChannel = -1;
                currSpec->primAddress = -1;
                currSpec->auxAddress = -1;
                currSpec->offset = -1;
                currSpec->multiplier = 1;

                QDomNodeList controlXteristics = ctrlSpecs.at(j).childNodes();
                for (int k = 0; k < controlXteristics.size(); k++){
                    if (controlXteristics.at(k).nodeName().toStdString().compare("name") == 0){
                        currSpec->name = controlXteristics.at(k).firstChild().nodeValue().toStdString();
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("usbchannel") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->usbChannel = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: USB channel not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("primaddress") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->primAddress = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: primary address not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("auxaddress") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->auxAddress = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: aux address not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("offset") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->offset = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: address offset not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("gpiopin") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->gpiopin = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: GPIO pin not an integer");
                    }else if (controlXteristics.at(k).nodeName().toStdString().compare("minrange") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->minslider = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: min slider value not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("maxrange") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->maxslider = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: max slider value not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("multiplier") == 0){
                            currSpec->multiplier = stod(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("type") == 0){
                        if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("button") == 0){
                            currSpec->button = true;
                        } else if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("slider") == 0){
                            currSpec->slider = true;
                        } else if(controlXteristics.at(k).firstChild().nodeValue().toStdString().compare("textfield") == 0){
                            currSpec->textField = true;
                        } else {
                            configErrors.push_back("CONFIG ERROR: invalid control type");
                        }
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("pressvalue") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->pressVal = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: press value not an integer");
                    } else if (controlXteristics.at(k).nodeName().toStdString().compare("releasevalue") == 0){
                        if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                            currSpec->releaseVal = stoi(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: release value not an integer");
                    }
                }
                controlSpecs.push_back(currSpec);
            }
        }
    }

    //***************************************//
    //*****process system state machines*****//
    //***************************************//

    for (int i = 0; i < stateMachines.size(); i++){
        thisFSM = new statemachine;
        QDomNodeList machineXteristics = stateMachines.at(i).childNodes();
        for (int j = 0; j < machineXteristics.size(); j++){
            if (machineXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisFSM->name = machineXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->primAddress = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->auxAddress = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->offset = stoi(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: address offset not an integer");
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
                        if (isInteger(stateXteristics.at(k).firstChild().nodeValue().toStdString()))
                            thisState->value = stoi(stateXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: state value not an integer");
                    }
                }
                thisFSM->states.push_back(thisState);
            }
        }
        FSMs.push_back(thisFSM);
    }

#ifdef CONFIG_PRINT
    for (uint i = 0; i < FSMs.size(); i++){
        cout << "state machine name: " << FSMs.at(i)->name << endl;
        cout << "state machine states: " << FSMs.at(i)->states.size() << endl;
        cout << "state machine address: " << FSMs.at(i)->primAddress << endl;
    }
#endif

    //*********************************//
    //*****process system statuses*****//
    //*********************************//

    for (int i = 0; i < systemStatuses.size(); i++){
        thisState = new system_state;
        QDomNodeList statusXteristics = systemStatuses.at(i).childNodes();
        for (int j = 0; j < statusXteristics.size(); j++){
            if (statusXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisState->name = statusXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->primAddress = stoi(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->auxAddress = stoi(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->offset = stoi(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: address offset not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("value") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->value = stoi(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: state value not an integer");
            }
        }
        sysStates.push_back(thisState);
    }

    //**********************************//
    //*****process system responses*****//
    //**********************************//

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
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.responseIndex = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: response index not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("description") == 0){
                thisRsp.msg = responseXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.primAddress = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.auxAddress = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.offset = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: address offset not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("gpiopin") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.gpioPin = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: GPIO pin not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("gpioval") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.gpioValue = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: GPIO response value not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("canval") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.canValue = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: CAN response value not an integer");
            }
        }
        allResponses.push_back(thisRsp);
    }

#ifdef CONFIG_PRINT
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
#endif

    //***********************************************//
    //*****process system subsystems and sensors*****//
    //***********************************************//

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
                if (isInteger(subsystemXteristics.at(j).firstChild().nodeValue().toStdString()))
                    minrate = stoi(subsystemXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: subsystem min rate not an integer");
            } else if (subsystemXteristics.at(j).nodeName().toStdString().compare("maxrate") == 0){
                if (isInteger(subsystemXteristics.at(j).firstChild().nodeValue().toStdString()))
                    maxrate = stoi(subsystemXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: subsystem min rate not an integer");
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
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->sensorIndex = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor index not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("primaddress") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->primAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor primary address not an integer");
                            canSensors.push_back(storedSensor);
                        } else if (attributeList.at(m).nodeName().toStdString().compare("auxaddress") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->auxAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor aux address not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("offset") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->offset = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor address offset not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minimum") == 0){
                            storedSensor->minimum = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("main") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->main = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor main field not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maximum") == 0){
                            storedSensor->maximum = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("minresponse") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->minRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor min reaction code not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("maxresponse") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->maxRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor max reaction code not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("normresponse") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->normRxnCode = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor norm reaction code not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("checkrate") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->checkRate = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor check rate not an integer");
                        } else if (attributeList.at(m).nodeName().toStdString().compare("multiplier") == 0){
                            storedSensor->calConst = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
                        } else if (attributeList.at(m).nodeName().toStdString().compare("gpiopin") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->gpioPin = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor GPIO pin not an integer");
                            gpioSensors.push_back(storedSensor);
                        } else if (attributeList.at(m).nodeName().toStdString().compare("usbchannel") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->usbChannel = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor USB channel not an integer");
                            usbSensors.push_back(storedSensor);
                        }else if (attributeList.at(m).nodeName().toStdString().compare("i2caddress") == 0){
                            if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                                storedSensor->i2cAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                            else configErrors.push_back("CONFIG ERROR: sensor i2c address not an integer");
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

        //launch a subsystem thread

        SubsystemThread * thread = new SubsystemThread(sensors,subSystemId,allResponses,logicVector,mainMeta);
        subsystems.push_back(thread);
        sensorVector.push_back(sensors);
        minrates.push_back(minrate);
    }

    //*********************************************//
    //*****process system logic configurations*****//
    //*********************************************//

    for (int i = 0; i < systemLogic.size(); i++){
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
                    } else if (stateNodes.at(k).nodeName().toStdString().compare("responseid") == 0){
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

#ifdef CONFIG_PRINT
    cout << "Logic Configured: " << endl;
    for (uint i = 0; i < logicVector.size(); i++){
        cout << "Logic Name: " << logicVector.at(i).logicName << endl;
        cout << "Logic Sensor1: " << logicVector.at(i).sensor1->sensorName << " value: " << logicVector.at(i).val1 << endl;
        cout << "Logic Sensor2: " << logicVector.at(i).sensor2->sensorName << " value: " << logicVector.at(i).val2 << endl;
        cout << "Logic response: " << logicVector.at(i).rsp.msg << endl << endl;
    }
#endif

    //********************************************************//
    //              PREPARE DATABASE INIT SCRIPT              //
    //********************************************************//

    ofstream dbScript;
    dbScript.open ("script.sql");

    // write create universal tables
    dbScript << "create table if not exists system_info(" << endl;
    dbScript << "starttime char not null," << endl;
    dbScript << "endtime char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists system_log(" << endl;
    dbScript << "time char not null," << endl;
    dbScript << "responseid char not null," << endl;
    dbScript << "message char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists sensors(" << endl;
    dbScript << "sensorindex char not null," << endl;
    dbScript << "sensorname char not null," << endl;
    dbScript << "subsystem char not null," << endl;
    dbScript << "minthreshold char not null," << endl;
    dbScript << "maxthreshold char not null," << endl;
    dbScript << "maxresponseid char not null," << endl;
    dbScript << "minresponseid char not null," << endl;
    dbScript << "calconstant char not null" << endl;
    dbScript << ");" << endl;

//    dbScript << "create table if not exists responses(" << endl;
//    dbScript << "responseid char not null," << endl;
//    dbScript << "message char not null" << endl;
//    dbScript << ");" << endl;

//    dbScript << "create table if not exists statemachines(" << endl;
//    dbScript << "time char not null," << endl;
//    dbScript << "state char not null," << endl;
//    dbScript << "message char not null" << endl;
//    dbScript << ");" << endl;

//    dbScript << "create table if not exists statuses(" << endl;
//    dbScript << "time char not null," << endl;
//    dbScript << "status char not null," << endl;
//    dbScript << "value char not null," << endl;
//    dbScript << "message char not null" << endl;
//    dbScript << ");" << endl;

    //create subsystem tables
    for (uint i = 0; i < subsystems.size(); i++){
        string scriptTableArg = "create table if not exists " + subsystems.at(i)->subsystemId + "_rawdata(";
        dbScript << scriptTableArg << endl;
        dbScript << "time char not null," << endl;
        dbScript << "sensorindex char not null," << endl;
        dbScript << "sensorname char not null," << endl;
        dbScript << "value char not null" << endl;
        dbScript << ");" << endl;
    }

    for (uint i = 0; i < subsystems.size(); i++){
        string scriptTableArg = "create table if not exists " + subsystems.at(i)->subsystemId + "_caldata(";
        dbScript << scriptTableArg << endl;
        dbScript << "time char not null," << endl;
        dbScript << "sensorindex char not null," << endl;
        dbScript << "sensorname char not null," << endl;
        dbScript << "value char not null" << endl;
        dbScript << ");" << endl;
    }
    dbScript.close();

    //run script
    dbase = new DB_Engine();
    dbase->runScript("script.sql");

    //****************************************//
    //*****record all sensors to database*****//
    //****************************************//
    vector<string> cols;
    cols.push_back("sensorindex");
    cols.push_back("sensorname");
    cols.push_back("subsystem");
    cols.push_back("minthreshold");
    cols.push_back("maxthreshold");
    cols.push_back("maxresponseid");
    cols.push_back("minresponseid");
    cols.push_back("calconstant");
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

    //********************************//
    //*****initialize system info*****//
    //********************************//
    cols.clear();
    rows.clear();
    cols.push_back("starttime");
    cols.push_back("endtime");
    rows.push_back(get_curr_time());
    rows.push_back("0");
    dbase->insert_row("system_info",cols,rows);


    //****************************************//
    //*****launch internal worker modules*****//
    //****************************************//
    dataMtr = new DataMonitor(allSensors,allResponses);
    usb7204 = new usb7402_interface(usbSensors,subsystems);
    gpioInterface = new gpio_interface(gpioSensors,i2cSensors,allResponses,subsystems);
    canInterface = new canbus_interface(storedSensors,"STUFF",subsystems,sysStates,FSMs);
    dataCtrl = new DataControl(dataMtr,gpioInterface,canInterface,usb7204,dbase,subsystems,sysStates,
                               FSMs,systemMode,controlSpecs,storedSensors,allResponses);


    //**********************************//
    //*****launch subsystem threads*****//
    //**********************************//
    for (uint i = 0; i < subsystems.size(); i++){
        SubsystemThread * thread = subsystems.at(i);
        thread->setMonitor(dataMtr);
        thread->set_rate(minrates.at(i));
        thread->setDB(dbase);
        subsystems.at(i)->start();
    }
    gpioInterface->startGPIOCheck();
    usb7204->startUSBCheck();
    return true;
}

/**
 * @brief Config::isInteger checks whether string represents an integer
 * @param s : input string
 * @return true if input string is an integer. Otherwise returns false
 */
bool Config::isInteger(const string & s){
   if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;

   char * p ;
   strtol(s.c_str(), &p, 10) ;

   return (*p == 0) ;
}

/**
 * @brief SubsystemThread::get_curr_time - retrieves current operation system time
 * @return
 */
string Config::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D_%T",&now);
    return buf;
}
