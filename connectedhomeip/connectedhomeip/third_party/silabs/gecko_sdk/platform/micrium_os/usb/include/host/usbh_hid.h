/***************************************************************************//**
 * @file
 * @brief USB Host HID Class
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

/********************************************************************************************************
 * @defgroup USBH_HID USB Host HID API
 * @ingroup USBH
 * @brief   USB Host HID API
 *
 * @addtogroup USBH_HID
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_HID_H_
#define  _USBH_HID_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_class.h>
#include  <usb/include/host/usbh_core_handle.h>
#include  <usb/include/host/usbh_core_utils.h>
#include  <usb/include/host/usbh_core_opt_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_HID_STD_REQ_TIMEOUT_DFLT                  5000u

/********************************************************************************************************
 *                                           HID PROTOCOL CODES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 4.3 for more details about HID protocol codes.
 *******************************************************************************************************/

#define  USBH_HID_PROTOCOL_CODE_NONE                    0x00u
#define  USBH_HID_PROTOCOL_CODE_KBD                     0x01u
#define  USBH_HID_PROTOCOL_CODE_MOUSE                   0x02u

/********************************************************************************************************
 *                                               HID PROTOCOLS
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 7.2 for more details about HID protocols.
 *******************************************************************************************************/

#define  USBH_HID_REQ_PROTOCOL_BOOT                   0x0000u
#define  USBH_HID_REQ_PROTOCOL_REPORT                 0x0001u

/********************************************************************************************************
 *                                       HID DESCRIPTOR COUNTRY CODES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 6.2.1',
 *               section 7.2 for more details about HID country codes.
 *******************************************************************************************************/

#define  USBH_HID_COUNTRY_CODE_NOT_SUPPORTED               0u
#define  USBH_HID_COUNTRY_CODE_ARABIC                      1u
#define  USBH_HID_COUNTRY_CODE_BELGIAN                     2u
#define  USBH_HID_COUNTRY_CODE_CANADIAN_BILINGUAL          3u
#define  USBH_HID_COUNTRY_CODE_CANADIAN_FRENCH             4u
#define  USBH_HID_COUNTRY_CODE_CZECH_REPUBLIC              5u
#define  USBH_HID_COUNTRY_CODE_DANISH                      6u
#define  USBH_HID_COUNTRY_CODE_FINNISH                     7u
#define  USBH_HID_COUNTRY_CODE_FRENCH                      8u
#define  USBH_HID_COUNTRY_CODE_GERMAN                      9u
#define  USBH_HID_COUNTRY_CODE_GREEK                      10u
#define  USBH_HID_COUNTRY_CODE_HEBREW                     11u
#define  USBH_HID_COUNTRY_CODE_HUNGARY                    12u
#define  USBH_HID_COUNTRY_CODE_INTERNATIONAL              13u
#define  USBH_HID_COUNTRY_CODE_ITALIAN                    14u
#define  USBH_HID_COUNTRY_CODE_JAPAN_KATAKANA             15u
#define  USBH_HID_COUNTRY_CODE_KOREAN                     16u
#define  USBH_HID_COUNTRY_CODE_LATIN_AMERICAN             17u
#define  USBH_HID_COUNTRY_CODE_NETHERLANDS_DUTCH          18u
#define  USBH_HID_COUNTRY_CODE_NORWEGIAN                  19u
#define  USBH_HID_COUNTRY_CODE_PERSIAN_FARSI              20u
#define  USBH_HID_COUNTRY_CODE_POLAND                     21u
#define  USBH_HID_COUNTRY_CODE_PORTUGUESE                 22u
#define  USBH_HID_COUNTRY_CODE_RUSSIA                     23u
#define  USBH_HID_COUNTRY_CODE_SLOVAKIA                   24u
#define  USBH_HID_COUNTRY_CODE_SPANISH                    25u
#define  USBH_HID_COUNTRY_CODE_SWEDISH                    26u
#define  USBH_HID_COUNTRY_CODE_SWISS_FRENCH               27u
#define  USBH_HID_COUNTRY_CODE_SWISS_GERMAN               28u
#define  USBH_HID_COUNTRY_CODE_SWITZERLAND                29u
#define  USBH_HID_COUNTRY_CODE_TAIWAN                     30u
#define  USBH_HID_COUNTRY_CODE_TURKISH_Q                  31u
#define  USBH_HID_COUNTRY_CODE_UK                         32u
#define  USBH_HID_COUNTRY_CODE_US                         33u
#define  USBH_HID_COUNTRY_CODE_YUGOSLAVIA                 34u
#define  USBH_HID_COUNTRY_CODE_TURKISH_F                  35u

