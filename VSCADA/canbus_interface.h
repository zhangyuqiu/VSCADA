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
    canbus_interface(std::vector<meta> sensorVector, std::string modulename);
    ~canbus_interface();
    datapoint getdatapoint(uint32_t index);
    datapoint getdatapoint_canadd(uint32_t canaddrss);
    std::string get_curr_time();

private:

    QCanBusDevice *can_bus = nullptr;
    QString errmsg;
    bool canconnect();
    std::vector<datapoint> dpa;
    std::string modulename;

private slots:
    void recieve_frame();

};



#endif // CANBUS_INTERFACE_H
