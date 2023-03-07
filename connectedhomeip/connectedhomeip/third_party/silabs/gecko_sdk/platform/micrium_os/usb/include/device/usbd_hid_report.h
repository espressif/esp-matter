/***************************************************************************//**
 * @file
 * @brief USB Device HID Report
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
 *                                               MODULE
 *******************************************************************************************************/

#ifndef  _USBD_HID_REPORT_H_
#define  _USBD_HID_REPORT_H_

/********************************************************************************************************
 *                                               INCLUDE FILES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices (HID), 6/27/01, Version 1.11'
 *               for more details about :
 *
 *               (a) SHORT  ITEM TYPES          (Section 6.2.2.2)
 *
 *               (e) MAIN   ITEMS               (Section 6.2.2.4)
 *
 *               (f) INPUT/OUTPUT/FEATURE ITEMS (Section 6.2.2.5)
 *
 *               (g) COLLECTION ITEMS           (Section 6.2.2.6)
 *
 *               (h) GLOBAL     ITEMS           (Section 6.2.2.7)
 *
 *               (i) LOCAL      ITEMS           (Section 6.2.2.7)
 *******************************************************************************************************/

//                                                                 ------------------ HID REPORT DESC -----------------
#define  USBD_HID_ITEM_LONG                             0xFE
//                                                                 Short item types (see Note #1a).
#define  USBD_HID_ITEM_TYPE_MAIN                        0x00
#define  USBD_HID_ITEM_TYPE_GLOBAL                      0x04
#define  USBD_HID_ITEM_TYPE_LOCAL                       0x08
#define  USBD_HID_ITEM_TYPE_RESERVED                    0x0C
//                                                                 Main item (see Note #1e).
#define  USBD_HID_MAIN_INPUT                            0x80
#define  USBD_HID_MAIN_OUTPUT                           0x90
#define  USBD_HID_MAIN_COLLECTION                       0xA0
#define  USBD_HID_MAIN_FEATURE                          0xB0
#define  USBD_HID_MAIN_ENDCOLLECTION                    0xC0
//                                                                 Input/output/feature item (see Note #1f).
#define  USBD_HID_MAIN_CONSTANT                         0x01
#define  USBD_HID_MAIN_DATA                             0x00
#define  USBD_HID_MAIN_VARIABLE                         0x02
#define  USBD_HID_MAIN_ARRAY                            0x00
#define  USBD_HID_MAIN_RELATIVE                         0x04
#define  USBD_HID_MAIN_ABSOLUTE                         0x00
#define  USBD_HID_MAIN_WRAP                             0x08
#define  USBD_HID_MAIN_NOWRAP                           0x00
#define  USBD_HID_MAIN_NONLINEAR                        0x10
#define  USBD_HID_MAIN_LINEAR                           0x00
#define  USBD_HID_MAIN_NOPREFERRED                      0x20
#define  USBD_HID_MAIN_PREFERREDSTATE                   0x00
#define  USBD_HID_MAIN_NULLSTATE                        0x40
#define  USBD_HID_MAIN_NONULLPOSITION                   0x00
#define  USBD_HID_MAIN_VOLATILE                         0x80
#define  USBD_HID_MAIN_NONVOLATILE                      0x00
#define  USBD_HID_MAIN_BUFFEREDBYTES                  0x0100
#define  USBD_HID_MAIN_BITFIELD                       0x0000
//                                                                 Collection item (see Note #1g).
#define  USBD_HID_COLLECTION_PHYSICAL                   0x00
#define  USBD_HID_COLLECTION_APPLICATION                0x01
#define  USBD_HID_COLLECTION_LOGICAL                    0x02
#define  USBD_HID_COLLECTION_REPORT                     0x03
#define  USBD_HID_COLLECTION_NAMEDARRAY                 0x04
#define  USBD_HID_COLLECTION_USAGESWITCH                0x05
#define  USBD_HID_COLLECTION_USAGEMODIFIER              0x06
//                                                                 Global item (see Note #1h).
#define  USBD_HID_GLOBAL_USAGE_PAGE                     0x04
#define  USBD_HID_GLOBAL_LOG_MIN                        0x14
#define  USBD_HID_GLOBAL_LOG_MAX                        0x24
#define  USBD_HID_GLOBAL_PHY_MIN                        0x34
#define  USBD_HID_GLOBAL_PHY_MAX                        0x44
#define  USBD_HID_GLOBAL_UNIT_EXPONENT                  0x54
#define  USBD_HID_GLOBAL_UNIT                           0x64
#define  USBD_HID_GLOBAL_REPORT_SIZE                    0x74
#define  USBD_HID_GLOBAL_REPORT_ID                      0x84
#define  USBD_HID_GLOBAL_REPORT_COUNT                   0x94
#define  USBD_HID_GLOBAL_PUSH                           0xA4
#define  USBD_HID_GLOBAL_POP                            0xB4
//                                                                 Local item (see Note #1i).
#define  USBD_HID_LOCAL_USAGE                           0x08
#define  USBD_HID_LOCAL_USAGE_MIN                       0x18
#define  USBD_HID_LOCAL_USAGE_MAX                       0x28
#define  USBD_HID_LOCAL_DESIGNATOR_INDEX                0x38
#define  USBD_HID_LOCAL_DESIGNATOR_MIN                  0x48
#define  USBD_HID_LOCAL_DESIGNATOR_MAX                  0x58
#define  USBD_HID_LOCAL_STRING_INDEX                    0x78
#define  USBD_HID_LOCAL_STRING_MIN                      0x88
#define  USBD_HID_LOCAL_STRING_MAX                      0x98
#define  USBD_HID_LOCAL_DELIMITER                       0xA8

