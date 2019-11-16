#include "stdint.h"
#include "ThisThread.h"
#include "usb_phy_api.h"

#include "USBDDWS1.h"

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

const uint8_t *USBDDWS1::report_desc()
{
    static const uint8_t report_descriptor[] = {
        USAGE_PAGE(1), 0x01,           // Generic Desktop
        USAGE(1), 0x04,                // Joystick
        COLLECTION(1), 0x01,           // Application
        USAGE_PAGE(1), 0x02,           // Simulation Controls
        USAGE(1), 0xC8,                // Steering
        LOGICAL_MINIMUM(1), 0x81,      // -127
        LOGICAL_MAXIMUM(1), 0x7f,      // 127
        REPORT_SIZE(1), 0x08,
        REPORT_COUNT(1), 0x01,
        INPUT(1), 0x06,                // Data, Variable, Relative
        END_COLLECTION(0)
    };
    reportLength = sizeof(report_descriptor);
    return report_descriptor;
}

#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * HID_DESCRIPTOR_LENGTH) \
                               + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

const uint8_t *USBDDWS1::configuration_desc(uint8_t index)
{
    if (index != 0) {
        return NULL;
    }
    uint8_t configuration_descriptor_temp[] = {
        CONFIGURATION_DESCRIPTOR_LENGTH,    // bLength
        CONFIGURATION_DESCRIPTOR,           // bDescriptorType
        LSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (LSB)
        MSB(TOTAL_DESCRIPTOR_LENGTH),       // wTotalLength (MSB)
        0x01,                               // bNumInterfaces
        DEFAULT_CONFIGURATION,              // bConfigurationValue
        0x00,                               // iConfiguration
        C_RESERVED | C_SELF_POWERED,        // bmAttributes
        C_POWER(0),                         // bMaxPower

        INTERFACE_DESCRIPTOR_LENGTH,        // bLength
        INTERFACE_DESCRIPTOR,               // bDescriptorType
        0x00,                               // bInterfaceNumber
        0x00,                               // bAlternateSetting
        0x02,                               // bNumEndpoints
        HID_CLASS,                          // bInterfaceClass
        HID_SUBCLASS_BOOT,                  // bInterfaceSubClass
        HID_PROTOCOL_MOUSE,                 // bInterfaceProtocol
        0x00,                               // iInterface

        HID_DESCRIPTOR_LENGTH,              // bLength
        HID_DESCRIPTOR,                     // bDescriptorType
        LSB(HID_VERSION_1_11),              // bcdHID (LSB)
        MSB(HID_VERSION_1_11),              // bcdHID (MSB)
        0x00,                               // bCountryCode
        0x01,                               // bNumDescriptors
        REPORT_DESCRIPTOR,                  // bDescriptorType
        (uint8_t)(LSB(report_desc_length())), // wDescriptorLength (LSB)
        (uint8_t)(MSB(report_desc_length())), // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        _int_in,                            // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        _int_out,                           // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (LSB)
        MSB(MAX_HID_REPORT_SIZE),           // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)
    };
    MBED_ASSERT(sizeof(configuration_descriptor_temp) == sizeof(_configuration_descriptor));
    memcpy(_configuration_descriptor, configuration_descriptor_temp, sizeof(_configuration_descriptor));
    return _configuration_descriptor;
}