/***************************************************************************//**
 * @file
 * @brief USB Device Core Operations
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.
 * The software is governed by the sections of the MSLA applicable to Micrium
 * Software.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup USBD USB Device API
 * @brief   USB Device API
 *
 * @defgroup USBD_CORE USB Device Core API
 * @ingroup USBD
 * @brief   USB Device Core API
 *
 * @addtogroup USBD_CORE
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CORE_H
#define  SL_USBD_CORE_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "em_device.h"

#include "sl_enum.h"
#include "sl_status.h"

#include "sl_usbd_core_config.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                   FRAME
 *
 * Note(s) : (1) Section 8.3.3 of USB spec 2.0 describes the frame number as:
 *               "The frame number field is an 11-bit field that is incremented by the host on a per-frame
 *                   basis. The frame number field rolls over upon reaching its maximum value of 7FFH and
 *                   is sent only in SOF tokens at the start of each (micro)frame."
 *******************************************************************************************************/

#define  SL_USBD_FRAME_NBR_MAX                     2047u              // See Note #1.
#define  SL_USBD_FRAME_NBR_MASK                    0b0000011111111111 // 0x07FF // DEF_BIT_FIELD_16(11u, 0u)
#define  SL_USBD_MICROFRAME_NBR_MASK               0b0011100000000000 // 0x3800 //DEF_BIT_FIELD_16(3u, 11u)

/********************************************************************************************************
 *                                       CONFIGURATION ATTRIBUTES
 *******************************************************************************************************/

#define  SL_USBD_DEV_ATTRIB_SELF_POWERED           0x01u  // BIT_00
#define  SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP          0x02u  // BIT_01

/********************************************************************************************************
 *                                       DEVICE DESCRIPTOR LENGTH
 *******************************************************************************************************/

#define  SL_USBD_DESC_LEN_DEV                                18u   // Device                    descriptor length.
#define  SL_USBD_DESC_LEN_DEV_QUAL                           10u   // Device qualifier          descriptor length.
#define  SL_USBD_DESC_LEN_HDR                                 2u   // Descriptor header                    length.
#define  SL_USBD_DESC_LEN_CFG                                 9u   // Configuration             descriptor length.
#define  SL_USBD_DESC_LEN_OTHER_SPD_CFG                       9u   // Configuration other speed descriptor length.
#define  SL_USBD_DESC_LEN_IF                                  9u   // Interface                 descriptor length.
#define  SL_USBD_DESC_LEN_IF_ASSOCIATION                      8u   // Interface association     descriptor length.
#define  SL_USBD_DESC_LEN_EP                                  7u   // Endpoint                  descriptor length.
#define  SL_USBD_DESC_LEN_OTG                                 3u   // On-The-Go                 descriptor length

/****************************************************************************************************//**
 *                                       REQUEST CHARACTERISTICS
 *
 * Note(s) : (1) Request types are defined in USB spec 2.0, section 9.3, Table 9-2, 'RequestType' field.
 *******************************************************************************************************/

#define  SL_USBD_REQ_DIR_MASK                              0x80u  // BIT_07
#define  SL_USBD_REQ_DIR_HOST_TO_DEVICE                    0x00u  // NONE
#define  SL_USBD_REQ_DIR_DEVICE_TO_HOST                    0x80u  // BIT_07
#define  SL_USBD_REQ_DIR_BIT                               0x80u  // BIT_07

#define  SL_USBD_REQ_TYPE_MASK                             0x60u
#define  SL_USBD_REQ_TYPE_STANDARD                         0x00u
#define  SL_USBD_REQ_TYPE_CLASS                            0x20u
#define  SL_USBD_REQ_TYPE_VENDOR                           0x40u
#define  SL_USBD_REQ_TYPE_RESERVED                         0x60u

#define  SL_USBD_REQ_RECIPIENT_MASK                        0x1Fu
#define  SL_USBD_REQ_RECIPIENT_DEVICE                      0x00u
#define  SL_USBD_REQ_RECIPIENT_INTERFACE                   0x01u
#define  SL_USBD_REQ_RECIPIENT_ENDPOINT                    0x02u
#define  SL_USBD_REQ_RECIPIENT_OTHER                       0x03u

/****************************************************************************************************//**
 *                                           STANDARD REQUESTS
 *
 * Note(s) : (1) Request types are defined in USB spec 2.0, section 9.4, Table 9-4, 'RequestType' field.
 *******************************************************************************************************/

#define  SL_USBD_REQ_GET_STATUS                               0u
#define  SL_USBD_REQ_CLEAR_FEATURE                            1u
#define  SL_USBD_REQ_SET_FEATURE                              3u
#define  SL_USBD_REQ_SET_ADDRESS                              5u
#define  SL_USBD_REQ_GET_DESCRIPTOR                           6u
#define  SL_USBD_REQ_SET_DESCRIPTOR                           7u
#define  SL_USBD_REQ_GET_CONFIGURATION                        8u
#define  SL_USBD_REQ_SET_CONFIGURATION                        9u
#define  SL_USBD_REQ_GET_INTERFACE                           10u
#define  SL_USBD_REQ_SET_INTERFACE                           11u
#define  SL_USBD_REQ_SYNCH_FRAME                             12u

/****************************************************************************************************//**
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) Descriptors types are defined in the USB spec 2.0 section 9.2.6, Table 9-5.
 *******************************************************************************************************/

#define  SL_USBD_DESC_TYPE_DEVICE                             1u
#define  SL_USBD_DESC_TYPE_CONFIGURATION                      2u
#define  SL_USBD_DESC_TYPE_STRING                             3u
#define  SL_USBD_DESC_TYPE_INTERFACE                          4u
#define  SL_USBD_DESC_TYPE_ENDPOINT                           5u
#define  SL_USBD_DESC_TYPE_DEVICE_QUALIFIER                   6u
#define  SL_USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION          7u
#define  SL_USBD_DESC_TYPE_INTERFACE_POWER                    8u
#define  SL_USBD_DESC_TYPE_OTG                                9u
#define  SL_USBD_DESC_TYPE_IAD                               11u

/****************************************************************************************************//**
 *                                           LANGUAGE IDENTIFIERS
 *
 * Note(s) : (1) Languages identifier are defined in "http://www.usb.org/developers/docs/USB_LANGIDs.pdf".
 *******************************************************************************************************/

#define  SL_USBD_LANG_ID_ARABIC_SAUDI_ARABIA             0x0401u
#define  SL_USBD_LANG_ID_CHINESE_TAIWAN                  0x0404u
#define  SL_USBD_LANG_ID_ENGLISH_US                      0x0409u
#define  SL_USBD_LANG_ID_ENGLISH_UK                      0x0809u
#define  SL_USBD_LANG_ID_FRENCH                          0x040Cu
#define  SL_USBD_LANG_ID_GERMAN                          0x0407u
#define  SL_USBD_LANG_ID_GREEK                           0x0408u
#define  SL_USBD_LANG_ID_ITALIAN                         0x0410u
#define  SL_USBD_LANG_ID_PORTUGUESE                      0x0816u
#define  SL_USBD_LANG_ID_SANSKRIT                        0x044Fu

/****************************************************************************************************//**
 *                                               CLASS CODES
 *
 * Note(s) : (1) Class codes are defined in "http://www.usb.org/developers/defined_class".
 *
 *           (2) Class code information can be placed in the Device descriptor or in the Interface
 *               descriptor. Some defined class code are allowed to be used only in the Device
 *               descriptor, others can used in both Device and Interface descriptors and come can
 *               only be used in Interface Descriptors.
 *
 *           (3) Subclass & protocol codes are defined in the relevant class drivers.
 *******************************************************************************************************/

