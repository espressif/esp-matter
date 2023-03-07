/***************************************************************************//**
 * @file
 * @brief USB Device HID Class
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
 * @defgroup USBD_HID USB Device HID API
 * @ingroup USBD
 * @brief   USB Device HID API
 *
 * @addtogroup USBD_HID
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  SL_USBD_CLASS_HID_H
#define  SL_USBD_CLASS_HID_H

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "sl_enum.h"

#include "sl_usbd_core.h"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               EXTERNS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  SLI_USBD_HID_MODULE
#define  SLI_USBD_HID_EXTERN
#else
#define  SLI_USBD_HID_EXTERN  extern
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                          HUMAN INTERFACE DEVICE COUNTRY CODES DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.1 for more details about HID descriptor country code.
 *
 *               (a) The country code identifies which country the hardware is localized for. Most
 *                   hardware is not localized and thus this value would be zero (0). However, keyboards
 *                   may use the field to indicate the language of the key caps.
 *******************************************************************************************************/

SL_ENUM(sl_usbd_hid_country_code_t) {
  SL_USBD_HID_COUNTRY_CODE_NOT_SUPPORTED = 0u,                     // See Note #1a.
  SL_USBD_HID_COUNTRY_CODE_ARABIC = 1u,
  SL_USBD_HID_COUNTRY_CODE_BELGIAN = 2u,
  SL_USBD_HID_COUNTRY_CODE_CANADIAN_BILINGUAL = 3u,
  SL_USBD_HID_COUNTRY_CODE_CANADIAN_FRENCH = 4u,
  SL_USBD_HID_COUNTRY_CODE_CZECH_REPUBLIC = 5u,
  SL_USBD_HID_COUNTRY_CODE_DANISH = 6u,
  SL_USBD_HID_COUNTRY_CODE_FINNISH = 7u,
  SL_USBD_HID_COUNTRY_CODE_FRENCH = 8u,
  SL_USBD_HID_COUNTRY_CODE_GERMAN = 9u,
  SL_USBD_HID_COUNTRY_CODE_GREEK = 10u,
  SL_USBD_HID_COUNTRY_CODE_HEBREW = 11u,
  SL_USBD_HID_COUNTRY_CODE_HUNGARY = 12u,
  SL_USBD_HID_COUNTRY_CODE_INTERNATIONAL = 13u,
  SL_USBD_HID_COUNTRY_CODE_ITALIAN = 14u,
  SL_USBD_HID_COUNTRY_CODE_JAPAN_KATAKANA = 15u,
  SL_USBD_HID_COUNTRY_CODE_KOREAN = 16u,
  SL_USBD_HID_COUNTRY_CODE_LATIN_AMERICAN = 17u,
  SL_USBD_HID_COUNTRY_CODE_NETHERLANDS_DUTCH = 18u,
  SL_USBD_HID_COUNTRY_CODE_NORWEGIAN = 19u,
  SL_USBD_HID_COUNTRY_CODE_PERSIAN_FARSI = 20u,
  SL_USBD_HID_COUNTRY_CODE_POLAND = 21u,
  SL_USBD_HID_COUNTRY_CODE_PORTUGUESE = 22u,
  SL_USBD_HID_COUNTRY_CODE_RUSSIA = 23u,
  SL_USBD_HID_COUNTRY_CODE_SLOVAKIA = 24u,
  SL_USBD_HID_COUNTRY_CODE_SPANISH = 25u,
  SL_USBD_HID_COUNTRY_CODE_SWEDISH = 26u,
  SL_USBD_HID_COUNTRY_CODE_SWISS_FRENCH = 27u,
  SL_USBD_HID_COUNTRY_CODE_SWISS_GERMAN = 28u,
  SL_USBD_HID_COUNTRY_CODE_SWITZERLAND = 29u,
  SL_USBD_HID_COUNTRY_CODE_TAIWAN = 30u,
  SL_USBD_HID_COUNTRY_CODE_TURKISH_Q = 31u,
  SL_USBD_HID_COUNTRY_CODE_UK = 32u,
  SL_USBD_HID_COUNTRY_CODE_US = 33u,
  SL_USBD_HID_COUNTRY_CODE_YUGOSLAVIA = 34u,
  SL_USBD_HID_COUNTRY_CODE_TURKISH_F = 35u
};

/********************************************************************************************************
 *                           HUMAN INTERFACE DEVICE CLASS SUBCLASS CODES DEFINES
 *
 * Note(s) : (1) Human interface device class subclass codes are defined in section 4.2 of HID
 *               specification revision 1.11.
 *******************************************************************************************************/