/********************************************************************************************************
 *                                           HID REPORT ITEM
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2 for more details about HID report items.
 *******************************************************************************************************/

#define  USBH_HID_ITEM_LONG                             0xFEu

//                                                                 ----------------- SHORT ITEM TYPES -----------------
#define  USBH_HID_ITEM_TYPE_MAIN                        0x00u
#define  USBH_HID_ITEM_TYPE_GLOBAL                      0x01u
#define  USBH_HID_ITEM_TYPE_LOCAL                       0x02u
#define  USBH_HID_ITEM_TYPE_RESERVED                    0x03u

//                                                                 ------------------ MAIN ITEM TAGS ------------------
#define  USBH_HID_MAIN_ITEM_TAG_IN                      0x08u
#define  USBH_HID_MAIN_ITEM_TAG_OUT                     0x09u
#define  USBH_HID_MAIN_ITEM_TAG_COLL                    0x0Au
#define  USBH_HID_MAIN_ITEM_TAG_FEATURE                 0x0Bu
#define  USBH_HID_MAIN_ITEM_TAG_ENDCOLL                 0x0Cu

//                                                                 ----------------- GLOBAL ITEM TAGS -----------------
#define  USBH_HID_GLOBAL_ITEM_TAG_USAGE_PAGE            0x00u
#define  USBH_HID_GLOBAL_ITEM_TAG_LOG_MIN               0x01u
#define  USBH_HID_GLOBAL_ITEM_TAG_LOG_MAX               0x02u
#define  USBH_HID_GLOBAL_ITEM_TAG_PHY_MIN               0x03u
#define  USBH_HID_GLOBAL_ITEM_TAG_PHY_MAX               0x04u
#define  USBH_HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT         0x05u
#define  USBH_HID_GLOBAL_ITEM_TAG_UNIT                  0x06u
#define  USBH_HID_GLOBAL_ITEM_TAG_REPORT_SIZE           0x07u
#define  USBH_HID_GLOBAL_ITEM_TAG_REPORT_ID             0x08u
#define  USBH_HID_GLOBAL_ITEM_TAG_REPORT_COUNT          0x09u
#define  USBH_HID_GLOBAL_ITEM_TAG_PUSH                  0x0Au
#define  USBH_HID_GLOBAL_ITEM_TAG_POP                   0x0Bu

//                                                                 ----------------- LOCAL ITEM TAGS ------------------
#define  USBH_HID_LOCAL_ITEM_TAG_USAGE                  0x00u
#define  USBH_HID_LOCAL_ITEM_TAG_USAGE_MIN              0x01u
#define  USBH_HID_LOCAL_ITEM_TAG_USAGE_MAX              0x02u
#define  USBH_HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX       0x03u
#define  USBH_HID_LOCAL_ITEM_TAG_DESIGNATOR_MIN         0x04u
#define  USBH_HID_LOCAL_ITEM_TAG_DESIGNATOR_MAX         0x05u
#define  USBH_HID_LOCAL_ITEM_TAG_STR_IX                 0x07u
#define  USBH_HID_LOCAL_ITEM_TAG_STR_MIN                0x08u
#define  USBH_HID_LOCAL_ITEM_TAG_STR_MAX                0x09u
#define  USBH_HID_LOCAL_ITEM_TAG_DELIMITER              0x0Au

/********************************************************************************************************
 *                                               HID REPORT
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2 for more details about HID reports.
 *******************************************************************************************************/

//                                                                 -------------------- MAIN ITEMS --------------------
#define  USBH_HID_MAIN_IN                               0x80u
#define  USBH_HID_MAIN_OUT                              0x90u
#define  USBH_HID_MAIN_COLL                             0xA0u
#define  USBH_HID_MAIN_FEATURE                          0xB0u
#define  USBH_HID_MAIN_ENDCOLL                          0xC0u

