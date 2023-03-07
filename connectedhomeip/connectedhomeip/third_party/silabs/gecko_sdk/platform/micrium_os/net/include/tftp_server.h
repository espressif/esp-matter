/***************************************************************************//**
 * @file
 * @brief Network - Tftp Server Module
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
 * @defgroup NET_TFTPs TFTP Server API
 * @ingroup  NET
 * @brief    TFTP Server API
 *
 * @addtogroup NET_TFTPs
 * @{
 ********************************************************************************************************
 * @note     (1) This is an full implementation of the server side of the TFTP protocol, as
 *               described in RFC #1350.
 * @note     (2) This server is a 'single-user' one, meaning that while a transaction is in progress,
 *               other transactions are held off by returning an error condition indicating that
 *               the server is busy.
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _TFTP_SERVER_H_
#define  _TFTP_SERVER_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_fs.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  TFTP_SERVER_CFG_PORT_NBR_DFLT                          69u
#define  TFTP_SERVER_CFG_RX_TIMEOUT_MS_DFLT                     5000u
#define  TFTP_SERVER_CFG_SOCK_SEL_DFLT                          TFTPs_SOCK_SEL_IPv4
#define  TFTP_SERVER_CFG_TX_TIMEOUT_MS_DFLT                     5000u

#define  TFTP_SERVER_TASK_CFG_STK_SIZE_ELEMENTS_DFLT            512u
#define  TFTP_SERVER_TASK_CFG_STK_PTR_DFLT                      DEF_NULL

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                         IP TYPE DATA TYPE
 *******************************************************************************************************/

typedef enum tftps_sock_sel {
  TFTPs_SOCK_SEL_IPv4,
  TFTPs_SOCK_SEL_IPv6,
  TFTPs_SOCK_SEL_IPv4_IPv6
} TFTPs_SOCK_SEL;

/*
 ********************************************************************************************************
 *                                  CONNECTION CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  tftps_conn_cfg {
  TFTPs_SOCK_SEL SockSel;
  CPU_INT16U     Port;
  CPU_INT32U     RxTimeoutMax;
} TFTPs_CONN_CFG;

/*
 ********************************************************************************************************
 *                                     INIT CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  tftps_init_cfg {
  TFTPs_CONN_CFG ConnCfg;

  CPU_STK_SIZE   StkSizeElements;                               ///< Size of the stk, in CPU_STK elements.
  void           *StkPtr;                                       ///< Ptr to base of the stk.

  MEM_SEG        *MemSegPtr;
} TFTPs_INIT_CFG;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const TFTPs_INIT_CFG TFTPs_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void TFTPs_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base);

void TFTPs_ConfigureMemSeg(MEM_SEG *p_mem_seg);

void TFTPs_ConfigureConnParam(TFTPs_CONN_CFG *p_conn_cfg);

void TFTPs_Init(const CPU_CHAR *p_root_dir,
                RTOS_ERR       *p_err);

void TFTPs_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _TFTP_SERVER_H_