#define  SL_USBD_CLASS_CODE_USE_IF_DESC                    0x00u
#define  SL_USBD_CLASS_CODE_AUDIO                          0x01u
#define  SL_USBD_CLASS_CODE_CDC_CONTROL                    0x02u
#define  SL_USBD_CLASS_CODE_HID                            0x03u
#define  SL_USBD_CLASS_CODE_PHYSICAL                       0x05u
#define  SL_USBD_CLASS_CODE_IMAGE                          0x06u
#define  SL_USBD_CLASS_CODE_PRINTER                        0x07u
#define  SL_USBD_CLASS_CODE_MASS_STORAGE                   0x08u
#define  SL_USBD_CLASS_CODE_HUB                            0x09u
#define  SL_USBD_CLASS_CODE_CDC_DATA                       0x0Au
#define  SL_USBD_CLASS_CODE_SMART_CARD                     0x0Bu
#define  SL_USBD_CLASS_CODE_CONTENT_SECURITY               0x0Du
#define  SL_USBD_CLASS_CODE_VIDEO                          0x0Eu
#define  SL_USBD_CLASS_CODE_PERSONAL_HEALTHCARE            0x0Fu
#define  SL_USBD_CLASS_CODE_DIAGNOSTIC_DEVICE              0xDCu
#define  SL_USBD_CLASS_CODE_WIRELESS_CONTROLLER            0xE0u
#define  SL_USBD_CLASS_CODE_MISCELLANEOUS                  0xEFu
#define  SL_USBD_CLASS_CODE_APPLICATION_SPECIFIC           0xFEu
#define  SL_USBD_CLASS_CODE_VENDOR_SPECIFIC                0xFFu

/********************************************************************************************************
 *                                           SUB-CLASS CODES
 *******************************************************************************************************/

#define  SL_USBD_SUBCLASS_CODE_USE_IF_DESC                 0x00u
#define  SL_USBD_SUBCLASS_CODE_USE_COMMON_CLASS            0x02u
#define  SL_USBD_SUBCLASS_CODE_VENDOR_SPECIFIC             0xFFu

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *******************************************************************************************************/

#define  SL_USBD_PROTOCOL_CODE_USE_IF_DESC                 0x00u
#define  SL_USBD_PROTOCOL_CODE_USE_IAD                     0x01u
#define  SL_USBD_PROTOCOL_CODE_VENDOR_SPECIFIC             0xFFu

/********************************************************************************************************
 *                                           FEATURE SELECTORS
 *******************************************************************************************************/

#define  SL_USBD_FEATURE_SEL_ENDPOINT_HALT                    0u
#define  SL_USBD_FEATURE_SEL_DEVICE_REMOTE_WAKEUP             1u
#define  SL_USBD_FEATURE_SEL_TEST_MODE                        2u

/********************************************************************************************************
 *                                       DEVICE POWER CONSTRAINS
 *******************************************************************************************************/

#define  SL_USBD_MAX_BUS_PWR_LIMIT_mA                       500u

/********************************************************************************************************
 *                                               ENDPOINT TYPE
 *
 * Note(s) : (1) Endpoint types are defined in the USB spec 2.0 section 9.6.6, Table 9-13, 'bmAttributes'
 *               field.
 *******************************************************************************************************/

#define  SL_USBD_ENDPOINT_TYPE_MASK                              0x03u
#define  SL_USBD_ENDPOINT_TYPE_CTRL                              0x00u
#define  SL_USBD_ENDPOINT_TYPE_ISOC                              0x01u
#define  SL_USBD_ENDPOINT_TYPE_BULK                              0x02u
#define  SL_USBD_ENDPOINT_TYPE_INTR                              0x03u

#define  SL_USBD_ENDPOINT_TYPE_SYNC_MASK                         0x0Cu
#define  SL_USBD_ENDPOINT_TYPE_SYNC_NONE                         0x00u
#define  SL_USBD_ENDPOINT_TYPE_SYNC_ASYNC                        0x04u
#define  SL_USBD_ENDPOINT_TYPE_SYNC_ADAPTIVE                     0x08u
#define  SL_USBD_ENDPOINT_TYPE_SYNC_SYNC                         0x0Cu

#define  SL_USBD_ENDPOINT_TYPE_USAGE_MASK                        0x30u
#define  SL_USBD_ENDPOINT_TYPE_USAGE_DATA                        0x00u
#define  SL_USBD_ENDPOINT_TYPE_USAGE_FEEDBACK                    0x10u
#define  SL_USBD_ENDPOINT_TYPE_USAGE_IMPLICIT_FEEDBACK           0x20u

/********************************************************************************************************
 *                                           ENDPOINT ADDRESS
 *
 * Note(s) : (1) Endpoint address is defined in the USB spec 2.0, section 9.6.6, Table 9-13,
 *               'bEndpointAddress' field.
 *******************************************************************************************************/

#define  SL_USBD_ENDPOINT_NBR_MASK                               0x0Fu
#define  SL_USBD_ENDPOINT_MAX_NBR                                  32u

#define  SL_USBD_ENDPOINT_DIR_MASK                               0x80u
#define  SL_USBD_ENDPOINT_DIR_BIT                                0x80u // BIT_07
#define  SL_USBD_ENDPOINT_DIR_OUT                                0x00u // NONE
#define  SL_USBD_ENDPOINT_DIR_IN                                 0x80u // BIT_07

#define  SL_USBD_ENDPOINT_TRANSACTION_PER_UFRAME_1                  1u
#define  SL_USBD_ENDPOINT_TRANSACTION_PER_UFRAME_2                  2u
#define  SL_USBD_ENDPOINT_TRANSACTION_PER_UFRAME_3                  3u

#define  SL_USBD_ENDPOINT_MAX_INTERVAL_VAL                      32768u   // Max val is 2^(16 - 1).

/********************************************************************************************************
 *                                       MICROSOFT OS DESCRIPTOR DEFINES
 *
 * Note(s)     : (1) For more information on Microsoft OS descriptors, see
 *                   'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *******************************************************************************************************/

//                                                                 Microsoft OS compatible IDs index.
#define  SL_USBD_MICROSOFT_COMPAT_ID_NULL                         0u
#define  SL_USBD_MICROSOFT_COMPAT_ID_RNDIS                        1u
#define  SL_USBD_MICROSOFT_COMPAT_ID_PTP                          2u
#define  SL_USBD_MICROSOFT_COMPAT_ID_MTP                          3u
#define  SL_USBD_MICROSOFT_COMPAT_ID_XUSB20                       4u
#define  SL_USBD_MICROSOFT_COMPAT_ID_BLUETOOTH                    5u
#define  SL_USBD_MICROSOFT_COMPAT_ID_WINUSB                       6u
#define  SL_USBD_MICROSOFT_COMPAT_ID_NONE                       255u

//                                                                 Microsoft OS subcompatible IDs index.
#define  SL_USBD_MICROSOFT_SUBCOMPAT_ID_NULL                      0u
#define  SL_USBD_MICROSOFT_SUBCOMPAT_ID_BLUETOOTH_1_1             1u
#define  SL_USBD_MICROSOFT_SUBCOMPAT_ID_BLUETOOTH_1_2             2u
#define  SL_USBD_MICROSOFT_SUBCOMPAT_ID_BLUETOOTH_EDR             3u

//                                                                 Microsoft OS extended property types.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_SZ                   1u   // NULL-terminated Unicode str.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_EXPAND_SZ            2u   // NULL-terminated Unicode str with environment vars.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_BINARY               3u   // Free-form binary.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN  4u   // Little-endian 32-bit integer.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN     5u   // Big-endian 32-bit integer.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_LINK                 6u   // NULL-terminated Unicode str with a symbolic link.
#define  SL_USBD_MICROSOFT_PROPERTY_TYPE_REG_MULTI_SZ             7u   // Multiple NULL-terminated Unicode strings.

/********************************************************************************************************
 *                                   USB EP INFORMATION TABLE DEFINES
 *******************************************************************************************************/

