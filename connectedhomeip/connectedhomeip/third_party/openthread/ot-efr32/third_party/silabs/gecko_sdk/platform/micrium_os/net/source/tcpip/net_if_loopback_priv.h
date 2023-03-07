/***************************************************************************//**
 * @file
 * @brief Network Loopback Interface Layer
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

#ifndef  _NET_IF_LOOPBACK_PRIV_H_
#define  _NET_IF_LOOPBACK_PRIV_H_

#include  "../../include/net_cfg_net.h"

#ifdef   NET_IF_LOOPBACK_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_if_loopback.h>
#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/tcpip/net_if_802x_priv.h>
#include  <net/source/tcpip/net_type_priv.h>
#include  <net/source/tcpip/net_stat_priv.h>
#include  <net/source/tcpip/net_buf_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_IF_LOOPBACK_BUF_RX_LEN_MIN     NET_IF_ETHER_FRAME_MIN_SIZE
#define  NET_IF_LOOPBACK_BUF_TX_LEN_MIN     NET_IF_802x_BUF_TX_LEN_MIN

/********************************************************************************************************
 *                                   NETWORK INTERFACE HEADER DEFINES
 *
 * Note(s) : (1) The following network interface value MUST be pre-#define'd in 'net_def.h' PRIOR to
 *               'net_cfg.h' so that the developer can configure the network interface for the correct
 *               network interface layer values (see 'net_def.h  NETWORK INTERFACE LAYER DEFINES' &
 *               'net_cfg_net.h  NETWORK INTERFACE LAYER CONFIGURATION  Note #4') :
 *
 *               (a) NET_IF_HDR_SIZE_LOOPBACK               0
 *******************************************************************************************************/

#if 0
#define  NET_IF_HDR_SIZE_LOOPBACK                          0    // See Note #1a.
#endif

#define  NET_IF_HDR_SIZE_LOOPBACK_MIN                    NET_IF_HDR_SIZE_LOOPBACK
#define  NET_IF_HDR_SIZE_LOOPBACK_MAX                    NET_IF_HDR_SIZE_LOOPBACK

/********************************************************************************************************
 *               NETWORK LOOPBACK INTERFACE SIZE & MAXIMUM TRANSMISSION UNIT (MTU) DEFINES
 *
 * Note(s) : (1) The loopback interface is NOT linked to, associated with, or handled by any physical
 *               network device(s) & therefore has NO physical protocol overhead.
 *
 *               See also 'net_if_loopback.h  Note #1a'.
 *******************************************************************************************************/
//                                                                 See Note #1.
#define  NET_IF_MTU_LOOPBACK                             NET_MTU_MAX_VAL

#define  NET_IF_LOOPBACK_BUF_SIZE_MIN                   (NET_IF_LOOPBACK_SIZE_MIN + NET_BUF_DATA_SIZE_MIN - NET_BUF_DATA_PROTOCOL_HDR_SIZE_MIN)

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern const NET_IF_CFG_LOOPBACK NetIF_Cfg_Loopback;            // Net loopback IF cfg.

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetIF_Loopback_Init(MEM_SEG  *p_mem_seg,
                         RTOS_ERR *p_err);

//                                                                 --------------------- RX FNCTS ---------------------
NET_BUF_SIZE NetIF_Loopback_Rx(NET_IF   *p_if,
                               RTOS_ERR *p_err);

//                                                                 --------------------- TX FNCTS ---------------------
NET_BUF_SIZE NetIF_Loopback_Tx(NET_IF   *p_if,
                               NET_BUF  *p_buf_tx,
                               RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_LOOPBACK_MODULE_EN
#endif // _NET_IF_LOOPBACK_PRIV_H_
