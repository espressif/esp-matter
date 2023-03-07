/***************************************************************************//**
 * @file
 * @brief Common - General Shell Commands
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

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_str.h>
#include  <common/include/shell.h>
#include  <common/include/rtos_err.h>

#include  <common/source/shell/shell_priv.h>
#include  <common/source/shell/shell_cmd_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                            (COMMON, SHELL, CMD)
#define  RTOS_MODULE_CUR                         RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 *                                           ARGUMENT ERROR MESSAGES
 *******************************************************************************************************/

#define  SH_SHELL_ARG_ERR_HELP                  (CPU_CHAR *)"Sh_help: usage: Sh_help\r\n                Sh_help [command]"

/********************************************************************************************************
 *                                       COMMAND EXPLANATION MESSAGES
 *******************************************************************************************************/

#define  SH_SHELL_CMD_EXP_HELP                  (CPU_CHAR *)"                Get list of commands, or information about a command."

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S ShellCmd_help(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 *                                           SHELL COMMAND TABLE
 *******************************************************************************************************/

static SHELL_CMD ShellCmd_CmdTbl[] =
{
  { "help", ShellCmd_help },
  { 0, 0            }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PRIVATE FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               ShellCmd_Init()
 *
 * @brief    Initializes the Shell module for general shell commands.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from
 *                   this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_OWNERSHIP
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_INVALID_ARG
 *                       - RTOS_ERR_NO_MORE_RSRC
 *                       - RTOS_ERR_IS_OWNER
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void ShellCmd_Init(RTOS_ERR *p_err)
{
  Shell_CmdTblAdd((CPU_CHAR *)"Sh", ShellCmd_CmdTbl, p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           COMMAND FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               ShellCmd_help()
 *
 * @brief    List all commands or invoke '--help' for another command.
 *
 * @param    argc            The number of arguments.
 *
 * @param    argv            Array of arguments.
 *
 * @param    out_fnct        The output function.
 *
 * @param    p_cmd_param     Pointer to the command parameters.
 *
 * @return   SHELL_EXEC_ERR,      if an error is encountered.
 *           SHELL_EXEC_ERR_NONE, otherwise.
 *******************************************************************************************************/
static CPU_INT16S ShellCmd_help(CPU_INT16U      argc,
                                CPU_CHAR        *argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR *p_cmd_str;
  RTOS_ERR err;

  LOG_VRB(("ShShell_help: called."));

  if (argc == 2) {
    if (Str_Cmp(argv[1], SHELL_STR_HELP) == 0) {
      (void)out_fnct(SH_SHELL_ARG_ERR_HELP, (CPU_INT16U)Str_Len(SH_SHELL_ARG_ERR_HELP), p_cmd_param->OutputOptPtr);
      (void)out_fnct(STR_NEW_LINE, (CPU_INT16U)Str_Len(STR_NEW_LINE), p_cmd_param->OutputOptPtr);
      (void)out_fnct(SH_SHELL_CMD_EXP_HELP, (CPU_INT16U)Str_Len(SH_SHELL_CMD_EXP_HELP), p_cmd_param->OutputOptPtr);
      (void)out_fnct(STR_NEW_LINE, (CPU_INT16U)Str_Len(STR_NEW_LINE), p_cmd_param->OutputOptPtr);
      return (SHELL_EXEC_ERR_NONE);
    }
  }

  if ((argc != 1) && (argc != 2)) {
    (void)out_fnct(SH_SHELL_ARG_ERR_HELP, (CPU_INT16U)Str_Len(SH_SHELL_ARG_ERR_HELP), p_cmd_param->OutputOptPtr);
    (void)out_fnct(STR_NEW_LINE, (CPU_INT16U)Str_Len(STR_NEW_LINE), p_cmd_param->OutputOptPtr);
    return (SHELL_EXEC_ERR);
  }

  switch (argc) {
    case 1:
      Shell_ListCmdOutput(out_fnct, p_cmd_param);
      break;

    case 2:
      p_cmd_str = Shell_SharedBufGet(Str_Len(argv[1]) + 4u,
                                     &err);
      if (RTOS_ERR_CODE_GET(err) != RTOS_ERR_NONE) {
        return (SHELL_EXEC_ERR);
      }
      Str_Copy(p_cmd_str, argv[1]);
      Str_Cat(p_cmd_str, (CPU_CHAR *)" ");
      Str_Cat(p_cmd_str, SHELL_STR_HELP);

      Shell_Exec(p_cmd_str, out_fnct, p_cmd_param, &err);

      Shell_SharedBufFree();

      switch (RTOS_ERR_CODE_GET(err)) {
        case RTOS_ERR_NOT_FOUND:
        case RTOS_ERR_NO_MORE_RSRC:
          (void)out_fnct((CPU_CHAR *)"Command not recognized: ", 25, p_cmd_param->OutputOptPtr);
          (void)out_fnct(argv[1], (CPU_INT16U)Str_Len(argv[1]), p_cmd_param->OutputOptPtr);
          (void)out_fnct(STR_NEW_LINE, (CPU_INT16U)Str_Len(STR_NEW_LINE), p_cmd_param->OutputOptPtr);
          break;

        case RTOS_ERR_NONE:
        case RTOS_ERR_NULL_PTR:
        case RTOS_ERR_SHELL_CMD_EXEC:
        default:
          break;
      }
      break;

    default:
      break;
  }

  return (SHELL_EXEC_ERR_NONE);
}
