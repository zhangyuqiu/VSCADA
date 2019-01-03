#include "traffictest.h"

TrafficTest::TrafficTest(vector<meta*> can, vector<meta*> gpio, vector<meta*> i2c, vector<meta*> usb, int cRate, int gRate, int uRate, DataControl * ctrl)
{
    canSensors = can;
    gpioSensors = gpio;
    i2cSensors = i2c;
    usbSensors = usb;
    canRate = 500;
    gpioRate = gRate;
    usbRate = uRate;
    dataCtrl = ctrl;

    connect(canTimer, SIGNAL(timeout()), this, SLOT(sendCANItems()));
    connect(gpioTimer, SIGNAL(timeout()), this, SLOT(sendGPIOItems()));
    connect(gpioTimer, SIGNAL(timeout()), this, SLOT(sendI2CItems()));
    connect(usbTimer, SIGNAL(timeout()), this, SLOT(sendUSBItems()));
}

TrafficTest::~TrafficTest(){

}

void TrafficTest::startTests(){
    canTimer->start(canRate);
    gpioTimer->start(gpioRate);
    usbTimer->start(usbRate);
}

void TrafficTest::sendCANItems(){
    int num = rand();
    uint64_t num64 = (static_cast<uint64_t>(num) << 32) + static_cast<uint64_t>(num);
    for (uint i = 0; i < canSensors.size(); i++){
        dataCtrl->receive_can_data(canSensors.at(i)->primAddress,num64);
    }
}

void TrafficTest::sendGPIOItems(){
    for (uint i = 0; i < gpioSensors.size(); i++){
        int range = static_cast<int>(gpioSensors.at(i)->maximum - gpioSensors.at(i)->minimum) + 10;
        gpioSensors.at(i)->val = (rand() % range) + (gpioSensors.at(i)->minimum - 5);
        dataCtrl->receive_sensor_data(gpioSensors.at(i));
    }
}

void TrafficTest::sendI2CItems(){
    for (uint i = 0; i < i2cSensors.size(); i++){
        int range = static_cast<int>(i2cSensors.at(i)->maximum - i2cSensors.at(i)->minimum) + 10;
        i2cSensors.at(i)->val = (rand() % range) + (i2cSensors.at(i)->minimum - 5);
        dataCtrl->receive_sensor_data(i2cSensors.at(i));
    }
}

void TrafficTest::sendUSBItems(){
    for (uint i = 0; i < usbSensors.size(); i++){
        int range = static_cast<int>(usbSensors.at(i)->maximum - usbSensors.at(i)->minimum) + 10;
        usbSensors.at(i)->val = (rand() % range) + (usbSensors.at(i)->minimum - 5);
        dataCtrl->receive_sensor_data(usbSensors.at(i));
    }
}
