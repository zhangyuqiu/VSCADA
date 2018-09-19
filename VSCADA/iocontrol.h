#ifndef IOCONTROL_H
#define IOCONTROL_H
#include <QtSerialBus>

class iocontrol
{
public:
    iocontrol();
    ~iocontrol();
    int canBusInit();
};

#endif // IOCONTROL_H
