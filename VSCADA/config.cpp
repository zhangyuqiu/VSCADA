#include "config.h"

/**
 * @brief Config::Config class constructor
 */
Config::Config(){
    //blah...
}

/**
 * @brief Config::~Config class destructor
 */
Config::~Config(){
    if (dataCtrl != nullptr) delete dataCtrl;
    if (usb7204 != nullptr) delete usb7204;
    if (gpioInterface != nullptr) delete gpioInterface;
    if (canInterface != nullptr) delete canInterface;
    if (dbase != nullptr) delete dbase;


    for (auto const& x : sensorMap){
        if (x.second != nullptr) delete x.second;
    }

    for (uint i = 0; i < FSMs.size(); i++){
        if (FSMs.at(i) != nullptr) delete FSMs.at(i);
    }

    for (uint i = 0; i < sysStates.size(); i++){
        if (sysStates.at(i) != nullptr) delete sysStates.at(i);
    }

    for (uint i = 0; i < controlSpecs.size(); i++){
        if (controlSpecs.at(i) != nullptr) delete controlSpecs.at(i);
    }
}

/**
 * @brief Config::read_config_file_data : reads configuration file
 *  and stores all configured data
 * @return
 */
bool Config::read_config_file_data(){
    //local declarations
    vector<vector<meta *>> sensorVector;
    vector<meta> allSensors;
    vector<response> allResponses;
    vector<int> minrates;
    vector<logic *> logicVector;
    vector<meta *> dependentSensors;
    gpioRate = 1000;
    usb7204Rate = 1000;
    canRate = 125000;

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
    QDomNodeList gpioConfRate = doc.elementsByTagName("gpiorate");
    QDomNodeList canConfRate = doc.elementsByTagName("canrate");
    QDomNodeList usb7204ConfRate = doc.elementsByTagName("usb7204rate");
    QDomNodeList systemControls = doc.elementsByTagName("systemcontrols");
    QDomNodeList responseNodes = doc.elementsByTagName("response");
    QDomNodeList groupNodes = doc.elementsByTagName("group");
    QDomNodeList systemStatuses = doc.elementsByTagName("systemstatus");
    QDomNodeList stateMachines = doc.elementsByTagName("statemachine");
    QDomNodeList sensorNodes = doc.elementsByTagName("sensor");
    QDomNodeList canSyncNodes = doc.elementsByTagName("cansync");
    QDomNodeList i2cSyncNodes = doc.elementsByTagName("i2csync");
    QDomNodeList gpioSyncNodes = doc.elementsByTagName("gpiosync");
    QDomNodeList canBootNodes = doc.elementsByTagName("bootcan");
    QDomNodeList i2cBootNodes = doc.elementsByTagName("booti2c");
    QDomNodeList gpioBootNodes = doc.elementsByTagName("bootgpio");
    QDomNodeList recordNodes = doc.elementsByTagName("recordwindow");

#ifdef CONFIG_PRINT
    cout << "Number of responses: " << responseNodes.size() << endl;
    cout << "Number of system controls: " << systemControls.size() << endl;
    cout << "Number of sensor groupings: " << groupNodes.size() << endl;
    cout << "Number of system states: " << systemStatuses.size() << endl;
    cout << "Number of state machines: " << stateMachines.size() << endl;
    cout << "Number of configured sensors: " << sensorNodes.size() << endl;
    cout << "Number of record nodes: " << recordNodes.size() << endl;
#endif

    //*****************************//
    //*****set interface rates*****//
    //*****************************//
    if (gpioConfRate.size() > 0){
        gpioRate = stoi(gpioConfRate.at(0).firstChild().nodeValue().toStdString());
    }
    if (usb7204ConfRate.size() > 0){
        usb7204Rate = stoi(usb7204ConfRate.at(0).firstChild().nodeValue().toStdString());
    }
    if (canConfRate.size() > 0){
        canRate = stoi(canConfRate.at(0).firstChild().nodeValue().toStdString());
    }

#ifdef CONFIG_PRINT
    cout << "CAN Rate: " << canRate << endl;
    cout << "GPIO Rate: " << gpioRate << endl;
    cout << "USB7204 Rate: " << usb7204Rate << endl;
#endif

    //*************************//
    //*****get system mode*****//
    //*************************//

    for (int i = 0; i < mode.size(); i++){
        if (mode.at(i).firstChild().nodeValue().toStdString().compare("RUN") == 0){
            systemMode = RUN;
        } else if(mode.at(i).firstChild().nodeValue().toStdString().compare("TEST") == 0){
            systemMode = TEST;
        } else {
            systemMode = RUN;
        }
    }

#ifdef CONFIG_PRINT
    if (systemMode) cout << "System Mode: " << systemMode << ": CAR" << endl;
    else cout << "System Mode: " << systemMode << ": DYNO" << endl;
#endif

    //*******************************//
    //*****process control items*****//
    //*******************************//
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
            currSpec->primAddress = 1000;
            currSpec->auxAddress = 0;
            currSpec->offset = 0;
            currSpec->multiplier = 1;
            currSpec->endianness = 1;
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
                        currSpec->primAddress = stoul(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                    else configErrors.push_back("CONFIG ERROR: primary address not an integer");
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("auxaddress") == 0){
                    if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                        currSpec->auxAddress = stoul(controlXteristics.at(k).firstChild().nodeValue().toStdString());
                    else configErrors.push_back("CONFIG ERROR: aux address not an integer");
                } else if (controlXteristics.at(k).nodeName().toStdString().compare("offset") == 0){
                    if (isInteger(controlXteristics.at(k).firstChild().nodeValue().toStdString()))
                        currSpec->offset = stoul(controlXteristics.at(k).firstChild().nodeValue().toStdString());
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

    //***************************************//
    //*****process system state machines*****//
    //***************************************//

    for (int i = 0; i < stateMachines.size(); i++){
        thisFSM = new statemachine;
        thisFSM->endianness = 1;
        QDomNodeList machineXteristics = stateMachines.at(i).childNodes();
        for (int j = 0; j < machineXteristics.size(); j++){
            if (machineXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisFSM->name = machineXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->primAddress = stoul(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->auxAddress = stoul(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(machineXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisFSM->offset = stoul(machineXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: address offset not an integer");
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("state") == 0){
                QDomNodeList stateXteristics = machineXteristics.at(j).childNodes();
                thisState = new system_state;
                thisState->primAddress = thisFSM->primAddress;
                thisState->auxAddress = thisFSM->auxAddress;
                thisState->offset = thisFSM->offset;
                thisState->endianness = thisFSM->endianness;
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
            } else if (machineXteristics.at(j).nodeName().toStdString().compare("condition") == 0){
                QDomNodeList stateXteristics = machineXteristics.at(j).childNodes();
                thisCondition = new condition;
                thisCondition->value = -1;
                for (int k = 0; k < stateXteristics.size(); k++){
                    if (stateXteristics.at(k).nodeName().toStdString().compare("name") == 0){
                        thisCondition->name = stateXteristics.at(k).firstChild().nodeValue().toStdString();
                    } else if (stateXteristics.at(k).nodeName().toStdString().compare("auxaddress") == 0){
                        if (isInteger(stateXteristics.at(k).firstChild().nodeValue().toStdString()))
                            thisState->auxAddress = stoi(stateXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: state value not an integer");
                    } else if (stateXteristics.at(k).nodeName().toStdString().compare("offset") == 0){
                        if (isInteger(stateXteristics.at(k).firstChild().nodeValue().toStdString()))
                            thisState->offset = stoi(stateXteristics.at(k).firstChild().nodeValue().toStdString());
                        else configErrors.push_back("CONFIG ERROR: state value not an integer");
                    }
                }
                thisFSM->conditions.push_back(thisCondition);
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
        thisState->endianness = 1;
        QDomNodeList statusXteristics = systemStatuses.at(i).childNodes();
        for (int j = 0; j < statusXteristics.size(); j++){
            if (statusXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                thisState->name = statusXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->primAddress = stoul(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->auxAddress = stoul(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->offset = stoul(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: address offset not an integer");
            } else if (statusXteristics.at(j).nodeName().toStdString().compare("value") == 0){
                if (isInteger(statusXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisState->value = stoi(statusXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: state value not an integer");
            }
        }
        thisState->active = false;
        sysStates.push_back(thisState);
    }

    //**********************************//
    //*****process system responses*****//
    //**********************************//

    for (int i = 0; i < responseNodes.size(); i++){
        QDomNodeList responseXteristics = responseNodes.at(i).childNodes();
        response thisRsp;
        thisRsp.primAddress = 1000;
        thisRsp.auxAddress = -1;
        thisRsp.offset = 0;
        thisRsp.gpioPin = -1;
        thisRsp.canValue = -1;
        thisRsp.gpioValue = -1;
        thisRsp.endianness = 1;
        for (int j = 0; j < responseXteristics.size(); j++){
            if (responseXteristics.at(j).nodeName().toStdString().compare("id") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.responseIndex = stoi(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: response index not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("description") == 0){
                thisRsp.msg = responseXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.primAddress = stoul(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: primary address not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.auxAddress = stoul(responseXteristics.at(j).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: aux address not an integer");
            } else if (responseXteristics.at(j).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(responseXteristics.at(j).firstChild().nodeValue().toStdString()))
                    thisRsp.offset = stoul(responseXteristics.at(j).firstChild().nodeValue().toStdString());
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
        responseMap.insert(make_pair(thisRsp.responseIndex,thisRsp));
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

#ifdef CONFIG_PRINT
    cout << "Logic Configured: " << endl;
    for (uint i = 0; i < logicVector.size(); i++){
        cout << "Logic Name: " << logicVector.at(i)->logicName << endl;
        cout << "Logic Sensor1: " << logicVector.at(i)->sensorId1 << " value: " << logicVector.at(i)->val1 << endl;
        cout << "Logic Sensor2: " << logicVector.at(i)->sensorId2 << " value: " << logicVector.at(i)->val2 << endl;
        cout << "Logic response: " << logicVector.at(i)->rsp << endl << endl;
    }
#endif

    //***************************************//
    //*****process boot sequence signals*****//
    //***************************************//
    for (int i = 0; i < canBootNodes.size(); i++){
        canItem item;
        item.address = -1;
        item.data = 0;
        item.dataSize = 8;
        QDomNodeList canItemList = canBootNodes.at(i).childNodes();
        for (int m = 0; m < canItemList.size(); m++){
            if(canItemList.at(m).nodeName().toStdString().compare("address") == 0){
                if (isInteger(canItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.address = stoi(canItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: boot CAN address not an integer");
            } else if(canItemList.at(m).nodeName().toStdString().compare("data") == 0){
                item.data = stoull(canItemList.at(m).firstChild().nodeValue().toStdString());
            } else if(canItemList.at(m).nodeName().toStdString().compare("bytes") == 0){
                if (isInteger(canItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.dataSize = stoi(canItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: boot CAN data size not an integer");
            }
        }
        bootCmds.bootCanCmds.push_back(item);
    }

    for (int i = 0; i < i2cBootNodes.size(); i++){
        if (isInteger(i2cBootNodes.at(i).firstChild().nodeValue().toStdString()))
            bootCmds.bootI2cCmds.push_back(stoul(i2cBootNodes.at(i).firstChild().nodeValue().toStdString()));
        else configErrors.push_back("CONFIG ERROR: boot CAN address not an integer");
    }

    for (int i = 0; i < gpioBootNodes.size(); i++){
        gpioItem item;
        item.mode = -1;
        item.pin = -1;
        item.value = 0;
        QDomNodeList gpioItemList = gpioBootNodes.at(i).childNodes();
        for (int m = 0; m < gpioItemList.size(); m++){
            if(gpioItemList.at(m).nodeName().toStdString().compare("pin") == 0){
                if (isInteger(gpioItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.pin = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: boot GPIO pin not an integer");
            } else if(gpioItemList.at(m).nodeName().toStdString().compare("value") == 0){
                if (isInteger(gpioItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.value = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: boot GPIO val not an integer");
            } else if(gpioItemList.at(m).nodeName().toStdString().compare("mode") == 0){
                if (isInteger(gpioItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.mode = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: boot GPIO mode not an integer");
            }
        }
        bootCmds.bootGPIOCmds.push_back(item);
    }

    cout << "Boot sequence processed" << endl;

    //**********************************//
    //*****process sync signals*****//
    //**********************************//
    for (int i = 0; i < canSyncNodes.size(); i++){
        canItem item;
        item.address = -1;
        item.data = 0;
        item.dataSize = 8;
        QDomNodeList canItemList = canSyncNodes.at(i).childNodes();
        for (int m = 0; m < canItemList.size(); m++){
            if(canItemList.at(m).nodeName().toStdString().compare("address") == 0){
                if (isInteger(canItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.address = stoi(canItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast CAN address not an integer");
            } else if(canItemList.at(m).nodeName().toStdString().compare("data") == 0){
                item.data = stoull(canItemList.at(m).firstChild().nodeValue().toStdString());
            } else if(canItemList.at(m).nodeName().toStdString().compare("bytes") == 0){
                if (isInteger(canItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.dataSize = stoi(canItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast CAN data size not an integer");
            } else if(canItemList.at(m).nodeName().toStdString().compare("ratems") == 0){
                if (isInteger(canItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.rate_ms = stoi(canItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast CAN data size not an integer");
            }
        }
        canSyncs.push_back(item);
    }

    for (int i = 0; i < i2cSyncNodes.size(); i++){
        i2cItem item;
        item.address = -1;
        item.data = 0;
        QDomNodeList i2cItemList = i2cSyncNodes.at(i).childNodes();
        for (int m = 0; m < i2cItemList.size(); m++){
            if(i2cItemList.at(m).nodeName().toStdString().compare("address") == 0){
                if (isInteger(i2cItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.address = stoi(i2cItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast I2C address not an integer");
            } else if(i2cItemList.at(m).nodeName().toStdString().compare("data") == 0){
                item.data = stoi(i2cItemList.at(m).firstChild().nodeValue().toStdString());
            } else if(i2cItemList.at(m).nodeName().toStdString().compare("ratems") == 0){
                if (isInteger(i2cItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.rate_ms = stoi(i2cItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast CAN data size not an integer");
            }
        }
        i2cSyncs.push_back(item);
    }

    for (int i = 0; i < gpioSyncNodes.size(); i++){
        gpioItem item;
        item.pin = -1;
        item.value = 0;
        QDomNodeList gpioItemList = gpioSyncNodes.at(i).childNodes();
        for (int m = 0; m < gpioItemList.size(); m++){
            if(gpioItemList.at(m).nodeName().toStdString().compare("pin") == 0){
                if (isInteger(gpioItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.pin = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast GPIO address not an integer");
            } else if(gpioItemList.at(m).nodeName().toStdString().compare("data") == 0){
                item.value = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
            } else if(gpioItemList.at(m).nodeName().toStdString().compare("ratems") == 0){
                if (isInteger(gpioItemList.at(m).firstChild().nodeValue().toStdString()))
                    item.rate_ms = stoi(gpioItemList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: broadcast CAN data size not an integer");
            }
        }
        gpioSyncs.push_back(item);
    }

    cout << "Sync signals processed" << endl;

    //****************************************//
    //*****process data recording windows*****//
    //****************************************//
    for (int i = 0; i < recordNodes.size(); i++){
        recWin = new recordwindow;
        recWin->active = false;
        recWin->startVal = 0;
        recWin->stopVal = 0;
        recWin->triggerSensor = -1;
        recWin->triggerFSM = "";
        recWin->triggerState = "";
        QDomNodeList recWinList = recordNodes.at(i).childNodes();
        for (int m = 0; m < recWinList.size(); m++){
            if(recWinList.at(m).nodeName().toStdString().compare("id") == 0){
                if (isInteger(recWinList.at(m).firstChild().nodeValue().toStdString()))
                    recWin->id = stoi(recWinList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: record window ID not an integer");
            } else if(recWinList.at(m).nodeName().toStdString().compare("triggersensor") == 0){
                if (isInteger(recWinList.at(m).firstChild().nodeValue().toStdString()))
                    recWin->triggerSensor = stoi(recWinList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: trigger sensor not an integer");
            } else if(recWinList.at(m).nodeName().toStdString().compare("period") == 0){
                if (isInteger(recWinList.at(m).firstChild().nodeValue().toStdString()))
                    recWin->period = stoi(recWinList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: record period not an integer");
            } else if(recWinList.at(m).nodeName().toStdString().compare("startvalue") == 0){
                recWin->startVal = stoi(recWinList.at(m).firstChild().nodeValue().toStdString());
            } else if(recWinList.at(m).nodeName().toStdString().compare("stopvalue") == 0){
                recWin->stopVal = stoi(recWinList.at(m).firstChild().nodeValue().toStdString());
            } else if(recWinList.at(m).nodeName().toStdString().compare("savepath") == 0){
                recWin->savePath = recWinList.at(m).firstChild().nodeValue().toStdString();
            } else if(recWinList.at(m).nodeName().toStdString().compare("saveprefix") == 0){
                recWin->prefix = recWinList.at(m).firstChild().nodeValue().toStdString();
            } else if(recWinList.at(m).nodeName().toStdString().compare("triggerstate") == 0){
                QDomNode recItem = recWinList.at(m);
                QDomNodeList recItemList = recItem.childNodes();
                for (int n = 0; n < recItemList.size(); n++){
                    if (recItemList.at(n).nodeName().toStdString().compare("fsm") == 0) {
                        string fsmName = recItemList.at(n).firstChild().nodeValue().toStdString();
                        for (uint p = 0; p < FSMs.size(); p++){
                            if (FSMs.at(p)->name.compare(fsmName) == 0) recWin->triggerFSM = fsmName;
                        }
                        if (recWin->triggerFSM.compare("") == 0) configErrors.push_back("CONFIG ERROR: no such FSM configured for record window");
                    } else if (recItemList.at(n).nodeName().toStdString().compare("state") == 0){
                        string stateName = recItemList.at(n).firstChild().nodeValue().toStdString();
                        for (uint p = 0; p < FSMs.size(); p++){
                            for (uint q = 0; q < FSMs.at(p)->states.size(); q++){
                                if (FSMs.at(p)->states.at(q)->name.compare(stateName) == 0) recWin->triggerState = stateName;
                            }
                        }
                        if (recWin->triggerState.compare("") == 0) configErrors.push_back("CONFIG ERROR: no such state configured for record window");
                    }
                }
            } else if(recWinList.at(m).nodeName().toStdString().compare("sensors") == 0){
                QDomNode recItem = recWinList.at(m);
                QDomNodeList recItemList = recItem.childNodes();
                for (int n = 0; n < recItemList.size(); n++){
                    if (isInteger(recItemList.at(n).firstChild().nodeValue().toStdString())) {
                        recWin->sensorIds.push_back(stoi(recItemList.at(n).firstChild().nodeValue().toStdString()));
                    }
                    else configErrors.push_back("CONFIG ERROR: record sensor id not an integer");
                }
            }
        }
        if (recWin->triggerSensor >= 0) recordSensorConfigs.insert(make_pair(recWin->id,recWin));
        if (recWin->triggerState.compare("") != 0 && recWin->triggerFSM.compare("") != 0) recordStateConfigs.insert(make_pair(recWin->id,recWin));
    }

    cout << "Data recording windows processed" << endl;

    //*************************//
    //*****process sensors*****//
    //*************************//
    for (int k = 0; k < sensorNodes.size(); k++){
        meta thisSensor;
        storedSensor = new meta;
        storedSensor->val = 0;
        storedSensor->calVal = 0;
        storedSensor->main = 0;
        storedSensor->state = 0;
        storedSensor->sensorIndex = -1;
        storedSensor->minimum = -1;
        storedSensor->maximum = -1;
        storedSensor->checkRate = -1;
        storedSensor->maxRxnCode = 0;
        storedSensor->minRxnCode = 0;
        storedSensor->normRxnCode = 0;
        storedSensor->primAddress = 1000;
        storedSensor->auxAddress = 0;
        storedSensor->offset = 0;
        storedSensor->gpioPin = -1;
        storedSensor->calMultiplier = 1;
        storedSensor->calConst = 0;
        storedSensor->i2cAddress = -1;
        storedSensor->i2cReadPointer = 0;
        storedSensor->i2cReadDelay = 0;
        storedSensor->i2cDataField = 16;
        storedSensor->endianness = 1;
        storedSensor->senOperator = -1;
        storedSensor->lutIndex = -1;
        storedSensor->precision = 2;
        QDomNodeList attributeList = sensorNodes.at(k).childNodes();
        for (int m = 0; m < attributeList.size(); m++){
            if(attributeList.at(m).nodeName().toStdString().compare("name") == 0){
                storedSensor->sensorName = attributeList.at(m).firstChild().nodeValue().toStdString();
            } else if(attributeList.at(m).nodeName().toStdString().compare("alias") == 0){
                storedSensor->alias = attributeList.at(m).firstChild().nodeValue().toStdString();
            } else if(attributeList.at(m).nodeName().toStdString().compare("unit") == 0){
                storedSensor->unit = attributeList.at(m).firstChild().nodeValue().toStdString();
            } else if (attributeList.at(m).nodeName().toStdString().compare("id") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->sensorIndex = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor index not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("precision") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->precision = static_cast<uint8_t>(stoi(attributeList.at(m).firstChild().nodeValue().toStdString()));
                else configErrors.push_back("CONFIG ERROR: sensor precision not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("primaddress") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->primAddress = stoul(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor primary address not an integer");
                canSensors.push_back(storedSensor);
                if ( canAddressMap.find(storedSensor->primAddress) == canAddressMap.end() ) {
                    canAddressMap.insert(make_pair(storedSensor->primAddress,1));
                } else {
                  // found so skip
                }
                canSensorMap.insert(make_pair(storedSensor->primAddress+storedSensor->auxAddress, storedSensor));
                if ( canSensorGroup.find(storedSensor->primAddress) == canSensorGroup.end() ) {
                    canVectorItem =  new vector<meta*>;
                    canVectorItem->push_back(storedSensor);
                    canSensorGroup.insert(make_pair(storedSensor->primAddress, canVectorItem));
                } else {
                    uint32_t val = storedSensor->primAddress;
                    vector<meta*> * item = canSensorGroup[val];
                    item->push_back(storedSensor);
                }
            } else if (attributeList.at(m).nodeName().toStdString().compare("auxaddress") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->auxAddress = stoul(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor aux address not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("offset") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->offset = stoul(attributeList.at(m).firstChild().nodeValue().toStdString());
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
            } else if (attributeList.at(m).nodeName().toStdString().compare("calconstant") == 0){
                storedSensor->calConst = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
            } else if (attributeList.at(m).nodeName().toStdString().compare("calmultiplier") == 0){
                storedSensor->calMultiplier = stod(attributeList.at(m).firstChild().nodeValue().toStdString());
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
            } else if (attributeList.at(m).nodeName().toStdString().compare("endianness") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->endianness = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor USB channel not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("i2caddress") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->i2cAddress = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor i2c address not an integer");
                i2cSensors.push_back(storedSensor);
            } else if (attributeList.at(m).nodeName().toStdString().compare("i2creadpointer") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->i2cReadPointer = static_cast<uint8_t>(stoul(attributeList.at(m).firstChild().nodeValue().toStdString()));
                else configErrors.push_back("CONFIG ERROR: sensor i2c pointer set not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("i2cconfigdata") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->i2cConfigs.push_back(static_cast<uint32_t>(stoul(attributeList.at(m).firstChild().nodeValue().toStdString())));
                else configErrors.push_back("CONFIG ERROR: sensor i2c configuration stream not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("i2cdatafield") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->i2cDataField = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor i2c data field size not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("i2creaddelay") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->i2cReadDelay = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: sensor i2c read delay not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("lutindex") == 0){
                if (isInteger(attributeList.at(m).firstChild().nodeValue().toStdString()))
                    storedSensor->lutIndex = stoi(attributeList.at(m).firstChild().nodeValue().toStdString());
                else configErrors.push_back("CONFIG ERROR: LUT index not an integer");
            } else if (attributeList.at(m).nodeName().toStdString().compare("sensoroperation") == 0){
                QDomNode opItem = attributeList.at(m);
                QDomNodeList opItemList = opItem.childNodes();
                for (int n = 0; n < opItemList.size(); n++){
                    if (opItemList.at(n).nodeName().toStdString().compare("operation") == 0){
                        if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("SUM") == 0) storedSensor->senOperator = SUM;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("DIFF") == 0) storedSensor->senOperator = DIFF;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("MUL") == 0) storedSensor->senOperator = MUL;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("DIV") == 0) storedSensor->senOperator = DIV;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("AVG") == 0) storedSensor->senOperator = AVG;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("MAX") == 0) storedSensor->senOperator = MAX;
                        else if (opItemList.at(n).firstChild().nodeValue().toStdString().compare("MIN") == 0) storedSensor->senOperator = MIN;
                    } else if (opItemList.at(n).nodeName().toStdString().compare("opalias") == 0){
                        storedSensor->opAliases.push_back(opItemList.at(n).firstChild().nodeValue().toStdString());
                    }
                }
                dependentSensors.push_back(storedSensor);
            }else if (attributeList.at(m).nodeName().toStdString().compare("calpoly") == 0){
                QDomNode polyItem = attributeList.at(m);
                QDomNodeList polyItemList = polyItem.childNodes();
                int polyCount = 0;
                for (int n = 0; n < polyItemList.size(); n++){
                    poly item;
                    if (polyItemList.at(n).nodeName().toStdString().compare("coef") == 0){
                        item.exponent = polyCount;
                        item.coefficient = stod(polyItemList.at(n).firstChild().nodeValue().toStdString());
                        polyCount++;
                    }
                    storedSensor->calPolynomial.push_back(item);
                }
            }
        }
        sensorMap.insert(make_pair(storedSensor->sensorIndex,storedSensor));
        cout << "Sensor " << sensorMap[storedSensor->sensorIndex]->sensorName << " inserted into map" << endl;
    }

    cout << "Sensors Processed" << endl;
    //**************************************//
    //*****process group member sensors*****//
    //**************************************//
    for (int i = 0; i < groupNodes.size(); i++){
        string groupId;
        int sensorId;
        vector<meta *> sensors;

        //get group characteristics: groupId, minrate and maxrate
        QDomNodeList groupXteristics = groupNodes.at(i).childNodes();
        for (int j = 0; j < groupXteristics.size(); j++){
            if (groupXteristics.at(j).nodeName().toStdString().compare("name") == 0){
                groupId = groupXteristics.at(j).firstChild().nodeValue().toStdString();
            } else if (groupXteristics.at(j).nodeName().toStdString().compare("sensorid") == 0){
                sensorId = stoi(groupXteristics.at(j).firstChild().nodeValue().toStdString());
                if (sensorMap.count(sensorId) > 0) {
                    sensors.push_back(sensorMap[sensorId]);
                    sensorMap[sensorId]->groups.push_back(groupId);
                }
                else configErrors.push_back("CONFIG ERROR: sensor Id mismatch. Check IDs");
            }
        }
        cout << "Group Sensors Processed" << endl;

        //create group object
        grp = new Group(sensors,groupId,allResponses,logicVector,sensors,bootCmds);
        groupMap.insert(make_pair(grp->groupId,grp));
    }

    cout << "****************** depsensors size: " << dependentSensors.size() << endl;
    for (uint i = 0; i < dependentSensors.size(); i++){
        for (uint j = 0; j < dependentSensors.at(i)->opAliases.size(); j++){
            for (auto const& x : sensorMap){
                if (x.second->alias.compare(dependentSensors.at(i)->opAliases.at(j)) == 0){
                    x.second->dependencies.push_back(dependentSensors.at(i));
                    dependentSensors.at(i)->opSensors.push_back(x.second);
                }
            }
        }
    }

#ifdef CONFIG_PRINT
    cout << "Boot Configuration: " << endl;
    for (uint i = 0; i < bootCmds.bootCanCmds.size(); i++){
        cout << "CAN address: " << bootCmds.bootCanCmds.at(i).address << " data: " << bootCmds.bootCanCmds.at(i).data << " size: " << bootCmds.bootCanCmds.at(i).dataSize << endl;
    }
    for (uint i = 0; i < bootCmds.bootI2cCmds.size(); i++){
        cout << " I2c data: " << bootCmds.bootI2cCmds.at(i) << endl;
    }
    for (uint i = 0; i < bootCmds.bootGPIOCmds.size(); i++){
        cout << "GPIO pin: " << bootCmds.bootGPIOCmds.at(i).pin << " value: " << bootCmds.bootGPIOCmds.at(i).value << " mode: " << bootCmds.bootGPIOCmds.at(i).mode << endl;
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
    dbScript << "endtime char not null," << endl;
    dbScript << "recordindex char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists system_log(" << endl;
    dbScript << "time char not null," << endl;
    dbScript << "responseid char not null," << endl;
    dbScript << "message char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists sensor_data(" << endl;
    dbScript << "time char not null," << endl;
    dbScript << "sensorindex char not null," << endl;
    dbScript << "sensorname char not null," << endl;
    dbScript << "rawdata char not null," << endl;
    dbScript << "caldata char not null" << endl;
    dbScript << ");" << endl;

    dbScript << "create table if not exists sensors(" << endl;
    dbScript << "sensorindex char not null," << endl;
    dbScript << "sensorname char not null," << endl;
    dbScript << "minthreshold char not null," << endl;
    dbScript << "maxthreshold char not null," << endl;
    dbScript << "maxresponseid char not null," << endl;
    dbScript << "minresponseid char not null," << endl;
    dbScript << "calconstant char not null" << endl;
    dbScript << ");" << endl;

    //create group tables
    for (auto const &x : groupMap){
        string groupName = removeSpaces(x.second->groupId);
        string scriptTableArg = "create table if not exists " + groupName + "_sensors(";
        dbScript << scriptTableArg << endl;
        dbScript << "sensorindex char not null," << endl;
        dbScript << "sensorname char not null" << endl;
        dbScript << ");" << endl;
    }
    dbScript.close();

//    system("rm ./savedsessions/system.db");

    //run script
    dbase = new DB_Engine("./savedsessions/system.db");
    dbase->clear_tables_except("system_info");
    dbase->runScript("script.sql");

    //****************************************//
    //*****record all sensors to database*****//
    //****************************************//
    sensorColString = "sensorindex,sensorname,minthreshold,maxthreshold,maxresponseid,minresponseid,calconstant";
    for (auto const& x : sensorMap){
        sensorRowString = "'" + to_string(x.second->sensorIndex) + "','" + x.second->sensorName + "','" + to_string(x.second->minimum)
                + "','" + to_string(x.second->maximum) + "','" + to_string(x.second->maxRxnCode) +
                "','" + to_string(x.second->minRxnCode) + "','" + to_string(x.second->calConst) + "'";
        dbase->insert_row("sensors",sensorColString,sensorRowString);
    }

    for (auto const &x: groupMap){
        string grpSensorCol = "sensorindex,sensorname";
        string table_name = x.second->groupId + "_sensors";
        for (auto const &m: x.second->get_metadata()){
            string grpSensorRow = "'" + to_string(m->sensorIndex) + "','" + m->sensorName + "'";
            dbase->insert_row(table_name,grpSensorCol,grpSensorRow);
        }
    }

    //****************************************//
    //*****launch internal worker modules*****//
    //****************************************//
    usb7204 = new usb7402_interface(usbSensors);
    gpioInterface = new gpio_interface(gpioSensors,i2cSensors,allResponses);
    canInterface = new canbus_interface(canRate, canSensors);
    dataCtrl = new DataControl(gpioInterface,canInterface,usb7204,dbase,groupMap,sysStates,FSMs,
                               systemMode,controlSpecs,responseMap,bootConfigs,canSensorGroup,
                               bootCmds,canSyncs,i2cSyncs,gpioSyncs,recordSensorConfigs,recordStateConfigs,sensorMap);
    trafficTest = new TrafficTest(canSensorMap,gpioSensors,i2cSensors,usbSensors,canRate,gpioRate,usb7204Rate,dataCtrl);

    //********************************//
    //*****initialize system info*****//
    //********************************//
    systemColString = "starttime,endtime,recordindex";
    systemRowString = "'" + get_curr_time() + "','0','0'";
    if (dbase->number_of_rows("system_info") == 0) dbase->insert_row("system_info",systemColString,systemRowString);
    else {
        dbase->update_value("system_info","starttime","rowid","1",get_curr_time());
        dbase->update_value("system_info","endtime","rowid","1",to_string(0));
    }

    //*****************************//
    //*****set group databases*****//
    //*****************************//
    for (auto const &x: groupMap){
        x.second->setDB(dbase);
    }

    cout << "Returning from config" << endl;
    return true;
}

/**
 * @brief Config::isInteger checks whether string represents an integer
 * @param s : input string
 * @return true if input string is an integer. Otherwise returns false
 */
bool Config::isInteger(const string & s){
    char * p ;
    if(s.empty() || ((!isdigit(s[0])) && (s[0] != '-') && (s[0] != '+'))) return false ;
    strtol(s.c_str(), &p, 10) ;

    return (*p == 0);
}

/**
 * @brief Config::get_curr_time - retrieves current operation system time
 * @return
 */
string Config::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%D_%T",&now);
    return buf;
}

/**
 * @brief Function to remove all spaces from a given string
 */
string Config::removeSpaces(string &str)
    {
        int size = str.length();
        for(int j = 0; j<=size; j++){
            for(int i = 0; i <=j; i++){
                if(str[i] == ' ') str.erase(str.begin() + i);
            }
        }
        return str;
    }