//                                                                 ---------- INPUT / OUTPUT / FEATURE ITEMS ----------
#define  USBH_HID_MAIN_CONST                            0x01u
#define  USBH_HID_MAIN_DATA                             0x00u
#define  USBH_HID_MAIN_VAR                              0x02u
#define  USBH_HID_MAIN_ARRAY                            0x00u
#define  USBH_HID_MAIN_REL                              0x04u
#define  USBH_HID_MAIN_ABS                              0x00u
#define  USBH_HID_MAIN_WRAP                             0x08u
#define  USBH_HID_MAIN_NOWRAP                           0x00u
#define  USBH_HID_MAIN_NONLINE                          0x10u
#define  USBH_HID_MAIN_LINE                             0x00u
#define  USBH_HID_MAIN_NOPREFERRED                      0x20u
#define  USBH_HID_MAIN_PREFERREDSTATE                   0x00u
#define  USBH_HID_MAIN_NULLSTATE                        0x40u
#define  USBH_HID_MAIN_NO_NULL_POS                      0x00u
#define  USBH_HID_MAIN_VOLATILE                         0x80u
#define  USBH_HID_MAIN_NONVOLATILE                      0x00u
#define  USBH_HID_MAIN_BUFFEREDBYTES                  0x0100u
#define  USBH_HID_MAIN_BITFIELD                       0x0000u

//                                                                 ----------------- COLLECTION ITEMS -----------------
#define  USBH_HID_COLL_PHYSICAL                         0x00u
#define  USBH_HID_COLL_APP                              0x01u
#define  USBH_HID_COLL_LOGICAL                          0x02u
#define  USBH_HID_COLL_REPORT                           0x03u
#define  USBH_HID_COLL_NAMED_ARRAY                      0x04u
#define  USBH_HID_COLL_USAGE_SWITCH                     0x05u
#define  USBH_HID_COLL_USAGE_MODIFIER                   0x06u

//                                                                 ------------------- GLOBAL ITEMS -------------------
#define  USBH_HID_GLOBAL_USAGE_PAGE                     0x04u
#define  USBH_HID_GLOBAL_LOG_MIN                        0x14u
#define  USBH_HID_GLOBAL_LOG_MAX                        0x24u
#define  USBH_HID_GLOBAL_PHY_MIN                        0x34u
#define  USBH_HID_GLOBAL_PHY_MAX                        0x44u
#define  USBH_HID_GLOBAL_UNIT_EXPONENT                  0x54u
#define  USBH_HID_GLOBAL_UNIT                           0x64u
#define  USBH_HID_GLOBAL_REPORT_SIZE                    0x74u
#define  USBH_HID_GLOBAL_REPORT_ID                      0x84u
#define  USBH_HID_GLOBAL_REPORT_COUNT                   0x94u
#define  USBH_HID_GLOBAL_PUSH                           0xA4u
#define  USBH_HID_GLOBAL_POP                            0xB4u

//                                                                 ------------------- LOCAL ITEMS --------------------
#define  USBH_HID_LOCAL_USAGE                           0x08u
#define  USBH_HID_LOCAL_USAGE_MIN                       0x18u
#define  USBH_HID_LOCAL_USAGE_MAX                       0x28u
#define  USBH_HID_LOCAL_DESIGNATOR_IX                   0x38u
#define  USBH_HID_LOCAL_DESIGNATOR_MIN                  0x48u
#define  USBH_HID_LOCAL_DESIGNATOR_MAX                  0x58u
#define  USBH_HID_LOCAL_STR_INDEX                       0x78u
#define  USBH_HID_LOCAL_STR_MIN                         0x88u
#define  USBH_HID_LOCAL_STR_MAX                         0x98u
#define  USBH_HID_LOCAL_DELIMITER                       0xA8u

/********************************************************************************************************
 *                                       HID PHYSICAL DESCRIPTOR
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.3 for more details about HID physical descriptors.
 *******************************************************************************************************/

//                                                                 -------------------- BIAS VALUES -------------------
#define  USBH_HID_BIAS_NOT_APPLICABLE                      0u
#define  USBH_HID_BIAS_RIGHT_HAND                          1u
#define  USBH_HID_BIAS_LEFT_HAND                           2u
#define  USBH_HID_BIAS_BOTH_HANDS                          3u
#define  USBH_HID_BIAS_EITHER_HAND                         4u

