/********************************************************************************************************
 * @file	myudb_usbdesc.h
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

#ifdef		MYUDB_DISABLE
#else
#define MYUDB_ID_VENDOR					0x248a			// for report
#define MYUDB_ID_PRODUCT				0x9218
#define MYUDB_ID_VERSION				0x0101
#define MYUDB_STRING_VENDOR				L"Telink"
#define MYUDB_STRING_PRODUCT			L"HCI_VCD Dongle"
#define MYUDB_STRING_SERIAL				L"TLSR9218"


////////////////////////////////////////////////////////

/* Public Interface - May be used in end-application: */
/* Macros: */
	/** Mask for the request type parameter, to indicate the direction of the request data (Host to Device
	 *  or Device to Host). The result of this mask should then be compared to the request direction masks.
	 *
	 *  \see \c REQDIR_* macros for masks indicating the request data direction.
	 */
	#define CONTROL_REQTYPE_DIRECTION  0x80

	/** Mask for the request type parameter, to indicate the type of request (Device, Class or Vendor
	 *  Specific). The result of this mask should then be compared to the request type masks.
	 *
	 *  \see \c REQTYPE_* macros for masks indicating the request type.
	 */
	#define CONTROL_REQTYPE_TYPE       0x60

	/** Mask for the request type parameter, to indicate the recipient of the request (Device, Interface
	 *  Endpoint or Other). The result of this mask should then be compared to the request recipient
	 *  masks.
	 *
	 *  \see \c REQREC_* macros for masks indicating the request recipient.
	 */
	#define CONTROL_REQTYPE_RECIPIENT  0x1F

	/** \name Control Request Data Direction Masks */
	//@{
	/** Request data direction mask, indicating that the request data will flow from host to device.
	 *
	 *  \see \ref CONTROL_REQTYPE_DIRECTION macro.
	 */
	#define REQDIR_HOSTTODEVICE        (0 << 7)

	/** Request data direction mask, indicating that the request data will flow from device to host.
	 *
	 *  \see \ref CONTROL_REQTYPE_DIRECTION macro.
	 */
	#define REQDIR_DEVICETOHOST        (1 << 7)
	//@}

	/** \name Control Request Type Masks */
	//@{
	/** Request type mask, indicating that the request is a standard request.
	 *
	 *  \see \ref CONTROL_REQTYPE_TYPE macro.
	 */
	#define REQTYPE_STANDARD           (0 << 5)

	/** Request type mask, indicating that the request is a class-specific request.
	 *
	 *  \see \ref CONTROL_REQTYPE_TYPE macro.
	 */
	#define REQTYPE_CLASS              (1 << 5)

	/** Request type mask, indicating that the request is a vendor specific request.
	 *
	 *  \see \ref CONTROL_REQTYPE_TYPE macro.
	 */
	#define REQTYPE_VENDOR             (2 << 5)
	//@}

	/** \name Control Request Recipient Masks */
	//@{
	/** Request recipient mask, indicating that the request is to be issued to the device as a whole.
	 *
	 *  \see \ref CONTROL_REQTYPE_RECIPIENT macro.
	 */
	#define REQREC_DEVICE              (0 << 0)

	/** Request recipient mask, indicating that the request is to be issued to an interface in the
	 *  currently selected configuration.
	 *
	 *  \see \ref CONTROL_REQTYPE_RECIPIENT macro.
	 */
	#define REQREC_INTERFACE           (1 << 0)

	/** Request recipient mask, indicating that the request is to be issued to an endpoint in the
	 *  currently selected configuration.
	 *
	 *  \see \ref CONTROL_REQTYPE_RECIPIENT macro.
	 */
	#define REQREC_ENDPOINT            (2 << 0)

	/** Request recipient mask, indicating that the request is to be issued to an unspecified element
	 *  in the currently selected configuration.
	 *
	 *  \see \ref CONTROL_REQTYPE_RECIPIENT macro.
	 */
	#define REQREC_OTHER               (3 << 0)
	//@}

/* Type Defines: */
	/** \brief Standard USB Control Request
	 *
	 *  Type define for a standard USB control request.
	 *
	 *  \see The USB 2.0 specification for more information on standard control requests.
	 */
	typedef struct
	{
		u8  bmRequestType; /**< Type of the request. */
		u8  bRequest; /**< Request command code. */
		u16 wValue; /**< wValue parameter of the request. */
		u16 wIndex; /**< wIndex parameter of the request. */
		u16 wLength; /**< Length of the data to transfer in bytes. */
	}USB_Request_Header_t;

