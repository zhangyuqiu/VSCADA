#ifndef CANBUS_INTERFACE_H
#define CANBUS_INTERFACE_H

#include <QtDebug>
#include <QtSerialBus>
#include <QMainWindow>
#include <QString>
#include <array>
#include <vector>
#include "typedefs.h"
#include "subsystemthread.h"

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface(std::vector<meta *> sensorVec, std::string modulename, std::vector<SubsystemThread *> subs);
    ~canbus_interface();
    datapoint getdatapoint(uint32_t index);
    datapoint getdatapoint_canadd(uint32_t canaddrss);
    std::string get_curr_time();

    std::vector<meta *> sensorVector;
    std::vector<SubsystemThread * > subsystems;

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
