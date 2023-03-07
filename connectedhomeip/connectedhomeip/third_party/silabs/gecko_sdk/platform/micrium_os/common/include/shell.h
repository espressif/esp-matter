/***************************************************************************//**
 * @file
 * @brief Common - Shell Utility
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/****************************************************************************************************//**
 * @defgroup COMMON_SHELL Shell API
 * @ingroup  COMMON
 * @brief      Shell API
 *
 * @addtogroup COMMON_SHELL
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _SHELL_H_
#define  _SHELL_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_mem.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  SHELL_ASCII_ARG_END                              '\0'
#define  SHELL_ASCII_CDM_NAME_DELIMITER                   '_'

#define  SHELL_STR_HELP                       (CPU_CHAR *)"-h"

/****************************************************************************************************//**
 *                                       SHELL ERROR CODES DEFINES
 * @{
 * @note     (1) Command function MUST return SHELL_EXEC_ERR when an error occurred at execution time.
 *               Any other return value is command specific.
 *
 * @note     (2) Output function MUST return :
 *               - (a) The number of positive data octets transmitted, if NO errors
 *               - (b) SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *               - (c) SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/

#define  SHELL_EXEC_ERR_NONE                               0    // See Note #1.
#define  SHELL_EXEC_ERR                                   -1

#define  SHELL_OUT_RTN_CODE_CONN_CLOSED                    0    // See Note #2.
#define  SHELL_OUT_ERR                                    -1

///< @}

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           INIT CFG STRUCTURES
 * @{
 *******************************************************************************************************/

typedef struct shell_cfg_cmd_usage {
  CPU_INT16U CmdTblItemNbrInit;                                 ///< Init nbr of cmds allocated for the tbl.
  CPU_INT16U CmdTblItemNbrMax;                                  ///< Max  nbr of cmds in the tbl.
  CPU_INT16U CmdArgNbrMax;                                      ///< Max  nbr of args for a shell cmd.
  CPU_INT16U CmdNameLenMax;                                     ///< Max  len of cmd, including NULL character.
} SHELL_CFG_CMD_USAGE;

typedef struct shell_init_cfg {
  SHELL_CFG_CMD_USAGE CfgCmdUsage;                              ///< Cmd usage cfg struct.
  MEM_SEG             *MemSegPtr;                               ///< Ptr to shell mem seg.
} SHELL_INIT_CFG;

///< @}

/****************************************************************************************************//**
 *                                           SHELL COMMAND PARAMETER
 *
 * @note     (1) This structure is used to pass additional parameters to the command. Future
 *               implementation of this module could add fields to that structure.
 *
 * @note     (2) This variable is used to let the shell commands to control the session status. For
 *               instance, a command interacting with TELNETs could used this variable to terminate the
 *               current session.
 *******************************************************************************************************/

typedef struct shell_cmd_param {
  void        *CurWorkingDirPtr;                                ///< Cur working dir ptr.
  void        *OutputOptPtr;                                    ///< Output opt      ptr.
  CPU_BOOLEAN *SessionActiveFlagsPtr;                           ///< Session status flag (see Note #2).
} SHELL_CMD_PARAM;

