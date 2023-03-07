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

#ifndef  _USBD_CORE_H_
#define  _USBD_CORE_H_

#if !defined(SL_SUPRESS_MICRIUMOS_USB_DEVICE_DEPRECATION_WARNINGS)
#warning  Warning: MicriumOS-USB-Device is deprecated and will be removed in an upcoming release; Please consider migrating to sl_usb.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <cpu/include/cpu.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_mem.h>

#include  <usbd_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  USBD_CORE_MODULE
#define  USBD_CORE_EXT
#else
#define  USBD_CORE_EXT  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  USBD_CFG_DBG_STATS_EN
#define  USBD_CFG_DBG_STATS_EN                  DEF_DISABLED
#endif

#define  USBD_SETUP_PKT_LEN                                8u   // Setup packet length.

/********************************************************************************************************
 *                                                   FRAME
 *
 * Note(s) : (1) Section 8.3.3 of USB spec 2.0 describes the frame number as:
 *               "The frame number field is an 11-bit field that is incremented by the host on a per-frame
 *                   basis. The frame number field rolls over upon reaching its maximum value of 7FFH and
 *                   is sent only in SOF tokens at the start of each (micro)frame."
 *******************************************************************************************************/

#define  USBD_MAX_FRAME_NBR                             2047u   // See Note #1.
#define  USBD_FRAME_NBR_MASK                    DEF_BIT_FIELD_16(11u, 0u)
#define  USBD_MICROFRAME_NBR_MASK               DEF_BIT_FIELD_16(3u, 11u)

/********************************************************************************************************
 *                                       CONFIGURATION ATTRIBUTES
 *******************************************************************************************************/

#define  USBD_DEV_ATTRIB_SELF_POWERED           DEF_BIT_00
#define  USBD_DEV_ATTRIB_REMOTE_WAKEUP          DEF_BIT_01

/********************************************************************************************************
 *                                       DEVICE DESCRIPTOR LENGTH
 *******************************************************************************************************/

#define  USBD_DESC_LEN_DEV                                18u   // Device                    descriptor length.
#define  USBD_DESC_LEN_DEV_QUAL                           10u   // Device qualifier          descriptor length.
#define  USBD_DESC_LEN_HDR                                 2u   // Descriptor header                    length.
#define  USBD_DESC_LEN_CFG                                 9u   // Configuration             descriptor length.
#define  USBD_DESC_LEN_OTHER_SPD_CFG                       9u   // Configuration other speed descriptor length.
#define  USBD_DESC_LEN_IF                                  9u   // Interface                 descriptor length.
#define  USBD_DESC_LEN_IF_ASSOCIATION                      8u   // Interface association     descriptor length.
#define  USBD_DESC_LEN_EP                                  7u   // Endpoint                  descriptor length.
#define  USBD_DESC_LEN_OTG                                 3u   // On-The-Go                 descriptor length

/********************************************************************************************************
 *                                       REQUEST CHARACTERISTICS
 *
 * Note(s) : (1) Request types are defined in USB spec 2.0, section 9.3, Table 9-2, 'RequestType' field.
 *******************************************************************************************************/

#define  USBD_REQ_DIR_MASK                      DEF_BIT_07
#define  USBD_REQ_DIR_HOST_TO_DEVICE            DEF_BIT_NONE
#define  USBD_REQ_DIR_DEVICE_TO_HOST            DEF_BIT_07
#define  USBD_REQ_DIR_BIT                       DEF_BIT_07

#define  USBD_REQ_TYPE_MASK                             0x60u
#define  USBD_REQ_TYPE_STANDARD                         0x00u
#define  USBD_REQ_TYPE_CLASS                            0x20u
#define  USBD_REQ_TYPE_VENDOR                           0x40u
#define  USBD_REQ_TYPE_RESERVED                         0x60u

#define  USBD_REQ_RECIPIENT_MASK                        0x1Fu
#define  USBD_REQ_RECIPIENT_DEVICE                      0x00u
#define  USBD_REQ_RECIPIENT_INTERFACE                   0x01u
#define  USBD_REQ_RECIPIENT_ENDPOINT                    0x02u
#define  USBD_REQ_RECIPIENT_OTHER                       0x03u

/********************************************************************************************************
 *                                           STANDARD REQUESTS
 *
 * Note(s) : (1) Request types are defined in USB spec 2.0, section 9.4, Table 9-4, 'RequestType' field.
 *******************************************************************************************************/

#define  USBD_REQ_GET_STATUS                               0u
#define  USBD_REQ_CLEAR_FEATURE                            1u
#define  USBD_REQ_SET_FEATURE                              3u
#define  USBD_REQ_SET_ADDRESS                              5u
#define  USBD_REQ_GET_DESCRIPTOR                           6u
#define  USBD_REQ_SET_DESCRIPTOR                           7u
#define  USBD_REQ_GET_CONFIGURATION                        8u
#define  USBD_REQ_SET_CONFIGURATION                        9u
#define  USBD_REQ_GET_INTERFACE                           10u
#define  USBD_REQ_SET_INTERFACE                           11u
#define  USBD_REQ_SYNCH_FRAME                             12u

/********************************************************************************************************
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) Descriptors types are defined in the USB spec 2.0 section 9.2.6, Table 9-5.
 *******************************************************************************************************/

#define  USBD_DESC_TYPE_DEVICE                             1u
#define  USBD_DESC_TYPE_CONFIGURATION                      2u
#define  USBD_DESC_TYPE_STRING                             3u
#define  USBD_DESC_TYPE_INTERFACE                          4u
#define  USBD_DESC_TYPE_ENDPOINT                           5u
#define  USBD_DESC_TYPE_DEVICE_QUALIFIER                   6u
#define  USBD_DESC_TYPE_OTHER_SPEED_CONFIGURATION          7u
#define  USBD_DESC_TYPE_INTERFACE_POWER                    8u
#define  USBD_DESC_TYPE_OTG                                9u
#define  USBD_DESC_TYPE_IAD                               11u

/********************************************************************************************************
 *                                           LANGUAGE IDENTIFIERS
 *
 * Note(s) : (1) Languages identifier are defined in "http://www.usb.org/developers/docs/USB_LANGIDs.pdf".
 *******************************************************************************************************/

#define  USBD_LANG_ID_ARABIC_SAUDI_ARABIA             0x0401u
#define  USBD_LANG_ID_CHINESE_TAIWAN                  0x0404u
#define  USBD_LANG_ID_ENGLISH_US                      0x0409u
#define  USBD_LANG_ID_ENGLISH_UK                      0x0809u
#define  USBD_LANG_ID_FRENCH                          0x040Cu
#define  USBD_LANG_ID_GERMAN                          0x0407u
#define  USBD_LANG_ID_GREEK                           0x0408u
#define  USBD_LANG_ID_ITALIAN                         0x0410u
#define  USBD_LANG_ID_PORTUGUESE                      0x0816u
#define  USBD_LANG_ID_SANSKRIT                        0x044Fu

/********************************************************************************************************
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

#define  USBD_CLASS_CODE_USE_IF_DESC                    0x00u
#define  USBD_CLASS_CODE_AUDIO                          0x01u
#define  USBD_CLASS_CODE_CDC_CONTROL                    0x02u
#define  USBD_CLASS_CODE_HID                            0x03u
#define  USBD_CLASS_CODE_PHYSICAL                       0x05u
#define  USBD_CLASS_CODE_IMAGE                          0x06u
#define  USBD_CLASS_CODE_PRINTER                        0x07u
#define  USBD_CLASS_CODE_MASS_STORAGE                   0x08u
#define  USBD_CLASS_CODE_HUB                            0x09u
#define  USBD_CLASS_CODE_CDC_DATA                       0x0Au
#define  USBD_CLASS_CODE_SMART_CARD                     0x0Bu
#define  USBD_CLASS_CODE_CONTENT_SECURITY               0x0Du
#define  USBD_CLASS_CODE_VIDEO                          0x0Eu
#define  USBD_CLASS_CODE_PERSONAL_HEALTHCARE            0x0Fu
#define  USBD_CLASS_CODE_DIAGNOSTIC_DEVICE              0xDCu
#define  USBD_CLASS_CODE_WIRELESS_CONTROLLER            0xE0u
#define  USBD_CLASS_CODE_MISCELLANEOUS                  0xEFu
#define  USBD_CLASS_CODE_APPLICATION_SPECIFIC           0xFEu
#define  USBD_CLASS_CODE_VENDOR_SPECIFIC                0xFFu

/********************************************************************************************************
 *                                           SUB-CLASS CODES
 *******************************************************************************************************/

