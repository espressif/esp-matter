/***************************************************************************//**
 * @file
 * @brief Network Shell Command Output Utilities
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

#ifndef  _NET_CMD_OUTPUT_PRIV_H_
#define  _NET_CMD_OUTPUT_PRIV_H_

#include  <rtos_description.h>
#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>
#include  <common/include/shell.h>
#include  <common/include/rtos_err.h>
#include  <net/include/net_stat.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  enum  net_cmd_output_fmt {
  NET_CMD_OUTPUT_FMT_NONE,
  NET_CMD_OUTPUT_FMT_STRING,
  NET_CMD_OUTPUT_FMT_HEX
} NET_CMD_OUTPUT_FMT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_INT16S NetCmd_OutputBeginning(SHELL_OUT_FNCT  out_fnct,
                                  SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputEnd(SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputCmdTbl(SHELL_CMD       *p_cmd_tbl,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputNotImplemented(SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputCmdArgInvalid(SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputError(CPU_CHAR        *p_error,
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputErrorStr(RTOS_ERR        err,
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputSuccess(SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputInt32U(CPU_INT32U      nbr,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputSockID(CPU_INT16S      sock_id,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputMsg(CPU_CHAR        *p_msg,
                            CPU_BOOLEAN     new_line_start,
                            CPU_BOOLEAN     new_line_end,
                            CPU_BOOLEAN     tab_start,
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputData(CPU_INT08U         *p_buf,
                             CPU_INT16U         len,
                             NET_CMD_OUTPUT_FMT out_fmt,
                             SHELL_OUT_FNCT     out_fnct,
                             SHELL_CMD_PARAM    *p_cmd_param);

CPU_INT16S NetCmd_OutputNetStat(NET_STAT_POOL   *p_net_stat,
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_OutputStatVal(CPU_CHAR        *p_stat_title,
                                CPU_INT32U      stat_val,
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_COMMON_SHELL_AVAIL
#endif // _NET_CMD_OUTPUT_PRIV_H_
