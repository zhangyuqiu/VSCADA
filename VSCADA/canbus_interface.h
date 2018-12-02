#ifndef CANBUS_INTERFACE_H
#define CANBUS_INTERFACE_H

#include <QtDebug>
#include <QtSerialBus>
#include <QMainWindow>
#include <QString>
#include <array>
#include <vector>

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface();
    ~canbus_interface();

private:
    QString errmsg;
    bool canconnect();
    QCanBusDevice *can_bus = nullptr;

signals:
    void process_can_data(uint32_t addr, uint64_t data);

private slots:
    void recieve_frame();
    void sendData(int addr, uint64_t data);
    void sendDataByte(int addr, uint64_t data, int size);
};

#endif // CANBUS_INTERFACE_H
