/***************************************************************************//**
 * @file
 * @brief USB Host Controllers-Hub Interface
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
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_HCD_HUB_PRIV_H_
#define  _USBH_HCD_HUB_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  USBH_HUB_NBR_PORT                                7u    // Max nbr of port on external hub.

//                                                                 ----------------- HUB PORT NUMBER ------------------
#define  USBH_HUB_PORT_ALL                              255u
#define  USBH_HUB_PORT_HUB_STATUS                         0u

//                                                                 ------------- HUB STATUS CHANGE BITMAP -------------
#define  USBH_HUB_STATUS_CHNG                           DEF_BIT_00

/********************************************************************************************************
 *                                           HUB FEATURE SELECTORS
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2, Table 11-17.
 *
 *           (2) For a 'clear feature' setup request, the 'wValue' field may contain one of these values.
 *******************************************************************************************************/

#define  USBH_HUB_FEATURE_SEL_C_HUB_LOCAL_PWR              0u
#define  USBH_HUB_FEATURE_SEL_C_HUB_OVER_CUR               1u

#define  USBH_HUB_FEATURE_SEL_PORT_CONN                    0u
#define  USBH_HUB_FEATURE_SEL_PORT_EN                      1u
#define  USBH_HUB_FEATURE_SEL_PORT_SUSPEND                 2u
#define  USBH_HUB_FEATURE_SEL_PORT_OVER_CUR                3u
#define  USBH_HUB_FEATURE_SEL_PORT_RESET                   4u
#define  USBH_HUB_FEATURE_SEL_PORT_PWR                     8u
#define  USBH_HUB_FEATURE_SEL_PORT_LOW_SPD                 9u
#define  USBH_HUB_FEATURE_SEL_C_PORT_CONN                  16u
#define  USBH_HUB_FEATURE_SEL_C_PORT_EN                    17u
#define  USBH_HUB_FEATURE_SEL_C_PORT_SUSPEND               18u
#define  USBH_HUB_FEATURE_SEL_C_PORT_OVER_CUR              19u
#define  USBH_HUB_FEATURE_SEL_C_PORT_RESET                 20u
#define  USBH_HUB_FEATURE_SEL_PORT_TEST                    21u
#define  USBH_HUB_FEATURE_SEL_PORT_INDICATOR               22u

/********************************************************************************************************
 *                                       HUB STATUS (CHANGE) BITS
 *
 * Note(s) : (1) wHubStatus is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               +---------+------------------------------------------+
 *               | 0       | Local power source good/lost.            |
 *               +---------+------------------------------------------+
 *               | 1       | Over-current condition.                  |
 *               +---------+------------------------------------------+
 *               | 2..15   | Reserved.                                |
 *               +---------+------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2.6,
 *                   Table 11-19.
 *
 *           (2) wHubStatusChange is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               |---------+------------------------------------------+
 *               | 0       | Local power status change.               |
 *               |---------+------------------------------------------+
 *               | 1       | Over-current status change.              |
 *               |---------+------------------------------------------+
 *               | 2..15   | Reserved.                                |
 *               +---------+------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2.6,
 *                   Table 11-20.
 *******************************************************************************************************/

//                                                                 See note (1).
#define  USBH_HUB_STATUS_LOCAL_POWER                  DEF_BIT_00
#define  USBH_HUB_STATUS_OVER_CUR                     DEF_BIT_01

//                                                                 See note (2).
#define  USBH_HUB_STATUS_C_LOCAL_POWER                DEF_BIT_00
#define  USBH_HUB_STATUS_C_OVER_CUR                   DEF_BIT_01

/********************************************************************************************************
 *                                       HUB PORT STATUS (CHANGE) BITS
 *
 * Note(s) : (1) wPortStatus is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               +---------+------------------------------------------+
 *               | 0       | Current connect status.                  |
 *               +---------+------------------------------------------+
 *               | 1       | Port enabled/disabled.                   |
 *               +---------+------------------------------------------+
 *               | 2       | Suspend.                                 |
 *               +---------+------------------------------------------+
 *               | 3       | Over-current.                            |
 *               +---------+------------------------------------------+
 *               | 4       | Reset.                                   |
 *               +---------+------------------------------------------+
 *               | 5..7    | Reserved.                                |
 *               +---------+------------------------------------------+
 *               | 8       | Port power.                              |
 *               +---------+------------------------------------------+
 *               | 9       | Low-speed device attached.               |
 *               +---------+------------------------------------------+
 *               | 10      | High-speed device attached.              |
 *               +---------+------------------------------------------+
 *               | 11      | Port test mode.                          |
 *               +---------+------------------------------------------+
 *               | 12      | Port indicator control.                  |
 *               +---------+------------------------------------------+
 *               | 13..15  | Reserved.                                |
 *               +---------+------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2.7.1,
 *                   Table 11-21.
 *
 *           (2) wPortStatusChange is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               |---------+------------------------------------------+
 *               | 0       | Connect status change.                   |
 *               |---------+------------------------------------------+
 *               | 1       | Port enabled/disabled change.            |
 *               |---------+------------------------------------------+
 *               | 2       | Suspend change.                          |
 *               |---------+------------------------------------------+
 *               | 3       | Over-current indicator change.           |
 *               |---------+------------------------------------------+
 *               | 4       | Reset change.                            |
 *               |---------+------------------------------------------+
 *               | 5..15   | Reserved.                                |
 *               +---------+------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 11.24.2.7.2,
 *                   Table 11-22.
 *******************************************************************************************************/

//                                                                 See note (1).
#define  USBH_HUB_STATUS_PORT_CONN                    DEF_BIT_00
#define  USBH_HUB_STATUS_PORT_EN                      DEF_BIT_01
#define  USBH_HUB_STATUS_PORT_SUSPEND                 DEF_BIT_02
#define  USBH_HUB_STATUS_PORT_OVER_CUR                DEF_BIT_03
#define  USBH_HUB_STATUS_PORT_RESET                   DEF_BIT_04
#define  USBH_HUB_STATUS_PORT_PWR                     DEF_BIT_08
#define  USBH_HUB_STATUS_PORT_SPD_MASK               (DEF_BIT_09 | DEF_BIT_10)
#define  USBH_HUB_STATUS_PORT_SPD_LOW                 DEF_BIT_09
#define  USBH_HUB_STATUS_PORT_SPD_FULL                DEF_BIT_NONE
#define  USBH_HUB_STATUS_PORT_SPD_HIGH                DEF_BIT_10
#define  USBH_HUB_STATUS_PORT_TEST                    DEF_BIT_11
#define  USBH_HUB_STATUS_PORT_INDICATOR               DEF_BIT_12

#define  USBH_HUB_STATUS_PORT_SPD_GET(port_status)   ((port_status) & USBH_HUB_STATUS_PORT_SPD_MASK)

//                                                                 See note (2).
#define  USBH_HUB_STATUS_C_PORT_CONN                  DEF_BIT_00
#define  USBH_HUB_STATUS_C_PORT_EN                    DEF_BIT_01
#define  USBH_HUB_STATUS_C_PORT_SUSPEND               DEF_BIT_02
#define  USBH_HUB_STATUS_C_PORT_OVER_CUR              DEF_BIT_03
#define  USBH_HUB_STATUS_C_PORT_RESET                 DEF_BIT_04

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void USBH_HUB_RootEvent(CPU_INT08U host_nbr,
                        CPU_INT08U hc_nbr,
                        CPU_INT08U port_status_chng_bitmap);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
