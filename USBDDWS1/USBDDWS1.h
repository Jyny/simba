#ifndef USBDDWS1_H
#define USBDDWS1_H

#include "USBHID.h"

class USBDDWS1: public USBHID
{
public:
    USBDDWS1(bool connect_blocking = true, uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0006, uint16_t product_release = 0x0001);
    USBDDWS1(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release);
    virtual ~USBDDWS1();

    bool steering(int16_t t);

protected:
    virtual const uint8_t *report_desc();
    bool update();

private:
    int8_t _steering;

    void _init();
};

#endif