//                                                                 ------------- ENDPOINT TYPE BIT DEFINES ------------
#define  SL_USBD_ENDPOINT_INFO_TYPE_CTRL                 0x01u // BIT_00
#define  SL_USBD_ENDPOINT_INFO_TYPE_ISOC                 0x02u // BIT_01
#define  SL_USBD_ENDPOINT_INFO_TYPE_BULK                 0x04u // BIT_02
#define  SL_USBD_ENDPOINT_INFO_TYPE_INTR                 0x08u // BIT_03

//                                                                 ---------- ENDPOINT DIRECTION BIT DEFINES ----------
#define  SL_USBD_ENDPOINT_INFO_DIR_OUT                   0x10u // BIT_04
#define  SL_USBD_ENDPOINT_INFO_DIR_IN                    0x20u // BIT_05

/********************************************************************************************************
 *                                           USB OBJECT NUMBER
 *******************************************************************************************************/

#define  SL_USBD_CONFIG_NBR_NONE                          255u
#define  SL_USBD_CONFIG_NBR_SPD_BIT                       0x80u // BIT_07

#define  SL_USBD_INTERFACE_NBR_NONE                       255u

#define  SL_USBD_ALT_INTERFACE_NBR_NONE                   255u

#define  SL_USBD_INTERFACE_GROUP_NBR_NONE                 255u

#define  SL_USBD_ENDPOINT_ADDR_NONE                       255u
#define  SL_USBD_ENDPOINT_PHY_NONE                        255u

#define  SL_USBD_ENDPOINT_NBR_NONE                        255u

#define  SL_USBD_CLASS_NBR_NONE                           255u

/********************************************************************************************************
 *                                           USB DEFAULT CONFIGS
 *******************************************************************************************************/

#ifndef USBD_CFG_STR_EN
#define USBD_CFG_STR_EN            1
#endif

#ifndef USBD_CFG_MS_OS_DESC_EN
#define USBD_CFG_MS_OS_DESC_EN     0
#endif

#ifndef USBD_CFG_URB_EXTRA_EN
#define USBD_CFG_URB_EXTRA_EN      0
#endif

#if (USBD_CFG_URB_EXTRA_EN == 1)
#define SL_USBD_URB_MAX_NBR  (SL_USBD_OPEN_ENDPOINTS_QUANTITY + SL_USBD_EXTRA_URB_QUANTITY)
#else
#define SL_USBD_URB_MAX_NBR  SL_USBD_OPEN_ENDPOINTS_QUANTITY
#endif

/********************************************************************************************************
 *                                               USB CORE EVENTS
 *
 * Note(s) : (1) There are 7 possible USB callback events:
 *
 *                   sli_usbd_core_reset_event(),
 *                   sli_usbd_core_suspend_event(),
 *                   sli_usbd_core_resume_event(),
 *                   sli_usbd_core_connect_event(),
 *                   sli_usbd_core_disconnect_event(),
 *                   sli_usbd_core_high_speed_event().
 *******************************************************************************************************/

#define  SL_USBD_CORE_EVENT_BUS_NBR                          7u    // Number of bus events per controller.

#define SL_USBD_CORE_EVENT_BUS_NBR_TOTAL  (SL_USBD_CORE_EVENT_BUS_NBR * SL_USBD_DEVICE_QUANTITY)
#define SL_USBD_CORE_EVENT_ASYNC_NBR ((SL_USBD_OPEN_ENDPOINTS_QUANTITY * SL_USBD_DEVICE_QUANTITY) + SL_USBD_EXTRA_URB_QUANTITY)
#define SL_USBD_CORE_EVENT_NBR_TOTAL  (SL_USBD_CORE_EVENT_BUS_NBR_TOTAL + SL_USBD_CORE_EVENT_ASYNC_NBR)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       USB DEVICE SPEED DATA TYPE
 *
 * @note     (1) 'Universal Serial Bus Specification Rev 2.0', section 4.2.1 defines three data rates:
 *               - (a) The USB  Low-Speed (LS) signaling bit rate is   1.5 Mb/s.
 *               - (b) The USB Full-Speed (FS) signaling bit rate is  12   Mb/s.
 *               - (c) The USB High-Speed (HS) signaling bit rate is 480   Mb/s.
 *******************************************************************************************************/

SL_ENUM(sl_usbd_device_speed_t) {
  SL_USBD_DEVICE_SPEED_INVALID = 0u,
  SL_USBD_DEVICE_SPEED_LOW,                                             ///<  Low-Speed (see Note #1a).
  SL_USBD_DEVICE_SPEED_FULL,                                            ///< Full-Speed (see Note #1b).
  SL_USBD_DEVICE_SPEED_HIGH                                             ///< High-Speed (see Note #1c).
};

/****************************************************************************************************//**
 *                                       USB DEVICE STATES DATA TYPE
 *
 * @note     (1) 'Universal Serial Bus Specification Rev 2.0', section 9.1.1 defines seven visible device
 *                   states:
 *               - (a) SL_USBD_DEVICE_STATE_INIT       : Device is not attached to the USB.
 *               - (b) SL_USBD_DEVICE_STATE_ATTACHED   : Device is attached to the USB, but not powered.
 *               - (c) USBD_DEV_STATE_POWERED    : Device is attached to the USB and powered, but has not
 *                                               been reset.
 *               - (d) SL_USBD_DEVICE_STATE_DEFAULT    : Device is attached to the USB and powered and has been
 *                                               reset, but has not been assigned a unique address.
 *                                               Device responds at the default address.
 *               - (e) SL_USBD_DEVICE_STATE_ADDRESSED  : Device is attached to the USB, powered, has been reset,
 *                                               and a unique device address has been assigned. Device
 *                                               is not configured.
 *               - (f) SL_USBD_DEVICE_STATE_CONFIGURED : Device is attached to the USB, powered, has been reset,
 *                                               has a unique  address, is configured, and is not suspended.
 *                                               The host may now use the function provided by the device.
 *               - (g) SL_USBD_DEVICE_STATE_SUSPENDED  : Device is, at minimum, attached to the USB and is powered
 *                                               and has not  seen bus activity for 3 ms. It may also have
 *                                               a unique address and be  configured for use. However,
 *                                               because the device is suspended, the host may not use the
 *                                               device's function.
 * @note     (2) An additional state is added (USBD_DEV_STATE_START) to determine if the device controller
 *               has been initialized.
 *******************************************************************************************************/

SL_ENUM(sl_usbd_device_state_t) {
  SL_USBD_DEVICE_STATE_NONE = 0,
  SL_USBD_DEVICE_STATE_INIT,
  SL_USBD_DEVICE_STATE_ATTACHED,
  SL_USBD_DEVICE_STATE_DEFAULT,
  SL_USBD_DEVICE_STATE_ADDRESSED,
  SL_USBD_DEVICE_STATE_CONFIGURED,
  SL_USBD_DEVICE_STATE_SUSPENDED
};

/********************************************************************************************************
 *                                       BUS EVENT DATA TYPE
 *******************************************************************************************************/

SL_ENUM(sl_usbd_bus_event_t) {
  SL_USBD_EVENT_BUS_CONNECT = 0,
  SL_USBD_EVENT_BUS_DISCONNECT,
  SL_USBD_EVENT_BUS_RESET,
  SL_USBD_EVENT_BUS_SUSPEND,
  SL_USBD_EVENT_BUS_RESUME
};

/****************************************************************************************************//**
 *                                      CONFIG EVENT DATA TYPE
 *******************************************************************************************************/

SL_ENUM(sl_usbd_config_event_t) {
  SL_USBD_EVENT_CONFIG_SET = 0,
  SL_USBD_EVENT_CONFIG_UNSET
};

/****************************************************************************************************//**
 *                                   USB DEVICE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef struct {
  uint16_t    vendor_id;                                      ///< Vendor  id.
  uint16_t    product_id;                                     ///< Product id.
  uint16_t    device_bcd;                                     ///< Device release number.
  const char  *manufacturer_str_ptr;                          ///< Manufacturer string.
  const char  *product_str_ptr;                               ///< Product      string.
  const char  *serial_nbr_str_ptr;                            ///< Serial Number id.
  uint16_t    lang_id;                                        ///< Language      id.
} sl_usbd_device_config_t;

/****************************************************************************************************//**
 *                           USB DEVICE MICROSOFT XTENDED PROPERTIES DATA TYPE
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
typedef struct {
  uint8_t       property_type;

  const uint8_t *property_name_ptr;
  uint16_t       property_name_len;

  const uint8_t *property_ptr;
  uint32_t       property_len;
} sl_usbd_microsoft_ext_property_t;
#endif

/****************************************************************************************************//**
 *                                           USB SETUP REQUEST
 *******************************************************************************************************/

typedef struct {
  uint8_t  bmRequestType;                                     ///< Characteristics of request.
  uint8_t  bRequest;                                          ///< Specific request.
  uint16_t wValue;                                            ///< Varies according to request.
  uint16_t wIndex;                                            ///< Varies according to request; typically used as index.
  uint16_t wLength;                                           ///< Transfer length if data stage present.
} sl_usbd_setup_req_t;

/****************************************************************************************************//**
 *                                       INTERFACE CLASS DRIVER
 *******************************************************************************************************/
typedef const struct {
  void (*enable)(uint8_t    cfg_nbr,
                 void       *p_if_class_arg);                                 ///< Notify class that cfg is active.

  void (*disable)(uint8_t    cfg_nbr,
                  void       *p_if_class_arg);                                ///< Notify class that cfg is not active.

  void (*alt_settings_update)(uint8_t    cfg_nbr,
                              uint8_t    if_nbr,
                              uint8_t    if_alt_nbr,
                              void       *p_if_class_arg,
                              void       *p_if_alt_class_arg);                ///< Notify class that alt setting has been updated.

  void (*endpoint_state_update)(uint8_t    cfg_nbr,
                                uint8_t    if_nbr,
                                uint8_t    if_alt_nbr,
                                uint8_t    ep_addr,
                                void       *p_if_class_arg,
                                void       *p_if_alt_class_arg);              ///< Notify class that EP state has been updated.

  void (*interface_descriptor)(uint8_t    cfg_nbr,
                               uint8_t    if_nbr,
                               uint8_t    if_alt_nbr,
                               void       *p_if_class_arg,
                               void       *p_if_alt_class_arg);               ///< Construct  IF functional descriptor.

  uint16_t (*interface_get_descriptor_size)(uint8_t    cfg_nbr,
                                            uint8_t    if_nbr,
                                            uint8_t    if_alt_nbr,
                                            void       *p_if_class_arg,
                                            void       *p_if_alt_class_arg);  ///< Get size of IF functional descriptor.

  void (*endpoint_descriptor)(uint8_t    cfg_nbr,
                              uint8_t    if_nbr,
                              uint8_t    if_alt_nbr,
                              uint8_t    ep_addr,
                              void       *p_if_class_arg,
                              void       *p_if_alt_class_arg);                ///< Construct  EP functional descriptor.

  uint16_t (*endpoint_get_descriptor_size)(uint8_t    cfg_nbr,
                                           uint8_t    if_nbr,
                                           uint8_t    if_alt_nbr,
                                           uint8_t    ep_addr,
                                           void       *p_if_class_arg,
                                           void       *p_if_alt_class_arg);   ///< Get size of EP functional descriptor.

  bool (*interface_req)(const sl_usbd_setup_req_t *p_setup_req,
                        void                      *p_if_class_arg);           ///< Interface request callback.

  bool (*class_req)(const sl_usbd_setup_req_t *p_setup_req,
                    void                      *p_if_class_arg);               ///< Class-specific  request callback.

  bool (*vendor_req)(const sl_usbd_setup_req_t *p_setup_req,
                     void                      *p_if_class_arg);              ///< Vendor-specific request callback.

#if (USBD_CFG_MS_OS_DESC_EN == 1)
  uint8_t (*microsoft_get_compat_id)(uint8_t *p_sub_compat_id_ix);                                                      ///< Get MS compatible id nbr.
  uint8_t (*microsoft_get_ext_property_table)(sl_usbd_microsoft_ext_property_t **pp_ext_property_tbl, uint8_t if_nbr);  ///< Get MS ext properties.
#endif
} sl_usbd_class_driver_t;

/********************************************************************************************************
 *                                       EP ASYNCHRONOUS CALLBACK API
 *******************************************************************************************************/

typedef void (*sl_usbd_async_function_t)(uint8_t      ep_addr,    // Endpoint address.
                                         void         *p_buf,     // Pointer to the buffer.
                                         uint32_t     buf_len,    // Buffer   length.
                                         uint32_t     xfer_len,   // Transfer length.
                                         void         *p_arg,     // Function argument.
                                         sl_status_t  status);    // Error status.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               FRAME MACROS
 *
 * Note(s) : (1) This macro takes into account when the frame number rolls over upon reaching the maximum
 *               value of 2047.
 *******************************************************************************************************/

#define  SL_USBD_FRAME_NBR_GET(frame_nbr)                      ((frame_nbr) & SL_USBD_FRAME_NBR_MASK)

#define  SL_USBD_MICROFRAME_NBR_GET(microframe_nbr)            ((microframe_nbr) >> 11u)

//                                                                 See Note #1.
#define  SL_USBD_FRAME_NBR_DIFF_GET(frame_nbr1, frame_nbr2)    (((frame_nbr2) >= (frame_nbr1))  \
                                                                ? ((frame_nbr2) - (frame_nbr1)) \
                                                                : ((SL_USBD_FRAME_NBR_MAX + 1u + (frame_nbr2)) - (frame_nbr1)))

/********************************************************************************************************
 *                                       ENDPOINT CONVERSION MACROS
 *******************************************************************************************************/

#define  SL_USBD_ENDPOINT_PHY_TO_ADDR(ep_phy_nbr)                 ( ((ep_phy_nbr) / 2u) \
                                                                    | (((ep_phy_nbr) % 2u) ? SL_USBD_ENDPOINT_DIR_BIT : 0x00u))

#define  SL_USBD_ENDPOINT_ADDR_TO_PHY(ep_addr)                    ((((ep_addr) & SL_USBD_ENDPOINT_NBR_MASK) *  2u) \
                                                                   + ((((ep_addr) & SL_USBD_ENDPOINT_DIR_MASK) != 0u) ? 1u : 0u))

#define  SL_USBD_ENDPOINT_ADDR_TO_LOG(ep_addr)                    (uint8_t)((ep_addr) & SL_USBD_ENDPOINT_NBR_MASK)

#define  SL_USBD_ENDPOINT_PHY_TO_LOG(ep_phy_nbr)                  (ep_phy_nbr / 2u)

#define  SL_USBD_ENDPOINT_LOG_TO_ADDR_IN(ep_log_nbr)              ((ep_log_nbr) | SL_USBD_ENDPOINT_DIR_IN)

#define  SL_USBD_ENDPOINT_LOG_TO_ADDR_OUT(ep_log_nbr)             ((ep_log_nbr) | SL_USBD_ENDPOINT_DIR_OUT)