#define  USBD_SUBCLASS_CODE_USE_IF_DESC                 0x00u
#define  USBD_SUBCLASS_CODE_USE_COMMON_CLASS            0x02u
#define  USBD_SUBCLASS_CODE_VENDOR_SPECIFIC             0xFFu

/********************************************************************************************************
 *                                           PROTOCOL CODES
 *******************************************************************************************************/

#define  USBD_PROTOCOL_CODE_USE_IF_DESC                 0x00u
#define  USBD_PROTOCOL_CODE_USE_IAD                     0x01u
#define  USBD_PROTOCOL_CODE_VENDOR_SPECIFIC             0xFFu

/********************************************************************************************************
 *                                           FEATURE SELECTORS
 *******************************************************************************************************/

#define  USBD_FEATURE_SEL_ENDPOINT_HALT                    0u
#define  USBD_FEATURE_SEL_DEVICE_REMOTE_WAKEUP             1u
#define  USBD_FEATURE_SEL_TEST_MODE                        2u

/********************************************************************************************************
 *                                       DEVICE POWER CONSTRAINS
 *******************************************************************************************************/

#define  USBD_MAX_BUS_PWR_LIMIT_mA                       500u

/********************************************************************************************************
 *                                               ENDPOINT TYPE
 *
 * Note(s) : (1) Endpoint types are defined in the USB spec 2.0 section 9.6.6, Table 9-13, 'bmAttributes'
 *               field.
 *******************************************************************************************************/

#define  USBD_EP_TYPE_MASK                              0x03u
#define  USBD_EP_TYPE_CTRL                              0x00u
#define  USBD_EP_TYPE_ISOC                              0x01u
#define  USBD_EP_TYPE_BULK                              0x02u
#define  USBD_EP_TYPE_INTR                              0x03u

#define  USBD_EP_TYPE_SYNC_MASK                         0x0Cu
#define  USBD_EP_TYPE_SYNC_NONE                         0x00u
#define  USBD_EP_TYPE_SYNC_ASYNC                        0x04u
#define  USBD_EP_TYPE_SYNC_ADAPTIVE                     0x08u
#define  USBD_EP_TYPE_SYNC_SYNC                         0x0Cu

#define  USBD_EP_TYPE_USAGE_MASK                        0x30u
#define  USBD_EP_TYPE_USAGE_DATA                        0x00u
#define  USBD_EP_TYPE_USAGE_FEEDBACK                    0x10u
#define  USBD_EP_TYPE_USAGE_IMPLICIT_FEEDBACK           0x20u

/********************************************************************************************************
 *                                           ENDPOINT ADDRESS
 *
 * Note(s) : (1) Endpoint address is defined in the USB spec 2.0, section 9.6.6, Table 9-13,
 *               'bEndpointAddress' field.
 *******************************************************************************************************/

#define  USBD_EP_NBR_MASK                               0x0Fu
#define  USBD_EP_MAX_NBR                                  32u

#define  USBD_EP_DIR_MASK                               0x80u
#define  USBD_EP_DIR_BIT                        DEF_BIT_07
#define  USBD_EP_DIR_OUT                        DEF_BIT_NONE
#define  USBD_EP_DIR_IN                         DEF_BIT_07

#define  USBD_EP_TRANSACTION_PER_UFRAME_1                  1u
#define  USBD_EP_TRANSACTION_PER_UFRAME_2                  2u
#define  USBD_EP_TRANSACTION_PER_UFRAME_3                  3u

#define  USBD_EP_MAX_INTERVAL_VAL                      32768u   // Max val is 2^(16 - 1).

/********************************************************************************************************
 *                                       MICROSOFT OS DESCRIPTOR DEFINES
 *
 * Note(s)     : (1) For more information on Microsoft OS descriptors, see
 *                   'http://msdn.microsoft.com/en-us/library/windows/hardware/gg463179.aspx'.
 *******************************************************************************************************/

//                                                                 Microsoft OS compatible IDs index.
#define  USBD_MS_OS_COMPAT_ID_NULL                         0u
#define  USBD_MS_OS_COMPAT_ID_RNDIS                        1u
#define  USBD_MS_OS_COMPAT_ID_PTP                          2u
#define  USBD_MS_OS_COMPAT_ID_MTP                          3u
#define  USBD_MS_OS_COMPAT_ID_XUSB20                       4u
#define  USBD_MS_OS_COMPAT_ID_BLUETOOTH                    5u
#define  USBD_MS_OS_COMPAT_ID_WINUSB                       6u
#define  USBD_MS_OS_COMPAT_ID_NONE                       255u

//                                                                 Microsoft OS subcompatible IDs index.
#define  USBD_MS_OS_SUBCOMPAT_ID_NULL                      0u
#define  USBD_MS_OS_SUBCOMPAT_ID_BLUETOOTH_1_1             1u
#define  USBD_MS_OS_SUBCOMPAT_ID_BLUETOOTH_1_2             2u
#define  USBD_MS_OS_SUBCOMPAT_ID_BLUETOOTH_EDR             3u

//                                                                 Microsoft OS extended property types.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_SZ                   1u   // NULL-terminated Unicode str.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_EXPAND_SZ            2u   // NULL-terminated Unicode str with environment vars.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_BINARY               3u   // Free-form binary.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_DWORD_LITTLE_ENDIAN  4u   // Little-endian 32-bit integer.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_DWORD_BIG_ENDIAN     5u   // Big-endian 32-bit integer.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_LINK                 6u   // NULL-terminated Unicode str with a symbolic link.
#define  USBD_MS_OS_PROPERTY_TYPE_REG_MULTI_SZ             7u   // Multiple NULL-terminated Unicode strings.

/********************************************************************************************************
 *                                   USB EP INFORMATION TABLE DEFINES
 *******************************************************************************************************/

//                                                                 ------------- ENDPOINT TYPE BIT DEFINES ------------
#define  USBD_EP_INFO_TYPE_CTRL                 DEF_BIT_00
#define  USBD_EP_INFO_TYPE_ISOC                 DEF_BIT_01
#define  USBD_EP_INFO_TYPE_BULK                 DEF_BIT_02
#define  USBD_EP_INFO_TYPE_INTR                 DEF_BIT_03

//                                                                 ---------- ENDPOINT DIRECTION BIT DEFINES ----------
#define  USBD_EP_INFO_DIR_OUT                   DEF_BIT_04
#define  USBD_EP_INFO_DIR_IN                    DEF_BIT_05

/********************************************************************************************************
 *                                           USB OBJECT NUMBER
 *******************************************************************************************************/

#define  USBD_DEV_NBR_NONE                      DEF_INT_08U_MAX_VAL

#define  USBD_CONFIG_NBR_NONE                   DEF_INT_08U_MAX_VAL
#define  USBD_CONFIG_NBR_SPD_BIT                DEF_BIT_07

#define  USBD_IF_NBR_NONE                       DEF_INT_08U_MAX_VAL

#define  USBD_IF_ALT_NBR_NONE                   DEF_INT_08U_MAX_VAL

#define  USBD_IF_GRP_NBR_NONE                   DEF_INT_08U_MAX_VAL

#define  USBD_EP_ADDR_NONE                      DEF_INT_08U_MAX_VAL
#define  USBD_EP_PHY_NONE                       DEF_INT_08U_MAX_VAL

#define  USBD_EP_NBR_NONE                       DEF_INT_08U_MAX_VAL

#define  USBD_CLASS_NBR_NONE                    DEF_INT_08U_MAX_VAL

/********************************************************************************************************
 *                                               USB CORE EVENTS
 *
 * Note(s) : (1) There are 7 possible USB callback events:
 *
 *                   USBD_EventReset(),
 *                   USBD_EventResetCmpl(),
 *                   USBD_EventSuspend(),
 *                   USBD_EventResume(),
 *                   USBD_EventConn(),
 *                   USBD_EventDisconn(),
 *                   USBD_EventHS().
 *******************************************************************************************************/

#define  USBD_CORE_EVENT_BUS_NBR                          7u    // Number of bus events per controller.

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FORWARD DECLARATIONS
 *******************************************************************************************************/

typedef struct usbd_drv USBD_DRV;
typedef const struct usbd_drv_api USBD_DRV_API;

/****************************************************************************************************//**
 *                                   USB DEVICE CONFIGURATION DATA TYPE
 * @{
 *******************************************************************************************************/

