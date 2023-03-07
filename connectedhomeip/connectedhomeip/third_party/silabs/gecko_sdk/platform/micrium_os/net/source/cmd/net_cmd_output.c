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
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#if (defined(RTOS_MODULE_NET_AVAIL) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                             INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/source/cmd/net_cmd_output_priv.h>

#include  <cpu/include/cpu.h>
#include  <common/include/shell.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_err.h>
#include  <common/include/rtos_utils.h>
#include  <common/include/toolchains.h>

#include  <rtos_err_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CMD_OUTPUT_USAGE                         ("Usage: ")
#define  NET_CMD_OUTPUT_ERR                           ("Error: ")
#define  NET_CMD_OUTPUT_SUCCESS                       ("Completed successfully")
#define  NET_CMD_OUTPUT_TABULATION                    ("\t")

#define  NET_CMD_OUTPUT_ERR_CMD_ARG_INVALID           ("Invalid Arguments")
#define  NET_CMD_OUTPUT_ERR_CMD_NOT_IMPLEMENTED       ("This command is not yet implemented")
#define  NET_CMD_OUTPUT_ERR_CMD_BEGINNING             ("---")
#define  NET_CMD_OUTPUT_ERR_CMD_END                   ("---------------------------------------------")

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                     NetCmd_OutputBeginning()
 *
 * @brief    Print command not yet implemented message.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputBeginning(SHELL_OUT_FNCT  out_fnct,
                                  SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_ERR_CMD_BEGINNING, DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                     NetCmd_OutputBeginning()
 *
 * @brief    Print command not yet implemented message.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           _OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           _OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputEnd(SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_ERR_CMD_END, DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_OutputCmdTbl()
 *
 * @brief    Print out command tables.
 *
 * @param    argc            is a count of the arguments supplied.
 *
 * @param    p_argv          an array of pointers to the strings which are those arguments.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputCmdTbl(SHELL_CMD       *p_cmd_tbl,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  SHELL_CMD  *p_shell_cmd;
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_USAGE, DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  p_shell_cmd = p_cmd_tbl;

  while (p_shell_cmd->Fnct != 0) {
    ret_val = NetCmd_OutputMsg((CPU_CHAR *)p_shell_cmd->Name, DEF_NO, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    switch (ret_val) {
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:
      case SHELL_OUT_ERR:
        return (SHELL_EXEC_ERR);

      default:
        break;
    }

    p_shell_cmd++;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                     NetCmd_OutputNotImplemented()
 *
 * @brief    Print command not yet implemented message.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputNotImplemented(SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputError(NET_CMD_OUTPUT_ERR_CMD_NOT_IMPLEMENTED, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                     NetCmd_OutputCmdArgInvalid()
 *
 * @brief    Print Invalid argument error.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputCmdArgInvalid(SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputError(NET_CMD_OUTPUT_ERR_CMD_ARG_INVALID, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_OutputError()
 *
 * @brief    Print out error message.
 *
 * @param    p_msg           String that contains the error message.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputError(CPU_CHAR        *p_error,
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_ERR, DEF_YES, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  ret_val = NetCmd_OutputMsg(p_error, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                      NetCmd_OutputErrorNetNbr()
 *
 * @brief    Print out error message using the net error code.
 *
 * @param    err             Net error code.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputErrorStr(RTOS_ERR        err,
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S     ret_val;
  const CPU_CHAR *p_err_str;

  PP_UNUSED_PARAM(err);

#if (RTOS_ERR_CFG_STR_EN == DEF_ENABLED)
  p_err_str = RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err));
#else
  p_err_str = DEF_NULL;
#endif

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_ERR, DEF_YES, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  ret_val += NetCmd_OutputMsg((CPU_CHAR *)p_err_str,
                              DEF_NO,
                              DEF_YES,
                              DEF_NO,
                              out_fnct,
                              p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_OutputSuccess()
 *
 * @brief    Output a success message in the console.
 *
 * @param    p_msg           String that contains the message to output.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputSuccess(SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputMsg(NET_CMD_OUTPUT_SUCCESS, DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_OutputInt32U()
 *
 * @brief    Output a integer of 32 bits unsigned
 *
 * @param    nbr             Number
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputInt32U(CPU_INT32U      nbr,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR   str_output[DEF_INT_32U_NBR_DIG_MAX + 1];
  CPU_INT16S ret_val = 0;

  (void)Str_FmtNbr_Int32U(nbr,
                          DEF_INT_32U_NBR_DIG_MAX,
                          DEF_NBR_BASE_DEC,
                          DEF_NULL,
                          DEF_NO,
                          DEF_YES,
                          str_output);

  ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_OutputSockID()
 *
 * @brief    Print out socket ID.
 *
 * @param    sock_id         Socket ID.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputSockID(CPU_INT16S      sock_id,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val = 0;

  ret_val = NetCmd_OutputMsg("Socket ID = ", DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
  ret_val += NetCmd_OutputInt32U(sock_id, out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_OutputMsg()
 *
 * @brief    Output a message in the console.
 *
 * @param    p_msg           String that contains the message to output.
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputMsg(CPU_CHAR        *p_msg,
                            CPU_BOOLEAN     new_line_start,
                            CPU_BOOLEAN     new_line_end,
                            CPU_BOOLEAN     tab_start,
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16U output_len;
  CPU_INT16S output;

  if (new_line_start == DEF_YES) {
    output = out_fnct(STR_NEW_LINE,
                      STR_NEW_LINE_LEN,
                      p_cmd_param->OutputOptPtr);
    switch (output) {
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:
      case SHELL_OUT_ERR:
        return (SHELL_EXEC_ERR);

      default:
        break;
    }
  }

  if (tab_start == DEF_YES) {
    output = out_fnct(NET_CMD_OUTPUT_TABULATION,
                      1,
                      p_cmd_param->OutputOptPtr);
    switch (output) {
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:
      case SHELL_OUT_ERR:
        return (SHELL_EXEC_ERR);

      default:
        break;
    }
  }

  output_len = Str_Len(p_msg);
  output = out_fnct(p_msg,
                    output_len,
                    p_cmd_param->OutputOptPtr);
  switch (output) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  if (new_line_end == DEF_YES) {
    output = out_fnct(STR_NEW_LINE,
                      STR_NEW_LINE_LEN,
                      p_cmd_param->OutputOptPtr);
    switch (output) {
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:
      case SHELL_OUT_ERR:
        return (SHELL_EXEC_ERR);

      default:
        break;
    }
  }

  return (output);
}

/****************************************************************************************************//**
 *                                          NetCmd_OutputData()
 *
 * @brief    Output data
 *
 * @param    p_buf           Pointer to a buffer that contains the data to output.
 *
 * @param    len             Data len contained in the buffer
 *
 * @param    out_fmt         Output format
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputData(CPU_INT08U         *p_buf,
                             CPU_INT16U         len,
                             NET_CMD_OUTPUT_FMT out_fmt,
                             SHELL_OUT_FNCT     out_fnct,
                             SHELL_CMD_PARAM    *p_cmd_param)
{
  CPU_INT16S output = 0;
  CPU_INT16U i;

  output += out_fnct(STR_NEW_LINE,
                     STR_NEW_LINE_LEN,
                     p_cmd_param->OutputOptPtr);

  switch (out_fmt) {
    case NET_CMD_OUTPUT_FMT_STRING:
      output += out_fnct((CPU_CHAR *)p_buf,
                         len,
                         p_cmd_param->OutputOptPtr);
      break;

    case NET_CMD_OUTPUT_FMT_HEX:
      for (i = 0; i < len; i++) {
        CPU_INT08U *p_val = &p_buf[i];
        CPU_INT08U str_len;
        CPU_CHAR   str_output[DEF_INT_08U_NBR_DIG_MAX + 1];

        (void)Str_FmtNbr_Int32U(*p_val,
                                DEF_INT_08U_NBR_DIG_MAX,
                                DEF_NBR_BASE_HEX,
                                DEF_NULL,
                                DEF_NO,
                                DEF_YES,
                                str_output);

        output += out_fnct("0x",
                           2,
                           p_cmd_param->OutputOptPtr);

        str_len = Str_Len_N(str_output, DEF_INT_08U_NBR_DIG_MAX);
        output += out_fnct(str_output,
                           str_len,
                           p_cmd_param->OutputOptPtr);

        if (i < (len - 1)) {
          output += out_fnct(", ",
                             2,
                             p_cmd_param->OutputOptPtr);
        }
      }
      break;

    default:
      break;
  }

  return (output);
}

/****************************************************************************************************//**
 *                                          NetCmd_OutputStatVal()
 *
 * @brief    Output statu value
 *
 * @param    p_stat_title    Stat name
 *
 * @param    stat_val        Value
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           SHELL_OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputNetStat(NET_STAT_POOL   *p_net_stat,
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S rtn_val = 0;

  rtn_val += NetCmd_OutputStatVal("EntriesAllocCtr", p_net_stat->EntriesAllocCtr, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesAvail", p_net_stat->EntriesAvail, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesDeallocCtr", p_net_stat->EntriesDeallocCtr, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesInit", p_net_stat->EntriesInit, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesLostCur", p_net_stat->EntriesLostCur, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesLostTot", p_net_stat->EntriesLostTot, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesTot", p_net_stat->EntriesTot, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesUsed", p_net_stat->EntriesUsed, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputStatVal("EntriesUsedMax", p_net_stat->EntriesUsedMax, out_fnct, p_cmd_param);

  return (rtn_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_OutputStatVal()
 *
 * @brief    Output statu value
 *
 * @param    p_stat_title    Stat name
 *
 * @param    stat_val        Value
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO errors
 *
 *           _OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *
 *           _OUT_ERR,                  otherwise.
 *******************************************************************************************************/
CPU_INT16S NetCmd_OutputStatVal(CPU_CHAR        *p_stat_title,
                                CPU_INT32U      stat_val,
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S rtn_val = 0;

  rtn_val += NetCmd_OutputMsg(p_stat_title, DEF_YES, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputMsg(" = ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
  rtn_val += NetCmd_OutputInt32U(stat_val, out_fnct, p_cmd_param);

  return (rtn_val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