//                                                                 ----------------- DESIGNATOR VALUES ----------------
#define  USBH_HID_DESIGNATOR_NONE                       0x00u
#define  USBH_HID_DESIGNATOR_HAND                       0x01u
#define  USBH_HID_DESIGNATOR_EYEBALL                    0x02u
#define  USBH_HID_DESIGNATOR_EYEBROW                    0x03u
#define  USBH_HID_DESIGNATOR_EYELID                     0x04u
#define  USBH_HID_DESIGNATOR_EAR                        0x05u
#define  USBH_HID_DESIGNATOR_NOSE                       0x06u
#define  USBH_HID_DESIGNATOR_MOUTH                      0x07u
#define  USBH_HID_DESIGNATOR_UPPER_LIP                  0x08u
#define  USBH_HID_DESIGNATOR_LOWER_LIP                  0x09u
#define  USBH_HID_DESIGNATOR_JAW                        0x0Au
#define  USBH_HID_DESIGNATOR_NECK                       0x0Bu
#define  USBH_HID_DESIGNATOR_UPPER_ARM                  0x0Cu
#define  USBH_HID_DESIGNATOR_ELBOW                      0x0Du
#define  USBH_HID_DESIGNATOR_FOREARM                    0x0Eu
#define  USBH_HID_DESIGNATOR_WRIST                      0x0Fu
#define  USBH_HID_DESIGNATOR_PALM                       0x10u
#define  USBH_HID_DESIGNATOR_THUMB                      0x11u
#define  USBH_HID_DESIGNATOR_INDEX_FINGER               0x12u
#define  USBH_HID_DESIGNATOR_MIDDLE_FINGER              0x13u
#define  USBH_HID_DESIGNATOR_RING_FINGER                0x14u
#define  USBH_HID_DESIGNATOR_LITTLE_FINGER              0x15u
#define  USBH_HID_DESIGNATOR_HEAD                       0x16u
#define  USBH_HID_DESIGNATOR_SHOULDER                   0x17u
#define  USBH_HID_DESIGNATOR_HIP                        0x18u
#define  USBH_HID_DESIGNATOR_WAIST                      0x19u
#define  USBH_HID_DESIGNATOR_THIGH                      0x1Au
#define  USBH_HID_DESIGNATOR_KNEE                       0x1Bu
#define  USBH_HID_DESIGNATOR_CALF                       0x1Cu
#define  USBH_HID_DESIGNATOR_ANKLE                      0x1Du
#define  USBH_HID_DESIGNATOR_FOOT                       0x1Eu
#define  USBH_HID_DESIGNATOR_HEEL                       0x1Fu
#define  USBH_HID_DESIGNATOR_BALL_OF_FOOT               0x20u
#define  USBH_HID_DESIGNATOR_BIG_TOE                    0x21u
#define  USBH_HID_DESIGNATOR_SECOND_TOE                 0x22u
#define  USBH_HID_DESIGNATOR_THIRD_TOE                  0x23u
#define  USBH_HID_DESIGNATOR_FOURTH_TOE                 0x24u
#define  USBH_HID_DESIGNATOR_LITTLE_TOE                 0x25u
#define  USBH_HID_DESIGNATOR_BROW                       0x26u
#define  USBH_HID_DESIGNATOR_CHEEK                      0x27u

//                                                                 ----------------- QUALIFIER VALUES -----------------
#define  USBH_HID_QUALIFIER_NOT_APPLICABLE                 0u
#define  USBH_HID_QUALIFIER_RIGHT                          1u
#define  USBH_HID_QUALIFIER_LEFT                           2u
#define  USBH_HID_QUALIFIER_BOTH                           3u
#define  USBH_HID_QUALIFIER_EITHER                         4u
#define  USBH_HID_QUALIFIER_CENTER                         5u

/********************************************************************************************************
 *                                           HID USAGE PAGES
 *
 * Note(s) : (1) See 'Universal Serial Bus HID Usage Tables, 10/28/04, Version 1.12',
 *               section 3 for more details about usage pages.
 *******************************************************************************************************/