typedef struct usbd_qty_cfg {
  CPU_INT08U DevQty;                                            ///< Maximum quantity of device.
  CPU_INT08U ConfigQty;                                         ///< Maximum total quantity of USB configurations.
  CPU_INT08U IF_Qty;                                            ///< Maximum total quantity of interfaces.
  CPU_INT08U IF_AltQty;                                         ///< Maximum total quantity of alternate interfaces.
  CPU_INT08U IF_GrpQty;                                         ///< Maximum total quantity of interface groups.
  CPU_INT08U EP_DescQty;                                        ///< Maximum total quantity of endpoints.
  CPU_INT16U URB_ExtraQty;                                      ///< Maximum total quantity of extra URB.
  CPU_INT08U StrQty;                                            ///< Maximum quantity of USB strings per device.
  CPU_INT08U EP_OpenQty;                                        ///< Maximum quantity of opened endpoints per device.
} USBD_QTY_CFG;

typedef struct usbd_init_cfg {
  CPU_SIZE_T BufAlignOctets;                                    ///< Alignment of buffers, in octets.

  MEM_SEG    *MemSegPtr;                                        ///< Ptr to mem seg used for internal data alloc.
  MEM_SEG    *MemSegBufPtr;                                     ///< Ptr to mem seg used for internal buffers alloc.
} USBD_INIT_CFG;

///< @}

/****************************************************************************************************//**
 *                                       USB DEVICE SPEED DATA TYPE
 *
 * @note     (1) 'Universal Serial Bus Specification Rev 2.0', section 4.2.1 defines three data rates:
 *               - (a) The USB  Low-Speed (LS) signaling bit rate is   1.5 Mb/s.
 *               - (b) The USB Full-Speed (FS) signaling bit rate is  12   Mb/s.
 *               - (c) The USB High-Speed (HS) signaling bit rate is 480   Mb/s.
 *******************************************************************************************************/

typedef enum usbd_dev_spd {
  USBD_DEV_SPD_INVALID = 0u,
  USBD_DEV_SPD_LOW,                                             ///<  Low-Speed (see Note #1a).
  USBD_DEV_SPD_FULL,                                            ///< Full-Speed (see Note #1b).
  USBD_DEV_SPD_HIGH                                             ///< High-Speed (see Note #1c).
} USBD_DEV_SPD;

/****************************************************************************************************//**
 *                                       USB DEVICE STATES DATA TYPE
 *
 * @note     (1) 'Universal Serial Bus Specification Rev 2.0', section 9.1.1 defines seven visible device
 *                   states:
 *               - (a) USBD_DEV_STATE_INIT       : Device is not attached to the USB.
 *               - (b) USBD_DEV_STATE_ATTACHED   : Device is attached to the USB, but not powered.
 *               - (c) USBD_DEV_STATE_POWERED    : Device is attached to the USB and powered, but has not
 *                                               been reset.
 *               - (d) USBD_DEV_STATE_DEFAULT    : Device is attached to the USB and powered and has been
 *                                               reset, but has not been assigned a unique address.
 *                                               Device responds at the default address.
 *               - (e) USBD_DEV_STATE_ADDRESSED  : Device is attached to the USB, powered, has been reset,
 *                                               and a unique device address has been assigned. Device
 *                                               is not configured.
 *               - (f) USBD_DEV_STATE_CONFIGURED : Device is attached to the USB, powered, has been reset,
 *                                               has a unique  address, is configured, and is not suspended.
 *                                               The host may now use the function provided by the device.
 *               - (g) USBD_DEV_STATE_SUSPENDED  : Device is, at minimum, attached to the USB and is powered
 *                                               and has not  seen bus activity for 3 ms. It may also have
 *                                               a unique address and be  configured for use. However,
 *                                               because the device is suspended, the host may not use the
 *                                               device's function.
 * @note     (2) An additional state is added (USBD_DEV_STATE_START) to determine if the device controller
 *               has been initialized.
 *******************************************************************************************************/

typedef enum usbd_dev_state {
  USBD_DEV_STATE_NONE = 0,
  USBD_DEV_STATE_INIT,
  USBD_DEV_STATE_ATTACHED,
  USBD_DEV_STATE_DEFAULT,
  USBD_DEV_STATE_ADDRESSED,
  USBD_DEV_STATE_CONFIGURED,
  USBD_DEV_STATE_SUSPENDED
} USBD_DEV_STATE;

/********************************************************************************************************
 *                                       DEVICE EVENT CALLBACKS
 *******************************************************************************************************/

typedef const struct usbd_bus_fncts {                           // --------- DEVICE EVENT CALLBACKS STRUCTURE ---------
  void (*Reset)(CPU_INT08U dev_nbr);                            // Notify application about reset   event.

  void (*Suspend)(CPU_INT08U dev_nbr);                          // Notify application about suspend event.

  void (*Resume)(CPU_INT08U dev_nbr);                           // Notify application about resume  event.

  void (*CfgSet)(CPU_INT08U dev_nbr,                            // Notify application about    configured state.
                 CPU_INT08U cfg_val);

  void (*CfgClr)(CPU_INT08U dev_nbr,                            // Notify application about de-configured state.
                 CPU_INT08U cfg_val);

  void (*Conn)(CPU_INT08U dev_nbr);                             // Notify application about device    connect.

  void (*Disconn)(CPU_INT08U dev_nbr);                          // Notify application about device disconnect.
} USBD_BUS_FNCTS;

/*
 ********************************************************************************************************
 *                               USB DEVICE DRIVER CONFIGURATION STRUCT
 *******************************************************************************************************/

typedef struct usbd_dev_drv_cfg {
  CPU_INT08U EP_OpenQty;                                        ///< Maximum quantity of opened endpoint on device.
  CPU_INT08U URB_ExtraQty;                                      ///< Maximum quantity of extra URB on device. Can be 0.
} USBD_DEV_DRV_CFG;

/*
 ********************************************************************************************************
 *                                   USB DEVICE CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef struct usbd_dev_cfg {
  CPU_INT16U     VendorID;                                      ///< Vendor  ID.
  CPU_INT16U     ProductID;                                     ///< Product ID.
  CPU_INT16U     DeviceBCD;                                     ///< Device release number.
  const CPU_CHAR *ManufacturerStrPtr;                           ///< Manufacturer string.
  const CPU_CHAR *ProductStrPtr;                                ///< Product      string.
  const CPU_CHAR *SerialNbrStrPtr;                              ///< Serial Number ID.
  CPU_INT16U     LangId;                                        ///< Language      ID.
} USBD_DEV_CFG;

/********************************************************************************************************
 *                           USB DEVICE MICROSOFT XTENDED PROPERTIES DATA TYPE
 *******************************************************************************************************/
#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
typedef struct usbd_ms_os_ext_property {
  CPU_INT08U       PropertyType;

  const CPU_INT08U *PropertyNamePtr;
  CPU_INT16U       PropertyNameLen;

  const CPU_INT08U *PropertyPtr;
  CPU_INT32U       PropertyLen;
} USBD_MS_OS_EXT_PROPERTY;
#endif

/*
 ********************************************************************************************************
 *                                           USB SETUP REQUEST
 *******************************************************************************************************/

typedef struct usbd_setup_req {
  CPU_INT08U bmRequestType;                                     ///< Characteristics of request.
  CPU_INT08U bRequest;                                          ///< Specific request.
  CPU_INT16U wValue;                                            ///< Varies according to request.
  CPU_INT16U wIndex;                                            ///< Varies according to request; typically used as index.
  CPU_INT16U wLength;                                           ///< Transfer length if data stage present.
} USBD_SETUP_REQ;

/********************************************************************************************************
 *                                       USB DEVICE CONTROLLER BSP API
 *******************************************************************************************************/

typedef const struct usbd_drv_bsp_api {
  void (*Init)(USBD_DRV *p_drv);                                // Initialize.

  void (*Conn)(void);                                           // Connect.

  void (*Disconn)(void);                                        // Disconnect.
} USBD_DRV_BSP_API;

/****************************************************************************************************//**
 *                                   ENDPOINT INFORMATION DATA TYPE
 *
 * @note     (1) The endpoint information data type provides information about the USB device controller
 *               physical EPs.
 *               - (a) The 'Attrib' bit-field defines the EP attributes. The EP attributes is combination
 *                   of the following flags:
 *                       - USBD_EP_INFO_TYPE_CTRL  Indicate control     type      capable.
 *                       - USBD_EP_INFO_TYPE_ISOC  Indicate isochronous type      capable.
 *                       - USBD_EP_INFO_TYPE_BULK  Indicate bulk        type      capable.
 *                       - USBD_EP_INFO_TYPE_INTR  Indicate interrupt   type      capable.
 *                       - USBD_EP_INFO_DIR_OUT    Indicate OUT         direction capable.
 *                       - USBD_EP_INFO_DIR_IN     Indicate IN          direction capable.
 *******************************************************************************************************/

