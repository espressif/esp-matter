/***************************************************************************//**
 * @file
 * @brief Network - IPerf Shell
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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_IPERF_AVAIL) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error IPerf Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "iperf_priv.h"

#include  <common/include/shell.h>

#include  <common/source/rtos/rtos_utils_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

#define  IPERF_CMD_NAME                     "iperf"
#define  IPERF_CMD_TBL_NAME                  IPERF_CMD_NAME
#define  IPERF_CMD_NAME_START                IPERF_CMD_NAME

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  {
  SHELL_OUT_FNCT  OutFnct;
  SHELL_CMD_PARAM *OutOpt_Ptr;
} IPERF_SHELL_OUT_PARAM;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S IPerfShell_Start(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param);

static void IPerfShell_OutputFnct(CPU_CHAR        *p_buf,
                                  IPERF_OUT_PARAM *p_param);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD IPerfCmdTbl[] =
{
  { IPERF_CMD_NAME_START, IPerfShell_Start },
  { 0, 0 }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               IPerfShell_Init()
 *
 * @brief    Adds the IPerf functions to the Shell table.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error
 *                   code(s) from this function :
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
void IPerfShell_Init(RTOS_ERR *p_err)
{
  Shell_CmdTblAdd(IPERF_CMD_TBL_NAME, IPerfCmdTbl, p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                           IPerfShell_Start()
 *
 * @brief    IPERF_TEST_TODO Add function description.
 *
 * @param    argc            IPERF_TEST_TODO Add description for 'argc'
 *
 * @param    p_argv          IPERF_TEST_TODO Add description for 'p_argv'
 *
 * @param    out_fnct        IPERF_TEST_TODO Add description for 'out_fnct'
 *
 * @param    p_cmd_param     IPERF_TEST_TODO Add description for 'p_cmd_param'
 *
 * @return   IPERF_TEST_TODO Add return value description.
 *******************************************************************************************************/
static CPU_INT16S IPerfShell_Start(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param)
{
  IPERF_SHELL_OUT_PARAM outparam;
  IPERF_OUT_PARAM       param;
  IPERF_TEST_ID         test_id;
  RTOS_ERR              local_err;

  outparam.OutFnct = out_fnct;
  outparam.OutOpt_Ptr = p_cmd_param;
  param.p_out_opt = &outparam;

  test_id = IPerf_TestShellStart(argc, p_argv, &IPerfShell_OutputFnct, &param, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    IPerf_Reporter(test_id,
                   &IPerfShell_OutputFnct,
                   &param);
  }

  return (1);
}

/****************************************************************************************************//**
 *                                           IPerfShell_OutputFnct()
 *
 * @brief    Output a string.
 *
 * @param    p_buf       Pointer to buffer to output.
 *
 * @param    p_param     Pointer to output IPerf parameter.
 *
 * @note     (1) The string pointed to by p_buf has to be NUL ('\0') terminated.
 *******************************************************************************************************/
static void IPerfShell_OutputFnct(CPU_CHAR        *p_buf,
                                  IPERF_OUT_PARAM *p_param)
{
  IPERF_SHELL_OUT_PARAM *p_param_shell;
  SHELL_CMD_PARAM       *p_cmd_param;
  CPU_INT16S            output;
  CPU_INT32U            str_len;

  //                                                               ----------------- VALIDATE POINTER -----------------
  if (p_buf == DEF_NULL) {
    return;
  }
  //                                                               ------------------ DISPLAY STRING ------------------
  p_param_shell = p_param->p_out_opt;
  p_cmd_param = p_param_shell->OutOpt_Ptr;

  str_len = Str_Len(p_buf);
  output = p_param_shell->OutFnct(p_buf,
                                  str_len,
                                  p_cmd_param->OutputOptPtr);
  PP_UNUSED_PARAM(output);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // defined(RTOS_MODULE_NET_IPERF_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