/********************************************************************************************************
 *                                   HID PHYSICAL DESCRIPTOR DEFINES
 *
 * Note(s) : (1) See 'Device Class Definition for Human Interface Devices Version 1.11', Section 6.2.3.
 *******************************************************************************************************/

//                                                                 ---------- HID PHYSICAL DESC (see Note #1) ---------
//                                                                 Bias values.
#define  USBD_HID_BIAS_NOT_APPLICABLE                      0
#define  USBD_HID_BIAS_RIGHT_HAND                          1
#define  USBD_HID_BIAS_LEFT_HAND                           2
#define  USBD_HID_BIAS_BOTH_HANDS                          3
#define  USBD_HID_BIAS_EITHER_HAND                         4
//                                                                 Designator values.
#define  USBD_HID_DESIGNATOR_NONE                       0x00
#define  USBD_HID_DESIGNATOR_HAND                       0x01
#define  USBD_HID_DESIGNATOR_EYEBALL                    0x02
#define  USBD_HID_DESIGNATOR_EYEBROW                    0x03
#define  USBD_HID_DESIGNATOR_EYELID                     0x04
#define  USBD_HID_DESIGNATOR_EAR                        0x05
#define  USBD_HID_DESIGNATOR_NOSE                       0x06
#define  USBD_HID_DESIGNATOR_MOUTH                      0x07
#define  USBD_HID_DESIGNATOR_UPPER_LIP                  0x08
#define  USBD_HID_DESIGNATOR_LOWER_LIP                  0x09
#define  USBD_HID_DESIGNATOR_JAW                        0x0A
#define  USBD_HID_DESIGNATOR_NECK                       0x0B
#define  USBD_HID_DESIGNATOR_UPPER_ARM                  0x0C
#define  USBD_HID_DESIGNATOR_ELBOW                      0x0D
#define  USBD_HID_DESIGNATOR_FOREARM                    0x0E
#define  USBD_HID_DESIGNATOR_WRIST                      0x0F
#define  USBD_HID_DESIGNATOR_PALM                       0x10
#define  USBD_HID_DESIGNATOR_THUMB                      0x11
#define  USBD_HID_DESIGNATOR_INDEX_FINGER               0x12
#define  USBD_HID_DESIGNATOR_MIDDLE_FINGER              0x13
#define  USBD_HID_DESIGNATOR_RING_FINGER                0x14
#define  USBD_HID_DESIGNATOR_LITTLE_FINGER              0x15
#define  USBD_HID_DESIGNATOR_HEAD                       0x16
#define  USBD_HID_DESIGNATOR_SHOULDER                   0x17
#define  USBD_HID_DESIGNATOR_HIP                        0x18
#define  USBD_HID_DESIGNATOR_WAIST                      0x19
#define  USBD_HID_DESIGNATOR_THIGH                      0x1A
#define  USBD_HID_DESIGNATOR_KNEE                       0x1B
#define  USBD_HID_DESIGNATOR_CALF                       0x1C
#define  USBD_HID_DESIGNATOR_ANKLE                      0x1D
#define  USBD_HID_DESIGNATOR_FOOT                       0x1E
#define  USBD_HID_DESIGNATOR_HEEL                       0x1F
#define  USBD_HID_DESIGNATOR_BALL_OF_FOOT               0x20
#define  USBD_HID_DESIGNATOR_BIG_TOE                    0x21
#define  USBD_HID_DESIGNATOR_SECOND_TOE                 0x22
#define  USBD_HID_DESIGNATOR_THIRD_TOE                  0x23
#define  USBD_HID_DESIGNATOR_FOURTH_TOE                 0x24
#define  USBD_HID_DESIGNATOR_LITTLE_TOE                 0x25
#define  USBD_HID_DESIGNATOR_BROW                       0x26
#define  USBD_HID_DESIGNATOR_CHEEK                      0x27
//                                                                 Qualifier values.
#define  USBD_HID_QUALIFIER_NOT_APPLICABLE                 0
#define  USBD_HID_QUALIFIER_RIGHT                          1
#define  USBD_HID_QUALIFIER_LEFT                           2
#define  USBD_HID_QUALIFIER_BOTH                           3
#define  USBD_HID_QUALIFIER_EITHER                         4
#define  USBD_HID_QUALIFIER_CENTER                         5

