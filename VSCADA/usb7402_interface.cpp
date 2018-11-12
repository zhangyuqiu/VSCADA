#include "usb7402_interface.h"

/**
 * @brief usb7402_interface::usb7402_interface : class constructor
 * @param sensors : configures USB7204 configured sensors
 * @param subs : configured subsystems
 */
usb7402_interface::usb7402_interface(vector<meta *> sensors, vector<SubsystemThread *> subs)
{
    subsystems = subs;
    sensorVector = sensors;
    udev = 0;
                //    int ret = libusb_init(NULL);
                //    if (ret < 0) {
                //      perror("libusb_init: Failed to initialize libusb");
                //      exit(1);
                //    }

                //    if ((udev = usb_device_find_USB_MCC(USB7204_PID, NULL))) {
                //      printf("USB-7204 Device is found!\n");
                //    } else {
                //      printf("No device found.\n");
                //      exit(0);
                //    }

                //    // some initialization
                //    printf("Building calibration table.  This may take a while ...\n");
                //    usbBuildGainTable_USB7204(udev, table_AIN);
                //    int i = 0;
                //    int j = 0;
                //    for (i = 0; i < NGAINS_USB7204; i++ ) {
                //      for (j = 0; j < NCHAN_USB7204/2; j++) {
                //        printf("Calibration Table: Range = %d Channel = %d Slope = %f   Offset = %f\n",
                //           i, j, table_AIN[DF][i][j].slope, table_AIN[DF][i][j].intercept);
                //      }
                //    }
                //    i = BP_10_00V;
                //    for (j = 0; j < NCHAN_USB7204; j++) {
                //      printf("Calibration Table: Range = %d Channel = %d Slope = %f   Offset = %f\n",
                //         i, j, table_AIN[SE][i][j].slope, table_AIN[SE][i][j].intercept);
                //    }

                //    //print out the wMaxPacketSize.  Should be 64.
                //    printf("\nwMaxPacketSize = %d\n", usb_get_max_packet_size(udev,0));

                //    struct tm date;

                //    // Print the calibration date
                //    getMFGCAL_USB7204(udev, &date);
                //    printf("\nLast Calibration date: %s", asctime(&date));

    gain = BP_5_00V;
    voltage = 4.9;
    timer = new QTimer;
    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));

    for (uint i = 0; i < subsystems.size(); i++){
        connect(this, SIGNAL(sensorValueChanged(meta*)), subsystems.at(i), SLOT(receiveData(meta*)));
        connect(subsystems.at(i), SIGNAL(pushGPIOData(response)), this, SLOT(writeGPIOData(response)));
    }
}

/**
 * @brief usb7402_interface::~usb7402_interface : class destructor
 */
usb7402_interface::~usb7402_interface()
{
    //blah...
}

/**
 * @brief usb7402_interface::writeUSBData : writes data to specified channel
 * @param channel : channel to write data to
 * @param voltage : value to write to channel
 */
void usb7402_interface::writeUSBData(uint8_t channel, float voltage){
    usbAOut_USB7204(udev, channel, voltage);
}

/**
 * @brief usb7402_interface::setSamplingRate : sets the sampling rate of reading USB sensors
 * @param newRate : rate to be set
 * @return
 */
void usb7402_interface::setSamplingRate(int newRate){
    samplingRate = newRate;
    stopUSBCheck();
    startUSBCheck();
}

/**
 * @brief usb7402_interface::startUSBCheck : start sampling data
 * @return
 */
void usb7402_interface::startUSBCheck(){
    timer->start(samplingRate);
}

/**
 * @brief usb7402_interface::stopUSBCheck : stop sampling data
 */
void usb7402_interface::stopUSBCheck(){
    timer->stop();
}

/**
 * @brief usb7402_interface::readChannel : reads specified channel
 * @param channel : channel to be read
 * @return
 */
double usb7402_interface::readChannel(uint8_t channel){
    int flag = fcntl(fileno(stdin), F_GETFL);
    fcntl(0, F_SETFL, flag | O_NONBLOCK);

    wvalue = usbAIn_USB7204(udev, DF, channel, gain);
    wvalue = rint(wvalue*table_AIN[DF][gain][channel].slope + table_AIN[DF][gain][channel].intercept);
    double readVal = volts_USB7204(wvalue, gain);
    printf("Channel: %d: value = %#hx, %.4fV\n", channel, wvalue, readVal);
    fcntl(fileno(stdin), F_SETFL, flag);

    return readVal;
}

/**
 * @brief usb7402_interface::usbCheckTasks : tasks to check whether sensor values have changed
 */
void usb7402_interface::usbCheckTasks(){
    for (uint i = 0; i < sensorVector.size(); i++){
        meta * currSensor = sensorVector.at(i);
        double val = readChannel(static_cast<uint8_t>(currSensor->usbChannel));
        if (abs(currSensor->val - val) > 0.001){
            currSensor->val = val;
            emit sensorValueChanged(currSensor);
        }
    }
}
