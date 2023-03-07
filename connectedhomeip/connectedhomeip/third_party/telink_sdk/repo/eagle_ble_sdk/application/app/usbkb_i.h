/********************************************************************************************************
 * @file	usbkb_i.h
 *
 * @brief	This is the header file for BLE SDK
 *
 * @author	BLE GROUP
 * @date	2020.06
 *
 * @par     Copyright (c) 2020, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#pragma once

#include "usbkb.h"
#include "application/usbstd/usb.h"

/** HID class report descriptor. This is a special descriptor constructed with values from the
 *  USBIF HID class specification to describe the reports and capabilities of the HID device. This
 *  descriptor is parsed by the host and its contents used to determine what data (and in what encoding)
 *  the device will send, and what it may be sent back from the host. Refer to the HID specification for
 *  more details on HID report descriptors.
 */
static const USB_Descriptor_HIDReport_Datatype_t keyboard_report_desc[] = {
    HID_DESCRIPTOR_KEYBOARD(KEYBOARD_REPORT_KEY_MAX),
};

static inline u8* usbkb_get_report_desc(void) {
	return (u8*) (keyboard_report_desc);
}

static inline u16 usbkb_get_report_desc_size(void) {
	return sizeof(keyboard_report_desc);
}

#if (AUDIO_HOGP)
static const USB_Descriptor_HIDReport_Datatype_t audio_hogp_report_desc[] = {

	    0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
	    0x09, 0x02,        // Usage (tmp usage)
	    0xA1, 0x02,        //   Collection (Logical)

	    0x85, 0x02,        //   Report ID (2)
	    0x09, 0x14,        //   Usage (tmp usage)
	    0x75, 0x08,        //   Report Size (8)
	    0x95, 0x14,        //   Report Count (20)
	    0x15, 0x80,        //   Logical Minimum (128)
	    0x25, 0x7F,        //   Logical Maximum (127)
	    0x81, 0x22,        //   Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)

	    0x85, 0x0a,        //   Report ID (10)
	    0x09, 0x14,        //   Usage (tmp usage)
	    0x75, 0x08,        //   Report Size (8)
	    0x95, 0x14,        //   Report Count (20)
	    0x15, 0x80,        //   Logical Minimum (128)
	    0x25, 0x7F,        //   Logical Maximum (127)
	    0x81, 0x22,        //   Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)

	    0x85, 0x0b,        //   Report ID (11)
	    0x09, 0x14,        //   Usage (tmp usage)
	    0x75, 0x08,        //   Report Size (8)
	    0x95, 0x14,        //   Report Count (20)
	    0x15, 0x80,        //   Logical Minimum (128)
	    0x25, 0x7F,        //   Logical Maximum (127)
	    0x81, 0x22,        //   Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)


	    0x85, 0x0c,        //   Report ID (12)
	    0x09, 0x14,        //   Usage (tmp usage)
	    0x75, 0x08,        //   Report Size (8)
	    0x95, 0x14,        //   Report Count (20)
	    0x15, 0x80,        //   Logical Minimum (128)
	    0x25, 0x7F,        //   Logical Maximum (127)
	    0x81, 0x22,        //   Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)

	    0x85, 0x04,        //   Report ID (4)
	    0x09, 0x04,        //   Usage (tmp usage)
	    0x75, 0x08,        //   Report Size (8)
	    0x95, 0x01,        //   Report Count (1)
	    0x91, 0x02,        //   Output

	    0xC0,              // End Collection
};
static const USB_Descriptor_HIDReport_Datatype_t vendor_report_desc[] = {

		0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
		0x09, 0x02, 	   // Usage (tmp usage)
		0xA1, 0x02, 	   //	Collection (Logical)
		0x85, 0x03, 	   //	Report ID (3)
		0x09, 0x14, 	   //	Usage (tmp usage)
		0x75, 0x08, 	   //	Report Size (8)
		0x95, 0x20, 	   //	Report Count (20)
		0x15, 0x80, 	   //	Logical Minimum (128)
		0x25, 0x7F, 	   //	Logical Maximum (127)
		0x81, 0x22, 	   //	Input (Data,Var,Abs,No Wrap,Linear,No Preferred State,No Null Position)

		0x85, 0x04, 	   //	Report ID (4)
		0x09, 0x04, 	   //	Usage (tmp usage)
		0x75, 0x08, 	   //	Report Size (8)
		0x95, 0x20, 	   //	Report Count (1)
		0x91, 0x02, 	   //	Output

		0xC0,			   // End Collection

};

static inline u8* usbaudio_hogp_get_report_desc(void) {
	return (u8*) (audio_hogp_report_desc);
}
static inline u16 usbaudio_hogp_get_report_desc_size(void) {
	return sizeof(audio_hogp_report_desc);
}

static inline u8* usb_vendor_get_report_desc(void) {
	return (u8*) (vendor_report_desc);
}
static inline u16 usb_vendor_get_report_desc_size(void) {
	return sizeof(vendor_report_desc);
}
#endif

