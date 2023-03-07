/********************************************************************************************************
 * @file	myudb_usbdesc.c
 *
 * @brief	This is the source file for BLE SDK
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
#if(1)

#include "drivers.h"
#include "myudb.h"
#include "myudb_usbdesc.h"

// request parameters
/** Language descriptor structure. This descriptor, located in FLASH memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
static const USB_Descriptor_String_t language_desc = { {
		sizeof(USB_Descriptor_Header_t) + 2, DTYPE_String },
		{ LANGUAGE_ID_ENG } };

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const USB_Descriptor_String_t vendor_desc = { { sizeof(USB_Descriptor_Header_t)
		+ sizeof(MYUDB_STRING_VENDOR) - 2, DTYPE_String }, // Header
		MYUDB_STRING_VENDOR };

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const USB_Descriptor_String_t product_desc = { {
		sizeof(USB_Descriptor_Header_t) + sizeof(MYUDB_STRING_PRODUCT) - 2,
		DTYPE_String }, // Header
		MYUDB_STRING_PRODUCT };

/** Serial number string. This is a Unicode string containing the device's unique serial number, expressed as a
 *  series of uppercase hexadecimal digits.
 */
static const USB_Descriptor_String_t serial_desc = { { sizeof(USB_Descriptor_Header_t)
		+ sizeof(MYUDB_STRING_SERIAL) - 2, DTYPE_String }, // Header
		MYUDB_STRING_SERIAL };


static const USB_Descriptor_Device_t device_desc = { {
		sizeof(USB_Descriptor_Device_t), DTYPE_Device }, // Header
		0x0200, // USBSpecification, USB 2.0
		USB_CSCP_NoDeviceClass, // Class
		USB_CSCP_NoDeviceSubclass, // SubClass
		USB_CSCP_NoDeviceProtocol, // Protocol
		8, // Endpoint0Size, Maximum Packet Size for Zero Endpoint. Valid Sizes are 8, 16, 32, 64
		MYUDB_ID_VENDOR, // VendorID
		MYUDB_ID_PRODUCT, // ProductID
		MYUDB_ID_VERSION/*0x0100*/, // .ReleaseNumber
		MYUDB_USB_STRING_VENDOR, 	// .ManufacturerStrIndex
		MYUDB_USB_STRING_PRODUCT, // .ProductStrIndex
		0, 	// .SerialNumStrIndex, iSerialNumber
		1 };

static const MYUDB_USB_Descriptor_Configuration_t
		configuration_desc = {
				{
					{sizeof(USB_Descriptor_Configuration_Header_t),
					DTYPE_Configuration }, // Length, type
					sizeof(MYUDB_USB_Descriptor_Configuration_t), // TotalLength: variable
					2, // NumInterfaces
					1, // Configuration index
					NO_DESCRIPTOR, // Configuration String
					USB_CONFIG_ATTR_RESERVED, // Attributes
					USB_CONFIG_POWER_MA(100) // MaxPower = 100mA
				},
				// printer_interface
				{ { sizeof(USB_Descriptor_Interface_t), DTYPE_Interface },
						0, 0, // AlternateSetting
						2, // bNumEndpoints
						PRNT_CSCP_PrinterClass, // bInterfaceclass ->Printer
						PRNT_CSCP_PrinterSubclass, // bInterfaceSubClass -> Control
						PRNT_CSCP_BidirectionalProtocol,// bInterfaceProtocol
						NO_DESCRIPTOR // iInterface,  same as iProduct in USB_Descriptor_Device_t, or else not working
				},
				// printer_in_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						ENDPOINT_DIR_IN | MYUDB_EDP_IN_HCI, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // bInterval
				},
				// printer_out_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						MYUDB_EDP_OUT_HCI, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // polling bInterval. valid for iso or interrupt type
				},

				// printer_interface
				{ { sizeof(USB_Descriptor_Interface_t), DTYPE_Interface },
						1, 0, // AlternateSetting
						2, // bNumEndpoints
						PRNT_CSCP_PrinterClass, // bInterfaceclass ->Printer
						PRNT_CSCP_PrinterSubclass, // bInterfaceSubClass -> Control
						PRNT_CSCP_BidirectionalProtocol,// bInterfaceProtocol
						NO_DESCRIPTOR // iInterface,  same as iProduct in USB_Descriptor_Device_t, or else not working
				},
				// printer_in_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						ENDPOINT_DIR_IN | MYUDB_EDP_IN_VCD, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // bInterval
				},
				// printer_out_endpoint
				{ { sizeof(USB_Descriptor_Endpoint_t), DTYPE_Endpoint }, // length, bDescriptorType
						MYUDB_EDP_OUT_VCD, // endpoint id
						EP_TYPE_BULK, // endpoint type
						0x0040, // wMaxPacketSize
						0 // polling bInterval. valid for iso or interrupt type
				},
		};

u8* myudb_usbdesc_get_language(void) {
	return (u8*) (&language_desc);
}

u8* myudb_usbdesc_get_vendor(void) {
	return (u8*) (&vendor_desc);
}

u8* myudb_usbdesc_get_product(void) {
	return (u8*) (&product_desc);
}
u8* myudb_usbdesc_get_serial(void) {
	return (u8*) (&serial_desc);
}

u8* myudb_usbdesc_get_device(void) {
	return (u8*) (&device_desc);
}

u8* myudb_usbdesc_get_configuration(void) {
	return (u8*) (&configuration_desc);
}

#endif


