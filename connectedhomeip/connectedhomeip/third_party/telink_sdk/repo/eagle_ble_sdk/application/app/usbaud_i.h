/********************************************************************************************************
 * @file	usbaud_i.h
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

#include "drivers.h"
#include "usbaud.h"
#include "application/usbstd/AudioClassCommon.h"
#include "application/usbstd/usb.h"

#if 0
static const USB_Descriptor_HIDReport_Datatype_t usbaud_report_desc[] = {
	HID_RI_USAGE_PAGE(8, 0x0c), /* Consumer Page */
	HID_RI_USAGE(8, 0x01)		, /* Consumer Controls */
    HID_RI_COLLECTION(8, 0x01)		, /* Application */

    HID_RI_REPORT_ID(8, 0x01)		, /*Report ID*/

    HID_RI_LOGICAL_MINIMUM(8, 0x00)		,
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),

    HID_RI_USAGE(8, 0xcd),  /* Play/Pause (toggle) */
	HID_RI_USAGE(8, 0xb5)		, /* Next Track */
	HID_RI_USAGE(8, 0xb6)		, /* Previous Track */
	HID_RI_USAGE(8, 0xb7)		, /* Stop */

    HID_RI_REPORT_SIZE(8, 0x01)		,
    HID_RI_REPORT_COUNT(8, 0x04),
    HID_RI_INPUT(8, HID_IOF_VARIABLE),
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),
    
    HID_RI_USAGE(8, 0xb0),  /* Play */
    HID_RI_USAGE(8, 0xb1)		, /* Pause */
    HID_RI_USAGE(8, 0xb3)		, /* Fast Forward */
    HID_RI_USAGE(8, 0xb4)		, /* Rewind */

    HID_RI_REPORT_SIZE(8, 0x01)		,
    HID_RI_REPORT_COUNT(8, 0x04),
    HID_RI_INPUT(8, HID_IOF_NO_PREFERRED_STATE | HID_IOF_VARIABLE),
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
    HID_RI_LOGICAL_MAXIMUM(8, 0x01),
    HID_RI_USAGE(8, 0xe9),
    HID_RI_USAGE(8, 0xea),
    HID_RI_USAGE(8, 0xe2),
    HID_RI_REPORT_SIZE(8, 0x01),
    HID_RI_REPORT_COUNT(8, 0x03),
    HID_RI_INPUT(8, HID_IOF_NO_PREFERRED_STATE | HID_IOF_VARIABLE),
    HID_RI_REPORT_SIZE(8, 0x05),
    HID_RI_REPORT_COUNT(8, 0x01),
    HID_RI_INPUT(8, HID_IOF_CONSTANT),
    HID_RI_END_COLLECTION(0),
	HID_RI_USAGE_PAGE(16, 0xffa0),
    HID_RI_USAGE(8, 0x01),
    HID_RI_COLLECTION(8, 0x01),  /* Application */

    HID_RI_REPORT_ID(8, 0x02)		, /*Report ID*/

    HID_RI_USAGE(8, 0x01)		,
    HID_RI_LOGICAL_MINIMUM(8, 0x00),
	HID_RI_LOGICAL_MAXIMUM(16, 0x00ff),
    HID_RI_REPORT_SIZE(8, 0x08),
    HID_RI_REPORT_COUNT(8, 0x02),
    HID_RI_OUTPUT(8, HID_IOF_DATA),
    HID_RI_USAGE(8, 0x02),  /* mouse? */

    HID_RI_REPORT_SIZE(8, 0x08)		,
    HID_RI_REPORT_COUNT(8, 0x02),
    HID_RI_INPUT(8, HID_IOF_DATA),
    HID_RI_END_COLLECTION(0),  };

#endif

//Definition of USB HID report descriptor
static const USB_Descriptor_HIDReport_Datatype_t usbaud_report_desc[] = {
		0x05, 0x0c, 0x09, 0x01, 0xa1, 0x01,
		0x85, 0x01, 0x15, 0x00, 0x25, 0x01, 0x09, 0xcd, 0x09, 0xb5, 0x09, 0xb6,
		0x09, 0xb7, 0x75, 0x01, 0x95, 0x04, 0x81, 0x02, 0x15, 0x00, 0x25, 0x01,
		0x09, 0xb0, 0x09, 0xb1, 0x09, 0xb3, 0x09, 0xb4, 0x75, 0x01, 0x95, 0x04,
		0x81, 0x22, 0x15, 0x00, 0x25, 0x01, 0x09, 0xe9, 0x09, 0xea, 0x09, 0xe2,
		0x75, 0x01, 0x95, 0x03, 0x81, 0x22, 0x75, 0x05, 0x95, 0x01, 0x81, 0x01,
		0xc0, 0x06, 0xa0, 0xff, 0x09, 0x01, 0xa1, 0x01, 0x85, 0x02, 0x09, 0x01,
		0x15, 0x00, 0x26, 0xff, 0x00, 0x75, 0x08, 0x95, 0x02, 0x91, 0x00, 0x09,
		0x02, 0x75, 0x08, 0x95, 0x02, 0x81, 0x00, 0xc0, };

static inline u8* usbaud_get_report_desc(void) {
	return (u8*) (usbaud_report_desc);
}

static inline u16 usbaud_get_report_desc_size(void) {
	return sizeof(usbaud_report_desc);
}
