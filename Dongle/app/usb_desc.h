/* Copyright (c) 2006 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 */

/** @file
 * Definition of the USB configuration descriptor type
 */

/** @{
 * @ingroup nordic_usb
 */
#ifndef  USB_DESC_H__
#define  USB_DESC_H__

#include <stdint.h>
#include <hal_usb_desc.h>
#include <hal_usb_hid_desc.h>

#define USB_DESC_TEMPLATE

//------------------------------------------
// Vendor ID and Product ID definitions
//------------------------------------------
#define VID   0x16C0
#define PID   0x05DF

//---------------------------------------------
// HID Configuration Descriptor Type Definition
//---------------------------------------------
// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
// "When a Get_Descriptor(Configuration) request is issued,
// it returns the Configuration descriptor, all Interface descriptors,
// all Endpoint descriptors, and the HID descriptor for each interface."

#define USB_STRING_DESC_COUNT 3

typedef struct {
    hal_usb_conf_desc_t conf;
    hal_usb_if_desc_t 	if0;  
    hal_usb_hid_desc_t 	hid1;   
    hal_usb_ep_desc_t 	ep1in;

} usb_conf_desc_templ_t; 

typedef struct {
     volatile uint8_t* idx[USB_STRING_DESC_COUNT];
} usb_string_desc_templ_t;

extern code usb_string_desc_templ_t g_usb_string_desc;
extern code usb_conf_desc_templ_t 	g_usb_conf_desc;
extern code hal_usb_dev_desc_t 		g_usb_dev_desc;

typedef struct {
     hal_usb_dev_desc_t* 		dev;
     usb_conf_desc_templ_t* 	conf;
     usb_string_desc_templ_t* 	string;
     uint8_t 					string_zero[4];
} usb_descs_templ_t;

#define HID_REPORT_DESCRIPTOR_SIZE_RC 33

extern code uint8_t g_usb_hid_report_1[HID_REPORT_DESCRIPTOR_SIZE_RC];

#endif  /* _USB_DESC_H_ */

/** @} */   