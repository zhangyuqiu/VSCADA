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
#include "datacontrol.h"

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface(std::vector<meta *> sensorVec, std::string modulename, std::vector<SubsystemThread *> subs, vector<system_state *> stts, DataControl * control, vector<statemachine *> FSMs);
    ~canbus_interface();
    datapoint getdatapoint(int index);
    datapoint getdatapoint_canadd(int canaddrss);
    std::string get_curr_time();

    DataControl * ctrl;
    std::vector<meta *> sensorVector;
    std::vector<SubsystemThread * > subsystems;
    std::vector<system_state *> states;
    std::vector<statemachine *> stateMachines;

private:

    QCanBusDevice *can_bus = nullptr;
    QString errmsg;
    bool canconnect();
    std::vector<datapoint> dpa;
    std::string modulename;

signals:
    void process_can_data(uint32_t addr, uint64_t data);

private slots:
    void recieve_frame();
    void sendFrame(response rsp);
    void sendData(int addr, uint64_t data);
};



#endif // CANBUS_INTERFACE_H