typedef const struct usbd_drv_ep_info {
  CPU_INT08U Attrib;                                            ///< Endpoint attributes (see Note #1a).
  CPU_INT08U Nbr;                                               ///< Endpoint number.
  CPU_INT16U MaxPktSize;                                        ///< Endpoint maximum packet size.
} USBD_DRV_EP_INFO;

/*
 ********************************************************************************************************
 *                               USB DEVICE DRIVER INFORMATION DATA TYPE
 *******************************************************************************************************/

typedef const struct usbd_drv_info {
  CPU_ADDR         BaseAddr;                                    ///< Base address of device controller hardware registers.
  CPU_ADDR         MemAddr;                                     ///< Base address of device controller dedicated memory.
  CPU_ADDR         MemSize;                                     ///< Size         of device controller dedicated memory.
  USBD_DEV_SPD     Spd;                                         ///< Speed        of device controller.
  USBD_DRV_EP_INFO *EP_InfoTbl;                                 ///< Device controller EP information table.
} USBD_DRV_INFO;

/*
 ********************************************************************************************************
 *                                       USB DEVICE DRIVER DATA TYPE
 *******************************************************************************************************/

struct usbd_drv {
  CPU_INT08U       DevNbr;
  USBD_DRV_API     *API_Ptr;                                    ///< Device controller API.
  USBD_DRV_INFO    *CfgPtr;                                     ///< Device controller configuration.
  USBD_DEV_DRV_CFG DrvCfg;                                      ///< Driver configuration.
  void             *DataPtr;                                    ///< Device controller local data.
  USBD_DRV_BSP_API *BSP_API_Ptr;                                ///< Device controller board specific API.
};

/********************************************************************************************************
 *                                       INTERFACE CLASS DRIVER
 *******************************************************************************************************/

typedef const struct usbd_class_drv {
  //                                                               Notify class that cfg is active.
  void (*Conn)(CPU_INT08U dev_nbr,
               CPU_INT08U cfg_nbr,
               void       *p_if_class_arg);

  //                                                               Notify class that cfg is not active.
  void (*Disconn)(CPU_INT08U dev_nbr,
                  CPU_INT08U cfg_nbr,
                  void       *p_if_class_arg);

  //                                                               Notify class that alt setting has been updated.
  void (*AltSettingUpdate)(CPU_INT08U dev_nbr,
                           CPU_INT08U cfg_nbr,
                           CPU_INT08U if_nbr,
                           CPU_INT08U if_alt_nbr,
                           void       *p_if_class_arg,
                           void       *p_if_alt_class_arg);

  //                                                               Notify class that EP state has been updated.
  void (*EP_StateUpdate)(CPU_INT08U dev_nbr,
                         CPU_INT08U cfg_nbr,
                         CPU_INT08U if_nbr,
                         CPU_INT08U if_alt_nbr,
                         CPU_INT08U ep_addr,
                         void       *p_if_class_arg,
                         void       *p_if_alt_class_arg);

  //                                                               Construct  IF functional descriptor.
  void (*IF_Desc)(CPU_INT08U dev_nbr,
                  CPU_INT08U cfg_nbr,
                  CPU_INT08U if_nbr,
                  CPU_INT08U if_alt_nbr,
                  void       *p_if_class_arg,
                  void       *p_if_alt_class_arg);

  //                                                               Get size of IF functional descriptor.
  CPU_INT16U (*IF_DescSizeGet)(CPU_INT08U dev_nbr,
                               CPU_INT08U cfg_nbr,
                               CPU_INT08U if_nbr,
                               CPU_INT08U if_alt_nbr,
                               void       *p_if_class_arg,
                               void       *p_if_alt_class_arg);

  //                                                               Construct  EP functional descriptor.
  void (*EP_Desc)(CPU_INT08U dev_nbr,
                  CPU_INT08U cfg_nbr,
                  CPU_INT08U if_nbr,
                  CPU_INT08U if_alt_nbr,
                  CPU_INT08U ep_addr,
                  void       *p_if_class_arg,
                  void       *p_if_alt_class_arg);

  //                                                               Get size of EP functional descriptor.
  CPU_INT16U (*EP_DescSizeGet)(CPU_INT08U dev_nbr,
                               CPU_INT08U cfg_nbr,
                               CPU_INT08U if_nbr,
                               CPU_INT08U if_alt_nbr,
                               CPU_INT08U ep_addr,
                               void       *p_if_class_arg,
                               void       *p_if_alt_class_arg);

  //                                                               Interface       request callback.
  CPU_BOOLEAN (*IF_Req)(CPU_INT08U           dev_nbr,
                        const USBD_SETUP_REQ *p_setup_req,
                        void                 *p_if_class_arg);

  //                                                               Class-specific  request callback.
  CPU_BOOLEAN (*ClassReq)(CPU_INT08U           dev_nbr,
                          const USBD_SETUP_REQ *p_setup_req,
                          void                 *p_if_class_arg);

  //                                                               Vendor-specific request callback.
  CPU_BOOLEAN (*VendorReq)(CPU_INT08U           dev_nbr,
                           const USBD_SETUP_REQ *p_setup_req,
                           void                 *p_if_class_arg);

#if (USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED)
  //                                                               Get MS compatible ID nbr.
  CPU_INT08U (*MS_GetCompatID)(CPU_INT08U dev_nbr,
                               CPU_INT08U *p_sub_compat_id_ix);

  //                                                               Get MS ext properties.
  CPU_INT08U (*MS_GetExtPropertyTbl)(CPU_INT08U              dev_nbr,
                                     USBD_MS_OS_EXT_PROPERTY **pp_ext_property_tbl);
#endif
} USBD_CLASS_DRV;

/********************************************************************************************************
 *                                       EP ASYNCHRONOUS CALLBACK API
 *******************************************************************************************************/

typedef void (*USBD_ASYNC_FNCT)(CPU_INT08U dev_nbr,             // Device number.
                                CPU_INT08U ep_addr,             // Endpoint address.
                                void       *p_buf,              // Pointer to the buffer.
                                CPU_INT32U buf_len,             // Buffer   length.
                                CPU_INT32U xfer_len,            // Transfer length.
                                void       *p_arg,              // Function argument.
                                RTOS_ERR   err);                // Error status.

/********************************************************************************************************
 *                                           USB DEVICE DRIVER API
 *******************************************************************************************************/

struct usbd_drv_api {
  void        (*Init)       (USBD_DRV *p_drv,                   // Initialize.
                             RTOS_ERR *p_err);

  void        (*Start)      (USBD_DRV *p_drv,                   // Start.
                             RTOS_ERR *p_err);

  void        (*Stop)       (USBD_DRV *p_drv);                  // Stop.

  CPU_BOOLEAN (*AddrSet)    (USBD_DRV   *p_drv,                 // Set    Address.
                             CPU_INT08U dev_addr);

  void        (*AddrEn)     (USBD_DRV   *p_drv,                 // Enable Address.
                             CPU_INT08U dev_addr);

  CPU_BOOLEAN (*CfgSet)     (USBD_DRV   *p_drv,                 // Set   Configuration.
                             CPU_INT08U cfg_val);

  void        (*CfgClr)     (USBD_DRV   *p_drv,                 // Clear Configuration.
                             CPU_INT08U cfg_val);

  CPU_INT16U  (*FrameNbrGet)(USBD_DRV *p_drv);                  // Get Frame number.

  void        (*EP_Open)    (USBD_DRV   *p_drv,                 // EP open.
                             CPU_INT08U ep_addr,
                             CPU_INT08U ep_type,
                             CPU_INT16U max_pkt_size,
                             CPU_INT08U transaction_frame,
                             RTOS_ERR   *p_err);

  void       (*EP_Close)   (USBD_DRV   *p_drv,                  // EP close.
                            CPU_INT08U ep_addr);

  CPU_INT32U (*EP_RxStart) (USBD_DRV   *p_drv,                  // EP receive start.
                            CPU_INT08U ep_addr,
                            CPU_INT08U *p_buf,
                            CPU_INT32U buf_len,
                            RTOS_ERR   *p_err);