#define  SL_USBD_HID_SUBCLASS_NONE                        0x00u    // No subclass.
#define  SL_USBD_HID_SUBCLASS_BOOT                        0x01u    // Boot interface.

/********************************************************************************************************
 *                           HUMAN INTERFACE DEVICE CLASS PROTOCOL CODES DEFINES
 *
 * Note(s) : (1) Human interface device class protocol codes are defined in section 4.3 of HID
 *               specification revision 1.11.
 *******************************************************************************************************/

#define  SL_USBD_HID_PROTOCOL_NONE                        0x00u    // No class specific protocol.
#define  SL_USBD_HID_PROTOCOL_KBD                         0x01u    // Keyboard protocol.
#define  SL_USBD_HID_PROTOCOL_MOUSE                       0x02u    // Mouse protocol.

/********************************************************************************************************
 *                                          ITEM TYPES
 *******************************************************************************************************/

// HID Report Descriptor
#define  SL_USBD_HID_ITEM_LONG                             0xFE
// Short item types
#define  SL_USBD_HID_ITEM_TYPE_MAIN                        0x00
#define  SL_USBD_HID_ITEM_TYPE_GLOBAL                      0x04
#define  SL_USBD_HID_ITEM_TYPE_LOCAL                       0x08
#define  SL_USBD_HID_ITEM_TYPE_RESERVED                    0x0C
// Main item
#define  SL_USBD_HID_MAIN_INPUT                            0x80
#define  SL_USBD_HID_MAIN_OUTPUT                           0x90
#define  SL_USBD_HID_MAIN_COLLECTION                       0xA0
#define  SL_USBD_HID_MAIN_FEATURE                          0xB0
#define  SL_USBD_HID_MAIN_ENDCOLLECTION                    0xC0
// Input/output/feature item
#define  SL_USBD_HID_MAIN_CONSTANT                         0x01
#define  SL_USBD_HID_MAIN_DATA                             0x00
#define  SL_USBD_HID_MAIN_VARIABLE                         0x02
#define  SL_USBD_HID_MAIN_ARRAY                            0x00
#define  SL_USBD_HID_MAIN_RELATIVE                         0x04
#define  SL_USBD_HID_MAIN_ABSOLUTE                         0x00
#define  SL_USBD_HID_MAIN_WRAP                             0x08
#define  SL_USBD_HID_MAIN_NOWRAP                           0x00
#define  SL_USBD_HID_MAIN_NONLINEAR                        0x10
#define  SL_USBD_HID_MAIN_LINEAR                           0x00
#define  SL_USBD_HID_MAIN_NOPREFERRED                      0x20
#define  SL_USBD_HID_MAIN_PREFERREDSTATE                   0x00
#define  SL_USBD_HID_MAIN_NULLSTATE                        0x40
#define  SL_USBD_HID_MAIN_NONULLPOSITION                   0x00
#define  SL_USBD_HID_MAIN_VOLATILE                         0x80
#define  SL_USBD_HID_MAIN_NONVOLATILE                      0x00
#define  SL_USBD_HID_MAIN_BUFFEREDBYTES                  0x0100
#define  SL_USBD_HID_MAIN_BITFIELD                       0x0000
// Collection item
#define  SL_USBD_HID_COLLECTION_PHYSICAL                   0x00
#define  SL_USBD_HID_COLLECTION_APPLICATION                0x01
#define  SL_USBD_HID_COLLECTION_LOGICAL                    0x02
#define  SL_USBD_HID_COLLECTION_REPORT                     0x03
#define  SL_USBD_HID_COLLECTION_NAMEDARRAY                 0x04
#define  SL_USBD_HID_COLLECTION_USAGESWITCH                0x05
#define  SL_USBD_HID_COLLECTION_USAGEMODIFIER              0x06
// Global item
#define  SL_USBD_HID_GLOBAL_USAGE_PAGE                     0x04
#define  SL_USBD_HID_GLOBAL_LOG_MIN                        0x14
#define  SL_USBD_HID_GLOBAL_LOG_MAX                        0x24
#define  SL_USBD_HID_GLOBAL_PHY_MIN                        0x34
#define  SL_USBD_HID_GLOBAL_PHY_MAX                        0x44
#define  SL_USBD_HID_GLOBAL_UNIT_EXPONENT                  0x54
#define  SL_USBD_HID_GLOBAL_UNIT                           0x64
#define  SL_USBD_HID_GLOBAL_REPORT_SIZE                    0x74
#define  SL_USBD_HID_GLOBAL_REPORT_ID                      0x84
#define  SL_USBD_HID_GLOBAL_REPORT_COUNT                   0x94
#define  SL_USBD_HID_GLOBAL_PUSH                           0xA4
#define  SL_USBD_HID_GLOBAL_POP                            0xB4
// Local item
#define  SL_USBD_HID_LOCAL_USAGE                           0x08
#define  SL_USBD_HID_LOCAL_USAGE_MIN                       0x18
#define  SL_USBD_HID_LOCAL_USAGE_MAX                       0x28
#define  SL_USBD_HID_LOCAL_DESIGNATOR_INDEX                0x38
#define  SL_USBD_HID_LOCAL_DESIGNATOR_MIN                  0x48
#define  SL_USBD_HID_LOCAL_DESIGNATOR_MAX                  0x58
#define  SL_USBD_HID_LOCAL_STRING_INDEX                    0x78
#define  SL_USBD_HID_LOCAL_STRING_MIN                      0x88
#define  SL_USBD_HID_LOCAL_STRING_MAX                      0x98
#define  SL_USBD_HID_LOCAL_DELIMITER                       0xA8

