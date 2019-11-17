#include "mbed.h"
#include "USBDDWS1.h"
#include "QEI.h"

// Config
DigitalOut led(LED1);

// USB Config
InterruptIn button(USER_BUTTON);
USBDDWS1 ddw;

// QEI config
QEI wheel(D14, D15, NC, 1000, QEI::X4_ENCODING);

// Global Variable
int16_t steering_USB = 0;
int16_t accelerator_USB = 0;

void buttonRise(void)
{
    accelerator_USB = 100;
    led = true;
}

void buttonFall(void)
{
    accelerator_USB = 0;
    led = false;
    
}

int read_steering()
{
    int t = wheel.getPulses();
    if (t > 1270) {
        return 127;
    }
    if (t < -1270) {
        return -127;
    }
    return t/10;
}

int main()
{
    // Init
    button.rise(callback(buttonRise));
    button.fall(callback(buttonFall));

    // while
    while (1) {
        steering_USB = read_steering();
        ddw.steering(steering_USB);
        ddw.accelerator(accelerator_USB);
    }
}