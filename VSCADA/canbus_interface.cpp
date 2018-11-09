#include "canbus_interface.h"

/**
 * @brief canbus_interface::canbus_interface class constructor
 */
canbus_interface::canbus_interface() {
    can_bus = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),QStringLiteral("can0"),&errmsg);
    canconnect();
    connect(can_bus, &QCanBusDevice::framesReceived, this, &canbus_interface::recieve_frame);

}

/**
 * @brief canbus_interface::~canbus_interface class destructor
 */
canbus_interface::~canbus_interface()
{
    delete can_bus;
}

/**
 * @brief canbus_interface::canconnect establishes connection with canbus shield
 * @return true on success, false otherwise
 */
bool canbus_interface::canconnect() {
    if (!can_bus) {
        qDebug() << "Error creating device" << endl;
        qDebug() << errmsg << endl;
        return false;
    } else {
        bool connectflag = can_bus->connectDevice();
        if (connectflag) {
            qDebug() << "Connected!" << endl;
            return true;
        } else {
            errmsg = can_bus->errorString();
            qDebug() << "Error connectiong device." << endl;
            qDebug() << errmsg << endl;
            return false;
        }
    }
}

/**
 * @brief canbus_interface::recieve_frame signal triggered when CAN frame is received
 */
void canbus_interface::recieve_frame() {
    qDebug() << "revieve_frame called" << endl;
    QCanBusFrame recframe = can_bus->readFrame();
    QByteArray b = recframe.payload();
    uint32_t a = recframe.frameId();

    uint64_t data = 0;
    qDebug() << "QByteArray: " << b << endl;
    for (int i = 0; i < b.size(); i++){
        data = data + ((static_cast<uint64_t>(b[i]) & 0xFF) << ((b.size()-1)*8 - i*8));
    }
    emit process_can_data(a,data);
}

/**
 * @brief canbus_interface::sendData sends 64 bit data over CAN to specified address
 * @param addr CAN address
 * @param data : data to be sent
 */
void canbus_interface::sendData(int addr, uint64_t data){
    qDebug() << "actively wirting" << endl;
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