/****************************************************************************************************//**
 *                                   HID PHYSICAL DESCRIPTOR DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices Version 1.11', Section 6.2.3.
 *******************************************************************************************************/

// HID Physical Descriptor (see Note #1)
// Bias values.
#define  SL_USBD_HID_BIAS_NOT_APPLICABLE                      0
#define  SL_USBD_HID_BIAS_RIGHT_HAND                          1
#define  SL_USBD_HID_BIAS_LEFT_HAND                           2
#define  SL_USBD_HID_BIAS_BOTH_HANDS                          3
#define  SL_USBD_HID_BIAS_EITHER_HAND                         4
// Designator values.
#define  SL_USBD_HID_DESIGNATOR_NONE                       0x00
#define  SL_USBD_HID_DESIGNATOR_HAND                       0x01
#define  SL_USBD_HID_DESIGNATOR_EYEBALL                    0x02
#define  SL_USBD_HID_DESIGNATOR_EYEBROW                    0x03
#define  SL_USBD_HID_DESIGNATOR_EYELID                     0x04
#define  SL_USBD_HID_DESIGNATOR_EAR                        0x05
#define  SL_USBD_HID_DESIGNATOR_NOSE                       0x06
#define  SL_USBD_HID_DESIGNATOR_MOUTH                      0x07
#define  SL_USBD_HID_DESIGNATOR_UPPER_LIP                  0x08
#define  SL_USBD_HID_DESIGNATOR_LOWER_LIP                  0x09
#define  SL_USBD_HID_DESIGNATOR_JAW                        0x0A
#define  SL_USBD_HID_DESIGNATOR_NECK                       0x0B
#define  SL_USBD_HID_DESIGNATOR_UPPER_ARM                  0x0C
#define  SL_USBD_HID_DESIGNATOR_ELBOW                      0x0D
#define  SL_USBD_HID_DESIGNATOR_FOREARM                    0x0E
#define  SL_USBD_HID_DESIGNATOR_WRIST                      0x0F
#define  SL_USBD_HID_DESIGNATOR_PALM                       0x10
#define  SL_USBD_HID_DESIGNATOR_THUMB                      0x11
#define  SL_USBD_HID_DESIGNATOR_INDEX_FINGER               0x12
#define  SL_USBD_HID_DESIGNATOR_MIDDLE_FINGER              0x13
#define  SL_USBD_HID_DESIGNATOR_RING_FINGER                0x14
#define  SL_USBD_HID_DESIGNATOR_LITTLE_FINGER              0x15
#define  SL_USBD_HID_DESIGNATOR_HEAD                       0x16
#define  SL_USBD_HID_DESIGNATOR_SHOULDER                   0x17
#define  SL_USBD_HID_DESIGNATOR_HIP                        0x18
#define  SL_USBD_HID_DESIGNATOR_WAIST                      0x19
#define  SL_USBD_HID_DESIGNATOR_THIGH                      0x1A
#define  SL_USBD_HID_DESIGNATOR_KNEE                       0x1B
#define  SL_USBD_HID_DESIGNATOR_CALF                       0x1C
#define  SL_USBD_HID_DESIGNATOR_ANKLE                      0x1D
#define  SL_USBD_HID_DESIGNATOR_FOOT                       0x1E
#define  SL_USBD_HID_DESIGNATOR_HEEL                       0x1F
#define  SL_USBD_HID_DESIGNATOR_BALL_OF_FOOT               0x20
#define  SL_USBD_HID_DESIGNATOR_BIG_TOE                    0x21
#define  SL_USBD_HID_DESIGNATOR_SECOND_TOE                 0x22
#define  SL_USBD_HID_DESIGNATOR_THIRD_TOE                  0x23
#define  SL_USBD_HID_DESIGNATOR_FOURTH_TOE                 0x24
#define  SL_USBD_HID_DESIGNATOR_LITTLE_TOE                 0x25
#define  SL_USBD_HID_DESIGNATOR_BROW                       0x26
#define  SL_USBD_HID_DESIGNATOR_CHEEK                      0x27
// Qualifier values.
#define  SL_USBD_HID_QUALIFIER_NOT_APPLICABLE                 0
#define  SL_USBD_HID_QUALIFIER_RIGHT                          1
#define  SL_USBD_HID_QUALIFIER_LEFT                           2
#define  SL_USBD_HID_QUALIFIER_BOTH                           3
#define  SL_USBD_HID_QUALIFIER_EITHER                         4
#define  SL_USBD_HID_QUALIFIER_CENTER                         5

/****************************************************************************************************//**
 *                                           HID USAGE PAGES
 *
 * Note(s) : (1) See 'Universal Serial Bus HID Usage Tables', Version 1.12, Section 3 for more details
 *               about Usage Pages.
 *
 *           (2) See 'Universal Serial Bus HID Usage Tables', Version 1.12, Section 4 for more details
 *               about Generic Desktop Page usages and controls.
 *******************************************************************************************************/
// Usage Pages (see Note #1)
#define  SL_USBD_HID_USAGE_PAGE_UNDEFINED                  0x00
#define  SL_USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS   0x01
#define  SL_USBD_HID_USAGE_PAGE_SIMULATION_CONTROLS        0x02
#define  SL_USBD_HID_USAGE_PAGE_VR_CONTROLS                0x03
#define  SL_USBD_HID_USAGE_PAGE_SPORT_CONTROLS             0x04
#define  SL_USBD_HID_USAGE_PAGE_GAME_CONTROLS              0x05
#define  SL_USBD_HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS    0x06
#define  SL_USBD_HID_USAGE_PAGE_KEYBOARD                   0x07
#define  SL_USBD_HID_USAGE_PAGE_LEDS                       0x08
#define  SL_USBD_HID_USAGE_PAGE_BUTTON                     0x09
#define  SL_USBD_HID_USAGE_PAGE_ORDINAL                    0x0A
#define  SL_USBD_HID_USAGE_PAGE_TELEPHONY                  0x0B
#define  SL_USBD_HID_USAGE_PAGE_CONSUMER                   0x0C
#define  SL_USBD_HID_USAGE_PAGE_DIGITIZER                  0x0D
#define  SL_USBD_HID_USAGE_PAGE_PID_PAGE                   0x0F
#define  SL_USBD_HID_USAGE_PAGE_UNICODE                    0x10
#define  SL_USBD_HID_USAGE_PAGE_ALPHANUMERIC_DISPLAY       0x14
#define  SL_USBD_HID_USAGE_PAGE_MEDICAL_INSTRUMENTS        0x40
#define  SL_USBD_HID_USAGE_PAGE_MONITOR_0                  0x80
#define  SL_USBD_HID_USAGE_PAGE_MONITOR_1                  0x81
#define  SL_USBD_HID_USAGE_PAGE_MONITOR_2                  0x82
#define  SL_USBD_HID_USAGE_PAGE_MONITOR_3                  0x83
#define  SL_USBD_HID_USAGE_PAGE_POWER_0                    0x84
#define  SL_USBD_HID_USAGE_PAGE_POWER_1                    0x85
#define  SL_USBD_HID_USAGE_PAGE_POWER_2                    0x86
#define  SL_USBD_HID_USAGE_PAGE_POWER_3                    0x87
#define  SL_USBD_HID_USAGE_PAGE_BAR_CODE_SCANNER_PAGE      0x8C
#define  SL_USBD_HID_USAGE_PAGE_SCALE_PAGE                 0x8D
#define  SL_USBD_HID_USAGE_PAGE_MSR_DEVICES                0x8E
#define  SL_USBD_HID_USAGE_PAGE_POINT_OF_SALE_PAGES        0x8F
#define  SL_USBD_HID_USAGE_PAGE_CAMERA_CONTROL_PAGE        0x90
#define  SL_USBD_HID_USAGE_PAGE_ARCADE_PAGE                0x91

