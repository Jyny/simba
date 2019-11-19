#ifndef USBDDWS1_H
#define USBDDWS1_H

#include "USBHID.h"

class USBDDWS1: public USBHID
{
public:
    USBDDWS1(bool connect_blocking = true, uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0006, uint16_t product_release = 0x0001);
    USBDDWS1(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release);
    virtual ~USBDDWS1();

    bool update();
    void steering(int16_t t);
    void accelerator(int16_t t);
    void brake(int16_t t);

    void button1(bool b);
    void button2(bool b);
    void button3(bool b);
    void button4(bool b);
    void button5(bool b);
    void button6(bool b);
    void button7(bool b);
    void button8(bool b);

protected:
    virtual const uint8_t *report_desc();

private:
    int16_t _steering;
    int8_t _accelerator;
    int8_t _brake;
    int8_t _button;

    void _init();
};

#endif