  CPU_INT32U (*EP_Rx)      (USBD_DRV   *p_drv,                  // EP receive/read data.
                            CPU_INT08U ep_addr,
                            CPU_INT08U *p_buf,
                            CPU_INT32U buf_len,
                            RTOS_ERR   *p_err);

  void (*EP_RxZLP)   (USBD_DRV   *p_drv,                        // EP receive zero-length packet.
                      CPU_INT08U ep_addr,
                      RTOS_ERR   *p_err);

  CPU_INT32U (*EP_Tx)      (USBD_DRV   *p_drv,                  // EP transmit/write data.
                            CPU_INT08U ep_addr,
                            CPU_INT08U *p_buf,
                            CPU_INT32U buf_len,
                            RTOS_ERR   *p_err);

  void (*EP_TxStart) (USBD_DRV   *p_drv,                        // EP transmit start.
                      CPU_INT08U ep_addr,
                      CPU_INT08U *p_buf,
                      CPU_INT32U buf_len,
                      RTOS_ERR   *p_err);

  void (*EP_TxZLP)   (USBD_DRV   *p_drv,                        // EP transmit zero-length packet.
                      CPU_INT08U ep_addr,
                      RTOS_ERR   *p_err);

  CPU_BOOLEAN (*EP_Abort)   (USBD_DRV   *p_drv,                 // EP abort.
                             CPU_INT08U ep_addr);

  CPU_BOOLEAN (*EP_Stall)   (USBD_DRV    *p_drv,                // EP stall.
                             CPU_INT08U  ep_addr,
                             CPU_BOOLEAN state);

  void (*ISR_Handler)(USBD_DRV *p_drv);                         // ISR handler.
};

/********************************************************************************************************
 *                                   USB DEVICE HARDWARE INFO STRUCT
 *
 * Notes (1) : The content of the structures to which the fields point to MUST be persistent.
 *******************************************************************************************************/

typedef struct usbd_dev_hw_info {
  USBD_DRV_API     *DrvAPI_Ptr;                                 // Pointer to driver API structure.
  USBD_DRV_INFO    *DrvInfoPtr;                                 // Pointer to struct containing hardware info.
  USBD_DRV_BSP_API *BSP_API_Ptr;                                // Pointer to BSP API structure.
} USBD_DEV_CTRLR_HW_INFO;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   DEVICE TASK DEFAULT CONFIGURATION
 *
 * Note(s) : (1) This is the default configurations for a device task. You can pass DEF_NULL to the
 *               p_task_cfg argument of USBD_DevAdd() to use the default configurations or pass this
 *               structure. If you need to modify some field, you can also create a RTOS_TASK_CFG and copy
 *               this structure to your structure and modify the fields as needed.
 *******************************************************************************************************/

extern const RTOS_TASK_CFG USBD_DevTaskCfgDflt;

extern const USBD_INIT_CFG USBD_InitCfgDflt;                    // USBD dflt configurations.

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

#define  USBD_FRAME_NBR_GET(frame_nbr)                      ((frame_nbr) & USBD_FRAME_NBR_MASK)

#define  USBD_MICROFRAME_NBR_GET(microframe_nbr)            ((microframe_nbr) >> 11u)

//                                                                 See Note #1.
#define  USBD_FRAME_NBR_DIFF_GET(frame_nbr1, frame_nbr2)    (((frame_nbr2) >= (frame_nbr1))  \
                                                             ? ((frame_nbr2) - (frame_nbr1)) \
                                                             : ((USBD_MAX_FRAME_NBR + 1u + (frame_nbr2)) - (frame_nbr1)))

/********************************************************************************************************
 *                                       ENDPOINT CONVERSION MACROS
 *******************************************************************************************************/

#define  USBD_EP_PHY_TO_ADDR(ep_phy_nbr)                 ( ((ep_phy_nbr) / 2u) \
                                                           | (((ep_phy_nbr) % 2u) ? USBD_EP_DIR_BIT : DEF_BIT_NONE))

#define  USBD_EP_ADDR_TO_PHY(ep_addr)                    ((((ep_addr) & USBD_EP_NBR_MASK) *  2u) \
                                                          + ((((ep_addr) & USBD_EP_DIR_MASK) != 0u) ? 1u : 0u))

#define  USBD_EP_ADDR_TO_LOG(ep_addr)                    (CPU_INT08U)((ep_addr) & USBD_EP_NBR_MASK)

#define  USBD_EP_PHY_TO_LOG(ep_phy_nbr)                  (ep_phy_nbr / 2u)

#define  USBD_EP_LOG_TO_ADDR_IN(ep_log_nbr)              ((ep_log_nbr) | USBD_EP_DIR_IN)

#define  USBD_EP_LOG_TO_ADDR_OUT(ep_log_nbr)             ((ep_log_nbr) | USBD_EP_DIR_OUT)

#define  USBD_EP_IS_IN(ep_addr)                          ((((ep_addr) & USBD_EP_DIR_MASK) != 0u) ? DEF_YES : DEF_NO)

/********************************************************************************************************
 *                                               DEBUG STATS
 *******************************************************************************************************/

#if (USBD_CFG_DBG_STATS_EN == DEF_ENABLED)
#ifdef  USBD_CFG_DBG_STATS_CNT_TYPE                             // Adjust size of the stats cntrs.
typedef USBD_CFG_DBG_STATS_CNT_TYPE USBD_DBG_STATS_CNT;
#else
typedef CPU_INT08U USBD_DBG_STATS_CNT;
#endif

typedef struct usbd_dev_stats {                                 // ------------------- DEVICE STATS -------------------
  CPU_INT08U         DevNbr;                                    // Dev nbr associated with stat struct.

  USBD_DBG_STATS_CNT DevResetEventNbr;                          ///< Nbr of reset   events.
  USBD_DBG_STATS_CNT DevSuspendEventNbr;                        ///< Nbr of suspend events.
  USBD_DBG_STATS_CNT DevResumeEventNbr;                         ///< Nbr of resume  events.
  USBD_DBG_STATS_CNT DevConnEventNbr;                           ///< Nbr of conn    events.
  USBD_DBG_STATS_CNT DevDisconnEventNbr;                        ///< Nbr of disconn events.
  USBD_DBG_STATS_CNT DevSetupEventNbr;                          ///< Nbr of setup   events.

  USBD_DBG_STATS_CNT StdReqDevNbr;                              ///< Nbr of         std req with a recipient of 'dev'.
  USBD_DBG_STATS_CNT StdReqDevStallNbr;                         ///< Nbr of stalled std req with a recipient of 'dev'.
  USBD_DBG_STATS_CNT StdReqIF_Nbr;                              ///< Nbr of         std req with a recipient of 'IF'.
  USBD_DBG_STATS_CNT StdReqIF_StallNbr;                         ///< Nbr of stalled std req with a recipient of 'IF'.
  USBD_DBG_STATS_CNT StdReqEP_Nbr;                              ///< Nbr of         std req with a recipient of 'EP'.
  USBD_DBG_STATS_CNT StdReqEP_StallNbr;                         ///< Nbr of stalled std req with a recipient of 'EP'.
  USBD_DBG_STATS_CNT StdReqClassNbr;                            ///< Nbr of         std req with a recipient of 'class'.
  USBD_DBG_STATS_CNT StdReqClassStallNbr;                       ///< Nbr of stalled std req with a recipient of 'class'.

  USBD_DBG_STATS_CNT StdReqSetAddrNbr;                          ///< Nbr of SET_ADDRESS       std req.
  USBD_DBG_STATS_CNT StdReqSetCfgNbr;                           ///< Nbr of SET_CONFIGURATION std req.

  USBD_DBG_STATS_CNT CtrlRxSyncExecNbr;                         ///< Nbr of sync ctrl rx exec'd.
  USBD_DBG_STATS_CNT CtrlRxSyncSuccessNbr;                      ///< Nbr of sync ctrl rx exec'd successfully.
  USBD_DBG_STATS_CNT CtrlTxSyncExecNbr;                         ///< Nbr of sync ctrl tx exec'd.
  USBD_DBG_STATS_CNT CtrlTxSyncSuccessNbr;                      ///< Nbr of sync ctrl tx exec'd successfully.
  USBD_DBG_STATS_CNT CtrlRxStatusExecNbr;                       ///< Nbr of sync ctrl rx status exec'd.
  USBD_DBG_STATS_CNT CtrlRxStatusSuccessNbr;                    ///< Nbr of sync ctrl rx status exec'd successfully.
  USBD_DBG_STATS_CNT CtrlTxStatusExecNbr;                       ///< Nbr of sync ctrl tx status exec'd.
  USBD_DBG_STATS_CNT CtrlTxStatusSuccessNbr;                    ///< Nbr of sync ctrl tx status exec'd successfully.

  USBD_DBG_STATS_CNT BulkRxSyncExecNbr;                         ///< Nbr of sync  bulk rx exec'd.
  USBD_DBG_STATS_CNT BulkRxSyncSuccessNbr;                      ///< Nbr of sync  bulk rx exec'd successfully.
  USBD_DBG_STATS_CNT BulkRxAsyncExecNbr;                        ///< Nbr of async bulk rx exec'd.
  USBD_DBG_STATS_CNT BulkRxAsyncSuccessNbr;                     ///< Nbr of async bulk rx exec'd successfully.
  USBD_DBG_STATS_CNT BulkTxSyncExecNbr;                         ///< Nbr of sync  bulk tx exec'd.
  USBD_DBG_STATS_CNT BulkTxSyncSuccessNbr;                      ///< Nbr of sync  bulk tx exec'd successfully.
  USBD_DBG_STATS_CNT BulkTxAsyncExecNbr;                        ///< Nbr of async bulk tx exec'd.
  USBD_DBG_STATS_CNT BulkTxAsyncSuccessNbr;                     ///< Nbr of async bulk tx exec'd successfully.

  USBD_DBG_STATS_CNT IntrRxSyncExecNbr;                         ///< Nbr of sync  intr rx exec'd.
  USBD_DBG_STATS_CNT IntrRxSyncSuccessNbr;                      ///< Nbr of sync  intr rx exec'd successfully.
  USBD_DBG_STATS_CNT IntrRxAsyncExecNbr;                        ///< Nbr of async intr rx exec'd.
  USBD_DBG_STATS_CNT IntrRxAsyncSuccessNbr;                     ///< Nbr of async intr rx exec'd successfully.
  USBD_DBG_STATS_CNT IntrTxSyncExecNbr;                         ///< Nbr of sync  intr tx exec'd.
  USBD_DBG_STATS_CNT IntrTxSyncSuccessNbr;                      ///< Nbr of sync  intr tx exec'd successfully.
  USBD_DBG_STATS_CNT IntrTxAsyncExecNbr;                        ///< Nbr of async intr tx exec'd.
  USBD_DBG_STATS_CNT IntrTxAsyncSuccessNbr;                     ///< Nbr of async intr tx exec'd successfully.

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
  USBD_DBG_STATS_CNT IsocRxAsyncExecNbr;                        ///< Nbr of async isoc rx exec'd.
  USBD_DBG_STATS_CNT IsocRxAsyncSuccessNbr;                     ///< Nbr of async isoc rx exec'd successfully.
  USBD_DBG_STATS_CNT IsocTxAsyncExecNbr;                        ///< Nbr of async isoc tx exec'd.
  USBD_DBG_STATS_CNT IsocTxAsyncSuccessNbr;                     ///< Nbr of async isoc tx exec'd successfully.
#endif
} USBD_DBG_STATS_DEV;

