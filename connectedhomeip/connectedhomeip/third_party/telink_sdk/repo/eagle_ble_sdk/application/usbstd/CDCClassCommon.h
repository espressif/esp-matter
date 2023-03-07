/********************************************************************************************************
 * @file	CDCClassCommon.h
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

/* Includes: */
#include "application/usbstd/stdDescriptors.h"
#include "tl_common.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
extern "C" {
#endif



#define CDC_CONTROL_LINE_OUT_DTR         BIT(0)
#define CDC_CONTROL_LINE_OUT_RTS         BIT(1)
#define CDC_CONTROL_LINE_IN_DCD          BIT(0)
#define CDC_CONTROL_LINE_IN_DSR          BIT(1)
#define CDC_CONTROL_LINE_IN_BREAK        BIT(2)
#define CDC_CONTROL_LINE_IN_RING         BIT(3)
#define CDC_CONTROL_LINE_IN_FRAMEERROR   BIT(4)
#define CDC_CONTROL_LINE_IN_PARITYERROR  BIT(5)
#define CDC_CONTROL_LINE_IN_OVERRUNERROR BIT(6)

#define CDC_FUNCTIONAL_DESCRIPTOR(DataSize) \
	 struct \
	 { \
		  USB_Descriptor_Hdr_t Header; \
		  u8 SubType; \
		  u8 Data[DataSize]; \
	 }

enum CDC_Descriptor_ClassSubclass_Protocol_t
{
	CDC_CSCP_NoSpecific_Subclass = 0x00,
	CDC_CSCP_NoSpecific_Protocol = 0x00,
	CDC_CSCP_NoData_Subclass = 0x00,
	CDC_CSCP_NoData_Protocol = 0x00,
	CDC_CSCP_ATCmd_Protocol = 0x01,
	CDC_CSCP_CDC_Class = 0x02,
	CDC_CSCP_ACM_Subclass = 0x02,
	CDC_CSCP_CDCData_Class = 0x0A,
	CDC_CSCP_VendorSpecific_Protocol = 0xFF,
};

enum CDC_Class_Requestions_t
{
	CDC_REQ_SendEncapsulated_Cmd,
	CDC_REQ_GetEncapsulated_Rsp,
	CDC_REQ_SetLine_Encoding = 0x20,
	CDC_REQ_GetLine_Encoding,
	CDC_REQ_SetControlLine_State,
	CDC_REQ_SendBreak,
};

enum CDC_Class_Notifications_t
{
	CDC_NOTIF_Serial_State = 0x20,
};


enum CDC_Descriptor_Subtypes_t
{
	CDC_DSUBTYPE_CSInterface_Header,
	CDC_DSUBTYPE_CSInterface_CallManagement,
	CDC_DSUBTYPE_CSInterface_ACM,
	CDC_DSUBTYPE_CSInterface_DirectLine,
	CDC_DSUBTYPE_CSInterface_TelephoneRinger,
	CDC_DSUBTYPE_CSInterface_TelephoneCall,
	CDC_DSUBTYPE_CSInterface_Union,
	CDC_DSUBTYPE_CSInterface_CountrySelection,
	CDC_DSUBTYPE_CSInterface_TelephoneOpModes,
	CDC_DSUBTYPE_CSInterface_USBTerminal,
	CDC_DSUBTYPE_CSInterface_NetworkChannel,
	CDC_DSUBTYPE_CSInterface_ProtocolUnit,
	CDC_DSUBTYPE_CSInterface_ExtensionUnit,
	CDC_DSUBTYPE_CSInterface_MultiChannel,
	CDC_DSUBTYPE_CSInterface_CAPI,
	CDC_DSUBTYPE_CSInterface_Ethernet,
	CDC_DSUBTYPE_CSInterface_ATM,
};

enum CDC_LineEncoding_Formats_t
{
	CDC_LINEENCODING_OneStopBit,
	CDC_LINEENCODING_OneAndAHalfStopBits,
	CDC_LINEENCODING_TwoStopBits,
};

enum CDC_LineEncoding_Parity_t
{
	CDC_PARITY_None,
	CDC_PARITY_Odd,
	CDC_PARITY_Even,
	CDC_PARITY_Mark,
	CDC_PARITY_Space,
};

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u16 CDCSpecification;
} USB_CDC_Descriptor_FunctionalHeader_t;

typedef struct
{
	u8 bFunctionLength;
	u8 bDescriptorType;
	u8 bDescriptorSubType;
	u16 bcdCDC;
} USB_CDC_StdDescriptor_FunctionalHeader_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 Capabilities;
} USB_CDC_Descriptor_FunctionalACM_t;

typedef struct
{
	u8 bFunctionLength;
	u8 bDescriptorType;
	u8 bDescriptorSubType;
	u8 bmCapabilities;
} USB_CDC_StdDescriptor_FunctionalACM_t;

typedef struct
{
	USB_Descriptor_Hdr_t Header;
	u8 Subtype;
	u8 MasterInterfaceNumber;
	u8 SlaveInterfaceNumber;
} USB_CDC_Descriptor_FunctionalUnion_t;

typedef struct
{
	u8 bFunctionLength;
	u8 bDescriptorType;
	u8 bDescriptorSubType;
	u8 bMasterInterface;
	u8 bSlaveInterface0;
} USB_CDC_StdDescriptor_FunctionalUnion_t;

typedef struct
{
	u32 BaudRateBPS;
	u8 CharFormat;
	u8 ParityType;
	u8 DataBits;
} CDC_LineEncoding_t;

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	}
#endif