/* Enums: */
	/** Enumeration for the various standard request commands. These commands are applicable when the
	 *  request type is \ref REQTYPE_STANDARD (with the exception of \ref REQ_GetDescriptor, which is always
	 *  handled regardless of the request type value).
	 *
	 *  \see Chapter 9 of the USB 2.0 Specification.
	 */
	enum USB_Control_Request_t
	{
		REQ_GetStatus           = 0, /**< Implemented in the library for device and endpoint recipients. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_ClearFeature        = 1, /**< Implemented in the library for device and endpoint recipients. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SetFeature          = 3, /**< Implemented in the library for device and endpoint recipients. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SetAddress          = 5, /**< Implemented in the library for the device recipient. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_GetDescriptor       = 6, /**< Implemented in the library for device and interface recipients. Passed to the
		                              *   user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SetDescriptor       = 7, /**< Not implemented in the library, passed to the user application
		                              *   via the \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_GetConfiguration    = 8, /**< Implemented in the library for the device recipient. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SetConfiguration    = 9, /**< Implemented in the library for the device recipient. Passed
		                              *   to the user application for other recipients via the
		                              *   \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_GetInterface        = 10, /**< Not implemented in the library, passed to the user application
		                              *   via the \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SetInterface        = 11, /**< Not implemented in the library, passed to the user application
		                              *   via the \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
		REQ_SynchFrame          = 12, /**< Not implemented in the library, passed to the user application
		                              *   via the \ref EVENT_USB_Device_ControlRequest() event when received in
		                              *   device mode. */
	};

	/** Feature Selector values for Set Feature and Clear Feature standard control requests directed to the device, interface
	 *  and endpoint recipients.
	 */
	enum USB_Feature_Selectors_t
	{
		FEATURE_SEL_EndpointHalt       = 0x00, /**< Feature selector for Clear Feature or Set Feature commands. When
		                                        *   used in a Set Feature or Clear Feature request this indicates that an
		                                        *   endpoint (whose address is given elsewhere in the request) should have
		                                        *   its stall condition changed.
		                                        */
		FEATURE_SEL_DeviceRemoteWakeup = 0x01, /**< Feature selector for Device level Remote Wakeup enable set or clear.
	                                            *   This feature can be controlled by the host on devices which indicate
	                                            *   remote wakeup support in their descriptors to selectively disable or
	                                            *   enable remote wakeup.
	                                            */
		FEATURE_SEL_TestMode           = 0x02, /**< Feature selector for Test Mode features, used to test the USB controller
	                                            *   to check for incorrect operation.
	                                            */
	};

	#define FEATURE_SELFPOWERED_ENABLED     (1 << 0)
	#define FEATURE_REMOTE_WAKEUP_ENABLED   (1 << 1)
////////////////////////////////////////////////////////


/* Public Interface - May be used in end-application: */
/* Macros: */
/** Indicates that a given descriptor does not exist in the device. This can be used inside descriptors
 *  for string descriptor indexes, or may be use as a return value for GetDescriptor when the specified
 *  descriptor does not exist.
 */
#define NO_DESCRIPTOR                     0

/** Macro to calculate the power value for the configuration descriptor, from a given number of milliamperes.
 *
 *  \param[in] mA  Maximum number of milliamps the device consumes when the given configuration is selected.
 */
#define USB_CONFIG_POWER_MA(mA)           ((mA) >> 1)

/** Macro to calculate the Unicode length of a string with a given number of Unicode characters.
 *  Should be used in string descriptor's headers for giving the string descriptor's byte length.
 *
 *  \param[in] UnicodeChars  Number of Unicode characters in the string text.
 */
#define USB_STRING_LEN(UnicodeChars)      (sizeof(USB_Descriptor_Header_t) + ((UnicodeChars) << 1))

/** String language ID for the English language. Should be used in \ref USB_Descriptor_String_t descriptors
 *  to indicate that the English language is supported by the device in its string descriptors.
 */
#define LANGUAGE_ID_ENG                   0x0409

/** \name USB Configuration Descriptor Attribute Masks */
//@{
/** Mask for the reserved bit in the Configuration Descriptor's \c ConfigAttributes field, which must be set on all
 *  devices for historial purposes.
 */