// Generic Desktop Page Usages & Control
// See Note #2.
#define  SL_USBD_HID_CP_POINTER                            0x01
#define  SL_USBD_HID_CA_MOUSE                              0x02
#define  SL_USBD_HID_CA_JOYSTICK                           0x04
#define  SL_USBD_HID_CA_GAME_PAD                           0x05
#define  SL_USBD_HID_CA_KEYBOARD                           0x06
#define  SL_USBD_HID_CA_KEYPAD                             0x07
#define  SL_USBD_HID_CA_MULTI_AXIS_CONTROLLER              0x08
#define  SL_USBD_HID_DV_X                                  0x30
#define  SL_USBD_HID_DV_Y                                  0x31
#define  SL_USBD_HID_DV_Z                                  0x32
#define  SL_USBD_HID_DV_WHEEL                              0x38
#define  SL_USBD_HID_CA_SYSTEM_CONTROL                     0x80

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/// Async comm callback
typedef void (*sl_usbd_hid_async_function_t)(uint8_t      class_nbr,
                                             void         *p_buf,
                                             uint32_t     buf_len,
                                             uint32_t     xfer_len,
                                             void         *p_callback_arg,
                                             sl_status_t  status);

/// HID callbacks
typedef const struct {
  void (*enable)(uint8_t class_nbr);                        ///< Callback for enable event

  void (*disable)(uint8_t class_nbr);                       ///< Callback for disable event

  void (*get_report_desc)(uint8_t         class_nbr,
                          const uint8_t   **p_report_ptr,
                          uint16_t        *p_report_len);   ///< Callback for get report descriptor event

  void (*get_phy_desc)(uint8_t         class_nbr,
                       const uint8_t   **p_report_ptr,
                       uint16_t        *p_report_len);      ///< Callback for get physical descriptor event

  void (*set_output_report)(uint8_t  class_nbr,
                            uint8_t  report_id,
                            uint8_t  *p_report_buf,
                            uint16_t report_len);           ///< Callback for set output report event

  void (*get_feature_report)(uint8_t  class_nbr,
                             uint8_t  report_id,
                             uint8_t  *p_report_buf,
                             uint16_t report_len);          ///< Callback for get feature report event

  void (*set_feature_report)(uint8_t  class_nbr,
                             uint8_t  report_id,
                             uint8_t  *p_report_buf,
                             uint16_t report_len);          ///< Callback for set feature report event

  void (*get_protocol)(uint8_t  class_nbr,
                       uint8_t  *p_protocol);               ///< Callback for get protocol event

  void (*set_protocol)(uint8_t class_nbr,
                       uint8_t protocol);                   ///< Callback for set protocol event
} sl_usbd_hid_callbacks_t;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/********************************************************************************************************
 *                                               HID FUNCTIONS
 *******************************************************************************************************/