#define  USBH_HID_USAGE_PAGE_UNDEFINED                  0x00u
#define  USBH_HID_USAGE_PAGE_GENERIC_DESKTOP_CTRLS      0x01u
#define  USBH_HID_USAGE_PAGE_SIMULATION_CTRLS           0x02u
#define  USBH_HID_USAGE_PAGE_VR_CTRLS                   0x03u
#define  USBH_HID_USAGE_PAGE_SPORT_CTRLS                0x04u
#define  USBH_HID_USAGE_PAGE_GAME_CTRLS                 0x05u
#define  USBH_HID_USAGE_PAGE_GENERIC_DEV_CTRLS          0x06u
#define  USBH_HID_USAGE_PAGE_KBD                        0x07u
#define  USBH_HID_USAGE_PAGE_LEDS                       0x08u
#define  USBH_HID_USAGE_PAGE_BUTTON                     0x09u
#define  USBH_HID_USAGE_PAGE_ORDINAL                    0x0Au
#define  USBH_HID_USAGE_PAGE_TELEPHONY                  0x0Bu
#define  USBH_HID_USAGE_PAGE_CONSUMER                   0x0Cu
#define  USBH_HID_USAGE_PAGE_DIGITIZER                  0x0Du
#define  USBH_HID_USAGE_PAGE_PID_PAGE                   0x0Fu
#define  USBH_HID_USAGE_PAGE_UNICODE                    0x10u
#define  USBH_HID_USAGE_PAGE_ALPHANUMERIC_DISP          0x14u
#define  USBH_HID_USAGE_PAGE_MEDICAL_INSTRUMENTS        0x40u
#define  USBH_HID_USAGE_PAGE_MON_0                      0x80u
#define  USBH_HID_USAGE_PAGE_MON_1                      0x81u
#define  USBH_HID_USAGE_PAGE_MON_2                      0x82u
#define  USBH_HID_USAGE_PAGE_MON_3                      0x83u
#define  USBH_HID_USAGE_PAGE_PWR_0                      0x84u
#define  USBH_HID_USAGE_PAGE_PWR_1                      0x85u
#define  USBH_HID_USAGE_PAGE_PWR_2                      0x86u
#define  USBH_HID_USAGE_PAGE_PWR_3                      0x87u
#define  USBH_HID_USAGE_PAGE_BAR_CODE_SCANNER_PAGE      0x8Cu
#define  USBH_HID_USAGE_PAGE_SCALE_PAGE                 0x8Du
#define  USBH_HID_USAGE_PAGE_MSR_DEVS                   0x8Eu
#define  USBH_HID_USAGE_PAGE_POINT_OF_SALE_PAGES        0x8Fu
#define  USBH_HID_USAGE_PAGE_CAMERA_CTRL_PAGE           0x90u
#define  USBH_HID_USAGE_PAGE_ARCADE_PAGE                0x91u

//                                                                 ---------- GENERIC DESKTOP PAGE APP USAGES ---------
#define  USBH_HID_CA_MOUSE                        0x00010002u
#define  USBH_HID_CA_JOYSTICK                     0x00010004u
#define  USBH_HID_CA_GAME_PAD                     0x00010005u
#define  USBH_HID_CA_KBD                          0x00010006u
#define  USBH_HID_CA_KEYPAD                       0x00010007u
#define  USBH_HID_CA_MULTI_AXIS_CTRLR             0x00010008u
#define  USBH_HID_CA_SYSTEM_CTRL                  0x00010080u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           HID FUNCTION HANDLE
 *******************************************************************************************************/

typedef USBH_CLASS_FNCT_HANDLE USBH_HID_FNCT_HANDLE;

/********************************************************************************************************
 *                                       HID ASYNC NOTIFICATION CALLBACK
 *******************************************************************************************************/

//                                                                 --- APPLICATION REPORT RECEIVE CALLBACK FUNCTION ---
typedef void (*USBH_HID_RX_CALLBACK) (USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                      CPU_INT08U           *p_buf,
                                      CPU_INT32U           buf_len,
                                      void                 *p_arg,
                                      RTOS_ERR             err);

/****************************************************************************************************//**
 *                                               REPORT FORMAT
 *
 * @note     (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2 for more details about HID reports.
 * @{
 *******************************************************************************************************/