#define USB_CONFIG_ATTR_RESERVED          0x80
/** Can be masked with other configuration descriptor attributes for a \ref USB_Descriptor_Configuration_Header_t
 *  descriptor's \c ConfigAttributes value to indicate that the specified configuration can draw its power
 *  from the device's own power source.
 */
#define USB_CONFIG_ATTR_SELFPOWERED       0x40

/** Can be masked with other configuration descriptor attributes for a \ref USB_Descriptor_Configuration_Header_t
 *  descriptor's \c ConfigAttributes value to indicate that the specified configuration supports the
 *  remote wakeup feature of the USB standard, allowing a suspended USB device to wake up the host upon
 *  request.
 */
#define USB_CONFIG_ATTR_REMOTEWAKEUP      0x20
//@}

/** \name Endpoint Descriptor Attribute Masks */
//@{
/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is not synchronized.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_NO_SYNC             (0 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is asynchronous.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_ASYNC               (1 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is adaptive.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_ADAPTIVE            (2 << 2)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is synchronized.
 *
 *  \see The USB specification for more details on the possible Endpoint attributes.
 */
#define ENDPOINT_ATTR_SYNC                (3 << 2)
//@}

/** \name Endpoint Descriptor Usage Masks */
//@{
/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for data transfers.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_DATA               (0 << 4)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for feedback.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_FEEDBACK           (1 << 4)

/** Can be masked with other endpoint descriptor attributes for a \ref USB_Descriptor_Endpoint_t descriptor's
 *  \c Attributes value to indicate that the specified endpoint is used for implicit feedback.
 *
 *  \see The USB specification for more details on the possible Endpoint usage attributes.
 */
#define ENDPOINT_USAGE_IMPLICIT_FEEDBACK  (2 << 4)
//@}

/* Enums: */
/** Enum for the possible standard descriptor types, as given in each descriptor's header. */
enum USB_DescriptorTypes_t
{
	DTYPE_Device                    = 0x01, /**< Indicates that the descriptor is a device descriptor. */
	DTYPE_Configuration             = 0x02, /**< Indicates that the descriptor is a configuration descriptor. */
	DTYPE_String                    = 0x03, /**< Indicates that the descriptor is a string descriptor. */
	DTYPE_Interface                 = 0x04, /**< Indicates that the descriptor is an interface descriptor. */
	DTYPE_Endpoint                  = 0x05, /**< Indicates that the descriptor is an endpoint descriptor. */
	DTYPE_DeviceQualifier           = 0x06, /**< Indicates that the descriptor is a device qualifier descriptor. */
	DTYPE_Other                     = 0x07, /**< Indicates that the descriptor is of other type. */
	DTYPE_InterfacePower            = 0x08, /**< Indicates that the descriptor is an interface power descriptor. */
	DTYPE_InterfaceAssociation      = 0x0B, /**< Indicates that the descriptor is an interface association descriptor. */
	DTYPE_CSInterface               = 0x24, /**< Indicates that the descriptor is a class specific interface descriptor. */
	DTYPE_CSEndpoint                = 0x25, /**< Indicates that the descriptor is a class specific endpoint descriptor. */
};

/** Enum for possible Class, Subclass and Protocol values of device and interface descriptors. */
enum USB_Descriptor_ClassSubclassProtocol_t
{
	USB_CSCP_NoDeviceClass          = 0x00, /**< Descriptor Class value indicating that the device does not belong
	                                         *   to a particular class at the device level.
	                                         */
	USB_CSCP_NoDeviceSubclass       = 0x00, /**< Descriptor Subclass value indicating that the device does not belong
	                                         *   to a particular subclass at the device level.
	                                         */
	USB_CSCP_NoDeviceProtocol       = 0x00, /**< Descriptor Protocol value indicating that the device does not belong
	                                         *   to a particular protocol at the device level.
	                                         */
	USB_CSCP_VendorSpecificClass    = 0xFF, /**< Descriptor Class value indicating that the device/interface belongs
	                                         *   to a vendor specific class.
	                                         */
	USB_CSCP_VendorSpecificSubclass = 0xFF, /**< Descriptor Subclass value indicating that the device/interface belongs
	                                         *   to a vendor specific subclass.
	                                         */
	USB_CSCP_VendorSpecificProtocol = 0xFF, /**< Descriptor Protocol value indicating that the device/interface belongs
	                                         *   to a vendor specific protocol.
	                                         */
	USB_CSCP_IADDeviceClass         = 0xEF, /**< Descriptor Class value indicating that the device belongs to the
	                                         *   Interface Association Descriptor class.
	                                         */
	USB_CSCP_IADDeviceSubclass      = 0x02, /**< Descriptor Subclass value indicating that the device belongs to the
	                                         *   Interface Association Descriptor subclass.
	                                         */
	USB_CSCP_IADDeviceProtocol      = 0x01, /**< Descriptor Protocol value indicating that the device belongs to the
	                                         *   Interface Association Descriptor protocol.
	                                         */
};

/* Type Defines: */
/** \brief Standard USB Descriptor Header (LUFA naming conventions).
 *
 *  Type define for all descriptors' standard header, indicating the descriptor's length and type. This structure
 *  uses LUFA-specific element names to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Header_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 Size; /**< Size of the descriptor, in bytes. */
	u8 Type; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	               *   given by the specific class.
	               */
}USB_Descriptor_Header_t;

/** \brief Standard USB Descriptor Header (USB-IF naming conventions).
 *
 *  Type define for all descriptors' standard header, indicating the descriptor's length and type. This structure
 *  uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Header_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
}USB_StdDescriptor_Header_t;

/** \brief Standard USB Device Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses LUFA-specific element names to make each
 *  element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Device_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 USBSpecification; /**< BCD of the supported USB specification. */
	u8  Class; /**< USB device class. */
	u8  SubClass; /**< USB device subclass. */
	u8  Protocol; /**< USB device protocol. */

	u8  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */

	u16 VendorID; /**< Vendor ID for the USB product. */
	u16 ProductID; /**< Unique product ID for the USB product. */
	u16 ReleaseNumber; /**< Product release (version) number. */

	u8  ManufacturerStrIndex; /**< String index for the manufacturer's name. The
	                                *   host will request this string via a separate
	                                *   control request for the string descriptor.
	                                *
	                                *   \note If no string supplied, use \ref NO_DESCRIPTOR.
	                                */
	u8  ProductStrIndex; /**< String index for the product name/details.
	                           *
	                           *  \see ManufacturerStrIndex structure entry.
	                           */
	u8  SerialNumStrIndex; /**< String index for the product's globally unique hexadecimal
	                             *   serial number, in uppercase Unicode ASCII.
	                             *
	                             *  \note On some microcontroller models, there is an embedded serial number
	                             *        in the chip which can be used for the device serial number.
	                             *        To use this serial number, set this to USE_INTERNAL_SERIAL.
	                             *        On unsupported devices, this will evaluate to 0 and will cause
	                             *        the host to generate a pseudo-unique value for the device upon
	                             *        insertion.
	                             *
	                             *  \see ManufacturerStrIndex structure entry.
	                             */
	u8  NumberOfConfigurations; /**< Total number of configurations supported by
	                                  *   the device.
	                                  */
}USB_Descriptor_Device_t;

