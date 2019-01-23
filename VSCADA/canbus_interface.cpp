#include "canbus_interface.h"

/**
 * @brief canbus_interface::canbus_interface class constructor
 */
canbus_interface::canbus_interface(int canRate, vector<SubsystemThread *> subs) {
    subsystems = subs;
    bitrate = canRate;

    for (uint i = 0; i < subsystems.size(); i++){
        connect(subsystems.at(i), SIGNAL(sendCANData(int, uint64_t,int)), this,SLOT(sendDataByte(int, uint64_t,int)));
    }

    can_bus = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),QStringLiteral("can0"),&errmsg);
    canconnect();
    rebootCAN();
}

/**
 * @brief canbus_interface::~canbus_interface class destructor
 */
canbus_interface::~canbus_interface()
{
    if (can_bus != nullptr) delete can_bus;

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
            qDebug() << "Error connecting device." << endl;
            qDebug() << errmsg << endl;
            return false;
        }
    }
}

/**
 * @brief canbus_interface::recieve_frame signal triggered when CAN frame is received
 */
void canbus_interface::recieve_frame() {
    while (can_bus->framesAvailable()){
        QCanBusFrame recframe = can_bus->readFrame();
        QByteArray b = recframe.payload();
        uint32_t a = recframe.frameId();

        int64_t data = 0;
//        qDebug() << "CAN address: " << a << ", QByteArray: " << b << endl;
        for (int i = 0; i < b.size(); i++){
            data = data + ((static_cast<uint64_t>(b[i]) & 0xFF) << ((b.size()-1)*8 - i*8));
        }
        QCoreApplication::processEvents();
        emit process_can_data(a,data);
    }
}

/**
 * @brief canbus_interface::sendData sends 64 bit data over CAN to specified address
 * @param addr CAN address
 * @param data : data to be sent
 */
void canbus_interface::sendData(int addr, uint64_t data){
    QByteArray byteArr;
    char * charData = static_cast<char*>(static_cast<void*>(&data));
    for(int i = sizeof(data)-1; i >= 0; i--){
        byteArr.append(charData[i]);
    }
//    qDebug() << "Address: " << addr << " Value: " << byteArr << endl;
    QCanBusFrame * outFrame = new QCanBusFrame;
    outFrame->setFrameId(static_cast<quint32>(addr));
    outFrame->setPayload(byteArr);
    can_bus->writeFrame(*outFrame);

    delete outFrame;
}

/**
 * @brief canbus_interface::sendDataByte sends specified bit data over CAN to specified address
 * @param addr CAN address
 * @param data : data to be sent
 */
void canbus_interface::sendDataByte(int addr, uint64_t data, int bytes){
    QByteArray byteArr;
    char * charData = static_cast<char*>(static_cast<void*>(&data));
    for(int i = bytes-1; i >= 0; i--){
        byteArr.append(charData[i]);
    }
//    qDebug() << "Address: " << addr << " Value: " << byteArr << endl;
    QCanBusFrame * outFrame = new QCanBusFrame;
    outFrame->setFrameId(static_cast<quint32>(addr));
    outFrame->setPayload(byteArr);
    can_bus->writeFrame(*outFrame);

    delete outFrame;
}

/**
 * @brief canbus_interface::rebootCAN resets CAN network and sets bitrate
 */
void canbus_interface::rebootCAN(){
    system("sudo ip link set can0 down");
    std::string s = "sudo ip link set can0 up type can bitrate " + std::to_string(bitrate);
    const char * command = s.c_str();
    printf("Reboot Command: %c", *command);
    system(command);
    if (canconnect()){
        emit pushMsg("CAN boot successful");
    } else {
        emit pushMsg("ERROR: CAN boot failed");
    }
}

void canbus_interface::enableCAN(){
    connect(can_bus, &QCanBusDevice::framesReceived, this, &canbus_interface::recieve_frame);
}
