/***************************************************************************//**
 * @file
 * @brief USB Host Class-Endpoint Interface
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

#ifndef  _USBH_CLASS_EP_PRIV_H_
#define  _USBH_CLASS_EP_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>
#include  <usbh_cfg.h>

#include  <usb/include/host/usbh_core.h>
#include  <usb/include/host/usbh_core_handle.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   ENDPOINT ADDRESS (bEndpointAddress)
 *
 * Note(s) : (1) The bEndpointAddress field from the endpoint descriptor is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               |---------+------------------------------------------+
 *               | 0..3    | Endpoint number.                         |
 *               |---------+------------------------------------------+
 *               | 4..6    | Reserved.                                |
 *               |---------+------------------------------------------+
 *               | 7       | Direction.                               |
 *               |---------+------------------------------------------+
 *
 *           (2) See 'Universal Serial Bus Specification Revision 2.0', Section 9.6.6, Table 9-13
 *******************************************************************************************************/

#define  USBH_EP_NBR_MASK                               0x0Fu
#define  USBH_EP_MAX_NBR                                  32u

#define  USBH_EP_DIR_MASK                               0x80u
#define  USBH_EP_DIR_BIT                                DEF_BIT_07
#define  USBH_EP_DIR_OUT                                DEF_BIT_NONE
#define  USBH_EP_DIR_IN                                 DEF_BIT_07

/********************************************************************************************************
 *                                   ENDPOINT ATTRIBUTES (bmAttributes)
 *
 * Note(s) : (1) The bmAttributes field from the endpoint descriptor is organized the following way:
 *
 *               +---------+------------------------------------------+
 *               | BITS    | Description                              |
 *               |---------+------------------------------------------+
 *               | 0..1    | Transfer type (CTRL, ISOC, BULK, INTR).  |
 *               |---------+------------------------------------------+
 *               | 2..5    | Reserved if CTRL, BULK or INTR.          |
 *               |---------+------------------------------------------+
 *               | 2..3    | Synchronisation type (ISOC).             |
 *               |---------+------------------------------------------+
 *               | 4..5    | Usage type (ISOC).                       |
 *               +---------+------------------------------------------+
 *               | 6..7    | Reserved.                                |
 *               +---------+------------------------------------------+
 *
 *           (2) See 'Universal Serial Bus Specification Revision 2.0', Section 9.6.6, Table 9-13
 *******************************************************************************************************/

#define  USBH_EP_TYPE_MASK                            (DEF_BIT_00 | DEF_BIT_01)
#define  USBH_EP_TYPE_CTRL                             DEF_BIT_NONE
#define  USBH_EP_TYPE_ISOC                             DEF_BIT_00
#define  USBH_EP_TYPE_BULK                             DEF_BIT_01
#define  USBH_EP_TYPE_INTR                            (DEF_BIT_00 | DEF_BIT_01)

#define  USBH_EP_TYPE_SYNC_MASK                       (DEF_BIT_02 | DEF_BIT_03)
#define  USBH_EP_TYPE_SYNC_NONE                        DEF_BIT_NONE
#define  USBH_EP_TYPE_SYNC_ASYNC                       DEF_BIT_02
#define  USBH_EP_TYPE_SYNC_ADAPTIVE                    DEF_BIT_03
#define  USBH_EP_TYPE_SYNC_SYNC                       (DEF_BIT_02 | DEF_BIT_03)

#define  USBH_EP_TYPE_USAGE_MASK                      (DEF_BIT_04 | DEF_BIT_05)
#define  USBH_EP_TYPE_USAGE_DATA                       DEF_BIT_NONE
#define  USBH_EP_TYPE_USAGE_FEEDBACK                   DEF_BIT_04
#define  USBH_EP_TYPE_USAGE_EXPLICIT_FEEDBACK          DEF_BIT_05

