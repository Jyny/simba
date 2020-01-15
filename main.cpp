#include "mbed.h"
#include "SIMBAUSB.h"
#include "QEI.h"

// wheel angle config
const int support_max_angle = 1620/2;
int wheel_max_angle = 900/2;

// Config
DigitalOut led(LED1);

// USB Config
InterruptIn button(USER_BUTTON);
SIMBAUSB simbaUSB;

// QEI config
QEI steering(D14, D15, NC, 4000, QEI::X4_ENCODING);

// Global Variable
int16_t steering_USB = 0;
int button_no = 7;
bool button_press = false;
int countt = 0;

void buttonRise(void)
{
    if(countt < 10) {
        return;
    }
    
    led = true;
    button_press = true;
    if(button_no>=7) {
        button_no = 0;
    } else {
        button_no++;
    }
}

void buttonFall(void)
{
    led = false;
    button_press = false;
    countt = 0;
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
    set_time(0);
    button.rise(callback(buttonRise));
    button.fall(callback(buttonFall));

    // while
    while (1) {
        steering_USB = read_steering();
        simbaUSB.steering(steering_USB);
        if(button_press) {
            switch(button_no) {
                case 0:
                    simbaUSB.button_A(true);
                    break;
                case 1:
                    simbaUSB.button_DPadUp(true);
                    break;
                case 2:
                    simbaUSB.button_DPadRight(true);
                    break;
                case 3:
                    simbaUSB.button_B(true);
                    break;
                case 4:
                    simbaUSB.button_X(true);
                    break;
                case 5:
                    simbaUSB.button_Y(true);
                    break;
                case 6:
                    simbaUSB.button_DPadDown(true);
                    break;
                case 7:
                    simbaUSB.button_DPadLeft(true);
                    break;
                default:
                    break;
            }
        } else {
            simbaUSB.button_A(false);
            simbaUSB.button_DPadUp(false);
            simbaUSB.button_DPadRight(false);
            simbaUSB.button_B(false);
            simbaUSB.button_Y(false);
            simbaUSB.button_X(false);
            simbaUSB.button_DPadDown(false);
            simbaUSB.button_DPadLeft(false);
        }
        simbaUSB.update();
        countt++;
    }
}