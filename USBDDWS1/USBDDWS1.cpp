#include "stdint.h"
#include "USBDDWS1.h"

bool USBDDWS1::send(HID_REPORT *report)
{
    return write(EPINT_IN, report->data, report->length, MAX_HID_REPORT_SIZE);
}

bool USBDDWS1::sendNB(HID_REPORT *report)
{
    return writeNB(EPINT_IN, report->data, report->length, MAX_HID_REPORT_SIZE);
}


bool USBDDWS1::read(HID_REPORT *report)
{
    uint32_t bytesRead = 0;
    bool result;
    result = USBDevice::readEP(EPINT_OUT, report->data, &bytesRead, MAX_HID_REPORT_SIZE);
    if(!readStart(EPINT_OUT, MAX_HID_REPORT_SIZE))
        return false;
    report->length = bytesRead;
    return result;
}


bool USBDDWS1::readNB(HID_REPORT *report)
{
    uint32_t bytesRead = 0;
    bool result;
    result = USBDevice::readEP_NB(EPINT_OUT, report->data, &bytesRead, MAX_HID_REPORT_SIZE);
    // if readEP_NB did not succeed, does not issue a readStart
    if (!result)
        return false;
    report->length = bytesRead;
    if(!readStart(EPINT_OUT, MAX_HID_REPORT_SIZE))
        return false;
    return result;
}


uint16_t USBDDWS1::reportDescLength() {
    reportDesc();
    return reportLength;
}



//
//  Route callbacks from lower layers to class(es)
//


// Called in ISR context
// Called by USBDevice on Endpoint0 request
// This is used to handle extensions to standard requests
// and class specific requests
// Return true if class handles this request
bool USBDDWS1::USBCallback_request() {
    bool success = false;
    CONTROL_TRANSFER * transfer = getTransferPtr();
    uint8_t *hidDescriptor;

    // Process additional standard requests

    if ((transfer->setup.bmRequestType.Type == STANDARD_TYPE))
    {
        switch (transfer->setup.bRequest)
        {
            case GET_DESCRIPTOR:
                switch (DESCRIPTOR_TYPE(transfer->setup.wValue))
                {
                    case REPORT_DESCRIPTOR:
                        if ((reportDesc() != NULL) \
                            && (reportDescLength() != 0))
                        {
                            transfer->remaining = reportDescLength();
                            transfer->ptr = reportDesc();
                            transfer->direction = DEVICE_TO_HOST;
                            success = true;
                        }
                        break;
                    case HID_DESCRIPTOR:
                            // Find the HID descriptor, after the configuration descriptor
                            hidDescriptor = findDescriptor(HID_DESCRIPTOR);
                            if (hidDescriptor != NULL)
                            {
                                transfer->remaining = HID_DESCRIPTOR_LENGTH;
                                transfer->ptr = hidDescriptor;
                                transfer->direction = DEVICE_TO_HOST;
                                success = true;
                            }
                            break;

                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    // Process class-specific requests

    if (transfer->setup.bmRequestType.Type == CLASS_TYPE)
    {
        switch (transfer->setup.bRequest)
        {
             case SET_REPORT:
                // First byte will be used for report ID
                outputReport.data[0] = transfer->setup.wValue & 0xff;
                outputReport.length = transfer->setup.wLength + 1;

                transfer->remaining = sizeof(outputReport.data) - 1;
                transfer->ptr = &outputReport.data[1];
                transfer->direction = HOST_TO_DEVICE;
                transfer->notify = true;
                success = true;
            default:
                break;
        }
    }

    return success;
}


#define DEFAULT_CONFIGURATION (1)


// Called in ISR context
// Set configuration. Return false if the
// configuration is not supported
bool USBDDWS1::USBCallback_setConfiguration(uint8_t configuration) {
    if (configuration != DEFAULT_CONFIGURATION) {
        return false;
    }

    // Configure endpoints > 0
    addEndpoint(EPINT_IN, MAX_PACKET_SIZE_EPINT);
    addEndpoint(EPINT_OUT, MAX_PACKET_SIZE_EPINT);

    // We activate the endpoint to be able to recceive data
    readStart(EPINT_OUT, MAX_PACKET_SIZE_EPINT);
    return true;
}


uint8_t * USBDDWS1::stringIinterfaceDesc() {
    static uint8_t stringIinterfaceDescriptor[] = {
        0x08,               //bLength
        STRING_DESCRIPTOR,  //bDescriptorType 0x03
        'H',0,'I',0,'D',0,  //bString iInterface - HID
    };
    return stringIinterfaceDescriptor;
}

uint8_t * USBDDWS1::stringIproductDesc() {
    static uint8_t stringIproductDescriptor[] = {
        0x16,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'H',0,'I',0,'D',0,' ',0,'D',0,'E',0,'V',0,'I',0,'C',0,'E',0 //bString iProduct - HID device
    };
    return stringIproductDescriptor;
}



uint8_t * USBDDWS1::reportDesc() {
    static uint8_t reportDescriptor[] = {
        USAGE_PAGE(2), LSB(0xFFAB), MSB(0xFFAB),
        USAGE(2), LSB(0x0200), MSB(0x0200),
        COLLECTION(1), 0x01, // Collection (Application)

        REPORT_SIZE(1), 0x08, // 8 bits
        LOGICAL_MINIMUM(1), 0x00,
        LOGICAL_MAXIMUM(1), 0xFF,

        REPORT_COUNT(1), input_length,
        USAGE(1), 0x01,
        INPUT(1), 0x02, // Data, Var, Abs

        REPORT_COUNT(1), output_length,
        USAGE(1), 0x02,
        OUTPUT(1), 0x02, // Data, Var, Abs

        END_COLLECTION(0),
    };
    reportLength = sizeof(reportDescriptor);
    return reportDescriptor;
}

#define DEFAULT_CONFIGURATION (1)
#define TOTAL_DESCRIPTOR_LENGTH ((1 * CONFIGURATION_DESCRIPTOR_LENGTH) \
                               + (1 * INTERFACE_DESCRIPTOR_LENGTH) \
                               + (1 * HID_DESCRIPTOR_LENGTH) \
                               + (2 * ENDPOINT_DESCRIPTOR_LENGTH))

uint8_t * USBDDWS1::configurationDesc() {
    static uint8_t configurationDescriptor[] = {
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
        HID_SUBCLASS_NONE,                  // bInterfaceSubClass
        HID_PROTOCOL_NONE,                  // bInterfaceProtocol
        0x00,                               // iInterface

        HID_DESCRIPTOR_LENGTH,              // bLength
        HID_DESCRIPTOR,                     // bDescriptorType
        LSB(HID_VERSION_1_11),              // bcdHID (LSB)
        MSB(HID_VERSION_1_11),              // bcdHID (MSB)
        0x00,                               // bCountryCode
        0x01,                               // bNumDescriptors
        REPORT_DESCRIPTOR,                  // bDescriptorType
        (uint8_t)(LSB(reportDescLength())), // wDescriptorLength (LSB)
        (uint8_t)(MSB(reportDescLength())), // wDescriptorLength (MSB)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        PHY_TO_DESC(EPINT_IN),              // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)

        ENDPOINT_DESCRIPTOR_LENGTH,         // bLength
        ENDPOINT_DESCRIPTOR,                // bDescriptorType
        PHY_TO_DESC(EPINT_OUT),             // bEndpointAddress
        E_INTERRUPT,                        // bmAttributes
        LSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (LSB)
        MSB(MAX_PACKET_SIZE_EPINT),         // wMaxPacketSize (MSB)
        1,                                  // bInterval (milliseconds)
    };
    return configurationDescriptor;
}