/** \brief Standard USB Device Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 bcdUSB; /**< BCD of the supported USB specification. */
	u8  bDeviceClass; /**< USB device class. */
	u8  bDeviceSubClass; /**< USB device subclass. */
	u8  bDeviceProtocol; /**< USB device protocol. */
	u8  bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u16 idVendor; /**< Vendor ID for the USB product. */
	u16 idProduct; /**< Unique product ID for the USB product. */
	u16 bcdDevice; /**< Product release (version) number. */
	u8  iManufacturer; /**< String index for the manufacturer's name. The
	                         *   host will request this string via a separate
	                         *   control request for the string descriptor.
	                         *
	                         *   \note If no string supplied, use \ref NO_DESCRIPTOR.
	                         */
	u8  iProduct; /**< String index for the product name/details.
	                    *
	                    *  \see ManufacturerStrIndex structure entry.
	                    */
	u8 iSerialNumber; /**< String index for the product's globally unique hexadecimal
	                        *   serial number, in uppercase Unicode ASCII.
	                        *
	                        *  \note On some microcontroller models, there is an embedded serial number
	                        *        in the chip which can be used for the device serial number.
	                        *        To use this serial number, set this to USE_INTERNAL_SERIAL.
	                        *        On unsupported devices, this will evaluate to 0 and will cause
	                        *        the host to generate a pseudo-unique value for the device upon
	                        *        insertion.
	                        *
	                        *  \see ManufacturerStrIndex structure entry.
	                        */
	u8  bNumConfigurations; /**< Total number of configurations supported by
	                              *   the device.
	                              */
}USB_StdDescriptor_Device_t;

