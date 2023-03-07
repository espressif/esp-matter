/***************************************************************************//**
 * @file
 * @brief Network - Sntp Client Module
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
 * @defgroup NET_SNTP SNTP Client API
 * @ingroup  NET
 * @brief    SNTP Client API
 *
 * @addtogroup NET_SNTP
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _SNTP_CLIENT_H_
#define  _SNTP_CLIENT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_ip.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       SNTP DFLT CONFIG VALUE
 *******************************************************************************************************/

#define  SNTP_CLIENT_CFG_SERVER_PORT_NBR_DFLT                   123u
#define  SNTP_CLIENT_CFG_SERVER_ADDR_FAMILY_DFLT                NET_IP_ADDR_FAMILY_NONE
#define  SNTP_CLIENT_CFG_REQ_RX_TIMEOUT_MS_DFLT                 5000u

/********************************************************************************************************
 *                                            SNTP INIT CFG
 * @brief SNTP initialization configuration
 *******************************************************************************************************/

typedef  struct  sntp_init_cfg {
  MEM_SEG *MemSegPtr;                                           // Ptr to mem seg used for internal data alloc.
} SNTPc_INIT_CFG;

/********************************************************************************************************
 *                                    SNTP MESSAGE PACKET DATA TYPE
 *
 * @brief SNTP message packet
 *
 * @note (1) See RFC #2030, Section 4 for NTP message format.
 *
 * @note (2) The first 32 bits of the NTP message (CW) contain the following fields :
 *               @verbatim
 *               1                   2                   3
 *               0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *               |LI | VN  |Mode |    Stratum    |     Poll      |   Precision   |
 *               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *               @endverbatim
 *******************************************************************************************************/

typedef struct sntp_ts {
  CPU_INT32U Sec;
  CPU_INT32U Frac;
} SNTP_TS;

typedef struct sntp_pkt {
  CPU_INT32U CW;                                                // Ctrl word (see Note #2).
  CPU_INT32U RootDly;                                           // Round trip dly.
  CPU_INT32U RootDispersion;                                    // Nominal err returned by server.
  CPU_INT32U RefID;                                             // Server ref ID.
  SNTP_TS    TS_Ref;                                            // Timestamp of the last sync.
  SNTP_TS    TS_Originate;                                      // Local timestamp when sending req.
  SNTP_TS    TS_Rx;                                             // Remote timestamp when receiving req.
  SNTP_TS    TS_Tx;                                             // Remote timestamp when sending res.
} SNTP_PKT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const SNTPc_INIT_CFG SNTPc_InitCfgDflt;                  // SNTPc dflt configurations.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void SNTPc_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void SNTPc_Init(RTOS_ERR *p_err);

void SNTPc_DfltCfgSet(NET_PORT_NBR       port_nbr,
                      NET_IP_ADDR_FAMILY addr_family,
                      CPU_INT32U         rx_timeout_ms,
                      RTOS_ERR           *p_err);

void SNTPc_ReqRemoteTime(CPU_CHAR *hostname,
                         SNTP_PKT *p_pkt,
                         RTOS_ERR *p_err);

SNTP_TS SNTPc_GetRemoteTime(SNTP_PKT *ppkt,
                            RTOS_ERR *p_err);

CPU_INT32U SNTPc_GetRoundTripDly_us(SNTP_PKT *ppkt,
                                    RTOS_ERR *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/
#endif // _SNTP_CLIENT_H_