typedef struct usbd_ep_stats {                                  // --------------------- EP STATS ---------------------
  CPU_INT08U         Addr;                                      ///< EP address.

  USBD_DBG_STATS_CNT EP_OpenNbr;                                ///< Nbr of times this EP addr has been opened.
  USBD_DBG_STATS_CNT EP_AbortExecNbr;                           ///< Nbr of times EP has been aborted.
  USBD_DBG_STATS_CNT EP_AbortSuccessNbr;                        ///< Nbr of times EP has been aborted successfully.
  USBD_DBG_STATS_CNT EP_CloseExecNbr;                           ///< Nbr of times EP has been closed.
  USBD_DBG_STATS_CNT EP_CloseSuccessNbr;                        ///< Nbr of times EP has been closed successfully.

  USBD_DBG_STATS_CNT RxSyncExecNbr;                             ///< Nbr of sync  rx exec'd.
  USBD_DBG_STATS_CNT RxSyncSuccessNbr;                          ///< Nbr of sync  rx exec'd successfully.
  USBD_DBG_STATS_CNT RxSyncTimeoutErrNbr;                       ///< Nbr of sync  rx that timed-out.
  USBD_DBG_STATS_CNT RxAsyncExecNbr;                            ///< Nbr of async rx exec'd.
  USBD_DBG_STATS_CNT RxAsyncSuccessNbr;                         ///< Nbr of async rx exec'd successfully.
  USBD_DBG_STATS_CNT RxZLP_ExecNbr;                             ///< Nbr of sync  rx ZLP exec'd.
  USBD_DBG_STATS_CNT RxZLP_SuccessNbr;                          ///< Nbr of sync  rx ZLP exec'd successfully.

  USBD_DBG_STATS_CNT TxSyncExecNbr;                             ///< Nbr of sync  tx exec'd.
  USBD_DBG_STATS_CNT TxSyncSuccessNbr;                          ///< Nbr of sync  tx exec'd successfully.
  USBD_DBG_STATS_CNT TxSyncTimeoutErrNbr;                       ///< Nbr of sync  tx that timed-out.
  USBD_DBG_STATS_CNT TxAsyncExecNbr;                            ///< Nbr of async tx exec'd.
  USBD_DBG_STATS_CNT TxAsyncSuccessNbr;                         ///< Nbr of async tx exec'd successfully.
  USBD_DBG_STATS_CNT TxZLP_ExecNbr;                             ///< Nbr of sync  tx ZLP exec'd.
  USBD_DBG_STATS_CNT TxZLP_SuccessNbr;                          ///< Nbr of sync  tx ZLP exec'd successfully.

  USBD_DBG_STATS_CNT DrvRxStartNbr;                             ///< Nbr of            call to drv's RxStart().
  USBD_DBG_STATS_CNT DrvRxStartSuccessNbr;                      ///< Nbr of successful call to drv's RxStart().
  USBD_DBG_STATS_CNT DrvRxNbr;                                  ///< Nbr of            call to drv's Rx().
  USBD_DBG_STATS_CNT DrvRxSuccessNbr;                           ///< Nbr of successful call to drv's Rx().
  USBD_DBG_STATS_CNT DrvRxZLP_Nbr;                              ///< Nbr of            call to drv's RxZLP().
  USBD_DBG_STATS_CNT DrvRxZLP_SuccessNbr;                       ///< Nbr of successful call to drv's RxZLP().
  USBD_DBG_STATS_CNT RxCmplNbr;                                 ///< Nbr of            call to RxCmpl().
  USBD_DBG_STATS_CNT RxCmplErrNbr;                              ///< Nbr of successful call to RxCmpl().

  USBD_DBG_STATS_CNT DrvTxNbr;                                  ///< Nbr of            call to drv's Tx().
  USBD_DBG_STATS_CNT DrvTxSuccessNbr;                           ///< Nbr of successful call to drv's Tx().
  USBD_DBG_STATS_CNT DrvTxStartNbr;                             ///< Nbr of            call to drv's TxStart().
  USBD_DBG_STATS_CNT DrvTxStartSuccessNbr;                      ///< Nbr of successful call to drv's TxStart().
  USBD_DBG_STATS_CNT DrvTxZLP_Nbr;                              ///< Nbr of            call to drv's TxZLP().
  USBD_DBG_STATS_CNT DrvTxZLP_SuccessNbr;                       ///< Nbr of successful call to drv's TxZLP().
  USBD_DBG_STATS_CNT TxCmplNbr;                                 ///< Nbr of            call to TxCmpl().
  USBD_DBG_STATS_CNT TxCmplErrNbr;                              ///< Nbr of successful call to TxCmpl().
} USBD_DBG_STATS_EP;

USBD_CORE_EXT USBD_DBG_STATS_DEV USBD_DbgStatsDevTbl[4u];
USBD_CORE_EXT USBD_DBG_STATS_EP  USBD_DbgStatsEP_Tbl[4u][32u];