/** \brief Standard USB Device Qualifier Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_DeviceQualifier_t for the version of this type with standard element names.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 USBSpecification; /**< BCD of the supported USB specification. */
	u8  Class; /**< USB device class. */
	u8  SubClass; /**< USB device subclass. */
	u8  Protocol; /**< USB device protocol. */

	u8  Endpoint0Size; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u8  NumberOfConfigurations; /**< Total number of configurations supported by
	                                  *   the device.
	                                  */
	u8  Reserved; /**< Reserved for future use, must be 0. */
}USB_Descriptor_DeviceQualifier_t;

/** \brief Standard USB Device Qualifier Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Device Qualifier Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_DeviceQualifier_t for the version of this type with non-standard LUFA specific element names.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 bcdUSB; /**< BCD of the supported USB specification. */
	u8  bDeviceClass; /**< USB device class. */
	u8  bDeviceSubClass; /**< USB device subclass. */
	u8  bDeviceProtocol; /**< USB device protocol. */
	u8  bMaxPacketSize0; /**< Size of the control (address 0) endpoint's bank in bytes. */
	u8  bNumConfigurations; /**< Total number of configurations supported by
	                              *   the device.
	                              */
	u8  bReserved; /**< Reserved for future use, must be 0. */
}USB_StdDescriptor_DeviceQualifier_t;

/** \brief Standard USB Configuration Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Configuration_Header_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u16 TotalConfigurationSize; /**< Size of the configuration descriptor header,
	                                  *   and all sub descriptors inside the configuration.
	                                  */
	u8  TotalInterfaces; /**< Total number of interfaces in the configuration. */

	u8  ConfigurationNumber; /**< Configuration index of the current configuration. */
	u8  ConfigurationStrIndex; /**< Index of a string descriptor describing the configuration. */

	u8  ConfigAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.
	                            *   On all devices, this should include USB_CONFIG_ATTR_RESERVED at a minimum.
	                            */

	u8  MaxPowerConsumption; /**< Maximum power consumption of the device while in the
	                               *   current configuration, calculated by the \ref USB_CONFIG_POWER_MA()
	                               *   macro.
	                               */
}__attribute__((packed)) USB_Descriptor_Configuration_Header_t;

/** \brief Standard USB Configuration Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Configuration Descriptor header. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Device_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                              *   given by the specific class.
	                              */
	u16 wTotalLength; /**< Size of the configuration descriptor header,
	                           *   and all sub descriptors inside the configuration.
	                           */
	u8  bNumInterfaces; /**< Total number of interfaces in the configuration. */
	u8  bConfigurationValue; /**< Configuration index of the current configuration. */
	u8  iConfiguration; /**< Index of a string descriptor describing the configuration. */
	u8  bmAttributes; /**< Configuration attributes, comprised of a mask of \c USB_CONFIG_ATTR_* masks.
	                        *   On all devices, this should include USB_CONFIG_ATTR_RESERVED at a minimum.
	                        */
	u8  bMaxPower; /**< Maximum power consumption of the device while in the
	                     *   current configuration, calculated by the \ref USB_CONFIG_POWER_MA()
	                     *   macro.
	                     */
}USB_StdDescriptor_Configuration_Header_t;

/** \brief Standard USB Interface Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Interface_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8 InterfaceNumber; /**< Index of the interface in the current configuration. */
	u8 AlternateSetting; /**< Alternate setting for the interface number. The same
	                           *   interface number can have multiple alternate settings
	                           *   with different endpoint configurations, which can be
	                           *   selected by the host.
	                           */
	u8 TotalEndpoints; /**< Total number of endpoints in the interface. */

	u8 Class; /**< Interface class ID. */
	u8 SubClass; /**< Interface subclass ID. */
	u8 Protocol; /**< Interface protocol ID. */

	u8 InterfaceStrIndex; /**< Index of the string descriptor describing the interface. */
}__attribute__((packed))USB_Descriptor_Interface_t;

