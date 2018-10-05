#include "canbus_interface.h"

canbus_interface::canbus_interface() {
    can_bus = QCanBus::instance()->createDevice(QStringLiteral("socketcan"),QStringLiteral("can0"),&errmsg);
    canconnect();
    connect(can_bus, &QCanBusDevice::framesReceived, this, &canbus_interface::recieve_frame);
//    connect(can_bus, &QCanBusDevice::framesReceived, this, &MainWindow::recieve_frame);
}

canbus_interface::~canbus_interface()
{

}


bool canbus_interface::canconnect() {
    if (!can_bus) {
        qDebug() << "Error creating device" << endl;
        qDebug() << errmsg << endl;
//        ui->textOut->append(errmsg);
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
//            ui->textOut->append(errmsg);
            return false;
        }
    }

}



void canbus_interface::recieve_frame() {
    qDebug() << "revieve_frame called" << endl;
//    bool ok;
    QCanBusFrame recframe = can_bus->readFrame();
//    ui->textOut->append(recframe.toString());
//    ui->textOut->append(QString::number(recframe.frameId()));
//    ui->textOut->append(QString::number(recframe.payload().toDouble(&ok)));
    QByteArray a = recframe.payload();

    int data = 0;
    for (char i:a) {
        data = data + (int)i;
    }

    datapoint dp;
    dp.displayed = 0;
    dp.monitored = 0;
    dp.sensorIndex = recframe.frameId();
    dp.value = data;

    for (int i = 0; i < 10; i++) {
        if (dpa[i].sensorIndex == dp.sensorIndex) {
            dpa[i] = dp;
            break;
        }
    }

    qDebug() << "frame recieved" <<endl;
    qDebug() << QString::number(data) <<endl;
}

