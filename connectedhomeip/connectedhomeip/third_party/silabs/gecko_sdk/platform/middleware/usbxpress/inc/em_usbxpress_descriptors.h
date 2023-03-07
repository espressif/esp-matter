/***************************************************************************//**
 * @file
 * @brief   Header file for USB and VCP Initial Descriptors.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#ifndef EM_USBXPRESS_DESCRIPTORS_H
#define EM_USBXPRESS_DESCRIPTORS_H

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Constant Definitions

// -------------------------------
// USB Identification

// Initial VID/PID values.  Should never be used.  VID/PID loaded by USB_Init()
#define USB_VENDOR_ID               USB_VENDOR_ID_SILICON_LABS
#define USB_PRODUCT_ID              0x0000

// Interface number for USBXpress
#define USBXPRESS_IFC_NUMBER        0

#define USBXPRESS_SETUP_EP_ADDR     0x00
#define USBXPRESS_IN_EP_ADDR        0x81
#define USBXPRESS_OUT_EP_ADDR       0x01

// -------------------------------
// Extended Compatibility

// Microsoft OS Feature Descriptor Indexes
#define FEATURE_GENRE                       1   ///< Genre Descriptor
#define FEATURE_EXTENDED_COMPATIBILITY_ID   4   ///< Extended Compatibility ID Descriptor
#define FEATURE_EXTENDED_PROPERTIES_ID      5   ///< Extended Properties ID Descriptor

/// Size of Extended Compatibility Descriptor
#define EXT_COMP_DESC_SIZE                  sizeof(USB_ExtendedCompatIdOsFeatureDesc_t)

#define EXT_COMP_VENDOR_CODE                1   ///< Vendor-defined Extended Compatibility Code

/// Number of interfaces with extended compatibility descriptors
#define NUM_IFCS_WITH_EXT_COMPAT            1

// -------------------------------
// Extended Properties

// Data Types for Microsoft Extended Properties Feature Descriptor
#define EXTENDED_PROPERTY_TYPE_REG_SZ                   1   ///< NUL-terminated UNICODE String
#define EXTENDED_PROPERTY_TYPE_REG_EXPAND_SZ            2   /**< NUL-terminated Unicode String, that
                                                                 may include environment variables. */
#define EXTENDED_PROPERTY_TYPE_REG_BINARY               3   ///< Free-form Binary Data
#define EXTENDED_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN  4   ///< DWORD Value, Little Endian
#define EXTENDED_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN     5   ///< DWORD Value, Big Endian
#define EXTENDED_PROPERTY_TYPE_REG_LINK                 6   /**< NUL-terminated Unicode String,
                                                                 that may include a symbolic link */
#define EXTENDED_PROPERTY_TYPE_MULTI_SZ                 7   ///< Multiple NUL-terminated Unicode Strings

/// Length of GUID Property Name
#define GUID_PROPERTY_NAME_LEN  sizeof ("DeviceInterfaceGUIDs") * 2

/// GUID Property Name (DeviceInterfaceGUIDs)
#define GUID_PROPERTY_NAME \
  {                        \
    'D', 0,                \
    'e', 0,                \
    'v', 0,                \
    'i', 0,                \
    'c', 0,                \
    'e', 0,                \
    'I', 0,                \
    'n', 0,                \
    't', 0,                \
    'e', 0,                \
    'r', 0,                \
    'f', 0,                \
    'a', 0,                \
    'c', 0,                \
    'e', 0,                \
    'G', 0,                \
    'U', 0,                \
    'I', 0,                \
    'D', 0,                \
    's', 0,                \
    '\0', 0,               \
  }

/// Length of GUID Property
#define GUID_PROPERTY_LEN     (sizeof("{3C5E1462-5695-4e18-876B-F3F3D08AAF18}\0") * 2)

