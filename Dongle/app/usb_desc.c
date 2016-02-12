/* Copyright (c) 2008 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision$
 */

#include "usb_desc.h"
#include "nordic_common.h"               

#define MAX_PACKET_SIZE_EP0 32
#define EP1_PACKET_SIZE 	32

code const hal_usb_dev_desc_t g_usb_dev_desc =
{
	sizeof(hal_usb_dev_desc_t),         // bLength
	0x01,                               // bDescriptorType
	SWAP(0x0200),                       // bcdUSB
	0x00,                               // bDeviceClass
	0x00,                               // bDeviceSubClass
	0x00,                               // bDeviceProtocol
	MAX_PACKET_SIZE_EP0,                // bMaxPacketSize0
	SWAP(VID),                          // idVendor (Nordic )
	SWAP(PID),                          // idProduct (dongle)
	SWAP(0x0100),                       // bcdDevice
	0x01,                               // iManufacturer
	0x02,                               // iProduct
	0x03,                               // iSerialNumber
	0x01                                // bNumConfigurations
};

// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."
code const usb_conf_desc_templ_t g_usb_conf_desc =
{
	{ // configuration_descriptor hid_configuration_descriptor
		sizeof(hal_usb_conf_desc_t),         	// Length
		0x02,                               	// Type                             
		SWAP(sizeof(usb_conf_desc_templ_t)),    // Totallength
		0x01,                               	// NumInterfaces
		0x01,                               	// bConfigurationValue
		0x00,                               	// iConfiguration
		0xA0,                               	// bmAttributes (0x80 + Remote Wakeup)
		0x32                                	// MaxPower (in 2mA units) = 100 * 2mA
	},
	{ // interface_descriptor hid_interface_descriptor    (REMOTE CONTROL)
		sizeof(hal_usb_if_desc_t),          	// Length
		0x04,                               	// bDescriptorType
		0x00,                               	// bInterfaceNumber
		0x00,                               	// bAlternateSetting
		0x01,                               	// bNumEndpoints
		0x03,                               	// bInterfaceClass (3 = HID)
		0x00,                               	// bInterfaceSubClass
		0x00,                               	// bInterfaceProcotol(0=None, 1=Keyb, 2=Mouse)
		0x00                                	// iInterface
	},
	{ // class_descriptor hid_descriptor
		sizeof(hal_usb_hid_desc_t),         	// Length
		0x21,                               	// bDescriptorType
		SWAP(0x0111),                       	// bcdHID
		0x00,                               	// bCountryCode
		0x01,                               	// bNumDescriptors
		0x22,                               	// bDescriptorType
		SWAP(HID_REPORT_DESCRIPTOR_SIZE_RC),	// wItemLength (tot. len. of report
		                						// descriptor)
	},
	{ // endpoint_descriptor hid_endpoint_in_descriptor
		sizeof(hal_usb_ep_desc_t),          	// Length
		0x05,                               	// bDescriptorType
		0x81,                               	// bEndpointAddress
		0x03,                               	// bmAttributes
		SWAP(EP1_PACKET_SIZE),              	// MaxPacketSize (LITTLE ENDIAN)
		2                                   	// bInterval
	},
	{ // endpoint_descriptor hid_endpoint_in_descriptor
		sizeof(hal_usb_ep_desc_t),          	// Length
		0x05,                               	// bDescriptorType
		0x91,                               	// bEndpointAddress
		0x03,                               	// bmAttributes
		SWAP(EP1_PACKET_SIZE),              	// MaxPacketSize (LITTLE ENDIAN)
		2                                   	// bInterval
	}									   	
};

#define USB_STRING_IDX_1_DESC "iClass"
#define USB_STRING_IDX_2_DESC "iClass Host"
#define USB_STRING_IDX_3_DESC "0.01"

code uint8_t g_usb_string_desc_1[] = 
{
	sizeof(USB_STRING_IDX_1_DESC) * 2, 
	0x03, 
	'i',00,
	'C',00,
	'l',00,
	'a',00,
	's',00,
	's',00,
};

code uint8_t g_usb_string_desc_2[] = 
{
	sizeof(USB_STRING_IDX_2_DESC) * 2,
	0x03, 
	'i',00,
	'C',00,
	'l',00,
	'a',00,
	's',00,
	's',00,
	' ',00,
	'H',00,
	'o',00,
	's',00,
	't',00,
};

//This is a dummy serial number. The real serial number should be
//written by the application
code uint8_t g_usb_string_desc_3[] = 
{
	sizeof(USB_STRING_IDX_3_DESC) * 2, 
	0x03, 
	0xFF,0xFF,
	0xFF,0xFF,
	0xFF,0xFF,
	0xFF,0xFF
};

code usb_string_desc_templ_t g_usb_string_desc = 
{
	g_usb_string_desc_1,
	g_usb_string_desc_2,
	g_usb_string_desc_3
};