typedef struct usbh_hid_report_fmt {
  CPU_INT08U ReportID;                                          ///< Report ID.
  CPU_INT08U ReportType;                                        ///< Report type (INPUT / OUTPUT / FEATURE).
  CPU_INT16U UsagePage;                                         ///< Usage page.
  CPU_INT32U *UsageTbl;                                         ///< Tbl of usage(s).
  CPU_INT32U NbrUsage;                                          ///< Nbr of usage(s) contained in tbl.
  CPU_INT32U UsageMin;
  CPU_INT32U UsageMax;
  CPU_INT32S LogMin;
  CPU_INT32S LogMax;
  CPU_INT32S PhyMin;
  CPU_INT32S PhyMax;
  CPU_INT32S UnitExp;
  CPU_INT32U Unit;
  CPU_INT32U ReportSize;                                        ///< Report size (bits).
  CPU_INT32U ReportCnt;                                         ///< Nbr of reports.
  CPU_INT32U Flag;                                              ///< Flag (Data/constant, var/array ..).
  CPU_INT32U PhyUsage;                                          ///< Physical usage of this report.
  CPU_INT32U AppUsage;                                          ///< Application usage of this report.
  CPU_INT32U LogUsage;                                          ///< Logical usage of this report.
} USBH_HID_REPORT_FMT;

typedef struct usbh_hid_report_fmt_item USBH_HID_REPORT_FMT_ITEM;
struct usbh_hid_report_fmt_item {
  USBH_HID_REPORT_FMT      ReportFmt;
  USBH_HID_REPORT_FMT_ITEM *NextPtr;
};

///< @}

/****************************************************************************************************//**
 *                                           HID COLLECTION
 *
 * @note     (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11',
 *               section 6.2.2.6 for more details about HID collection.
 *******************************************************************************************************/

typedef struct usbh_hid_app_coll USBH_HID_APP_COLL;
struct usbh_hid_app_coll {
  CPU_INT32U               Usage;                               ///< Coll usage.
  CPU_INT08U               Type;                                ///< Coll type (App / Physical / Logical).
  USBH_HID_REPORT_FMT_ITEM *ReportFmtItemHeadPtr;               ///< Ptr to head of list of report fmt items.
  USBH_HID_APP_COLL        *NextPtr;                            ///< Ptr to next HID app coll.
};

/********************************************************************************************************
 *                                       HID APP NOTIFICATION FUNCTIONS
 *******************************************************************************************************/

typedef const struct usbh_hid_app_fncts {
  void *(*Conn)(USBH_DEV_HANDLE       dev_handle,
                USBH_FNCT_HANDLE      fnct_handle,
                USBH_HID_FNCT_HANDLE  hid_fnct_handle,
                USBH_HID_APP_COLL     *p_app_coll_head,
                RTOS_ERR              err);

  void (*DataRxd)(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                  void                 *p_arg,
                  CPU_INT08U           report_id,
                  CPU_INT08U           *p_buf,
                  CPU_INT32U           buf_len,
                  RTOS_ERR             err);

  void (*Disconn)(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                  void                 *p_arg);
} USBH_HID_APP_FNCTS;

/****************************************************************************************************//**
 *                                       HID CONFIGURATION STRUCTURES
 *
 * @note     (1) Can be DEF_NULL if USBH_CFG_OPTIMIZE_SPD_EN is set to DEF_DISABLED
 *
 *           (2) Can be DEF_NULL if USBH_CFG_INIT_ALLOC_EN is set to DEF_DISABLED
 * @{
 *******************************************************************************************************/

//                                                                 -- USB HOST HID CLASS OPTIMIZE SPD CONFIGURATIONS --
typedef struct usbh_hid_cfg_optimize_spd {
  CPU_INT08U FnctQty;                                           ///< Quantity of HID functions.
} USBH_HID_CFG_OPTIMIZE_SPD;

//                                                                 --- USB HOST HID CLASS INIT ALLOC CONFIGURATIONS ---
typedef struct usbh_hid_cfg_init_alloc {
  CPU_INT08U FnctQty;                                           ///< Quantity of HID functions (total).
  CPU_INT08U ReportDescParseAppCollItemQty;                     ///< Quantity of app collection items.
  CPU_INT08U ReportDescParseGlobalItemQty;                      ///< Quantity of global items.
  CPU_INT08U ReportDescParseCollItemQty;                        ///< Quantity of collection items.
  CPU_INT08U ReportDescParseReportFmtItemQty;                   ///< Quantity of report fmt items.
} USBH_HID_CFG_INIT_ALLOC;