#define  USBD_DBG_STATS_DEV_RESET(dev_nbr)                          { \
    Mem_Clr((void *)&USBD_DbgStatsDevTbl[dev_nbr],                    \
            (CPU_SIZE_T) sizeof(USBD_DBG_STATS_DEV));                 \
}
#define  USBD_DBG_STATS_DEV_SET_DEV_NBR(dev_nbr)                    { \
    USBD_DbgStatsDevTbl[dev_nbr].DevNbr = dev_nbr;                    \
}
#define  USBD_DBG_STATS_DEV_SET(dev_nbr, stat, val)                 { \
    USBD_DbgStatsDevTbl[dev_nbr].stat = val;                          \
}
#define  USBD_DBG_STATS_DEV_GET(dev_nbr, stat)                      { \
    USBD_DbgStatsDevTbl[dev_nbr].stat;                                \
}
#define  USBD_DBG_STATS_DEV_INC(dev_nbr, stat)                      { \
    USBD_DbgStatsDevTbl[dev_nbr].stat++;                              \
}
#define  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, stat, bool)        { \
    if (bool == DEF_TRUE) {                                           \
      USBD_DBG_STATS_DEV_INC(dev_nbr, stat);                          \
    }                                                                 \
}

#define  USBD_DBG_STATS_EP_RESET(dev_nbr, ep_ix)                    { \
    Mem_Clr((void *)&USBD_DbgStatsEP_Tbl[dev_nbr][ep_ix],             \
            (CPU_SIZE_T) sizeof(USBD_DBG_STATS_EP));                  \
}
#define  USBD_DBG_STATS_EP_SET_ADDR(dev_nbr, ep_ix, addr)           { \
    USBD_DbgStatsEP_Tbl[dev_nbr][ep_ix].Addr = addr;                  \
}
#define  USBD_DBG_STATS_EP_SET(dev_nbr, ep_ix, stat, val)           { \
    USBD_DbgStatsEP_Tbl[dev_nbr][ep_ix].stat = val;                   \
}
#define  USBD_DBG_STATS_EP_GET(dev_nbr, ep_ix, stat)                    USBD_DbgStatsEP_Tbl[dev_nbr][ep_ix].stat

#define  USBD_DBG_STATS_EP_INC(dev_nbr, ep_ix, stat)                { \
    USBD_DbgStatsEP_Tbl[dev_nbr][ep_ix].stat++;                       \
}
#define  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, ep_ix, stat, bool)  { \
    if (bool == DEF_TRUE) {                                           \
      USBD_DBG_STATS_EP_INC(dev_nbr, ep_ix, stat);                    \
    }                                                                 \
}
#else
#define  USBD_DBG_STATS_DEV_RESET(dev_nbr)
#define  USBD_DBG_STATS_DEV_SET_DEV_NBR(dev_nbr)
#define  USBD_DBG_STATS_DEV_SET(dev_nbr, stat, val)
#define  USBD_DBG_STATS_DEV_GET(dev_nbr, stat)
#define  USBD_DBG_STATS_DEV_INC(dev_nbr, stat)
#define  USBD_DBG_STATS_DEV_INC_IF_TRUE(dev_nbr, stat, bool)

#define  USBD_DBG_STATS_EP_RESET(dev_nbr, ep_ix)
#define  USBD_DBG_STATS_EP_SET_ADDR(dev_nbr, ep_ix, addr)
#define  USBD_DBG_STATS_EP_SET(dev_nbr, ep_ix, stat, val)
#define  USBD_DBG_STATS_EP_GET(dev_nbr, ep_ix, stat)
#define  USBD_DBG_STATS_EP_INC(dev_nbr, ep_ix, stat)
#define  USBD_DBG_STATS_EP_INC_IF_TRUE(dev_nbr, ep_ix, stat, bool)
#endif

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

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void USBD_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBD_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                          MEM_SEG *p_mem_seg_buf);
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
 *                   The 'USBD_EP_CtrlXXXX()' functions perform operations in the default endpoint.
 *                   Class drivers should use 'USBD_EP_CtrlXXXX()' to send/receive class specific requests.
 *******************************************************************************************************/

void USBD_Init(USBD_QTY_CFG *p_qty_cfg,
               RTOS_ERR     *p_err);

void USBD_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                           RTOS_ERR   *p_err);

//                                                                 ----------------- DEVICE OPERATIONS ----------------
CPU_INT08U USBD_DevAdd(const CPU_CHAR         *name,
                       const RTOS_TASK_CFG    *p_task_cfg,
                       const USBD_DEV_CFG     *p_dev_cfg,
                       const USBD_DEV_DRV_CFG *p_dev_drv_cfg,
                       USBD_BUS_FNCTS         *p_bus_fnct,
                       RTOS_ERR               *p_err);

void USBD_DevTaskPrioSet(CPU_INT08U     dev_nbr,
                         RTOS_TASK_PRIO prio,
                         RTOS_ERR       *p_err);

void USBD_DevStart(CPU_INT08U dev_nbr,
                   RTOS_ERR   *p_err);

void USBD_DevStop(CPU_INT08U dev_nbr,
                  RTOS_ERR   *p_err);

USBD_DEV_STATE USBD_DevStateGet(CPU_INT08U dev_nbr,
                                RTOS_ERR   *p_err);

USBD_DEV_SPD USBD_DevSpdGet(CPU_INT08U dev_nbr,
                            RTOS_ERR   *p_err);

void USBD_DevSelfPwrSet(CPU_INT08U  dev_nbr,
                        CPU_BOOLEAN self_pwr,
                        RTOS_ERR    *p_err);

void USBD_DevSetMS_VendorCode(CPU_INT08U dev_nbr,
                              CPU_INT08U vendor_code,
                              RTOS_ERR   *p_err);

USBD_DEV_CFG *USBD_DevCfgGet(CPU_INT08U dev_nbr,
                             RTOS_ERR   *p_err);
#if 0
void USBD_DevWaitForCfg(CPU_INT08U dev_nbr,
                        CPU_INT16U retry_max,
                        CPU_INT32U time_dly_ms,
                        RTOS_ERR   *p_err);
#endif

CPU_INT08U USBD_DevNbrGetFromName(const CPU_CHAR *name);

CPU_INT16U USBD_DevFrameNbrGet(CPU_INT08U dev_nbr,
                               RTOS_ERR   *p_err);

//                                                                 ------------- CONFIGUARTION OPERATIONS -------------
CPU_INT08U USBD_ConfigAdd(CPU_INT08U     dev_nbr,
                          CPU_INT08U     attrib,
                          CPU_INT16U     max_pwr,
                          USBD_DEV_SPD   spd,
                          const CPU_CHAR *p_name,
                          RTOS_ERR       *p_err);

#if (USBD_CFG_HS_EN == DEF_ENABLED)
void USBD_ConfigOtherSpeed(CPU_INT08U dev_nbr,
                           CPU_INT08U cfg_nbr,
                           CPU_INT08U cfg_other,
                           RTOS_ERR   *p_err);
#endif

//                                                                 --------------- INTERFACE OPERATIONS ---------------
CPU_INT08U USBD_IF_Add(CPU_INT08U     dev_nbr,
                       CPU_INT08U     cfg_nbr,
                       USBD_CLASS_DRV *p_class_drv,
                       void           *p_if_class_arg,
                       void           *p_if_alt_class_arg,
                       CPU_INT08U     class_code,
                       CPU_INT08U     class_sub_code,
                       CPU_INT08U     class_protocol_code,
                       const CPU_CHAR *p_name,
                       RTOS_ERR       *p_err);

CPU_INT08U USBD_IF_AltAdd(CPU_INT08U     dev_nbr,
                          CPU_INT08U     config_nbr,
                          CPU_INT08U     if_nbr,
                          void           *p_class_arg,
                          const CPU_CHAR *p_name,
                          RTOS_ERR       *p_err);

CPU_INT08U USBD_IF_Grp(CPU_INT08U     dev_nbr,
                       CPU_INT08U     config_nbr,
                       CPU_INT08U     class_code,
                       CPU_INT08U     class_sub_code,
                       CPU_INT08U     class_protocol_code,
                       CPU_INT08U     if_start,
                       CPU_INT08U     if_cnt,
                       const CPU_CHAR *p_name,
                       RTOS_ERR       *p_err);

CPU_INT08U USBD_DescDevGet(USBD_DRV   *p_drv,
                           CPU_INT08U *p_buf,
                           CPU_INT08U max_len,
                           RTOS_ERR   *p_err);

CPU_INT16U USBD_DescConfigGet(USBD_DRV   *p_drv,
                              CPU_INT08U *p_buf,
                              CPU_INT16U max_len,
                              CPU_INT08U config_ix,
                              RTOS_ERR   *p_err);

CPU_INT08U USBD_DescStrGet(USBD_DRV   *p_drv,
                           CPU_INT08U *p_buf,
                           CPU_INT08U max_len,
                           CPU_INT08U str_ix,
                           RTOS_ERR   *p_err);
