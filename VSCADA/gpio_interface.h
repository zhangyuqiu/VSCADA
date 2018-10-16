#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H
#include <QObject>
#include <QTimer>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "typedefs.h"
#include "subsystemthread.h"
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

using namespace std;

class gpio_interface : public QObject
{
    Q_OBJECT

public:
    gpio_interface(vector<meta *> gpioSen, vector<meta *> i2cSen, vector<response> responses, vector<SubsystemThread *> subs);
    ~gpio_interface();
    int GPIOExport(int pin);
    int GPIOUnexport(int pin);
    int GPIODirection(int pin, int dir);
    int GPIORead(int pin);
    int GPIOWrite(int pin, int value);
    int startGPIOCheck();
    int stopGPIOCheck();
    int i2cRead(int address);
    int i2cWrite(int address, int data);

    QTimer * timer;
    vector<int> pinData;
    vector<int> i2cData;
    vector<int> activePins;
    vector<meta *> i2cSensors;
    vector<meta *> gpioSensors;
    vector<int> i2cSlaveAddresses;
    vector<response> responseVector;
    vector<SubsystemThread *> subsystems;

 protected:
    virtual void gpioCheckTasks();                      //runs collection tasks

private:
    /** Links the member function to ordinary space */
    static void * InternalThreadEntryFunc(void * This) {((gpio_interface *)This)->gpioCheckTasks(); return NULL;}

    pthread_t _thread;

public slots:
    void StartInternalThread(){InternalThreadEntryFunc(this);}
    void writeGPIOData(response rsp);

signals:
    void sensorValueChanged(meta * sensor);
};

#endif // GPIO_INTERFACE_H
