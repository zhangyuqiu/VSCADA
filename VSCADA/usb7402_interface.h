#ifndef USB7402_INTERFACE_H
#define USB7402_INTERFACE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <vector>

#include "libusb_interface/pmd.h"
#include "libusb_interface/usb-7204.h"
#include <QObject>
#include <QTimer>
#include "typedefs.h"
#include "subsystemthread.h"

using namespace std;

class usb7402_interface : public QObject
{
    Q_OBJECT
public:
    usb7402_interface(vector<meta *> sensors, vector<SubsystemThread *> subs);
    ~usb7402_interface();

    double readChannel(uint8_t channel);
    int writeChannel(int channel);
    int startUSBCheck();

    QTimer * timer;
    vector<meta *> sensorVector;
    vector<SubsystemThread *> subsystems;

    float voltage;
    uint8_t gain;
    uint16_t wvalue;
    libusb_device_handle *udev;
    Calibration_AIN table_AIN[NMODE][NGAINS_USB7204][NCHAN_USB7204];

protected:
   virtual void usbCheckTasks();                      //runs collection tasks

private:
   /** Links the member function to ordinary space */
   static void * InternalThreadEntryFunc(void * This) {((usb7402_interface *)This)->usbCheckTasks(); return NULL;}

   pthread_t _thread;

public slots:
   void StartInternalThread(){InternalThreadEntryFunc(this);}
   void writeUSBData(uint8_t channel, float voltage);

signals:
   void sensorValueChanged(meta * sensor);
};

#endif // USB7402_INTERFACE_H