//                                                                 ---------------- STRING OPERATIONS -----------------
#if (USBD_CFG_STR_EN == DEF_ENABLED)
void USBD_StrAdd(CPU_INT08U     dev_nbr,
                 const CPU_CHAR *p_str,
                 RTOS_ERR       *p_err);

CPU_INT08U USBD_StrIxGet(CPU_INT08U     dev_nbr,
                         const CPU_CHAR *p_str);
#endif

//                                                                 ----------- DESCRIPTOR BUFFER OPERATIONS -----------
void USBD_DescWr08(CPU_INT08U dev_nbr,
                   CPU_INT08U val);

void USBD_DescWr16(CPU_INT08U dev_nbr,
                   CPU_INT16U val);

void USBD_DescWr24(CPU_INT08U dev_nbr,
                   CPU_INT32U val);

void USBD_DescWr32(CPU_INT08U dev_nbr,
                   CPU_INT32U val);

void USBD_DescWr(CPU_INT08U       dev_nbr,
                 const CPU_INT08U *p_buf,
                 CPU_INT16U       len);

//                                                                 ---------------- ENDPOINT OPERATIONS ---------------
CPU_INT32U USBD_CtrlTx(CPU_INT08U  dev_nbr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err);

CPU_INT32U USBD_CtrlRx(CPU_INT08U dev_nbr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err);

//                                                                 -------------- BULK TRANFER FUNCTIONS --------------
CPU_INT08U USBD_BulkAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT16U  max_pkt_len,
                        RTOS_ERR    *p_err);

CPU_INT32U USBD_BulkRx(CPU_INT08U dev_nbr,
                       CPU_INT08U ep_addr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err);

void USBD_BulkRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err);

CPU_INT32U USBD_BulkTx(CPU_INT08U  dev_nbr,
                       CPU_INT08U  ep_addr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err);

void USBD_BulkTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      CPU_BOOLEAN     end,
                      RTOS_ERR        *p_err);

//                                                                 ------------ INTERRUPT TRANFER FUNCTIONS -----------
CPU_INT08U USBD_IntrAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT16U  max_pkt_len,
                        CPU_INT16U  interval,
                        RTOS_ERR    *p_err);

CPU_INT32U USBD_IntrRx(CPU_INT08U dev_nbr,
                       CPU_INT08U ep_addr,
                       void       *p_buf,
                       CPU_INT32U buf_len,
                       CPU_INT16U timeout_ms,
                       RTOS_ERR   *p_err);

void USBD_IntrRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err);

CPU_INT32U USBD_IntrTx(CPU_INT08U  dev_nbr,
                       CPU_INT08U  ep_addr,
                       void        *p_buf,
                       CPU_INT32U  buf_len,
                       CPU_INT16U  timeout_ms,
                       CPU_BOOLEAN end,
                       RTOS_ERR    *p_err);

void USBD_IntrTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      CPU_BOOLEAN     end,
                      RTOS_ERR        *p_err);

#if (USBD_CFG_EP_ISOC_EN == DEF_ENABLED)
//                                                                 ----------- ISOCHRONOUS TRANFER FUNCTIONS ----------
CPU_INT08U USBD_IsocAdd(CPU_INT08U  dev_nbr,
                        CPU_INT08U  config_nbr,
                        CPU_INT08U  if_nbr,
                        CPU_INT08U  if_alt_nbr,
                        CPU_BOOLEAN dir_in,
                        CPU_INT08U  attrib,
                        CPU_INT16U  max_pkt_len,
                        CPU_INT08U  transaction_frame,
                        CPU_INT16U  interval,
                        RTOS_ERR    *p_err);

void USBD_IsocRxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err);

void USBD_IsocTxAsync(CPU_INT08U      dev_nbr,
                      CPU_INT08U      ep_addr,
                      void            *p_buf,
                      CPU_INT32U      buf_len,
                      USBD_ASYNC_FNCT async_fnct,
                      void            *p_async_arg,
                      RTOS_ERR        *p_err);

void USBD_IsocSyncRefreshSet(CPU_INT08U dev_nbr,
                             CPU_INT08U config_nbr,
                             CPU_INT08U if_nbr,
                             CPU_INT08U if_alt_nbr,
                             CPU_INT08U synch_ep_addr,
                             CPU_INT08U sync_refresh,
                             RTOS_ERR   *p_err);

void USBD_IsocSyncAddrSet(CPU_INT08U dev_nbr,
                          CPU_INT08U config_nbr,
                          CPU_INT08U if_nbr,
                          CPU_INT08U if_alt_nbr,
                          CPU_INT08U data_ep_addr,
                          CPU_INT08U sync_addr,
                          RTOS_ERR   *p_err);
#endif

//                                                                 ------------ STANDARD ENDPOINT FUNCTIONS -----------
void USBD_EP_TxZLP(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   CPU_INT16U timeout_ms,
                   RTOS_ERR   *p_err);

void USBD_EP_RxZLP(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   CPU_INT16U timeout_ms,
                   RTOS_ERR   *p_err);

void USBD_EP_Abort(CPU_INT08U dev_nbr,
                   CPU_INT08U ep_addr,
                   RTOS_ERR   *p_err);

void USBD_EP_RxCmpl(USBD_DRV   *p_drv,
                    CPU_INT08U ep_log_nbr);

void USBD_EP_TxCmpl(USBD_DRV   *p_drv,
                    CPU_INT08U ep_log_nbr);

void USBD_EP_TxCmplExt(USBD_DRV   *p_drv,
                       CPU_INT08U ep_log_nbr,
                       RTOS_ERR   xfer_err);

void USBD_EP_Stall(CPU_INT08U  dev_nbr,
                   CPU_INT08U  ep_addr,
                   CPU_BOOLEAN state,
                   RTOS_ERR    *p_err);

CPU_BOOLEAN USBD_EP_IsStalled(CPU_INT08U dev_nbr,
                              CPU_INT08U ep_addr,
                              RTOS_ERR   *p_err);

CPU_INT16U USBD_EP_MaxPktSizeGet(CPU_INT08U dev_nbr,
                                 CPU_INT08U ep_addr,
                                 RTOS_ERR   *p_err);

CPU_INT08U USBD_EP_MaxPhyNbrGet(CPU_INT08U dev_nbr);

CPU_INT08U USBD_EP_MaxNbrOpenGet(CPU_INT08U dev_nbr);

//                                                                 -------------- DEVICE DRIVER CALLBACKS -------------
void USBD_EventConn(USBD_DRV *p_drv);

void USBD_EventDisconn(USBD_DRV *p_drv);

void USBD_EventHS(USBD_DRV *p_drv);

void USBD_EventReset(USBD_DRV *p_drv);

void USBD_EventSuspend(USBD_DRV *p_drv);

void USBD_EventResume(USBD_DRV *p_drv);

void USBD_EventSetup(USBD_DRV *p_drv,
                     void     *p_buf);

//                                                                 ------------------- OS FUNCTIONS -------------------
void USBD_OS_DlyMs(CPU_INT32U ms);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  USBD_CFG_STR_EN
#error  "USBD_CFG_STR_EN not #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"

#elif  ((USBD_CFG_STR_EN != DEF_DISABLED) \
  && (USBD_CFG_STR_EN != DEF_ENABLED))
#error  "USBD_CFG_STR_EN illegally #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

#ifndef  USBD_CFG_URB_EXTRA_EN
#error  "USBD_CFG_URB_EXTRA_EN not #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"

#elif  ((USBD_CFG_URB_EXTRA_EN != DEF_DISABLED) \
  && (USBD_CFG_URB_EXTRA_EN != DEF_ENABLED))
#error  "USBD_CFG_URB_EXTRA_EN illegally #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"
#endif

#ifndef  USBD_CFG_MS_OS_DESC_EN
#error  "USBD_CFG_MS_OS_DESC_EN not #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"

#elif  ((USBD_CFG_MS_OS_DESC_EN != DEF_DISABLED) \
  && (USBD_CFG_MS_OS_DESC_EN != DEF_ENABLED))
#error  "USBD_CFG_MS_OS_DESC_EN illegally #define'd in 'usbd_cfg.h' [MUST be DEF_DISABLED || DEF_ENABLED]"

#elif  ((USBD_CFG_MS_OS_DESC_EN == DEF_ENABLED) \
  && (USBD_CFG_STR_EN == DEF_DISABLED))
#error  "USBD_CFG_STR_EN illegally #define'd in 'usbd_cfg.h'. MUST be DEF_ENABLED when USBD_CFG_MS_OS_DESC_EN is DEF_ENABLED"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
