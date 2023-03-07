/***************************************************************************//**
 * @file
 * @brief Network Utility Library
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

#ifndef  _NET_UTIL_PRIV_H_
#define  _NET_UTIL_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../../include/net_cfg_net.h"
#include  "net_tmr_priv.h"
#include  "net_type_priv.h"

#include  <common/include/lib_mem.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT32U NetUtil_TS_Get_ms_Internal(void);

CPU_INT32U NetUtil_TS_GetMaxDly_ms(void);

//                                                                 ------------------ CHK SUM FNCTS -------------------
NET_CHK_SUM NetUtil_16BitOnesCplChkSumHdrCalc(void       *p_hdr,
                                              CPU_INT16U hdr_size);

CPU_BOOLEAN NetUtil_16BitOnesCplChkSumHdrVerify(void       *p_hdr,
                                                CPU_INT16U hdr_size);

NET_CHK_SUM NetUtil_16BitOnesCplChkSumDataCalc(void       *p_data_buf,
                                               void       *p_pseudo_hdr,
                                               CPU_INT16U pseudo_hdr_size);

CPU_BOOLEAN NetUtil_16BitOnesCplChkSumDataVerify(void       *p_data_buf,
                                                 void       *p_pseudo_hdr,
                                                 CPU_INT16U pseudo_hdr_size);

//                                                                 -------------------- CRC FNCTS ---------------------
CPU_INT32U NetUtil_32BitCRC_Calc(CPU_INT08U *p_data,
                                 CPU_INT32U data_len);

CPU_INT32U NetUtil_32BitCRC_CalcCpl(CPU_INT08U *p_data,
                                    CPU_INT32U data_len);

CPU_INT32U NetUtil_32BitReflect(CPU_INT32U val);

//                                                                 -------------------- TIME FNCTS --------------------
CPU_INT32U NetUtil_TimeSec_uS_To_ms(CPU_INT32U time_sec,
                                    CPU_INT32U time_us);

CPU_INT32U NetUtil_InitSeqNbrGet(void);

CPU_INT32U NetUtil_RandomRangeGet(CPU_INT32U min,
                                  CPU_INT32U max);

/********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 *                                   DEFINED IN PRODUCT'S  net_util_a.*
 *
 * Note(s) : (1) The network protocol suite's network-specific library port optimization file(s) are located
 *               in the following directories :
 *
 *               (a) \<Network Protocol Suite>\Ports\<cpu>\<compiler>\net_util_a.*
 *
 *                       where
 *                               <Network Protocol Suite>        directory path for network protocol suite
 *                               <cpu>                           directory name for specific processor (CPU)
 *                               <compiler>                      directory name for specific compiler
 *******************************************************************************************************/

#if (NET_CFG_OPTIMIZE_ASM_EN == DEF_ENABLED)
//                                                                 Optimize 16-bit sum for 32-bit.
CPU_INT32U NetUtil_16BitSumDataCalcAlign_32(void       *pdata_32,
                                            CPU_INT32U size);
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       NETWORK CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#if     (NET_CFG_OPTIMIZE_ASM_EN != DEF_DISABLED)
#error  "NET_CFG_OPTIMIZE_ASM_EN            illegally #define'd in 'net_cfg.h'. Not yet supported. MUST be DEF_DISABLED."
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_UTIL_PRIV_H_