/****************************************************************************************************//**
 *                               SHELL COMMAND FUNCTION POINTER DATA TYPE
 *
 * @note     (1) 'SHELL_OUT_FNCT' data type defined to replace the commonly-used function pointer
 *               to the output facility.
 *               - (a)     @verbatim
 *                       CPU_INT16U        ret_val;
 *                       SHELL_CMD_FNCT    FnctName;
 *                       CPU_CHAR         *pbuf;
 *                       CPU_INT16U        buf_len;
 *                       void             *popt;
 *
 *                       ret_val = FnctName(pbuf, buf_len, popt);
 *                       @endverbatim
 *               - (b) Shell output function MUST return :
 *                   - (1) The number of positive data octets transmitted, if NO error
 *                   - (2) SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if link connection closed
 *                   - (3) SHELL_OUT_ERR,                                  otherwise
 *
 * @note     (2) 'SHELL_CMD_FNCT' data type defined to replace the commonly-used function pointer
 *               to a shell command.  The last parameter is a pointer to an 'out' function having
 *               the prototype specified in #1.
 *               - (a) Example function pointer usage :
 *                       @verbatim
 *                       CPU_INT16U        ret_val
 *                       SHELL_CMD_FNCT    FnctName;
 *                       CPU_INT16U        argc;
 *                       CPU_CHAR         *argv[];
 *                       SHELL_OUT_FNCT    pout_fnct;
 *                       SHELL_CMD_PARAM  *pcmd_param
 *
 *                       ret_val = FnctName(argc, argv, pout_fnct, pcmd_param);
 *                       @endverbatim
 *               - (b) Shell commands MUST return SHELL_EXEC_ERR when an error occured at execution
 *                   time.  Any other return value is command specific.
 * @{
 *******************************************************************************************************/

//                                                                 See Note #1.
typedef CPU_INT16S (*SHELL_OUT_FNCT)(CPU_CHAR   *p_buf,
                                     CPU_INT16U buf_len,
                                     void       *p_opt);

//                                                                 See Note #2.
typedef CPU_INT16S (*SHELL_CMD_FNCT)(CPU_INT16U      argc,
                                     CPU_CHAR        *argv[],
                                     SHELL_OUT_FNCT  out_fnct,
                                     SHELL_CMD_PARAM *p_cmd_param);

///< @}

/****************************************************************************************************//**
 *                                       SHELL COMMAND DATA TYPE
 *
 * @note     This structure is used to store a command (function pointer) along with a character string
 *           representing its name.
 *******************************************************************************************************/

typedef struct shell_cmd {
  const CPU_CHAR *Name;                                         ///< Ptr to cmd name.
  SHELL_CMD_FNCT Fnct;                                          ///< Ptr to cmd fnct.
} SHELL_CMD;

/****************************************************************************************************//**
 *                                       SHELL OPT PARSE DATA TYPE
 *
 * @note     This structure is used to store states when parsing opts.
 *******************************************************************************************************/

typedef struct shell_optparse_state {
  CPU_CHAR    OptVal;                                           ///< Parsed opt val.
  CPU_CHAR    *OptArg;                                          ///< Parsed opt arg.

  CPU_INT16U  CurArgIx;                                         ///< Cur arg ix.
  CPU_CHAR    *CurArgPosPtr;                                    ///< Ptr to cur arg pos.
  CPU_BOOLEAN CurArgIsOpt;                                      ///< Cur arg str has opts.
} SHELL_OPTPARSE_STATE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const SHELL_INIT_CFG Shell_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Shell_ConfigureCmdUsage(SHELL_CFG_CMD_USAGE *p_cfg);
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void Shell_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void Shell_Init(RTOS_ERR *p_err);

CPU_INT16S Shell_Exec(CPU_CHAR        *in_str,
                      SHELL_OUT_FNCT  out_fnct,
                      SHELL_CMD_PARAM *p_cmd_param,
                      RTOS_ERR        *p_err);

void Shell_CmdTblAdd(CPU_CHAR  *cmd_tbl_name,
                     SHELL_CMD cmd_tbl[],
                     RTOS_ERR  *p_err);

void Shell_CmdTblRem(CPU_CHAR *cmd_tbl_name,
                     RTOS_ERR *p_err);

CPU_INT16U Shell_Scanner(CPU_CHAR   *in_str,
                         CPU_CHAR   *arg_tbl[],
                         CPU_INT16U arg_tbl_size,
                         RTOS_ERR   *p_err);

SHELL_OPTPARSE_STATE Shell_OptParse(CPU_INT16U           argc,
                                    CPU_CHAR             *argv[],
                                    SHELL_OPTPARSE_STATE state,
                                    CPU_CHAR             *opt_str);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                           MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // End of shell module include.