#define  SL_USBD_ENDPOINT_IS_IN(ep_addr)                          ((((ep_addr) & SL_USBD_ENDPOINT_DIR_MASK) != 0u) ? true : false)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   CONFIGURATION OVERRIDE FUNCTIONS
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                           USBD CORE FUNCTIONS
 *
 * Note(s) : (1) USB Spec 2.0, Section 5.5 states "Control transfers allow access to different parts of
 *               a device.  Control transfers are intended to support configuration/command/status type
 *               communication flows between client software and its function".
 *
 *               (a) "Each USB device is required to implement the Default Control Pipe as a message
 *                       pipe.  This pipe is used by the USB System Software.  The Default Control Pipe
 *                       provides access to the USB device's configuration, status, and control information".
 *
 *                   The 'sl_usbd_core_ep_XXXX()' functions perform operations in the default endpoint.
 *                   Class drivers should use 'sl_usbd_core_ep_XXXX()' to send/receive class specific requests.
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                                 sl_usbd_core_init()
 *
 * @brief    Initialize the USB device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) sli_usbd_core_init_internals() MUST be called ... :
 *               - (a) ONLY ONCE from a product's application;
 *               - (b) With the following conditions:
 *                   - (1) AFTER  the product's OS has been initialized.
 *                   - (2) BEFORE the product's application calls any USB device stack function(s).
 *******************************************************************************************************/
sl_status_t sl_usbd_core_init(void);

/****************************************************************************************************//**
 * @brief    Start the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Device stack can be only started if the device is in one of the following states:
 *               - SL_USBD_DEVICE_STATE_NONE    Device controller has not been initialized.
 *               - SL_USBD_DEVICE_STATE_INIT    Device controller is already   initialized.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_start_device(void);

/****************************************************************************************************//**
 * @brief    Stop the device stack.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_stop_device(void);

/****************************************************************************************************//**
 * @brief    Get the current device state.
 *
 * @param    p_dev_state   Pointer to device state to be returned.
 *                         Set to current device state, if no errors are returned,
 *                         or SL_USBD_DEVICE_STATE_NONE,  if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_state(sl_usbd_device_state_t *p_dev_state);

/****************************************************************************************************//**
 * @brief    Get device speed.
 *
 * @param    p_dev_speed   Pointer to the variable that will be set to device speed upon return.
 *                         It will be set to current device speed, if successful,
 *                         or SL_USBD_DEVICE_SPEED_INVALID,     if unsuccessful.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_speed(sl_usbd_device_speed_t *p_dev_speed);

/****************************************************************************************************//**
 * @brief    Set the device's current power source.
 *
 * @param    self_pwr    The power source of the device :
 *                           - true  device is self-powered.
 *                           - false device is  bus-powered.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_set_device_self_power(bool self_pwr);

/****************************************************************************************************//**
 * @brief    Set the device's Microsoft vendor code.
 *
 * @param    vendor_code     Microsoft vendor code.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) The vendor code used MUST be different from any vendor bRequest value.
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == 1)
sl_status_t sl_usbd_core_set_device_microsoft_vendor_code(uint8_t vendor_code);
#endif

/****************************************************************************************************//**
 * @brief    Get the device configuration.
 *
 * @param    p_dev_cfg   Pointer to a variable of type (sl_usbd_device_config_t *) which will receive
 *                       the memory address of device configuration structure upon return.
 *                       The variable will be set to point to device configuration, if no errors are returned,
 *                       or it will be set to NULL if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_configuration(sl_usbd_device_config_t **p_dev_cfg);

/****************************************************************************************************//**
 * @brief    Get the last frame number from the driver.
 *
 * @param    p_frame_nbr  Pointer to variable that will receive frame number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) The frame number will always be in the range of 0-2047 (11 bits).
 *
 * @note     (2) Frame number returned to the caller contains the frame and microframe numbers. It is
 *               encoded following this 16-bit format:
 *               @verbatim
 *               | 15 | 14 | 13 | 12 | 11 | 10 | 9 | 8 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *               |  0    0 |  microframe  |                  frame                     |
 *               @endverbatim
 *               Caller must use the macros SL_USBD_FRAME_NBR_GET() or SL_USBD_MICROFRAME_NBR_GET() to get
 *               the frame or microframe number only.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_frame_number(uint16_t *p_frame_nbr);

/****************************************************************************************************//**
 * @brief    Add a configuration attribute to the device.
 *
 * @param    attrib      Available configuration attributes.
 *                           - SL_USBD_DEV_ATTRIB_SELF_POWERED      Power does not come from VBUS.
 *                           - SL_USBD_DEV_ATTRIB_REMOTE_WAKEUP     Remote wakeup feature enabled.
 *
 * @param    max_pwr     Bus power required for this device (see Note #1).
 *
 * @param    spd         Available configuration speeds.
 *                           - SL_USBD_DEVICE_SPEED_FULL   Configuration is added in the full-speed configuration set.
 *
 * @param    p_name      Pointer to string describing the configuration (see Note #2).
 *
 * @param    p_cfg_nbr   Pointer to variable that will receive the configuration number.
 *                       It will be set to the configuration number, if no errors are returned,
 *                       otherwise it will be set to SL_USBD_CONFIG_NBR_NONE if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB spec 2.0 (section 7.2.1.3/4) defines power constraints for bus-powered devices:
 *               "A low-power function draws up to one unit load from the USB cable when operational"
 *               "A function is high-power if, when fully powered, draws over one, but no more than
 *               five, unit loads from the USB cable."
 *               A unit load is defined as 100mA, so 'max_pwr' argument should be between 0 mA
 *               and 500mA
 *
 * @note     (2) String support is optional: 'p_name' can be a NULL string pointer.
 *
 * @note     (3) Configuration can only be added when the device is in the following states:
 *                   - SL_USBD_DEVICE_STATE_NONE    Device controller has not been initialized.
 *                   - SL_USBD_DEVICE_STATE_INIT    Device controller is already   initialized.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_configuration(uint8_t                  attrib,
                                           uint16_t                 max_pwr,
                                           sl_usbd_device_speed_t   spd,
                                           const char               *p_name,
                                           uint8_t                  *p_cfg_nbr);

/****************************************************************************************************//**
 * @brief    Associate a configuration with its alternative-speed counterpart.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    config_other    Other-speed configuration number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Configurations from high- and full-speed can be associated with each other to provide
 *               comparable functionality regardless of speed.
 *
 * @note     (2) Configuration can ONLY be associated when the device is in the following states:
 *                   - SL_USBD_DEVICE_STATE_NONE    Device controller has not been initialized.
 *                   - SL_USBD_DEVICE_STATE_INIT    Device controller is already   initialized.
 *******************************************************************************************************/
#if (USBD_CFG_HS_EN == 1)
sl_status_t sl_usbd_core_associate_other_speed_configuration(uint8_t cfg_nbr,
                                                             uint8_t cfg_other);
#endif

