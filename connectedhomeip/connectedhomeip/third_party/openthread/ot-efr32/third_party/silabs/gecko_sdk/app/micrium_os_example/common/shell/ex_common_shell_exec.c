/***************************************************************************//**
 * @file
 * @brief Common Shell Example
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
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL

#include  <stdio.h>
#include  <cpu/include/cpu.h>
#include  <common/include/shell.h>
#include  <common/include/lib_def.h>
#include  <common/include/rtos_utils.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT16S Ex_CommonShellExecOutFnct(CPU_CHAR   *p_buf,
                                            CPU_INT16U buf_len,
                                            void       *p_opt);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_CommonShellExec()
 *
 * @brief  Provides example on how to use the Shell sub-module of Common to execute a command. The
 *         'help' command will list every command currently available in the Shell sub-module.
 *******************************************************************************************************/
void Ex_CommonShellExec(void)
{
  CPU_INT16S      ret_val;
  SHELL_CMD_PARAM cmd_param;
  CPU_CHAR        cmd_buf[25u];
  RTOS_ERR        err;

  Str_Copy(cmd_buf, "help");                                    // String passed to Shell_Exec must not be const.

  //                                                               Call Shell_Exec() with:
  ret_val = Shell_Exec(cmd_buf,                                 // string containing the cmd to exec,
                       Ex_CommonShellExecOutFnct,               // function to use to output,
                       &cmd_param,                              // a cmd param structure that could be used by cmd.
                       &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  APP_RTOS_ASSERT_CRITICAL(ret_val == SHELL_EXEC_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       Ex_CommonShellExecOutFnct()
 *
 * @brief  Output data as requested by the Shell sub-module during the Shell_Exec() call.
 *
 * @param  p_buf    Pointer to buffer containing the data to output.
 *
 * @param  buf_len  Length of the buffer.
 *
 * @param  p_opt    Pointer to optional options parameter, unused in this case.
 *
 * @return  Number of bytes outputted.
 *******************************************************************************************************/
static CPU_INT16S Ex_CommonShellExecOutFnct(CPU_CHAR   *p_buf,
                                            CPU_INT16U buf_len,
                                            void       *p_opt)
{
  CPU_INT16U tx_len = buf_len;

  PP_UNUSED_PARAM(p_opt);                                       // Supplemental options not used in this command.

  while (tx_len != 0u) {
    putchar(*p_buf);                                            // Any putchar-like function could be used, here.
    tx_len--;
    p_buf++;
  }

  return (buf_len);                                             // Return nbr of tx'd characters.
}

#endif // RTOS_MODULE_COMMON_SHELL_AVAIL
