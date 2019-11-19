#include "mbed.h"
#include "USBDDWS1.h"
#include "QEI.h"

// wheel angle config
const int support_max_angle = 1620/2;
int wheel_max_angle = 900/2;

// Config
DigitalOut led(LED1);

// USB Config
InterruptIn button(USER_BUTTON);
USBDDWS1 ddw;

// QEI config
QEI steering(D14, D15, NC, 4000, QEI::X4_ENCODING);

// Global Variable
int16_t steering_USB = 0;
int8_t accelerator_USB = 0;
int8_t brake_USB = 0;
int8_t button_USB = 0x00;

void buttonRise(void)
{
    accelerator_USB = 0;
    brake_USB = 100;
    ddw.button1(true);
    ddw.button2(true);
    led = true;
}

void buttonFall(void)
{
    accelerator_USB = 100;
    brake_USB = 0;
    ddw.button1(false);
    ddw.button2(false);
    led = false;
}

int read_steering()
{
    int t = steering.getPulses()*360/4000;

    // resolution multiplier
    t = t*support_max_angle/wheel_max_angle;

    if (t < -support_max_angle) {
        return -support_max_angle;
    }
    if (t > support_max_angle) {
        return support_max_angle;
    }
    return t;
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
        ddw.brake(brake_USB);
        ddw.update();
    }
}