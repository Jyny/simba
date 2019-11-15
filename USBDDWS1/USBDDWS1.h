#ifndef USBDDWS1_H
#define USBDDWS1_H

#include "USBHID.h"

class USBDDWS1: public USBHID {
public:
    USBDDWS1(uint16_t vendor_id = 0x1234, uint16_t product_id = 0x0006, uint16_t product_release = 0x0001):
        USBHID(0, 0, vendor_id, product_id, product_release, false)
            {
                connect();
            };
    bool send(HID_REPORT *report);
    bool sendNB(HID_REPORT *report);
    bool read(HID_REPORT * report);
    bool readNB(HID_REPORT * report);

protected:
    uint16_t reportLength;
    virtual uint8_t * reportDesc();
    virtual uint16_t reportDescLength();
    virtual uint8_t * stringIproductDesc();
    virtual uint8_t * stringIinterfaceDesc();
    virtual uint8_t * configurationDesc();
    virtual void HID_callbackSetReport(HID_REPORT *report){};
    virtual bool USBCallback_request();
    virtual bool USBCallback_setConfiguration(uint8_t configuration);

private:
    HID_REPORT outputReport;
    uint8_t output_length;
    uint8_t input_length;
};

#endif