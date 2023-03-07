/***************************************************************************//**
 * @file
 * @brief USB Host Hub Operations
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
 * @defgroup USBH_HUB USB Host HUB API
 * @ingroup USBH
 * @brief   USB Host HUB API
 *
 * @addtogroup USBH_HUB
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _USBH_CORE_HUB_H_
#define  _USBH_CORE_HUB_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

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
 *                                       HUB CHARACTERISTICS BITS
 *
 * Note(s) : (1) wHubCharacteristics is organized the following way:
 *
 *               +--------+----------------------------------------------+
 *               | BITS   | Description                                  |
 *               +--------+----------------------------------------------+
 *               |  0..1  | Logical Power Switching Mode                 |
 *               |        |  00: Ganged     power switching.             |
 *               |        |  01: Individual power switching.             |
 *               |        |  1x: Reserved.                               |
 *               +--------+----------------------------------------------+
 *               |  2     | Identifies a compound device.                |
 *               |        |   0: Hub is not part of a compound device.   |
 *               |        |   1: Hob is     part of a compound device.   |
 *               +--------+----------------------------------------------+
 *               |  3..4  | Over-current protection mode.                |
 *               |        |  00: Global     over-current protection.     |
 *               |        |  01: Individual over-current protection.     |
 *               |        |  1x: No         over-current protection.     |
 *               +--------+----------------------------------------------+
 *               |  5..6  | Think time.                                  |
 *               |        |  00: TT requires at most  8 FS bit times.    |
 *               |        |  01: TT requires at most 16 FS bit times.    |
 *               |        |  10: TT requires at most 24 FS bit times.    |
 *               |        |  11: TT requires at most 32 FS bit times.    |
 *               +--------+----------------------------------------------+
 *               |  7     | Port indicators support.                     |
 *               |        |   0: Port indicators are not supported.      |
 *               |        |   1: Port indicators are     supported.      |
 *               +--------+----------------------------------------------+
 *               |  8..15 | Reserved.                                    |
 *               +--------+----------------------------------------------+
 *
 *               (a) See 'Universal Serial Bus Specification Revision 2.0', Section 11.23.2.1,
 *                   Table 11-13.
 *******************************************************************************************************/

//                                                                 ----------------- HUB POWER MODES ------------------
#define  USBH_HUB_PWR_MODE_MASK                         (DEF_BIT_00 | DEF_BIT_01)
#define  USBH_HUB_PWR_MODE_GANGED                        DEF_BIT_NONE
#define  USBH_HUB_PWR_MODE_INDIVIDUAL                    DEF_BIT_00
#define  USBH_HUB_PWR_MODE_GET(characteristics)         ((characteristics) & USBH_HUB_PWR_MODE_MASK)

//                                                                 --------------- HUB COUMPOUND STATUS ---------------
#define  USBH_HUB_COMPOUND_MASK                          DEF_BIT_02

//                                                                 --------- HUB OVER-CURRENT DETECTION MODE ----------
#define  USBH_HUB_OVER_CUR_MASK                         (DEF_BIT_03 | DEF_BIT_04)
#define  USBH_HUB_OVER_CUR_GLOBAL                        DEF_BIT_NONE
#define  USBH_HUB_OVER_CUR_INDIVIDUAL                    DEF_BIT_03
#define  USBH_HUB_OVER_CUR_NONE                          DEF_BIT_04
#define  USBH_HUB_OVER_CUR_GET(characteristics)         ((characteristics) & USBH_HUB_OVER_CUR_MASK)

//                                                                 --------------- HUB THINK TIME (TT) ----------------
#define  USBH_HUB_TT_MASK                               (DEF_BIT_05 | DEF_BIT_06)
#define  USBH_HUB_TT_8                                   DEF_BIT_NONE
#define  USBH_HUB_TT_16                                  DEF_BIT_05
#define  USBH_HUB_TT_24                                  DEF_BIT_06
#define  USBH_HUB_TT_32                                 (DEF_BIT_05 | DEF_BIT_06)
#define  USBH_HUB_TT_GET(characteristics)               ((characteristics) & USBH_HUB_TT_MASK)

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                           HUB PORT REQUESTS
 *******************************************************************************************************/

typedef enum usbh_hub_port_req {
  USBH_HUB_PORT_REQ_SUSPEND = 0u,                               ///< Hub port request suspend.
  USBH_HUB_PORT_REQ_RESUME,                                     ///< Hub port request resume.
  USBH_HUB_PORT_REQ_RESET,                                      ///< Hub port request reset.
  USBH_HUB_PORT_REQ_DISCONN                                     ///< Hub port request disconn.
} USBH_HUB_PORT_REQ;

typedef void (*USBH_PORT_REQ_CMPL)       (USBH_DEV_HANDLE   hub_dev_handle,
                                          USBH_HUB_PORT_REQ port_req,
                                          CPU_INT08U        port_nbr,
                                          void              *p_arg,
                                          RTOS_ERR          err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void USBH_HUB_TaskPrioSet(CPU_INT08U prio,
                          RTOS_ERR   *p_err);

void USBH_HUB_PortSuspendReq(USBH_DEV_HANDLE    hub_dev_handle,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err);

void USBH_HUB_PortResumeReq(USBH_DEV_HANDLE    hub_dev_handle,
                            CPU_INT08U         port_nbr,
                            USBH_PORT_REQ_CMPL cmpl_callback,
                            void               *p_arg,
                            RTOS_ERR           *p_err);

void USBH_HUB_PortResetReq(USBH_DEV_HANDLE    hub_dev_handle,
                           CPU_INT08U         port_nbr,
                           USBH_PORT_REQ_CMPL cmpl_callback,
                           void               *p_arg,
                           RTOS_ERR           *p_err);

void USBH_HUB_PortDisconnReq(USBH_DEV_HANDLE    hub_dev_handle,
                             CPU_INT08U         port_nbr,
                             USBH_PORT_REQ_CMPL cmpl_callback,
                             void               *p_arg,
                             RTOS_ERR           *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  USBH_HUB_CFG_EXT_HUB_EN
#error  "USBH_HUB_CFG_EXT_HUB_EN               not #define'd in 'usbh_cfg.h'"
#elif   ((USBH_HUB_CFG_EXT_HUB_EN != DEF_ENABLED) \
  && (USBH_HUB_CFG_EXT_HUB_EN != DEF_DISABLED))
#error  "USBH_HUB_CFG_EXT_HUB_EN               illegally #define'd in 'usbh_cfg.h'"
#error  "                                      [MUST be set to DEF_ENABLED or DEF_DISABLED]"
#endif

#if ((USBH_HUB_CFG_EXT_HUB_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN == DEF_DISABLED))
#error  "USBH_CFG_PERIODIC_XFER_EN   illegally #define'd in 'usbh_cfg.h', MUST be set to DEF_ENABLED"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
