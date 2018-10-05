#ifndef CANBUS_INTERFACE_H
#define CANBUS_INTERFACE_H

#include <QtDebug>
#include <QtSerialBus>
#include <QMainWindow>
#include <QString>
#include <array>
#include <vector>
#include "typedefs.h"

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface(std::vector<meta> sensorVector);
    ~canbus_interface();
    datapoint getdatapoint(uint32_t index);

private:

    QCanBusDevice *can_bus = nullptr;
    QString errmsg;
    bool canconnect();
    std::vector<datapoint> dpa;


private slots:
    void recieve_frame();
};



#endif // CANBUS_INTERFACE_H
