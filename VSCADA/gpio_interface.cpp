#include "gpio_interface.h"

gpio_interface::gpio_interface(vector<meta *> gpioSen, vector<meta *> i2cSen, vector<response> responses, vector<SubsystemThread *> subs)
{
    subsystems = subs;
    timer = new QTimer;
    i2cSensors = i2cSen;
    gpioSensors = gpioSen;
    responseVector = responses;

    for (uint i = 0; i < gpioSensors.size(); i++){
        meta * currSensor = gpioSensors.at(i);
        if (currSensor->gpioPin >= 0){
            pinData.push_back(0);
            activePins.push_back(currSensor->gpioPin);
            GPIOExport(currSensor->gpioPin);
            GPIODirection(currSensor->gpioPin,IN);
        }
    }

    for (uint i = 0; i < i2cSensors.size(); i++){
        meta * currSensor = i2cSensors.at(i);
        if (currSensor->i2cAddress >= 0){
            i2cData.push_back(0);
            i2cSlaveAddresses.push_back(currSensor->i2cAddress);
        }
    }

    for (uint i = 0; i < responseVector.size(); i++){
        response currRsp = responseVector.at(i);
        if (currRsp.gpioPin >= 0){
            GPIOExport(currRsp.gpioPin);
            GPIODirection(currRsp.gpioPin,OUT);
        }
    }

    for (uint i = 0; i < subsystems.size(); i++){
        connect(this, SIGNAL(sensorValueChanged(meta*)), subsystems.at(i), SLOT(updateEdits(meta*)));
        connect(this, SIGNAL(sensorValueChanged(meta*)), subsystems.at(i), SLOT(checkThresholds(meta*)));
        connect(subsystems.at(i), SIGNAL(pushGPIOData(response)), this, SLOT(writeGPIOData(response)));
    }

    connect(timer, SIGNAL(timeout()), this, SLOT(StartInternalThread()));
    timer->start(500);
}

gpio_interface::~gpio_interface(){
    for (uint i = 0; i < gpioSensors.size(); i++){
        meta * currSensor = gpioSensors.at(i);
        if (currSensor->gpioPin >= 0){
            GPIOUnexport(currSensor->gpioPin);
        }
    }
    for (uint i = 0; i < responseVector.size(); i++){
        response currRsp = responseVector.at(i);
        if (currRsp.gpioPin >= 0){
            GPIOUnexport(currRsp.gpioPin);
        }
    }
}

/**
 * @brief GPIOExport - enables GPIO pin for read/write
 * @param pin
 * @return
 */
int gpio_interface::GPIOExport(int pin)
{
    #define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

/**
 * @brief GPIOUnexport - disables GPIO pin for read/write
 * @param pin
 * @return
 */
int gpio_interface::GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

/**
 * @brief GPIODirection - sets input/output direction
 * @param pin
 * @param dir
 * @return
 */
int gpio_interface::GPIODirection(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

    #define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        fprintf(stderr, "Failed to set direction!\n");
        return(-1);
    }

    close(fd);
    return(0);
}

/**
 * @brief GPIORead - reads GPIO pin
 * @param pin
 * @return
 */
int gpio_interface::GPIORead(int pin)
{
    #define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
        fprintf(stderr, "Failed to read value!\n");
        return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

/**
 * @brief GPIOWrite - writes to GPIO pin
 * @param pin
 * @param value
 * @return
 */
int gpio_interface::GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        fprintf(stderr, "Failed to write value!\n");
        return(-1);
    }

    close(fd);
    return(0);
}

void gpio_interface::gpioCheckTasks(){
    cout << "GPIO Tasks running" << endl;
    for (uint i = 0; i < activePins.size(); i++){
        int currVal = GPIORead(activePins.at(i));
        if (currVal != pinData.at(i)){
            pinData.at(i) = currVal;
            gpioSensors.at(i)->val = currVal;
            sensorValueChanged(gpioSensors.at(i));
        }
    }
    for (uint i = 0; i < i2cSlaveAddresses.size(); i++){
        int currVal = i2cRead(i2cSlaveAddresses.at(i));
        if (currVal != i2cData.at(i)){
            i2cData.at(i) = currVal;
            i2cSensors.at(i)->val = currVal;
            sensorValueChanged(i2cSensors.at(i));
        }
    }
}

void gpio_interface::writeGPIOData(response rsp){
    GPIOWrite(rsp.gpioPin,rsp.gpioValue);
}

int gpio_interface::startGPIOCheck(){
    StartInternalThread();
    return 0;
}

int gpio_interface::stopGPIOCheck(){
    //stop thread
    return 0;
}

int gpio_interface::i2cRead(int address){
    int file_i2c;
    long length = 4; //number of bytes to read
    unsigned char buffer[60] = {0};

    //----- OPEN THE I2C BUS -----//
    const char * filename = "/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDONLY)) < 0)
    {
        printf("Failed to open the i2c bus");
        return -1;
    }

    if (ioctl(file_i2c, I2C_SLAVE, address) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return -1;
    }


    //----- READ BYTES -----//
    //read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
    if (read(file_i2c, buffer, static_cast<unsigned long>(length)) != length)
    {
        //ERROR HANDLING: i2c transaction failed
        printf("Failed to read from the i2c bus.\n");
        return -1;
    }
    else
    {
        printf("Data read: %s\n", buffer);
    }
    return static_cast<int>(*buffer);
}

int gpio_interface::i2cWrite(int address, int data){
    int file_i2c;
    long length = 4;
    unsigned char buffer[60] = {0};
    unsigned char * writeData = nullptr;
    memcpy(writeData, &data, sizeof (data));

    //----- OPEN THE I2C BUS -----//
    const char * filename = "/dev/i2c-1";
    if ((file_i2c = open(filename, O_RDONLY)) < 0)
    {
        printf("Failed to open the i2c bus");
        return -1;
    }

    if (ioctl(file_i2c, I2C_SLAVE, address) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return -1;
    }

    //----- WRITE BYTES -----//
    //write() returns the number of bytes actually written,
    //if it doesn't match then an error occurred (e.g. no response from the device)
    if (write(file_i2c, buffer, static_cast<unsigned long>(length)) != length)
    {
        /* ERROR HANDLING: i2c transaction failed */
        printf("Failed to write to the i2c bus.\n");
        return -1;
    }
    return 0;
}