/** \brief Standard USB Interface Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Descriptor. This structure uses the relevant standard's given element names
 *  to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Interface_t for the version of this type with non-standard LUFA specific element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
	u8 bInterfaceNumber; /**< Index of the interface in the current configuration. */
	u8 bAlternateSetting; /**< Alternate setting for the interface number. The same
	                            *   interface number can have multiple alternate settings
	                            *   with different endpoint configurations, which can be
	                            *   selected by the host.
	                            */
	u8 bNumEndpoints; /**< Total number of endpoints in the interface. */
	u8 bInterfaceClass; /**< Interface class ID. */
	u8 bInterfaceSubClass; /**< Interface subclass ID. */
	u8 bInterfaceProtocol; /**< Interface protocol ID. */
	u8 iInterface; /**< Index of the string descriptor describing the
	                     *   interface.
	                     */
}USB_StdDescriptor_Interface_t;

/** \brief Standard USB Interface Association Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_StdDescriptor_Interface_Association_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8 FirstInterfaceIndex; /**< Index of the first associated interface. */
	u8 TotalInterfaces; /**< Total number of associated interfaces. */

	u8 Class; /**< Interface class ID. */
	u8 SubClass; /**< Interface subclass ID. */
	u8 Protocol; /**< Interface protocol ID. */

	u8 IADStrIndex; /**< Index of the string descriptor describing the
	                      *   interface association.
	                      */
}USB_Descriptor_Interface_Association_t;

/** \brief Standard USB Interface Association Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Interface Association Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  This descriptor has been added as a supplement to the USB2.0 standard, in the ECN located at
 *  <a>http://www.usb.org/developers/docs/InterfaceAssociationDescriptor_ecn.pdf</a>. It allows composite
 *  devices with multiple interfaces related to the same function to have the multiple interfaces bound
 *  together at the point of enumeration, loading one generic driver for all the interfaces in the single
 *  function. Read the ECN for more information.
 *
 *  \see \ref USB_Descriptor_Interface_Association_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a value
	                          *   given by the specific class.
	                          */
	u8 bFirstInterface; /**< Index of the first associated interface. */
	u8 bInterfaceCount; /**< Total number of associated interfaces. */
	u8 bFunctionClass; /**< Interface class ID. */
	u8 bFunctionSubClass; /**< Interface subclass ID. */
	u8 bFunctionProtocol; /**< Interface protocol ID. */
	u8 iFunction; /**< Index of the string descriptor describing the
	                    *   interface association.
	                    */
}USB_StdDescriptor_Interface_Association_t;

/** \brief Standard USB Endpoint Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses LUFA-specific element names
 *  to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_Endpoint_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */

	u8  EndpointAddress; /**< Logical address of the endpoint within the device for the current
	                           *   configuration, including direction mask.
	                           */
	u8  Attributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
	                      *   and attributes (ENDPOINT_ATTR_*) masks.
	                      */
	u16 EndpointSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet
	                        *   size that the endpoint can receive at a time.
	                        */
	u8  PollingIntervalMS; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT
	                             *   or ISOCHRONOUS type.
	                             */
}__attribute__((packed)) USB_Descriptor_Endpoint_t;

/** \brief Standard USB Endpoint Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard Endpoint Descriptor. This structure uses the relevant standard's given
 *  element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_Endpoint_t for the version of this type with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8  bLength; /**< Size of the descriptor, in bytes. */
	u8  bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t or a
	                           *   value given by the specific class.
	                           */
	u8  bEndpointAddress; /**< Logical address of the endpoint within the device for the current
	                            *   configuration, including direction mask.
	                            */
	u8  bmAttributes; /**< Endpoint attributes, comprised of a mask of the endpoint type (EP_TYPE_*)
	                        *   and attributes (ENDPOINT_ATTR_*) masks.
	                        */
	u16 wMaxPacketSize; /**< Size of the endpoint bank, in bytes. This indicates the maximum packet size
	                          *   that the endpoint can receive at a time.
	                          */
	u8  bInterval; /**< Polling interval in milliseconds for the endpoint if it is an INTERRUPT or
	                     *   ISOCHRONOUS type.
	                     */
}__attribute__((packed)) USB_StdDescriptor_Endpoint_t;

/** \brief Standard USB String Descriptor (LUFA naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses LUFA-specific element names to make each element's purpose clearer.
 *
 *  \see \ref USB_StdDescriptor_String_t for the version of this type with standard element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
#ifndef __GNUC__
#pragma warning(push)
#pragma warning(disable:4200)	// none standard zero length array
#endif

typedef struct
{
	USB_Descriptor_Header_t Header; /**< Descriptor header, including type and size. */
	wchar_t UnicodeString[]; /**< String data, as unicode characters (alternatively,
	                           *   string language IDs). If normal ASCII characters are
	                           *   to be used, they must be added as an array of characters
	                           *   rather than a normal C string so that they are widened to
	                           *   Unicode size.
	                           *
	                           *   Under GCC, strings prefixed with the "L" character (before
	                           *   the opening string quotation mark) are considered to be
	                           *   Unicode strings, and may be used instead of an explicit
	                           *   array of ASCII characters on little endian devices with
	                           *   UTF-16-LE \c wchar_t encoding.
	                           */
}USB_Descriptor_String_t;