/********************************************************************************************************
 *                               ENDPOINT MAXIMUM PACKET SIZE (wMaxPacketSize)
 *
 * Note(s) : (1) The wMaxPacketSize field from the endpoint descriptor is organized the following way:
 *
 *               +---------+------------------------------------+
 *               | BITS    | Description                        |
 *               |---------+------------------------------------+
 *               | 0..10   | Maximum packet size in bytes.      |
 *               |---------+------------------------------------+
 *               | 11..12  | Number of transaction per uframe.  |
 *               |---------+------------------------------------+
 *               | 13..15  | Reserved.                          |
 *               +---------+------------------------------------+
 *
 *           (2) See 'Universal Serial Bus Specification Revision 2.0', Section 9.6.6
 *               Bits 12..11 from wMaxPacketSize indicates the number of transaction per microframe.
 *               Valid for high-speed isochronous and interrupt endpoint only.
 *******************************************************************************************************/

#define  USBH_EP_MAX_PKT_SIZE_MASK                       DEF_BIT_FIELD(10u, 0u)

#define  USBH_EP_NBR_TRANSACTION_MASK                   (DEF_BIT_12 | DEF_BIT_11)
#define  USBH_EP_NBR_TRANSACTION_1                       DEF_BIT_NONE
#define  USBH_EP_NBR_TRANSACTION_2                       DEF_BIT_11
#define  USBH_EP_NBR_TRANSACTION_3                       DEF_BIT_12

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   XFER COMPLETE NOTIFICATION FNCT
 *******************************************************************************************************/

typedef void (*USBH_ASYNC_FNCT)(USBH_DEV_HANDLE dev_handle,
                                USBH_EP_HANDLE  ep_handle,
                                CPU_INT08U      *p_buf,
                                CPU_INT32U      buf_len,
                                CPU_INT32U      xfer_len,
                                void            *p_arg,
                                RTOS_ERR        err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT08U USBH_EP_AddrGet(USBH_EP_HANDLE ep_handle,
                           RTOS_ERR       *p_err);

CPU_INT08U *USBH_EP_DescExtraGet(USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       ep_addr,
                                 CPU_INT16U       *p_desc_extra_len,
                                 RTOS_ERR         *p_err);

CPU_INT08U USBH_EP_IF_IxGet(USBH_EP_HANDLE ep_handle,
                            RTOS_ERR       *p_err);

void USBH_EP_StallSet(USBH_DEV_HANDLE dev_handle,
                      USBH_EP_HANDLE  ep_handle,
                      CPU_BOOLEAN     stall_set,
                      RTOS_ERR        *p_err);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT32U USBH_EP_IntrRx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err);

void USBH_EP_IntrRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);

CPU_INT32U USBH_EP_IntrTx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err);

void USBH_EP_IntrTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);
#endif

CPU_INT32U USBH_EP_BulkRx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err);

void USBH_EP_BulkRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);

CPU_INT32U USBH_EP_BulkTx(USBH_DEV_HANDLE dev_handle,
                          USBH_EP_HANDLE  ep_handle,
                          CPU_INT08U      *p_buf,
                          CPU_INT32U      buf_len,
                          CPU_INT32U      timeout,
                          RTOS_ERR        *p_err);

void USBH_EP_BulkTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);

#if ((USBH_CFG_ISOC_EN == DEF_ENABLED) \
  && (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED))
void USBH_EP_IsocRxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         CPU_INT16U      frm_start,
                         CPU_INT08U      frm_cnt,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);

void USBH_EP_IsocTxAsync(USBH_DEV_HANDLE dev_handle,
                         USBH_EP_HANDLE  ep_handle,
                         CPU_INT08U      *p_buf,
                         CPU_INT32U      buf_len,
                         CPU_INT16U      frm_start,
                         CPU_INT08U      frm_cnt,
                         USBH_ASYNC_FNCT async_fnct,
                         void            *p_async_arg,
                         RTOS_ERR        *p_err);
#endif

void USBH_EP_Abort(USBH_DEV_HANDLE dev_handle,
                   USBH_EP_HANDLE  ep_handle,
                   RTOS_ERR        *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