/****************************************************************************************************//**
 * @brief    Add an interface to a specific configuration.
 *
 * @param    cfg_nbr                 Configuration index to add the interface.
 *
 * @param    p_class_drv             Pointer to interface driver.
 *
 * @param    p_if_class_arg          Pointer to interface driver argument.
 *
 * @param    p_if_alt_class_arg      Pointer to alternate interface argument.
 *
 * @param    class_code              Class code assigned by the USB-IF.
 *
 * @param    class_sub_code          Subclass code assigned by the USB-IF.
 *
 * @param    class_protocol_code     protocol code assigned by the USB-IF.
 *
 * @param    p_name                  Pointer to string describing the Interface.
 *
 * @param    p_if_nbr                Pointer to a variable that will receive interface number after creation.
 *                                   The variable will be set to the interface number, if no errors are returned,
 *                                   otherwise it is set to SL_USBD_INTERFACE_NBR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB Spec 2.0 Interface (section 9.6.5) states: "An interface may include alternate
 *               settings that allow the endpoints and/or their characteristics to be varied after
 *               the device has been configured. The default setting for an interface is always
 *               an alternate setting of zero."
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interface(uint8_t                cfg_nbr,
                                       sl_usbd_class_driver_t *p_class_drv,
                                       void                   *p_if_class_arg,
                                       void                   *p_if_alt_class_arg,
                                       uint8_t                class_code,
                                       uint8_t                class_sub_code,
                                       uint8_t                class_protocol_code,
                                       const char             *p_name,
                                       uint8_t                *p_if_nbr);

/****************************************************************************************************//**
 * @brief    Add an alternate setting to a specific interface.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    p_class_arg     Pointer to alternate interface argument.
 *
 * @param    p_name          Pointer to alternate setting name.
 *
 * @param    p_if_alt_nbr    Pointer to variable that will receive alternate setting number upon return.
 *                           It will be set to interface alternate setting number if no error(s) are returned,
 *                           or SL_USBD_ALT_INTERFACE_NBR_NONE if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_alt_interface(uint8_t        config_nbr,
                                           uint8_t        if_nbr,
                                           void           *p_class_arg,
                                           const char     *p_name,
                                           uint8_t        *p_if_alt_nbr);

/****************************************************************************************************//**
 * @brief    Create an interface group.
 *
 * @param    config_nbr              Configuration number.
 *
 * @param    class_code              Class code assigned by the USB-IF.
 *
 * @param    class_sub_code          Subclass code assigned by the USB-IF.
 *
 * @param    class_protocol_code     protocol code assigned by the USB-IF.
 *
 * @param    if_start                Interface number of the first interface that is associated with this group.
 *
 * @param    if_cnt                  Number of consecutive interfaces that are associated with this group.
 *
 * @param    p_name                  Pointer to the string that describes the interface group.
 *
 * @param    p_if_grp_num            Pointer to the variable that will receive interface group number upon return.
 *                                   The variable is set to interface group number if no errors are returned,
 *                                   or SL_USBD_INTERFACE_GROUP_NBR_NONE if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interface_group(uint8_t      config_nbr,
                                             uint8_t      class_code,
                                             uint8_t      class_sub_code,
                                             uint8_t      class_protocol_code,
                                             uint8_t      if_start,
                                             uint8_t      if_cnt,
                                             const char   *p_name,
                                             uint8_t      *p_if_grp_num);

/****************************************************************************************************//**
 * @brief    Get the device descriptor.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in destination buffer.
 *
 * @param    p_desc_len  Pointer to a variable that will receive the actual size of the descriptor.
 *                       The variable is set to number of bytes actually in the descriptor if no errors
 *                       are returned, or set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function should be used by drivers that supporting the standard requests auto-reply,
 *               (during the initialization process).
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_device_descriptor(uint8_t *p_buf,
                                               uint8_t max_len,
                                               uint8_t *p_desc_len);

/****************************************************************************************************//**
 * @brief    Get a configuration descriptor.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in the destination buffer.
 *
 * @param    config_ix   Index of the desired configuration descriptor.
 *
 * @param    p_desc_len  Pointer to a variable that will receive the actual size of the descriptor.
 *                       The variable is set to number of bytes actually in the descriptor if no errors
 *                       are returned, or set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function should be used by drivers supporting a standard request's auto-reply,
 *               during the initialization process.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_configuration_descriptor(uint8_t  *p_buf,
                                                      uint16_t max_len,
                                                      uint8_t  config_ix,
                                                      uint16_t *p_desc_len);

/****************************************************************************************************//**
 * @brief    Get a string descriptor.
 *
 * @param    p_buf       Pointer to the destination buffer.
 *
 * @param    max_len     Maximum number of bytes to write in destination buffer.
 *
 * @param    str_ix      Index of the desired string descriptor.
 *
 * @param    p_desc_len  Pointer to a variable that will receive the actual size of the descriptor.
 *                       The variable is set to number of bytes actually in the descriptor if no errors
 *                       are returned, or set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function should be used by drivers supporting a standard request's auto-reply,
 *               during the initialization process.
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_get_string_descriptor(uint8_t  *p_buf,
                                               uint8_t  max_len,
                                               uint8_t  str_ix,
                                               uint8_t  *p_desc_len);
#endif

/****************************************************************************************************//**
 * @brief    Add string to USB device.
 *
 * @param    p_str       Pointer to string to add (see Note #1).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB spec 2.0 chapter 9.5 states: "Where appropriate, descriptors contain references
 *               to string descriptors that provide displayable information describing a descriptor
 *               in human-readable form. The inclusion of string descriptors is optional.  However,
 *               the reference fields within descriptors are mandatory. If a device does not support
 *               string descriptors, string reference fields must be reset to zero to indicate no
 *               string descriptor is available".
 *               Since string descriptors are optional, 'p_str' could be a NULL pointer.
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_add_string(const char *p_str);
#endif

/****************************************************************************************************//**
 * @brief    Get string index corresponding to a given string.
 *
 * @param    p_str       Pointer to string.
 *
 * @param    p_str_ix    Pointer to variable where string index will be returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
#if (USBD_CFG_STR_EN == 1)
sl_status_t sl_usbd_core_get_string_index(const char *p_str,
                                          uint8_t    *p_str_ix);
#endif

/****************************************************************************************************//**
 * @brief    Write an 8-bit value to the descriptor buffer.
 *
 * @param    val         8-bit value to write in the descriptor buffer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_08b_to_descriptor_buf(uint8_t val);

/****************************************************************************************************//**
 * @brief    Write a 16-bit value in the descriptor buffer.
 *
 * @param    val     16-bit value.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *
 * @note     (2) USB descriptors are in little-endian format.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_16b_to_descriptor_buf(uint16_t val);

/****************************************************************************************************//**
 * @brief    Write a 24-bit value to the descriptor buffer.
 *
 * @param    val         32-bit value containing 24 useful bits to write in the descriptor buffer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *
 * @note     (2) USB descriptors are in little-endian format.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_24b_to_descriptor_buf(uint32_t val);

/****************************************************************************************************//**
 * @brief    Write a 32-bit value to the descriptor buffer.
 *
 * @param    val         32-bit value to write in the descriptor buffer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *
 * @note     (2) USB descriptors are in little-endian format.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_32b_to_descriptor_buf(uint32_t val);

/****************************************************************************************************//**
 * @brief    Write a buffer into the descriptor buffer.
 *
 * @param    p_buf       Pointer to the buffer to write into the descriptor buffer.
 *
 * @param    len         Length of the buffer.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) USB classes may use this function to append class-specific descriptors to the
 *               configuration descriptor.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_buf_to_descriptor_buf(const uint8_t  *p_buf,
                                                     uint16_t       len);

/****************************************************************************************************//**
 * @brief    Send data on the Control IN endpoint.
 *
 * @param    p_buf       Pointer to the buffer of data that will be sent (see Note #2).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #1).
 *
 * @param    p_xfer_len  Pointer to a variable that will be set to the number of octets transmitted.
 *                       The variable is set to the number of octets transmitted if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *
 * @note     (2) Transmit buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_control_sync(void        *p_buf,
                                            uint32_t    buf_len,
                                            uint16_t    timeout_ms,
                                            bool        end,
                                            uint32_t    *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data on the Control OUT endpoint.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will be set to the number of octets received.
 *                       The variable is set to the number of octets received if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_control_sync(void       *p_buf,
                                           uint32_t   buf_len,
                                           uint16_t   timeout_ms,
                                           uint32_t   *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Add a bulk endpoint to alternate setting interface.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    dir_in          Endpoint direction.
 *                               - true      IN   direction.
 *                               - false     OUT  direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet length (see Note #1)
 *
 * @param    p_ep_addr       Pointer to a variable that will receive the new endpoint address.
 *                           The variable will be set to endpoint address if no errors are returned,
 *                           otherwise it will be set to SL_USBD_ENDPOINT_ADDR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack will allocate the first available
 *               BULK endpoint, regardless its maximum packet size.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_bulk_endpoint(uint8_t   config_nbr,
                                           uint8_t   if_nbr,
                                           uint8_t   if_alt_nbr,
                                           bool      dir_in,
                                           uint16_t  max_pkt_len,
                                           uint8_t   *p_ep_addr);

/****************************************************************************************************//**
 * @brief    Receive data on Bulk OUT endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will be set to the number of octets received.
 *                       The variable is set to the number of octets received if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_bulk_sync(uint8_t  ep_addr,
                                        void     *p_buf,
                                        uint32_t buf_len,
                                        uint16_t timeout_ms,
                                        uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data on Bulk OUT endpoint asynchronously.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_bulk_async(uint8_t                  ep_addr,
                                         void                     *p_buf,
                                         uint32_t                 buf_len,
                                         sl_usbd_async_function_t async_fnct,
                                         void                     *p_async_arg);

/****************************************************************************************************//**
 * @brief    Send data on a Bulk IN endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to buffer of data that will be transmitted (see Note #2).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #3).
 *
 * @param    p_xfer_len  Pointer to a variable that will receive the number of octets transmitted.
 *                       The variable is set to the number of octets transmitted if no errors are
 *                       returned, otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Transmit buffer must be 32 bits aligned.
 *
 * @note     (3) If end-of-transfer is set and transfer length is multiple of maximum packet sizes,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_bulk_sync(uint8_t   ep_addr,
                                         void      *p_buf,
                                         uint32_t  buf_len,
                                         uint16_t  timeout_ms,
                                         bool      end,
                                         uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data on the Bulk IN endpoint asynchronously.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Transmit buffer must be 32 bits aligned.
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_bulk_async(uint8_t                   ep_addr,
                                          void                      *p_buf,
                                          uint32_t                  buf_len,
                                          sl_usbd_async_function_t  async_fnct,
                                          void                      *p_async_arg,
                                          bool                      end);

/****************************************************************************************************//**
 * @brief    Add an interrupt endpoint to an alternate setting interface.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    dir_in          Endpoint Direction.
 *                               - true      IN   direction.
 *                               - false     OUT  direction.
 *
 * @param    max_pkt_len     Endpoint maximum packet length. (see Note #1)
 *
 * @param    interval        Endpoint interval in frames or microframes.
 *
 * @param    p_ep_addr       Pointer to a variable that will receive the new endpoint address.
 *                           The variable will be set to endpoint address if no errors are returned,
 *                           otherwise it will be set to SL_USBD_ENDPOINT_ADDR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack will allocate the first available
 *               INTERRUPT endpoint, regardless its maximum packet size.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_add_interrupt_endpoint(uint8_t    config_nbr,
                                                uint8_t    if_nbr,
                                                uint8_t    if_alt_nbr,
                                                bool       dir_in,
                                                uint16_t   max_pkt_len,
                                                uint16_t   interval,
                                                uint8_t    *p_ep_addr);

/****************************************************************************************************//**
 * @brief    Receive data on the Interrupt OUT endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the destination buffer to receive data (see Note #2).
 *
 * @param    buf_len     Number of octets to receive.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will be set to the number of octets received.
 *                       The variable is set to the number of octets received if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_interrupt_sync(uint8_t  ep_addr,
                                             void     *p_buf,
                                             uint32_t buf_len,
                                             uint16_t timeout_ms,
                                             uint32_t *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data on Interrupt OUT endpoint asynchronously.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of receive operation.
 *
 * @param    p_async_arg     Pointer to argument that will be passed as parameter of 'async_fnct'.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_read_interrupt_async(uint8_t                  ep_addr,
                                              void                     *p_buf,
                                              uint32_t                 buf_len,
                                              sl_usbd_async_function_t async_fnct,
                                              void                     *p_async_arg);

/****************************************************************************************************//**
 * @brief    Send data on Interrupt IN endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_buf       Pointer to the buffer of data that will be transmitted (see Note #2).
 *
 * @param    buf_len     Number of octets to transmit.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @param    end         End-of-transfer flag (see Note #3).
 *
 * @param    p_xfer_len  Pointer to a variable that will be set to the number of octets transmitted.
 *                       The variable is set to the number of octets transmitted if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function SHOULD NOT be called from interrupt service routine (ISR).
 *
 * @note     (2) Transmit buffer must be 32 bits aligned.
 *
 * @note     (3) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_interrupt_sync(uint8_t   ep_addr,
                                              void      *p_buf,
                                              uint32_t  buf_len,
                                              uint16_t  timeout_ms,
                                              bool      end,
                                              uint32_t  *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Send data on the Interrupt IN endpoint asynchronously.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 * @param    end             End-of-transfer flag (see Note #2).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Transmit buffer must be 32 bits aligned.
 *
 * @note     (2) If end-of-transfer is set and transfer length is multiple of maximum packet size,
 *               a zero-length packet is transferred to indicate a short transfer to the host.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_write_interrupt_async(uint8_t                  ep_addr,
                                               void                     *p_buf,
                                               uint32_t                 buf_len,
                                               sl_usbd_async_function_t async_fnct,
                                               void                     *p_async_arg,
                                               bool                     end);

/****************************************************************************************************//**
 * @brief    Add an isochronous endpoint to alternate setting interface.
 *
 * @param    config_nbr          Configuration number.
 *
 * @param    if_nbr              Interface number.
 *
 * @param    if_alt_nbr          Interface alternate setting number.
 *
 * @param    dir_in              Endpoint Direction :
 *                                   - true,      IN  direction.
 *                                   - false,     OUT direction.
 *
 * @param    attrib              Isochronous endpoint synchronization and usage type attributes.
 *
 * @param    max_pkt_len         Endpoint maximum packet length (see Note #1).
 *
 * @param    transaction_frame   Endpoint transactions per (micro)frame (see Note #2).
 *
 * @param    interval            Endpoint interval in frames or microframes.
 *
 * @param    p_ep_addr           Pointer to a variable that will receive the new endpoint address.
 *                               The variable will be set to endpoint address if no errors are returned,
 *                               otherwise it will be set to SL_USBD_ENDPOINT_ADDR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) If the 'max_pkt_len' argument is '0', the stack allocates the first available
 *               ISOCHRONOUS endpoint regardless of its maximum packet size.
 *
 * @note     (2) For full-speed endpoints, 'transaction_frame' must be set to 1 since there is no
 *               support for high-bandwidth endpoints.
 *
 * @note     (3) For full-/high-speed isochronous endpoints, bInterval value must be in the range
 *               from 1 to 16. The bInterval value is used as the exponent for a 2^(bInterval-1)
 *               value. Maximum polling interval value is 2^(16-1) = 32768 frames in full-speed and
 *               32768 microframes (i.e., 4096 frames) in high-speed.
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_add_isochronous_endpoint(uint8_t    config_nbr,
                                                  uint8_t    if_nbr,
                                                  uint8_t    if_alt_nbr,
                                                  bool       dir_in,
                                                  uint8_t    attrib,
                                                  uint16_t   max_pkt_len,
                                                  uint8_t    transaction_frame,
                                                  uint16_t   interval,
                                                  uint8_t    *p_ep_addr);
#endif

/****************************************************************************************************//**
 * @brief    Receive data on an isochronous OUT endpoint asynchronously.
 *
 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the destination buffer to receive data (see Note #1).
 *
 * @param    buf_len         Number of octets to receive.
 *
 * @param    async_fnct      Function that will be invoked upon completion of a receive operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Receive buffer must be 32 bits aligned.
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_read_isochronous_async(uint8_t                   ep_addr,
                                                void                      *p_buf,
                                                uint32_t                  buf_len,
                                                sl_usbd_async_function_t  async_fnct,
                                                void                      *p_async_arg);
#endif

/****************************************************************************************************//**
 * @brief    Send data on an isochronous IN endpoint asynchronously.

 * @param    ep_addr         Endpoint address.
 *
 * @param    p_buf           Pointer to the buffer of data that will be transmitted (see Note #1).
 *
 * @param    buf_len         Number of octets to transmit.
 *
 * @param    async_fnct      Function that will be invoked upon completion of transmit operation.
 *
 * @param    p_async_arg     Pointer to the argument that will be passed as parameter of 'async_fnct'.
 *
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Transmit buffer must be 32 bits aligned.
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_write_isochronous_async(uint8_t                  ep_addr,
                                                 void                     *p_buf,
                                                 uint32_t                 buf_len,
                                                 sl_usbd_async_function_t async_fnct,
                                                 void                     *p_async_arg);
#endif

/****************************************************************************************************//**
 * @brief    Set synchronization feedback rate on synchronization isochronous endpoint.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    synch_ep_addr   Synchronization endpoint address.
 *
 * @param    sync_refresh    Exponent of synchronization feedback rate (see Note #3).
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Synchronization endpoints can ONLY be associated when the device is in the following
 *               states:
 *                   - SL_USBD_DEVICE_STATE_NONE    Device controller has not been initialized.
 *                   - SL_USBD_DEVICE_STATE_INIT    Device controller is already      initialized.
 *
 * @note     (2) For audio class 1.0, the interface class code must be SL_USBD_CLASS_CODE_AUDIO and
 *               protocol 'zero'
 *
 * @note     (3) If explicit synchronization mechanism is needed to maintain synchronization during
 *               transfers, the information carried over the synchronization path must be available
 *               every 2 ^ (10 - P) frames, with P ranging from 1 to 9 (512 ms down to 2 ms).
 *
 * @note     (4) Table 4-22 "Standard AS Isochronous Synch Endpoint Descriptor" of Audio 1.0
 *               specification indicates for bmAttributes field no usage type for bits 5..4. But
 *               USB 2.0 specification, Table 9-13 "Standard Endpoint Descriptor" indicates several
 *               types of usage. When an explicit feedback is defined for a asynchronous isochronous
 *               endpoint, the associated synch feedback should use the Usage type 'Feedback endpoint'.
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_set_isochronous_endpoint_refresh_rate(uint8_t config_nbr,
                                                               uint8_t if_nbr,
                                                               uint8_t if_alt_nbr,
                                                               uint8_t synch_ep_addr,
                                                               uint8_t sync_refresh);
#endif

/****************************************************************************************************//**
 * @brief    Associate synchronization endpoint to isochronous endpoint.
 *
 * @param    config_nbr      Configuration number.
 *
 * @param    if_nbr          Interface number.
 *
 * @param    if_alt_nbr      Interface alternate setting number.
 *
 * @param    data_ep_addr    Data endpoint address.
 *
 * @param    sync_addr       Associated synchronization endpoint.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Synchronization endpoints can ONLY be associated when the device is in the following
 *               states:
 *                   - SL_USBD_DEVICE_STATE_NONE    Device controller has not been initialized.
 *                   - SL_USBD_DEVICE_STATE_INIT    Device controller is already      initialized.
 *
 * @note     (2) For audio class 1.0, interface class code must be SL_USBD_CLASS_CODE_AUDIO and protocol
 *               'zero'.
 *******************************************************************************************************/