/********************************************************************************************************
 *                                           HID USAGE PAGES
 *
 * Note(s) : (1) See 'Universal Serial Bus HID Usage Tables', Version 1.12, Section 3 for more details
 *               about Usage Pages.
 *
 *           (2) See 'Universal Serial Bus HID Usage Tables', Version 1.12, Section 4 for more details
 *               about Generic Desktop Page usages and controls.
 *******************************************************************************************************/
//                                                                 ------------- USAGE PAGES (see Note #1) ------------
#define  USBD_HID_USAGE_PAGE_UNDEFINED                  0x00
#define  USBD_HID_USAGE_PAGE_GENERIC_DESKTOP_CONTROLS   0x01
#define  USBD_HID_USAGE_PAGE_SIMULATION_CONTROLS        0x02
#define  USBD_HID_USAGE_PAGE_VR_CONTROLS                0x03
#define  USBD_HID_USAGE_PAGE_SPORT_CONTROLS             0x04
#define  USBD_HID_USAGE_PAGE_GAME_CONTROLS              0x05
#define  USBD_HID_USAGE_PAGE_GENERIC_DEVICE_CONTROLS    0x06
#define  USBD_HID_USAGE_PAGE_KEYBOARD                   0x07
#define  USBD_HID_USAGE_PAGE_LEDS                       0x08
#define  USBD_HID_USAGE_PAGE_BUTTON                     0x09
#define  USBD_HID_USAGE_PAGE_ORDINAL                    0x0A
#define  USBD_HID_USAGE_PAGE_TELEPHONY                  0x0B
#define  USBD_HID_USAGE_PAGE_CONSUMER                   0x0C
#define  USBD_HID_USAGE_PAGE_DIGITIZER                  0x0D
#define  USBD_HID_USAGE_PAGE_PID_PAGE                   0x0F
#define  USBD_HID_USAGE_PAGE_UNICODE                    0x10
#define  USBD_HID_USAGE_PAGE_ALPHANUMERIC_DISPLAY       0x14
#define  USBD_HID_USAGE_PAGE_MEDICAL_INSTRUMENTS        0x40
#define  USBD_HID_USAGE_PAGE_MONITOR_0                  0x80
#define  USBD_HID_USAGE_PAGE_MONITOR_1                  0x81
#define  USBD_HID_USAGE_PAGE_MONITOR_2                  0x82
#define  USBD_HID_USAGE_PAGE_MONITOR_3                  0x83
#define  USBD_HID_USAGE_PAGE_POWER_0                    0x84
#define  USBD_HID_USAGE_PAGE_POWER_1                    0x85
#define  USBD_HID_USAGE_PAGE_POWER_2                    0x86
#define  USBD_HID_USAGE_PAGE_POWER_3                    0x87
#define  USBD_HID_USAGE_PAGE_BAR_CODE_SCANNER_PAGE      0x8C
#define  USBD_HID_USAGE_PAGE_SCALE_PAGE                 0x8D
#define  USBD_HID_USAGE_PAGE_MSR_DEVICES                0x8E
#define  USBD_HID_USAGE_PAGE_POINT_OF_SALE_PAGES        0x8F
#define  USBD_HID_USAGE_PAGE_CAMERA_CONTROL_PAGE        0x90
#define  USBD_HID_USAGE_PAGE_ARCADE_PAGE                0x91

//                                                                 -------- GENERIC DESKTOP PAGE USAGES & CTRL --------
//                                                                 See Note #2.
#define  USBD_HID_CP_POINTER                            0x01
#define  USBD_HID_CA_MOUSE                              0x02
#define  USBD_HID_CA_JOYSTICK                           0x04
#define  USBD_HID_CA_GAME_PAD                           0x05
#define  USBD_HID_CA_KEYBOARD                           0x06
#define  USBD_HID_CA_KEYPAD                             0x07
#define  USBD_HID_CA_MULTI_AXIS_CONTROLLER              0x08
#define  USBD_HID_DV_X                                  0x30
#define  USBD_HID_DV_Y                                  0x31
#define  USBD_HID_DV_Z                                  0x32
#define  USBD_HID_DV_WHEEL                              0x38
#define  USBD_HID_CA_SYSTEM_CONTROL                     0x80

/********************************************************************************************************
 *                                               DATA TYPES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           GLOBAL VARIABLES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MACRO'S
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *******************************************************************************************************/

/********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MODULE END
 *******************************************************************************************************/

#endif