/****************************************************************************************************//**
 * @brief    Initialize HID class.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_init(void);

/****************************************************************************************************//**
 * @brief   Add a new instance of the HID class.
 *
 * @param   subclass            Subclass code.
 *
 * @param   protocol            protocol code.
 *
 * @param   country_code        Country code id.
 *
 * @param   interval_in         Polling interval for input transfers, in milliseconds.
 *                              It must be a power of 2.
 *
 * @param   interval_out        Polling interval for output transfers, in milliseconds.
 *                              It must be a power of 2. Used only when read operations are not
 *                              through control transfers.
 *
 * @param   ctrl_rd_en          Enable read operations through the control transfers.
 *
 * @param   p_hid_callbacks     Pointer to HID descriptor and request callback structure.
 *                              [Content MUST be persistent]
 *
 * @param   p_class_nbr         Pointer to variable that will receive class instance number.
 *                              The variable is set to class instance number if no errors are returned,
 *                              otherwise it is set to SL_USBD_CLASS_NBR_NONE.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_create_instance(uint8_t                     subclass,
                                        uint8_t                     protocol,
                                        sl_usbd_hid_country_code_t  country_code,
                                        uint16_t                    interval_in,
                                        uint16_t                    interval_out,
                                        bool                        ctrl_rd_en,
                                        sl_usbd_hid_callbacks_t     *p_hid_callbacks,
                                        uint8_t                     *p_class_nbr);

/****************************************************************************************************//**
 * @brief    Add the HID class instance to the USB device configuration (see Note #1).
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    config_nbr  Configuration index to add HID class instance to.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *
 * @note     (1) Called several times, it creates multiple instances and configurations.
 *               For instance, the following architecture could be created :
 *               @verbatim
 *               FS
 *               |-- Configuration 0 (HID class 0)
 *                                   (HID class 1)
 *                                   (HID class 2)
 *                   |-- Interface 0
 *               |-- Configuration 1 (HID class 0)
 *                   |-- Interface 0
 *               @endverbatim
 *
 * @note     (2) Configuration Descriptor corresponding to a HID device uses the following format :
 *               @verbatim
 *               Configuration Descriptor
 *               |-- Interface Descriptor (HID class)
 *                   |-- Endpoint Descriptor (Interrupt IN)
 *                   |-- Endpoint Descriptor (Interrupt OUT) - optional
 *               @endverbatim
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_add_to_configuration(uint8_t  class_nbr,
                                             uint8_t  config_nbr);

/****************************************************************************************************//**
 * @brief    Get the HID class enable state.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_enabled   Pointer to a variable that will receive HID class enable state.
 *                       The variable is set to true if HID class is enabled,
 *                       and is set to false if it is NOT enabled.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_is_enabled(uint8_t  class_nbr,
                                   bool     *p_enabled);

/****************************************************************************************************//**
 * @brief    Send data to the host through the Interrupt IN endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the transmit buffer. If more than one input report exists, the first
 *                       byte must represent the report id.
 *
 * @param    buf_len     Transmit buffer length, in octets.
 *
 * @param    timeout     Timeout in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_write_sync(uint8_t    class_nbr,
                                   void       *p_buf,
                                   uint32_t   buf_len,
                                   uint16_t   timeout,
                                   uint32_t   *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data from the host through the Interrupt OUT endpoint. This function is blocking.
 *
 * @param    class_nbr   Class instance number.
 *
 * @param    p_buf       Pointer to the receive buffer.
 *
 * @param    buf_len     Receive the buffer length, in octets.
 *
 * @param    timeout     Timeout, in milliseconds.
 *
 * @param    p_xfer_len  Pointer to a variable that will receive transfer length.
 *                       The variable is set to number of octets received, if no errors are returned,
 *                       or is set to 0 if any errors are returned.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 ***********************************************************************************************/
sl_status_t sl_usbd_hid_read_sync(uint8_t    class_nbr,
                                  void       *p_buf,
                                  uint32_t   buf_len,
                                  uint16_t   timeout,
                                  uint32_t   *p_xfer_len);

/****************************************************************************************************//**
 * @brief    Receive data from the host through the Interrupt OUT endpoint. This function is non-blocking
 *           and returns immediately after transfer preparation. Upon transfer completion, the provided
 *           callback is called to notify the application.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to receive buffer.
 *
 * @param    buf_len         Receive buffer length, in octets.
 *
 * @param    async_fnct      Receive callback.
 *
 * @param    p_async_arg     Additional argument provided by application for receive callback.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_read_async(uint8_t                      class_nbr,
                                   void                         *p_buf,
                                   uint32_t                     buf_len,
                                   sl_usbd_hid_async_function_t async_fnct,
                                   void                         *p_async_arg);

/****************************************************************************************************//**
 * @brief    Send data to the host through the Interrupt IN endpoint. This function is non-blocking,
 *           and returns immediately after transfer preparation. Upon transfer completion, the provided
 *           callback is called to notify the application.
 *
 * @param    class_nbr       Class instance number.
 *
 * @param    p_buf           Pointer to transmit buffer. If more than one input report exists, the first
 *                           byte must represent the report id.
 *
 * @param    buf_len         Transmit buffer length, in octets.
 *
 * @param    async_fnct      Transmit callback.
 *
 * @param    p_async_arg     Additional argument provided by application for transmit callback.
 *
 * @return   Returns SL_STATUS_OK on success or another SL_STATUS code on failure.
 *******************************************************************************************************/
sl_status_t sl_usbd_hid_write_async(uint8_t                      class_nbr,
                                    void                         *p_buf,
                                    uint32_t                     buf_len,
                                    sl_usbd_hid_async_function_t async_fnct,
                                    void                         *p_async_arg);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
