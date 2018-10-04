#ifndef CANBUS_INTERFACE_H
#define CANBUS_INTERFACE_H

#include <QtDebug>
#include <QtSerialBus>
#include <QMainWindow>
#include <QString>
#include <array>
#include "typedefs.h"

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface();
    ~canbus_interface();

private:

    QCanBusDevice *can_bus = nullptr;
    QString errmsg;
    bool canconnect();
    datapoint dpa[10];
    datapoint getdatapoint(int index);

private slots:
    void recieve_frame();
};



#endif // CANBUS_INTERFACE_H
