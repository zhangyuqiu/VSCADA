#include "canbus_interface.h"

canbus_interface::canbus_interface(std::vector<meta *> sensorVec, std::string modulename, std::vector<SubsystemThread *> subs, vector<system_state *> stts, DataControl *control, vector<statemachine *> FSMs) {
    this->modulename = modulename;
    ctrl = control;
    states = stts;
    stateMachines = FSMs;
    sensorVector = sensorVec;
    subsystems = subs;
    can_bus = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),QStringLiteral("can0"),&errmsg);
    canconnect();
    connect(can_bus, &QCanBusDevice::framesReceived, this, &canbus_interface::recieve_frame);

    for (uint i = 0; i < subsystems.size(); i++){
        connect(subsystems.at(i), SIGNAL(pushCANItem(response)), this, SLOT(sendFrame(response)));
    }

    datapoint dpt;
    for(uint i = 0; i < states.size(); i++){
        dpt.displayed = 0;
        dpt.monitored = 0;
        dpt.sensorIndex = -1;
        dpt.canAddress = states.at(i)->canAddress;
        dpt.gpioPin = -1;
        dpt.value = 0;
        dpa.push_back(dpt);
    }
    for(uint i = 0; i < stateMachines.size(); i++){
        for (uint j = 0; j < stateMachines.at(i)->states.size(); j++){
            dpt.displayed = 0;
            dpt.monitored = 0;
            dpt.sensorIndex = -1;
            dpt.canAddress = stateMachines.at(i)->states.at(j)->canAddress;
            dpt.gpioPin = -1;
            dpt.value = 0;
            dpa.push_back(dpt);
        }
    }

    for(uint i = 0; i < sensorVector.size(); i++){
        dpt.displayed = 0;
        dpt.monitored = 0;
        dpt.sensorIndex = sensorVector.at(i)->sensorIndex;
        dpt.canAddress = sensorVector.at(i)->canAddress;
        dpt.gpioPin = sensorVector.at(i)->gpioPin;
        dpt.value = 0;
        dpa.push_back(dpt);
    }
}

canbus_interface::~canbus_interface()
{
    delete can_bus;
}


bool canbus_interface::canconnect() {
    if (!can_bus) {
        qDebug() << "Error creating device" << endl;
        qDebug() << errmsg << endl;
        return false;
    }
    else {
        bool connectflag = can_bus->connectDevice();
        if (connectflag) {
            qDebug() << "Connected!" << endl;
            return true;
        }
        else {
            errmsg = can_bus->errorString();
            qDebug() << "Error connectiong device." << endl;
            qDebug() << errmsg << endl;
            return false;
        }
    }

}

void canbus_interface::recieve_frame() {
    qDebug() << "revieve_frame called" << endl;
    QCanBusFrame recframe = can_bus->readFrame();
    QByteArray a = recframe.payload();
    quint32 b = recframe.frameId();
    if (dpa.empty()) {
        qDebug() << "Datapoint array has no item" <<endl;
        return;
    }

    int data = 0;
    for (char i:a) {
        data = data + static_cast<int>(i);
    }

    for (uint i = 0; i < stateMachines.size(); i++){
        statemachine * currFSM = stateMachines.at(i);
        if (currFSM->canAddress == recframe.frameId()){
            cout << endl << "FSM found" << endl << endl;
            for (int j = 0; j < currFSM->states.size(); j++){
                system_state * currState = currFSM->states.at(j);
                if(currState->value == data){
                    cout << "ACTIVATING " << currState->name << endl;
                    ctrl->change_system_state(currState);
                } else if (currState->active){
                    cout << "DEACTIVATING " << currState->name << endl;
//                    emit ctrl->deactivateState(currState);
                    ctrl->deactivateLog(currState);
                }
            }
            emit ctrl->updateFSM(currFSM);
        }
    }

    for (uint i = 0; i < states.size(); i++){
        if(states.at(i)->canAddress == recframe.frameId() && states.at(i)->value == data){
            cout << endl << "State found" << endl << endl;
            ctrl->change_system_state(states.at(i));
        } else if (states.at(i)->canAddress == recframe.frameId()){
            emit ctrl->deactivateState(states.at(i));
        }
    }

    for(uint i = 0; i < sensorVector.size(); i++){
        if(sensorVector.at(i)->canAddress == recframe.frameId()){
            meta * currSensor = sensorVector.at(i);
            if (currSensor->val != data) {
                currSensor->val = data;
                for (uint j = 0; j < subsystems.size(); j++){
                    if (currSensor->subsystem.compare(subsystems.at(j)->subsystemId) == 0){
                        subsystems.at(j)->receiveData(currSensor);
                        break;
                    }
                }
            } else {
                for (uint j = 0; j < subsystems.size(); j++){
                    if (currSensor->subsystem.compare(subsystems.at(j)->subsystemId) == 0){
                        subsystems.at(j)->checkThresholds(currSensor);
                        subsystems.at(j)->updateEdits(currSensor);
                        break;
                    }
                }
            }
        }
    }

    cout << "recframe ID: " << recframe.frameId();
    qDebug() << "frame recieved" <<endl;
    qDebug() << data << endl;
//    qDebug() << QString::number(getdatapoint_canadd(recframe.frameId()).value) <<endl;
}

void canbus_interface::sendFrame(response rsp){
    cout << "sending out frame" << endl;
    QByteArray byteArr;
    QDataStream streamArr(&byteArr, QIODevice::WriteOnly);
    streamArr << rsp.canValue;
    QCanBusFrame * outFrame = new QCanBusFrame;
    outFrame->setFrameId(static_cast<quint32>(rsp.canAddress));
    outFrame->setPayload(byteArr);
    can_bus->writeFrame(*outFrame);
}

datapoint canbus_interface::getdatapoint(int index) { // return a datapoint with certain index. an empty datapoint will be returned if no such index
    for (datapoint d:dpa) {
        if (d.sensorIndex == index) {
            return d;
        }
    }
    datapoint edp;
    edp.sensorIndex = -1;
    return edp;
}

datapoint canbus_interface::getdatapoint_canadd(int canadd) { // return a datapoint with certain index. an empty datapoint will be returned if no such index
    for (datapoint d:dpa) {
        if (d.canAddress == canadd) {
            return d;
        }
    }
    datapoint edp;
    edp.sensorIndex = -1;
    return edp;
}

std::string canbus_interface::get_curr_time(){
    time_t t = time(nullptr);
    struct tm now = *localtime(&t);
    char buf[20];
    strftime(buf, sizeof(buf),"%X",&now);
    return buf;
}



