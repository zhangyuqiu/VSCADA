#ifndef GPIO_INTERFACE_H
#define GPIO_INTERFACE_H
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */

class gpio_interface
{
public:
    gpio_interface();
    int GPIOExport(int pin);
    int GPIOUnexport(int pin);
    int GPIODirection(int pin, int dir);
    int GPIORead(int pin);
    int GPIOWrite(int pin, int value);
};

#endif // GPIO_INTERFACE_H
