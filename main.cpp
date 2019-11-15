#include "mbed.h"
#include "USB.h"
 
 // Config
DigitalOut led(LED1);
InterruptIn button(USER_BUTTON);

void buttonFall(){
}
 
int main()
{
    // Init
    led = true
    button.fall(callback(buttonFall));
    
    // while
    while (1) {
    }
}