/***************************************************************************//**
 * @file
 * @brief Common - Memory Operations - Memory Commands
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
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
#include  <common/include/lib_mem.h>

#if (LIB_MEM_CFG_DBG_INFO_EN == DEF_ENABLED)
#include  <common/include/shell.h>

#include  <common/include/rtos_err.h>
#include  <common/include/rtos_path.h>

#include  <common/source/lib/lib_mem_cmd_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MEM_CMD_HELP                                        ("--help")
#define  MEM_CMD_HELP_SHORT                                  ("-h")

#define  MEM_CMD_INFO_SHORT                                  ("--short")
#define  MEM_CMD_INFO_FULL                                   ("--full")

#define  MEM_CMD_HELP_INFO                                   ("usage: mem_info [--short] [--full]\r\n"                  \
                                                              "\r\n"                                                    \
                                                              " -h,--help     this help message\r\n"                    \
                                                              "    --short    print memory usage summary (default)\r\n" \
                                                              "    --full     print full memory allocation table\r\n")

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S MemCmd_Info(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD MemCmdTbl[] =
{
  { "mem_info", MemCmd_Info },
  { 0, 0 }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef RTOS_MODULE_KERNEL_AVAIL
extern CPU_INT32U OSDbg_DataSize;
#else
static CPU_INT32U OSDbg_DataSize = 0;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_OUT_FNCT out_function;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               MemCmd_Init()
 *
 * @brief    Adds the Mem commands to Shell.
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
void MemCmd_Init(RTOS_ERR *p_err)
{
  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );

  Shell_CmdTblAdd((CPU_CHAR *)"mem",
                  MemCmdTbl,
                  p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/
static void OutputMessage(CPU_CHAR * str)
{
  out_function(str, Str_Len(str), 0);
}

/****************************************************************************************************//**
 *                                               MemCmd_Info()
 *
 * @brief    Output memory usage information
 *
 * @param    argc            Count of the arguments supplied.
 *
 * @param    p_argv          Array of pointers to the strings which are those arguments.
 *
 * @param    out_fnct        Callback to a respond to the requester.
 *
 * @param    p_cmd_param     Pointer to additional information to pass to the command.
 *
 * @return   SHELL_EXEC_ERR_NONE, if NO error(s).
 *               - SHELL_EXEC_ERR otherwise.
 *******************************************************************************************************/
static CPU_INT16S MemCmd_Info(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  out_function = out_fnct;

  (void)p_cmd_param;

  CPU_BOOLEAN summary = DEF_TRUE;
  RTOS_ERR    err;

  for (int i = 1; i < argc; i++) {
    if (!Str_Cmp(p_argv[i], MEM_CMD_INFO_SHORT)) {
      summary = DEF_TRUE;
    } else if (!Str_Cmp(p_argv[i], MEM_CMD_INFO_FULL)) {
      summary = DEF_FALSE;
    } else if (!Str_Cmp(p_argv[i], MEM_CMD_HELP) || !Str_Cmp(p_argv[i], MEM_CMD_HELP_SHORT)) {
      OutputMessage(MEM_CMD_HELP_INFO);
      return SHELL_EXEC_ERR_NONE;
    } else {
      OutputMessage("Invalid argument\r\n");
      return SHELL_EXEC_ERR;
    }
  }

  if (summary) {
    CPU_CHAR   line[128];
    CPU_INT16S len;
    CPU_SIZE_T used, remaining;

    Mem_SegTotalUsageGet(&used, &remaining, &err);

    len = Str_Sprintf(line,
                      "USED: %u\r\nREMAINING: %u\r\nTOTAL: %u\r\nOS DataSize: %u\r\n",
                      used,
                      remaining,
                      used + remaining,
                      OSDbg_DataSize);

    out_fnct(line, len, 0);
  } else {
    Mem_OutputUsage(OutputMessage, &err);
  }

  return RTOS_ERR_CODE_GET(err) == RTOS_ERR_NONE ? SHELL_EXEC_ERR_NONE : SHELL_EXEC_ERR;
}

#endif
#endif