/// GUID Property (USBXpress Device Interface GUID - {3C5E1462-5695-4e18-876B-F3F3D08AAF18})
#define GUID_PROPERTY \
  {                   \
    '{', 0,           \
    '3', 0,           \
    'C', 0,           \
    '5', 0,           \
    'E', 0,           \
    '1', 0,           \
    '4', 0,           \
    '6', 0,           \
    '2', 0,           \
    '-', 0,           \
    '5', 0,           \
    '6', 0,           \
    '9', 0,           \
    '5', 0,           \
    '-', 0,           \
    '4', 0,           \
    'e', 0,           \
    '1', 0,           \
    '8', 0,           \
    '-', 0,           \
    '8', 0,           \
    '7', 0,           \
    '6', 0,           \
    'B', 0,           \
    '-', 0,           \
    'F', 0,           \
    '3', 0,           \
    'F', 0,           \
    '3', 0,           \
    'D', 0,           \
    '0', 0,           \
    '8', 0,           \
    'A', 0,           \
    'A', 0,           \
    'F', 0,           \
    '1', 0,           \
    '8', 0,           \
    '}', 0,           \
    '\0', 0,          \
    '\0', 0,          \
  }

/// Size of Property Descriptor for the GUID Property
#define GUID_PROPERTY_DESCRIPTOR_SIZE (14 + GUID_PROPERTY_NAME_LEN + GUID_PROPERTY_LEN)

/// Total Size of Extended Properties Descriptor
#define PROPERTIES_DESCRIPTOR_SIZE    (10 + GUID_PROPERTY_DESCRIPTOR_SIZE)

// -----------------------------------------------------------------------------
// Typedef Definitions

/// Compatible ID Function Descriptor Typedef
SL_PACK_START(1)
typedef struct {
  uint8_t firstInterfaceNumber;     ///< Interface number for extended
                                    ///< compatibility feature descriptor
  uint8_t reserved1;                ///< Reserved (unused)
  char compatibleID[8];             ///< String describing the compatible ID
  char subCompatibleID[8];          ///< String describing the sub compatible ID
  uint8_t reserved2[6];             ///< Reserved (unused)
} SL_ATTRIBUTE_PACKED USB_CompatFunctionDesc_t;
SL_PACK_END()

/// Compatible ID Feature Descriptor Typedef
SL_PACK_START(1)
typedef struct {
  uint32_t length;                  ///< Size of this struct = 16 + bCount*24
  uint16_t bcdVersion;              ///< 1.00 -> 0x0100
  uint16_t index;                   ///< Command index - 0x04 for extended
                                    ///< compatibility ID
  uint8_t count;                    ///< Number of functions sections
  uint8_t reserved1[7];             ///< Reserved (unused)

  const USB_CompatFunctionDesc_t
    ecid[NUM_IFCS_WITH_EXT_COMPAT]; ///< Extended capabilities
} SL_ATTRIBUTE_PACKED USB_ExtendedCompatIdOsFeatureDesc_t;
SL_PACK_END()

/// Extended Properties Descriptor Typedef
SL_PACK_START(1)
typedef struct {
  uint32_t wLength;                                 ///< Size of this struct
  uint16_t bcdVersion;                              ///< 1.00 -> 0x0100
  uint16_t index;                                   ///< 0x05 for extended property
  uint16_t count;                                   ///< Number of properties

  // GUID Property
  uint32_t size2;                                   ///< Size of this property descriptor
  uint32_t propertyDataType;                        ///< Data type of this property
  uint16_t propertyNameLength;                      ///< Length of the name of this property
  uint8_t propertyName[GUID_PROPERTY_NAME_LEN];     ///< Name of this property
  uint32_t propertyDataLength;                      ///< Length the property data
  uint8_t propertyData2[GUID_PROPERTY_LEN];         ///< Property data
} SL_ATTRIBUTE_PACKED USB_ExtendedPropertiesDescriptor_t;
SL_PACK_END()

// -----------------------------------------------------------------------------
// Extern Variable Declarations

extern const USB_DeviceDescriptor_TypeDef USBXCORE_deviceDescInit;
extern const uint8_t USBXCORE_configDescInit[];
extern const uint8_t USBXCORE_stringLangDesc[];
extern const uint8_t USBXCORE_microsoftOsDesc[];
extern const USB_ExtendedCompatIdOsFeatureDesc_t USBXCORE_extendedCompatIdOsFeatureDesc;
extern const USB_ExtendedPropertiesDescriptor_t USBXCORE_extendedPropertiesDesc;
extern const uint8_t USBXCORE_buffMult[];

#ifdef __cplusplus
}
#endif

#endif // EM_USBXPRESS_DESCRIPTORS_H
