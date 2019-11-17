#include "USBDDWS1.h"

#include "stdint.h"
#include "usb_phy_api.h"

USBDDWS1::USBDDWS1(bool connect_blocking, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : USBHID(get_usb_phy(), 0, 0, vendor_id, product_id, product_release)
{
    if (connect_blocking) {
        USBDevice::connect();
        wait_ready();
    } else {
        init();
    }
}

USBDDWS1::USBDDWS1(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : USBHID(get_usb_phy(), 0, 0, vendor_id, product_id, product_release)
{
}

USBDDWS1::~USBDDWS1()
{
    deinit();
}

void USBDDWS1::_init()
{
    _steering = 0;
}

bool USBDDWS1::update()
{
    HID_REPORT report;

    report.data[0] = _steering & 0xffff;
    report.data[1] = (_steering & 0xffff)>>8;
    report.data[2] = _accelerator & 0xff;
    report.data[3] = _brake & 0xff;
    report.length = 4;

    bool ret = send(&report);
    return ret;
}

void USBDDWS1::steering(int16_t s)
{
    _steering = s;
}

void USBDDWS1::accelerator(int16_t a)
{
    _accelerator = a;
}

void USBDDWS1::brake(int16_t a)
{
    _brake = a;
}

const uint8_t *USBDDWS1::report_desc()
{
    static const uint8_t report_descriptor[] = {
        USAGE_PAGE(1), 0x01,           // Generic Desktop
        USAGE(1), 0x04,                // Joystick
        COLLECTION(1), 0x01,           // Application

        USAGE_PAGE(1), 0x02,           // Simulation Controls
        USAGE(1), 0xC8,                // Steering
        LOGICAL_MINIMUM(2), 0xD6, 0xFC,     // -810
        LOGICAL_MAXIMUM(2), 0x2A, 0x03,     // 810
        REPORT_SIZE(1), 0x10,
        REPORT_COUNT(1), 0x01,
        INPUT(1), 0x02,                // Data, Variable, Absolute

        USAGE_PAGE(1), 0x02,           // Simulation Controls
        USAGE(1), 0xC4,                // Accelerator
        LOGICAL_MINIMUM(1), 0x00,      // 0
        LOGICAL_MAXIMUM(1), 0x7f,      // 127
        REPORT_SIZE(1), 0x08,
        REPORT_COUNT(1), 0x01,
        INPUT(1), 0x02,                // Data, Variable, Absolute
        
        USAGE_PAGE(1), 0x02,           // Simulation Controls
        USAGE(1), 0xC5,                // Brake
        LOGICAL_MINIMUM(1), 0x00,      // 0
        LOGICAL_MAXIMUM(1), 0x7f,      // 127
        REPORT_SIZE(1), 0x08,
        REPORT_COUNT(1), 0x01,
        INPUT(1), 0x02,                // Data, Variable, Absolute

        END_COLLECTION(0)
    };
    reportLength = sizeof(report_descriptor);
    return report_descriptor;
}
