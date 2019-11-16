#include "mbed.h"
#include "USBDDWS1.h"
 
 // Config
DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);
USBDDWS1 ddw();

void buttonRise(void){
    led = true;
}

void buttonFall(void){
    led = false;
}
 
int main()
{
    // Init
    button.rise(callback(buttonRise));
    button.fall(callback(buttonFall));
    
    // while
    while (1) {
    }
}