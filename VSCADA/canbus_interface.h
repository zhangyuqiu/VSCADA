#ifndef CANBUS_INTERFACE_H
#define CANBUS_INTERFACE_H

#include <QtDebug>
#include <QtSerialBus>
#include <QMainWindow>
#include <QString>
#include <array>
#include <vector>

using namespace std;

class canbus_interface : public QObject
{
    Q_OBJECT
public:
    canbus_interface(int canRate);
    ~canbus_interface();
    void enableCAN();

    QString errmsg;
    bool canconnect();
    QCanBusDevice * can_bus = nullptr;
    int bitrate;

signals:
    void process_can_data(uint32_t addr, uint64_t data);
    void pushMsg(string str);

private slots:
    void recieve_frame();
    void sendData(int addr, uint64_t data);
    void sendDataByte(int addr, uint64_t data, int bytes);
    void rebootCAN();
};

#endif // CANBUS_INTERFACE_H