//                                                                 ------------- HID INIT CONFIGURATIONS --------------
typedef struct usbh_hid_init_cfg {
  CPU_SIZE_T                BufAlignOctets;                     ///< Indicates desired mem alignment for internal buf.
  CPU_INT08U                RxBufQty;                           ///< Qty of bufs avail for rx.
  CPU_INT08U                RxBufLen;                           ///< Len of bufs used  for rx.
  CPU_INT16U                ReportDescMaxLen;                   ///< Max len of report desc.
  CPU_INT08U                UsageMaxNbrPerItem;                 ///< Max nbr of usage associated with a given item.

  MEM_SEG                   *MemSegPtr;                         ///< Ptr to mem segment to use when allocating ctrl data.
  MEM_SEG                   *MemSegBufPtr;                      ///< Ptr to mem segment to use when allocating data buf.

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
  USBH_HID_CFG_OPTIMIZE_SPD OptimizeSpd;                        ///< Configurations for optimize speed mode.
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
  USBH_HID_CFG_INIT_ALLOC InitAlloc;                            ///< Configurations for allocations at init mode.
#endif
} USBH_HID_INIT_CFG;

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

extern const USBH_HID_INIT_CFG USBH_HID_InitCfgDflt;            // USBH HID class dflt configurations.

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
void USBH_HID_ConfigureBufAlignOctets(CPU_SIZE_T buf_align_octets);

void USBH_HID_ConfigureRxBuf(CPU_INT08U rx_buf_qty,
                             CPU_INT08U rx_buf_len);

void USBH_HID_ConfigureReportDescMaxLen(CPU_INT16U report_desc_max_len);

void USBH_HID_ConfigureUsageMaxNbrPerItem(CPU_INT08U usage_max_nbr_per_item);

void USBH_HID_ConfigureMemSeg(MEM_SEG *p_mem_seg,
                              MEM_SEG *p_mem_seg_buf);

#if (USBH_CFG_OPTIMIZE_SPD_EN == DEF_ENABLED)
void USBH_HID_ConfigureOptimizeSpdCfg(const USBH_HID_CFG_OPTIMIZE_SPD *p_optimize_spd_cfg);
#endif

#if (USBH_CFG_INIT_ALLOC_EN == DEF_ENABLED)
void USBH_HID_ConfigureInitAllocCfg(const USBH_HID_CFG_INIT_ALLOC *p_init_alloc_cfg);
#endif
#endif

/********************************************************************************************************
 *                                               HID FUNCTIONS
 *******************************************************************************************************/

void USBH_HID_Init(USBH_HID_APP_FNCTS *p_hid_app_fncts,
                   RTOS_ERR           *p_err);

void USBH_HID_StdReqTimeoutSet(CPU_INT32U std_req_timeout_ms,
                               RTOS_ERR   *p_err);

CPU_INT32U USBH_HID_UsageGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                             RTOS_ERR             *p_err);

CPU_BOOLEAN USBH_HID_IsBootCapable(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                                   RTOS_ERR             *p_err);

CPU_INT16U USBH_HID_ReportTx(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                             CPU_INT08U           report_id,
                             void                 *p_buf,
                             CPU_INT16U           buf_len,
                             CPU_INT32U           timeout_ms,
                             RTOS_ERR             *p_err);

void USBH_HID_ProtocolSet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                          CPU_INT16U           protocol,
                          RTOS_ERR             *p_err);

void USBH_HID_ProtocolGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                          CPU_INT16U           *p_protocol,
                          RTOS_ERR             *p_err);

void USBH_HID_IdleSet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                      CPU_INT08U           report_id,
                      CPU_INT32U           dur,
                      RTOS_ERR             *p_err);

CPU_INT32U USBH_HID_IdleGet(USBH_HID_FNCT_HANDLE hid_fnct_handle,
                            CPU_INT08U           report_id,
                            RTOS_ERR             *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of HID module include.