#if (USBD_CFG_EP_ISOC_EN == 1)
sl_status_t sl_usbd_core_set_isochronous_endpoint_sync_address(uint8_t config_nbr,
                                                               uint8_t if_nbr,
                                                               uint8_t if_alt_nbr,
                                                               uint8_t data_ep_addr,
                                                               uint8_t sync_addr);
#endif

/****************************************************************************************************//**
 * @brief    Send zero-length packet to the host.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function should only be called during a synchronous transfer.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_zlp(uint8_t  ep_addr,
                                            uint16_t timeout_ms);

/****************************************************************************************************//**
 * @brief    Receive zero-length packet from the host.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    timeout_ms  Timeout in milliseconds.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function should only be called during a synchronous transfer.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_read_zlp(uint8_t  ep_addr,
                                           uint16_t timeout_ms);

/****************************************************************************************************//**
 * @brief    Abort I/O transfer on the endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_abort_endpoint(uint8_t ep_addr);

/****************************************************************************************************//**
 * @brief    Notify USB stack that packet receive has completed.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_read_complete(uint8_t ep_log_nbr);

/****************************************************************************************************//**
 * @brief    Notify USB stack that packet transmit has completed.
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_complete(uint8_t ep_log_nbr);

/****************************************************************************************************//**
 * @brief    Notify USB stack that packet transmit has completed (see Note #1).
 *
 * @param    ep_log_nbr  Endpoint logical number.
 *
 * @param    xfer_err    Error code returned by the USB driver.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) This function is an alternative to the function sl_usbd_core_endpoint_write_complete() so that a USB device
 *               driver can return to the core an error code upon the Tx transfer completion.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_endpoint_write_complete_extended(uint8_t      ep_log_nbr,
                                                          sl_status_t  xfer_err);

/****************************************************************************************************//**
 * @brief    Stall the non-control endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    state       Endpoint stall state.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_stall_endpoint(uint8_t  ep_addr,
                                        bool     state);

/****************************************************************************************************//**
 * @brief    Get the stall status of a non-control endpoint.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_ep_stall  Pointer to a boolean that will receive the endpoint stalled status.
 *                       The boolean variable is set to true if endpoint is stalled,
 *                       or is set to false if endpoint is not stalled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_is_endpoint_stalled(uint8_t  ep_addr,
                                             bool     *p_ep_stall);

/****************************************************************************************************//**
 * @brief    Retrieve the endpoint maximum packet size.
 *
 * @param    ep_addr     Endpoint address.
 *
 * @param    p_max_size  Pointer to a variable that will receive the maximum packet size upon return.
 *                       The variable is set to the maximum packet size if no errors are returned,
 *                       otherwise it is set to 0.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_max_endpoint_packet_size(uint8_t  ep_addr,
                                                      uint16_t *p_max_size);

/****************************************************************************************************//**
 * @brief    Get the maximum physical endpoint number.
 *
 * @param    p_ep_phy_nbr  Pointer to variable where maximum physical endpoint number will be returned.
 *                         If no errors are returned, the variable will be set to maximum physical endpoint
 *                         number, otherwise it will be set to SL_USBD_ENDPOINT_PHY_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_core_get_max_phy_endpoint_number(uint8_t *p_ep_phy_nbr);

sl_status_t sl_usbd_core_get_max_open_endpoint_number(uint8_t *p_nbr_open);

//                                                                 ------------------ EVENT HANDLERS ------------------
__WEAK void sl_usbd_on_bus_event(sl_usbd_bus_event_t event);

__WEAK void sl_usbd_on_config_event(sl_usbd_config_event_t event,
                                    uint8_t                config_nbr);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  USBD_CFG_STR_EN
#error  "USBD_CFG_STR_EN not #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"

#elif  ((USBD_CFG_STR_EN != 0u) \
  && (USBD_CFG_STR_EN != 1u))
#error  "USBD_CFG_STR_EN illegally #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"
#endif

#ifndef  USBD_CFG_URB_EXTRA_EN
#error  "USBD_CFG_URB_EXTRA_EN not #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"

#elif  ((USBD_CFG_URB_EXTRA_EN != 0u) \
  && (USBD_CFG_URB_EXTRA_EN != 1u))
#error  "USBD_CFG_URB_EXTRA_EN illegally #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"
#endif

#ifndef  USBD_CFG_MS_OS_DESC_EN
#error  "USBD_CFG_MS_OS_DESC_EN not #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"

#elif  ((USBD_CFG_MS_OS_DESC_EN != 0u) \
  && (USBD_CFG_MS_OS_DESC_EN != 1u))
#error  "USBD_CFG_MS_OS_DESC_EN illegally #define'd in 'usbd_cfg.h' [MUST be 0u || 1u]"

#elif  ((USBD_CFG_MS_OS_DESC_EN == 1u) \
  && (USBD_CFG_STR_EN == 0u))
#error  "USBD_CFG_STR_EN illegally #define'd in 'usbd_cfg.h'. MUST be 1u when USBD_CFG_MS_OS_DESC_EN is 1u"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
