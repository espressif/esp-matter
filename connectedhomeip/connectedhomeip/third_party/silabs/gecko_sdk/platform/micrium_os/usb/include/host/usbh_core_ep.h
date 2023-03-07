/***************************************************************************//**
 * @file
 * @brief USB Host Endpoint Operations
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

#ifndef  _USBH_CORE_EP_H_
#define  _USBH_CORE_EP_H_

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
 *                                           DESCRIPTOR TYPES
 *
 * Note(s) : (1) See 'Universal Serial Bus Specification Revision 2.0', Section 9.4, Table 9-5, and
 *               Section 9.4.3.
 *
 *           (2) For a 'get descriptor' setup request, the low byte of the 'wValue' field may contain
 *               one of these values.
 *******************************************************************************************************/

#define  USBH_DESC_TYPE_EP                                 5u

#define  USBH_DESC_LEN_EP                                  7u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               USB URB TOKEN
 *******************************************************************************************************/

typedef enum usbh_token {
  USBH_TOKEN_SETUP = 0u,
  USBH_TOKEN_OUT = 1u,
  USBH_TOKEN_IN = 2u
} USBH_TOKEN;

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       ENDPOINT CONVERSION MACROS
 *******************************************************************************************************/

#define  USBH_EP_PHY_TO_ADDR(ep_phy_nbr)                 ( ((ep_phy_nbr) / 2u) \
                                                           | ((DEF_BIT_IS_SET(ep_phy_nbr, DEF_BIT_00) == DEF_YES) ? USBH_EP_DIR_BIT : DEF_BIT_NONE))

#define  USBH_EP_ADDR_TO_PHY(ep_addr)                    ((((ep_addr) & USBH_EP_NBR_MASK) *  2u) \
                                                          + ((((ep_addr) & USBH_EP_DIR_MASK) != 0u) ? 1u : 0u))

#define  USBH_EP_ADDR_TO_LOG(ep_addr)                    (CPU_INT08U)((ep_addr) & USBH_EP_NBR_MASK)

#define  USBH_EP_PHY_TO_LOG(ep_phy_nbr)                  (ep_phy_nbr / 2u)

#define  USBH_EP_LOG_TO_ADDR_IN(ep_log_nbr)              ((ep_log_nbr) | USBH_EP_DIR_IN)

#define  USBH_EP_LOG_TO_ADDR_OUT(ep_log_nbr)             ((ep_log_nbr) | USBH_EP_DIR_OUT)

#define  USBH_EP_IS_IN(ep_addr)                          ((((ep_addr) & USBH_EP_DIR_MASK) != 0u) ? DEF_YES : DEF_NO)

#define  USBH_EP_TYPE_GET(attr)                          (CPU_INT08U)((attr) & USBH_EP_TYPE_MASK)

#define  USBH_EP_SYNC_GET(attr)                          (CPU_INT08U)((attr) & USBH_EP_TYPE_SYNC_MASK)

#define  USBH_EP_USAGE_GET(attr)                         (CPU_INT08U)((attr) & USBH_EP_TYPE_USAGE_MASK)

#define  USBH_EP_MAX_PKT_SIZE_GET(max_pkt_size)         ((max_pkt_size) & USBH_EP_MAX_PKT_SIZE_MASK)

#define  USBH_EP_NBR_TRANSACTION_GET(max_pkt_size)      ((max_pkt_size) & USBH_EP_NBR_TRANSACTION_MASK)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_INT08U USBH_EP_AttribGet(USBH_DEV_HANDLE  dev_handle,
                             USBH_FNCT_HANDLE fnct_handle,
                             CPU_INT08U       if_ix,
                             CPU_INT08U       ep_addr,
                             RTOS_ERR         *p_err);

CPU_INT16U USBH_EP_MaxPktSizeGet(USBH_DEV_HANDLE  dev_handle,
                                 USBH_FNCT_HANDLE fnct_handle,
                                 CPU_INT08U       if_ix,
                                 CPU_INT08U       ep_addr,
                                 RTOS_ERR         *p_err);

#if (USBH_CFG_PERIODIC_XFER_EN == DEF_ENABLED)
CPU_INT08U USBH_EP_IntervalGet(USBH_DEV_HANDLE  dev_handle,
                               USBH_FNCT_HANDLE fnct_handle,
                               CPU_INT08U       if_ix,
                               CPU_INT08U       ep_addr,
                               RTOS_ERR         *p_err);
#endif

CPU_INT16U USBH_EP_CtrlXfer(USBH_DEV_HANDLE dev_handle,
                            CPU_INT08U      req,
                            CPU_INT08U      req_type,
                            CPU_INT16U      val,
                            CPU_INT16U      ix,
                            CPU_INT08U      *p_buf,
                            CPU_INT16U      len,
                            CPU_INT16U      buf_len,
                            CPU_INT32U      timeout_ms,
                            RTOS_ERR        *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif
