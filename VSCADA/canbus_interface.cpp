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
    connect(ctrl, SIGNAL(sendCANData(int, uint64_t)), this, SLOT(sendData(int, uint64_t)));

    for (uint i = 0; i < subsystems.size(); i++){
        connect(subsystems.at(i), SIGNAL(pushCANItem(response)), this, SLOT(sendFrame(response)));
    }
    connect(this, SIGNAL(process_can_data(uint32_t,uint64_t)), ctrl, SLOT(receive_can_data(uint32_t,uint64_t)));

    datapoint dpt;
    for(uint i = 0; i < states.size(); i++){
        dpt.displayed = 0;
        dpt.monitored = 0;
        dpt.sensorIndex = -1;
        dpt.primAddress = states.at(i)->primAddress;
        dpt.gpioPin = -1;
        dpt.value = 0;
        dpa.push_back(dpt);
    }
    for(uint i = 0; i < stateMachines.size(); i++){
        for (uint j = 0; j < stateMachines.at(i)->states.size(); j++){
            dpt.displayed = 0;
            dpt.monitored = 0;
            dpt.sensorIndex = -1;
            dpt.primAddress = stateMachines.at(i)->states.at(j)->primAddress;
            dpt.gpioPin = -1;
            dpt.value = 0;
            dpa.push_back(dpt);
        }
    }

    for(uint i = 0; i < sensorVector.size(); i++){
        dpt.displayed = 0;
        dpt.monitored = 0;
        dpt.sensorIndex = sensorVector.at(i)->sensorIndex;
        dpt.primAddress = sensorVector.at(i)->primAddress;
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
    QByteArray b = recframe.payload();
    uint32_t a = recframe.frameId();
    if (dpa.empty()) {
        qDebug() << "Datapoint array has no item" <<endl;
        return;
    }

    uint64_t data = 0;
    qDebug() << "QByteArray: " << b << endl;
    for (int i = 0; i < b.size(); i++){
        data = data + ((static_cast<uint64_t>(b[i]) & 0xFF) << ((b.size()-1)*8 - i*8));
    }
    emit process_can_data(a,data);
}

void canbus_interface::sendFrame(response rsp){
    cout << "sending out frame" << endl;
    QByteArray byteArr;
    QDataStream streamArr(&byteArr, QIODevice::WriteOnly);
    streamArr << rsp.canValue;
    QCanBusFrame * outFrame = new QCanBusFrame;
    outFrame->setFrameId(static_cast<quint32>(rsp.primAddress));
    outFrame->setPayload(byteArr);
    can_bus->writeFrame(*outFrame);
}

void canbus_interface::sendData(int addr, uint64_t data){
    cout << "actively wirting" << endl;
    QByteArray byteArr;
    char * charData = static_cast<char*>(static_cast<void*>(&data));
    for(int i = sizeof(data)-1; i >= 0; i--){
        byteArr.append(charData[i]);
    }
    qDebug() << "QByteArray Value: " << byteArr << endl;
    QCanBusFrame * outFrame = new QCanBusFrame;
    outFrame->setFrameId(static_cast<quint32>(addr));
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
        if (d.primAddress == canadd) {
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



