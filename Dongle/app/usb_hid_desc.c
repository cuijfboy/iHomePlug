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

#include "usb_hid_desc.h"
#include "usb_desc.h"

//-----------------------------------------------------------------------------
// Report Descriptor(s) Declarations
//-----------------------------------------------------------------------------

code const uint8_t g_usb_hid_report_1[HID_REPORT_DESCRIPTOR_SIZE_RC] = 
{
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x09, 0x00,                    //   USAGE (Undefined)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0xb1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xc0                           // END_COLLECTION
};


code hal_usb_hid_t g_usb_hid_hids[] = 
{
    { &g_usb_conf_desc.hid1, g_usb_hid_report_1, sizeof(g_usb_hid_report_1) }
};