/** \brief Standard USB String Descriptor (USB-IF naming conventions).
 *
 *  Type define for a standard string descriptor. Unlike other standard descriptors, the length
 *  of the descriptor for placement in the descriptor header must be determined by the \ref USB_STRING_LEN()
 *  macro rather than by the size of the descriptor structure, as the length is not fixed.
 *
 *  This structure should also be used for string index 0, which contains the supported language IDs for
 *  the device as an array.
 *
 *  This structure uses the relevant standard's given element names to ensure compatibility with the standard.
 *
 *  \see \ref USB_Descriptor_String_t for the version of this type with with non-standard LUFA specific
 *       element names.
 *
 *  \note Regardless of CPU architecture, these values should be stored as little endian.
 */
typedef struct
{
	u8 bLength; /**< Size of the descriptor, in bytes. */
	u8 bDescriptorType; /**< Type of the descriptor, either a value in \ref USB_DescriptorTypes_t
	                          *   or a value given by the specific class.
	                          */
	wchar_t bString[]; /**< String data, as unicode characters (alternatively, string language IDs).
	                     *   If normal ASCII characters are to be used, they must be added as an array
	                     *   of characters rather than a normal C string so that they are widened to
	                     *   Unicode size.
	                     *
	                     *   Under GCC, strings prefixed with the "L" character (before the opening string
	                     *   quotation mark) are considered to be Unicode strings, and may be used instead
	                     *   of an explicit array of ASCII characters.
	                     */
}USB_StdDescriptor_String_t;

////////////////////////////////////////////////////////////////////////////////////////////////
#if 1

#define	MYUDB_EDP_IN_HCI				3
#define	MYUDB_EDP_OUT_HCI				5
#define	MYUDB_EDP_IN_VCD				8
#define	MYUDB_EDP_OUT_VCD				6

#else

#define	MYUDB_EDP_IN_HCI				8
#define	MYUDB_EDP_OUT_HCI				5
#define	MYUDB_EDP_IN_VCD				3
#define	MYUDB_EDP_OUT_VCD				6

#endif

#define ENDPOINT_DIR_MASK				    BIT(7)
#define ENDPOINT_DIR_OUT				    0
#define ENDPOINT_DIR_IN 				    BIT(7)
#define EP_TYPE_MASK					    3
#define EP_TYPE_CONTROL 				    0
#define EP_TYPE_ISOCHRONOUS 			    1
#define EP_TYPE_BULK					    2
#define EP_TYPE_INTERRUPT				    3

enum PRNT_Descriptor_ClassSubclassProtocol_t
{
	PRNT_CSCP_PrinterClass = 0x07,
	PRNT_CSCP_PrinterSubclass = 0x01,
	PRNT_CSCP_BidirectionalProtocol = 0x02,
};

enum {
	MYUDB_USB_STRING_LANGUAGE = 0,
	MYUDB_USB_STRING_VENDOR,
	MYUDB_USB_STRING_PRODUCT,
	MYUDB_USB_STRING_SERIAL,
};

typedef struct {
	USB_Descriptor_Configuration_Header_t Config;
	USB_Descriptor_Interface_t intf0;
	USB_Descriptor_Endpoint_t endpi0;
	USB_Descriptor_Endpoint_t endpo0;
	USB_Descriptor_Interface_t intf1;
	USB_Descriptor_Endpoint_t endpi1;
	USB_Descriptor_Endpoint_t endpo1;
} MYUDB_USB_Descriptor_Configuration_t;

#ifdef __cplusplus
extern "C" {
#endif

u8* myudb_usbdesc_get_language(void);
u8* myudb_usbdesc_get_vendor(void);
u8* myudb_usbdesc_get_product(void);
u8* myudb_usbdesc_get_serial(void);
u8* myudb_usbdesc_get_device(void);
u8* myudb_usbdesc_get_configuration(void);

#ifdef __cplusplus
}
#endif

#endif
