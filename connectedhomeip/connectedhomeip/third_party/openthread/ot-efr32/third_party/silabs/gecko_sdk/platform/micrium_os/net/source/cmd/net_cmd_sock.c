/***************************************************************************//**
 * @file
 * @brief Network Shell Command
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

#include  <rtos_err_cfg.h>

#include  "net_cmd_priv.h"
#include  "net_cmd_args_parser_priv.h"

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_app.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_igmp.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/source/tcpip/net_ipv6_priv.h>
#include  <net/include/net_mldp.h>
#endif

#include  <common/include/rtos_err.h>

#include  <common/include/shell.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CMD_SOCK_ARG_ADDR                   'a'
#define  NET_CMD_SOCK_ARG_DATA                   'd'
#define  NET_CMD_SOCK_ARG_SEL_ERR                'e'
#define  NET_CMD_SOCK_ARG_FAMILY                 'f'
#define  NET_CMD_SOCK_ARG_FMT                    'F'
#define  NET_CMD_SOCK_ARG_IF                     'i'
#define  NET_CMD_SOCK_ARG_LEN                    'l'
#define  NET_CMD_SOCK_ARG_PORT                   'p'
#define  NET_CMD_SOCK_ARG_Q_SIZE                 'q'
#define  NET_CMD_SOCK_ARG_RETRY                  'r'
#define  NET_CMD_SOCK_ARG_SEL_RD                 'r'
#define  NET_CMD_SOCK_ARG_SOCK_ID                's'
#define  NET_CMD_SOCK_ARG_TYPE                   't'
#define  NET_CMD_SOCK_ARG_SEL_TIMEOUT            'T'
#define  NET_CMD_SOCK_ARG_VAL                    'v'
#define  NET_CMD_SOCK_ARG_SEL_WR                 'w'

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_CMD_SOCK_OPEN_CMD_ARG NetCmd_Sock_OpenCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err);

static NET_CMD_SOCK_OPEN_ARG NetCmd_Sock_OpenCmdTranslate(NET_CMD_SOCK_OPEN_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err);

static NET_CMD_SOCK_ID_CMD_ARG NetCmd_Sock_ID_CmdParse(CPU_INT16U argc,
                                                       CPU_CHAR   *p_argv[],
                                                       RTOS_ERR   *p_err);

static NET_CMD_SOCK_ID_ARG NetCmd_Sock_ID_CmdTranslate(NET_CMD_SOCK_ID_CMD_ARG cmd_args,
                                                       RTOS_ERR                *p_err);

static NET_CMD_SOCK_BIND_CMD_ARG NetCmd_Sock_BindCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err);

static NET_CMD_SOCK_BIND_ARG NetCmd_Sock_BindCmdTranslate(NET_CMD_SOCK_BIND_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err);

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
static NET_CMD_SOCK_LISTEN_CMD_ARG NetCmd_Sock_ListenCmdParse(CPU_INT16U argc,
                                                              CPU_CHAR   *p_argv[],
                                                              RTOS_ERR   *p_err);

static NET_CMD_SOCK_LISTEN_ARG NetCmd_Sock_ListenCmdTranslate(NET_CMD_SOCK_LISTEN_CMD_ARG cmd_args,
                                                              RTOS_ERR                    *p_err);
#endif

static NET_CMD_SOCK_CONN_CMD_ARG NetCmd_Sock_ConnCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err);

static NET_CMD_SOCK_CONN_ARG NetCmd_Sock_ConnCmdTranslate(NET_CMD_SOCK_CONN_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err);

static NET_CMD_SOCK_TX_CMD_ARG NetCmd_Sock_TxCmdParse(CPU_INT16U argc,
                                                      CPU_CHAR   *p_argv[],
                                                      RTOS_ERR   *p_err);

static NET_CMD_SOCK_TX_ARG NetCmd_Sock_TxCmdTranslate(NET_CMD_SOCK_TX_CMD_ARG cmd_args,
                                                      RTOS_ERR                *p_err);

static NET_CMD_SOCK_OPT_CMD_ARG NetCmd_Sock_OptCmdParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err);

static NET_CMD_SOCK_OPT_ARG NetCmd_Sock_OptCmdTranslate(NET_CMD_SOCK_OPT_CMD_ARG cmd_args,
                                                        RTOS_ERR                 *p_err);

static void NetCmd_Sock_CmdParse(NET_CMD_SOCK_ARG *p_cmd_arg,
                                 CPU_INT16U       argc,
                                 CPU_CHAR         *p_argv[],
                                 RTOS_ERR         *p_err);

static void NetCmd_Sock_CmdTranslate(const NET_CMD_SOCK_ARG *p_args,
                                     NET_CMD_SOCK_VAL       *p_vals,
                                     RTOS_ERR               *p_err);

#if 0
static NET_CMD_SOCK_SEL_CMD_ARG NetCmd_Sock_SelCmdParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err);

static NET_CMD_SOCK_SEL_ARG NetCmd_Sock_SelCmdTranslate(NET_CMD_SOCK_SEL_CMD_ARG cmd_args,
                                                        RTOS_ERR                 *p_err);

static NET_SOCK_ID NetCmd_Sock_SelGetSockID(CPU_CHAR *p_str,
                                            CPU_CHAR *p_str_next);
#endif

/****************************************************************************************************//**
 *                                          NetCmd_Sock_Open()
 *
 * @brief    Open a socket
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Open(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_OPEN_CMD_ARG cmd_args;
  NET_CMD_SOCK_OPEN_ARG     args;
  NET_SOCK_ID               sock_id;
  CPU_INT16S                ret_val = 0;
  RTOS_ERR                  local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_OpenCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val += NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_OpenCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val += NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  sock_id = NetSock_Open(args.Family, args.Type, NET_SOCK_PROTOCOL_DFLT, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val += NetCmd_OutputMsg("Unable to open a socket ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  } else {
    ret_val += NetCmd_OutputMsg("Socket opened: ", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSockID(sock_id, out_fnct, p_cmd_param);
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_Sock_Close()
 *
 * @brief    Close a socket
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Close(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_ID_CMD_ARG cmd_args;
  NET_CMD_SOCK_ID_ARG     args;
  CPU_INT16S              ret_val = 0;
  RTOS_ERR                local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_ID_CmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_ID_CmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetSock_Close(args.SockID, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to close the socket ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputMsg("Socket Closed", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  }

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_Sock_Bind()
 *
 * @brief    Bind a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Bind(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_SOCK_ADDR        sock_addr;
  NET_SOCK_ADDR_FAMILY addr_family;
  NET_SOCK_ADDR_LEN    sock_addr_len = sizeof(sock_addr);
  CPU_INT08U           *p_addr;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR addr;
#endif
  NET_IP_ADDR_LEN           addr_len;
  NET_CMD_SOCK_BIND_CMD_ARG cmd_args;
  NET_CMD_SOCK_BIND_ARG     args;
  CPU_INT16S                ret_val = 0;
  RTOS_ERR                  local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_BindCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_BindCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  switch (args.Family) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V4:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      addr = NET_IPv4_ADDR_ANY;
      p_addr = (CPU_INT08U *)&addr;
      addr_len = NET_IPv4_ADDR_SIZE;
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_SOCK_PROTOCOL_FAMILY_IP_V6:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      p_addr = (CPU_INT08U *)&NetIPv6_AddrAny;
      addr_len = NET_IPv6_ADDR_SIZE;
      break;
#endif

    default:
      ret_val += NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
      goto exit;
  }

  NetApp_SetSockAddr(&sock_addr,
                     addr_family,
                     args.Port,
                     p_addr,
                     addr_len,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to set the address ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  }

  NetSock_Bind(args.SockID, &sock_addr, sock_addr_len, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to Bind the socket ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  } else {
    ret_val = NetCmd_OutputMsg("Socket Binded", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  }

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_Sock_Listen()
 *
 * @brief    Listen on a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Listen(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val = 0;
  RTOS_ERR   local_err;
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
  NET_CMD_SOCK_LISTEN_CMD_ARG cmd_args;
  NET_CMD_SOCK_LISTEN_ARG     args;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_ListenCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_ListenCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetSock_Listen(args.SockID, args.QueueSize, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to listen on the socket ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputMsg("Listening", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  }

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
#else
  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  RTOS_ERR_SET(local_err, RTOS_ERR_NOT_AVAIL);
  ret_val = NetCmd_OutputMsg("Unable to listen on the socket ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
  ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
#endif
}

/****************************************************************************************************//**
 *                                         NetCmd_Sock_Accept()
 *
 * @brief    Accept connection from a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Accept(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val = 0;
  RTOS_ERR   local_err;
#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
  NET_CMD_SOCK_ID_CMD_ARG cmd_args;
  NET_CMD_SOCK_ID_ARG     args;
  NET_SOCK_ADDR           sock_addr;
  NET_SOCK_ADDR_LEN       addr_len;
  NET_SOCK_ID             sock_id;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_ID_CmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_ID_CmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  addr_len = sizeof(sock_addr);
  sock_id = NetSock_Accept(args.SockID, &sock_addr, &addr_len, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to accept connection ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  } else {
    ret_val = NetCmd_OutputMsg("Connection accepted: ", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSockID(sock_id, out_fnct, p_cmd_param);
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
#else
  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  RTOS_ERR_SET(local_err, RTOS_ERR_NOT_AVAIL);
  ret_val = NetCmd_OutputMsg("Unable to accept connection ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
  ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
#endif
}

/****************************************************************************************************//**
 *                                          NetCmd_Sock_Conn()
 *
 * @brief    Connect a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Conn(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_CONN_CMD_ARG cmd_args;
  NET_CMD_SOCK_CONN_ARG     args;
  NET_SOCK_ADDR             sock_addr;
  NET_SOCK_ADDR_FAMILY      addr_family;
  CPU_INT16S                ret_val = 0;
  RTOS_ERR                  local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_ConnCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_ConnCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  switch (args.AddrLen) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IPv4_ADDR_SIZE:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V4;
      break;
#endif
#ifdef  NET_IPv6_MODULE_EN
    case NET_IPv6_ADDR_SIZE:
      addr_family = NET_SOCK_ADDR_FAMILY_IP_V6;
      break;
#endif

    default:
      ret_val += NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
      goto exit;
  }

  NetApp_SetSockAddr(&sock_addr,
                     addr_family,
                     args.Port,
                     (CPU_INT08U *)args.Addr,
                     args.AddrLen,
                     &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to set the address ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  }

  NetSock_Conn(args.SockID, &sock_addr, sizeof(sock_addr), &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputMsg("Unable to accept connection ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  } else {
    ret_val = NetCmd_OutputMsg("Connected", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_Rx()
 *
 * @brief    Receive from a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Rx(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_ARG  cmd_args;
  NET_CMD_SOCK_VAL  vals;
  CPU_INT08U        rx_buf[1472];
  CPU_INT16U        rx_len = 1472u;
  CPU_INT16U        rx_len_tot = 0u;
  CPU_INT16S        ret_val = 0u;
  CPU_INT32U        retry = 0u;
  NET_SOCK_ADDR_LEN addr_len;
  NET_SOCK_ADDR     addr;
  RTOS_ERR          local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  NetCmd_Sock_CmdParse(&cmd_args, argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  if ((cmd_args.IDPtr == DEF_NULL)
      || (cmd_args.DataLenPtr == DEF_NULL)
      || (cmd_args.FmtPtr == DEF_NULL)) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  if ((cmd_args.AddrPtr != DEF_NULL)
      && (cmd_args.PortPtr == DEF_NULL)) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetCmd_Sock_CmdTranslate(&cmd_args, &vals, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  if (cmd_args.RetryPtr != DEF_NULL) {
    retry = vals.Retry;
  }

  while (rx_len_tot < vals.DataLen) {
    CPU_INT16U len_rem = vals.DataLen - rx_len_tot;
    CPU_INT32S len = 0;

    if (len_rem > 1472u) {
      rx_len = 1472u;
    } else {
      rx_len = len_rem;
    }

    if (cmd_args.AddrPtr != DEF_NULL) {
      addr_len = vals.AddrSockLen;
      NetApp_SetSockAddr(&addr, vals.AddrFamily, vals.Port, vals.Addr, vals.AddrLen, &local_err);
      len = NetSock_RxDataFrom(vals.ID, rx_buf, rx_len, NET_SOCK_FLAG_NONE, &addr, &addr_len, DEF_NULL, 0u, 0u, &local_err);
    } else {
      len = NetSock_RxData(vals.ID, rx_buf, rx_len, NET_SOCK_FLAG_NONE, &local_err);
    }

    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        ret_val += NetCmd_OutputData(rx_buf, len, vals.Fmt, out_fnct, p_cmd_param);
        rx_len_tot += len;
        break;

      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
        KAL_Dly(1);
        break;

      default:
        ret_val = NetCmd_OutputMsg("Receive error ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
        ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
        goto exit;
    }

    if (cmd_args.RetryPtr != DEF_NULL) {
      if (retry == 0) {
        ret_val = NetCmd_OutputMsg("Receive error ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
        ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
        goto exit;
      } else {
        retry--;
      }
    }
  }

  NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_Tx()
 *
 * @brief    Transmit on a socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Sock_Tx(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_TX_CMD_ARG cmd_args;
  NET_CMD_SOCK_TX_ARG     args;
  CPU_INT16U              tx_len_tot = 0u;
  CPU_INT16S              ret_val = 0;
  RTOS_ERR                local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_TxCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_TxCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  while (tx_len_tot < args.DataLen) {
    CPU_INT16U len_rem = args.DataLen - tx_len_tot;
    CPU_INT08U *p_data = args.DataPtr + tx_len_tot;

    tx_len_tot += NetSock_TxData(args.SockID, p_data, len_rem, NET_SOCK_FLAG_NONE, &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        break;

      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
        KAL_Dly(1);
        break;

      default:
        ret_val = NetCmd_OutputMsg("Receive error ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
        ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
        goto exit;
    }
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_Sel()
 *
 * @brief    Do a select on many socket.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
#if 0
CPU_INT16S NetCmd_Sock_Sel(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_SEL_CMD_ARG cmd_args;
  NET_CMD_SOCK_SEL_ARG     args;
  CPU_INT16U               i;
  CPU_INT16S               sock_ready_ctr;
  CPU_INT16S               ret_val = 0;
  RTOS_ERR                 local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_SelCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_SelCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  sock_ready_ctr = NetSock_Sel(args.SockNbrMax, &args.DescRd, &args.DescWr, &args.DescErr, &args.Timeout, &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      ret_val += NetCmd_OutputMsg("No Error,  Number of ready socket = ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      ret_val += NetCmd_OutputInt32U(sock_ready_ctr, out_fnct, p_cmd_param);

      ret_val += NetCmd_OutputMsg("Read Sockets: ", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
      for (i = 0; i <= args.SockNbrMax; i++) {
        if (NET_SOCK_DESC_IS_SET(i, (NET_SOCK_DESC *)&args.DescRd)) {
          ret_val += NetCmd_OutputInt32U(sock_ready_ctr, out_fnct, p_cmd_param);
          ret_val += NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
        }
      }

      ret_val += NetCmd_OutputMsg("Write Sockets: ", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
      for (i = 0; i <= args.SockNbrMax; i++) {
        if (NET_SOCK_DESC_IS_SET(i, &args.DescWr)) {
          ret_val += NetCmd_OutputInt32U(sock_ready_ctr, out_fnct, p_cmd_param);
          ret_val += NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
        }
      }

      ret_val += NetCmd_OutputMsg("Error Sockets: ", DEF_YES, DEF_YES, DEF_YES, out_fnct, p_cmd_param);
      for (i = 0; i <= args.SockNbrMax; i++) {
        if (NET_SOCK_DESC_IS_SET(i, &args.DescErr)) {
          ret_val += NetCmd_OutputInt32U(sock_ready_ctr, out_fnct, p_cmd_param);
          ret_val += NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
        }
      }
      break;

    default:
      ret_val = NetCmd_OutputMsg("Select error ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
      goto exit;
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_SockOptSetChild()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc            $$$$ Add description for 'argc'
 *
 * @param    p_argv          $$$$ Add description for 'p_argv'
 *
 * @param    out_fnct        $$$$ Add description for 'out_fnct'
 *
 * @param    p_cmd_param     $$$$ Add description for 'p_cmd_param'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
CPU_INT16S NetCmd_SockOptSetChild(CPU_INT16U      argc,
                                  CPU_CHAR        *p_argv[],
                                  SHELL_OUT_FNCT  out_fnct,
                                  SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_OPT_CMD_ARG cmd_args;
  NET_CMD_SOCK_OPT_ARG     args;
  CPU_INT16S               ret_val = 0;
  RTOS_ERR                 local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  cmd_args = NetCmd_Sock_OptCmdParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  args = NetCmd_Sock_OptCmdTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetSock_CfgConnChildQ_SizeSet(args.SockID, args.Value, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    goto exit;
  }

  ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);

exit:
  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_SockMcastJoin()
 *
 * @brief    Join a multicast group
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_SockMcastJoin(CPU_INT16U      argc,
                                CPU_CHAR        *p_argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_ARG args;
  NET_CMD_SOCK_VAL vals;
  CPU_INT16S       ret_val = 0;
#if  (defined(NET_IPv4_MODULE_EN) \
  && defined(NET_IGMP_MODULE_EN))
  NET_IPv4_ADDR *p_addr_ipv4;
#endif
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  NetCmd_Sock_CmdParse(&args, argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  if ((args.AddrPtr == DEF_NULL)
      || (args.IF_NbrPtr == DEF_NULL)) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetCmd_Sock_CmdTranslate((const NET_CMD_SOCK_ARG *)&args,
                           &vals,
                           &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  switch (vals.AddrFamily) {
#if  (defined(NET_IPv4_MODULE_EN) \
    && defined(NET_IGMP_MODULE_EN))
    case NET_IP_ADDR_FAMILY_IPv4:
      p_addr_ipv4 = (NET_IPv4_ADDR *)&vals.Addr[0];
      NetIGMP_HostGrpJoin(vals.IF,
                          *p_addr_ipv4,
                          &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
        && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
        ret_val = NetCmd_OutputError((CPU_CHAR *)local_err.CodeText, out_fnct, p_cmd_param);
#else
        ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
#endif
        goto exit;
      }
      ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;
#endif

#if  (defined(NET_IPv6_MODULE_EN) \
    && defined(NET_MLDP_MODULE_EN))
    case NET_IP_ADDR_FAMILY_IPv6:
      NetMLDP_HostGrpJoin(vals.IF,
                          (NET_IPv6_ADDR *)&vals.Addr[0],
                          &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
        && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
        ret_val = NetCmd_OutputError((CPU_CHAR *)local_err.CodeText, out_fnct, p_cmd_param);
#else
        ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
#endif
        goto exit;
      }
      ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;
#endif

    default:
      ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
      goto exit;
  }

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_SockMcastLeave()
 *
 * @brief    Leave a multicast group
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_SockMcastLeave(CPU_INT16U      argc,
                                 CPU_CHAR        *p_argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_SOCK_ARG cmd_args;
  NET_CMD_SOCK_VAL vals;
  CPU_INT16S       ret_val = 0;
#if  (defined(NET_IPv4_MODULE_EN) \
  && defined(NET_IGMP_MODULE_EN))
  NET_IPv4_ADDR *p_addr_ipv4;
#endif
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  NetCmd_OutputBeginning(out_fnct, p_cmd_param);

  NetCmd_Sock_CmdParse(&cmd_args, argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  if ((cmd_args.AddrPtr == DEF_NULL)
      || (cmd_args.IF_NbrPtr == DEF_NULL)) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  NetCmd_Sock_CmdTranslate((const NET_CMD_SOCK_ARG *)&cmd_args,
                           &vals,
                           &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    goto exit;
  }

  switch (vals.AddrFamily) {
#if  (defined(NET_IPv4_MODULE_EN) \
    && defined(NET_IGMP_MODULE_EN))
    case NET_IP_ADDR_FAMILY_IPv4:
      p_addr_ipv4 = (NET_IPv4_ADDR *)&vals.Addr[0];
      NetIGMP_HostGrpLeave(vals.IF,
                           *p_addr_ipv4,
                           &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
        && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
        ret_val = NetCmd_OutputError((CPU_CHAR *)local_err.CodeText, out_fnct, p_cmd_param);
#else
        ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
#endif
        goto exit;
      }
      ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;
#endif

#if  (defined(NET_IPv6_MODULE_EN) \
    && defined(NET_MLDP_MODULE_EN))
    case NET_IP_ADDR_FAMILY_IPv6:
      NetMLDP_HostGrpLeave(vals.IF,
                           (NET_IPv6_ADDR *)&vals.Addr[0],
                           &local_err);
      if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
        && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
        ret_val = NetCmd_OutputError((CPU_CHAR *)local_err.CodeText, out_fnct, p_cmd_param);
#else
        ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
#endif
        goto exit;
      }
      ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;
#endif

    default:
      ret_val = NetCmd_OutputError("Fault", out_fnct, p_cmd_param);
      goto exit;
  }

exit:
  NetCmd_OutputEnd(out_fnct, p_cmd_param);
  return (ret_val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetCmd_Sock_OpenCmdParse()
 *
 * @brief    Parse sock open command arguments
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   Socket open argument parsed
 *******************************************************************************************************/
static NET_CMD_SOCK_OPEN_CMD_ARG NetCmd_Sock_OpenCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_OPEN_CMD_ARG cmd_args;
  CPU_INT16U                i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 10) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      CPU_CHAR *p_letter = p_argv[i];

      p_letter++;
      i++;
      switch (*p_letter) {
        case NET_CMD_SOCK_ARG_FAMILY:
          NetCmd_ArgsParserParseSockFamily(&p_argv[i], &cmd_args.FamilyPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_TYPE:
          NetCmd_ArgsParserParseSockType(&p_argv[i], &cmd_args.TypePtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        default:
          break;
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_OpenCmdTranslate()
 *
 * @brief    Translate socket open command argument
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_OPEN_ARG NetCmd_Sock_OpenCmdTranslate(NET_CMD_SOCK_OPEN_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err)
{
  NET_CMD_SOCK_OPEN_ARG arg;

  arg.Family = NetCmd_ArgsParserTranslateSockFamily(cmd_args.FamilyPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.Type = NetCmd_ArgsParserTranslateSockType(cmd_args.TypePtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (arg);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_ID_CmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_ID_CMD_ARG NetCmd_Sock_ID_CmdParse(CPU_INT16U argc,
                                                       CPU_CHAR   *p_argv[],
                                                       RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_ID_CMD_ARG cmd_args;
  CPU_INT16U              i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 3) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_ID_CmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_ID_ARG NetCmd_Sock_ID_CmdTranslate(NET_CMD_SOCK_ID_CMD_ARG cmd_args,
                                                       RTOS_ERR                *p_err)
{
  NET_CMD_SOCK_ID_ARG arg;

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);

  return (arg);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_BindCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_BIND_CMD_ARG NetCmd_Sock_BindCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_BIND_CMD_ARG cmd_args;
  CPU_INT16U                i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_SOCK_ARG_FAMILY:
          i++;
          NetCmd_ArgsParserParseSockFamily(&p_argv[i], &cmd_args.FamilyPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_PORT:
          NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.PortPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          i++;
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_BindCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_BIND_ARG NetCmd_Sock_BindCmdTranslate(NET_CMD_SOCK_BIND_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err)
{
  NET_CMD_SOCK_BIND_ARG arg;

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.Family = NetCmd_ArgsParserTranslateSockFamily(cmd_args.FamilyPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.Port = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.PortPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (arg);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_ListenCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
static NET_CMD_SOCK_LISTEN_CMD_ARG NetCmd_Sock_ListenCmdParse(CPU_INT16U argc,
                                                              CPU_CHAR   *p_argv[],
                                                              RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_LISTEN_CMD_ARG cmd_args;
  CPU_INT16U                  i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 6) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_Q_SIZE:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.QueueSizePtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_Sock_ListenCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/

#ifdef  NET_SOCK_TYPE_STREAM_MODULE_EN
static NET_CMD_SOCK_LISTEN_ARG NetCmd_Sock_ListenCmdTranslate(NET_CMD_SOCK_LISTEN_CMD_ARG cmd_args,
                                                              RTOS_ERR                    *p_err)
{
  NET_CMD_SOCK_LISTEN_ARG arg;

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.QueueSize = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.QueueSizePtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (arg);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_Sock_ConnCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_CONN_CMD_ARG NetCmd_Sock_ConnCmdParse(CPU_INT16U argc,
                                                          CPU_CHAR   *p_argv[],
                                                          RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_CONN_CMD_ARG cmd_arg;
  CPU_INT16U                i;

  Mem_Clr(&cmd_arg, sizeof(cmd_arg));

  if (argc > 7) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_arg);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_arg.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_arg);
          }
          break;

        case NET_CMD_SOCK_ARG_PORT:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_arg.PortPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_arg);
          }
          break;

        case NET_CMD_SOCK_ARG_ADDR:
          i++;
          cmd_arg.AddrPtr = p_argv[i];
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_arg);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_arg);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_ConnCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_CONN_ARG NetCmd_Sock_ConnCmdTranslate(NET_CMD_SOCK_CONN_CMD_ARG cmd_args,
                                                          RTOS_ERR                  *p_err)
{
  NET_CMD_SOCK_CONN_ARG arg;
  NET_IP_ADDR_FAMILY    family;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.Port = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.PortPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  family = NetASCII_Str_to_IP(cmd_args.AddrPtr, &arg.Addr, sizeof(arg.Addr), &local_err);
  switch (family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      arg.AddrLen = NET_IPv4_ADDR_SIZE;
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      arg.AddrLen = NET_IPv4_ADDR_SIZE;
      break;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto exit;
  }

exit:
  return (arg);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_TxCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_TX_CMD_ARG NetCmd_Sock_TxCmdParse(CPU_INT16U argc,
                                                      CPU_CHAR   *p_argv[],
                                                      RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_TX_CMD_ARG cmd_args;
  CPU_INT16U              i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 6) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_LEN:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &cmd_args.DataLenPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_DATA:
          cmd_args.DataPtr = p_argv[i];
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_TxCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_OPT_CMD_ARG NetCmd_Sock_OptCmdParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_OPT_CMD_ARG cmd_args;
  CPU_INT16U               i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 6) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_VAL:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &cmd_args.ValPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_TxCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_OPT_ARG NetCmd_Sock_OptCmdTranslate(NET_CMD_SOCK_OPT_CMD_ARG cmd_args,
                                                        RTOS_ERR                 *p_err)
{
  NET_CMD_SOCK_OPT_ARG arg;

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.Value = NetCmd_ArgsParserTranslateVal32U(cmd_args.ValPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (arg);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_TxCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#if 0
static NET_CMD_SOCK_MCAST_CMD_ARG NetCmd_Sock_McastJoinCmdParse(CPU_INT16U argc,
                                                                CPU_CHAR   *p_argv[],
                                                                RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_MCAST_CMD_ARG cmd_args;
  CPU_INT16U                 i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 6) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.SockIDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.IF_NbrPtr, p_err);
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &cmd_args.DataLenPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_DATA:
          cmd_args.DataPtr = p_argv[i];
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_Sock_TxCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static NET_CMD_SOCK_TX_ARG NetCmd_Sock_TxCmdTranslate(NET_CMD_SOCK_TX_CMD_ARG cmd_args,
                                                      RTOS_ERR                *p_err)
{
  NET_CMD_SOCK_TX_ARG arg;

  arg.SockID = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.SockIDPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  arg.DataLen = NetCmd_ArgsParserTranslateDataLen(cmd_args.DataLenPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

exit:
  return (arg);
}

/****************************************************************************************************//**
 *                                           NetCmd_Sock_SelCmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    argc    $$$$ Add description for 'argc'
 *
 * @param    p_argv  $$$$ Add description for 'p_argv'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#if 0
static NET_CMD_SOCK_SEL_CMD_ARG NetCmd_Sock_SelCmdParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err)
{
  NET_CMD_SOCK_SEL_CMD_ARG cmd_args;
  CPU_INT16U               i;

  Mem_Clr(&cmd_args, sizeof(cmd_args));

  if (argc > 6) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  cmd_args.RdListPtr = DEF_NULL;
  cmd_args.WrListPtr = DEF_NULL;
  cmd_args.ErrListPtr = DEF_NULL;
  cmd_args.Timeout_sec_Ptr = DEF_NULL;

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      i++;
      switch (*(p_argv[i])) {
        case NET_CMD_SOCK_ARG_SEL_RD:
          cmd_args.RdListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_WR:
          cmd_args.WrListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_ERR:
          cmd_args.ErrListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_TIMEOUT:
          cmd_args.Timeout_sec_Ptr = p_argv[i];
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_Sock_SelCmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    cmd_args    $$$$ Add description for 'cmd_args'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#if 0
static NET_CMD_SOCK_SEL_ARG NetCmd_Sock_SelCmdTranslate(NET_CMD_SOCK_SEL_CMD_ARG cmd_args,
                                                        RTOS_ERR                 *p_err)
{
  NET_CMD_SOCK_SEL_ARG arg;
  CPU_CHAR             *p_str;
  NET_SOCK_ID          sock_id;

  NET_SOCK_DESC_INIT(&arg.DescRd);
  NET_SOCK_DESC_INIT(&arg.DescWr);
  NET_SOCK_DESC_INIT(&arg.DescErr);

  p_str = cmd_args.RdListPtr;
  while (p_str != DEF_NULL) {
    sock_id = NetCmd_Sock_SelGetSockID(p_str, p_str);
    if (sock_id != NET_SOCK_ID_NONE) {
      NET_SOCK_DESC_SET(sock_id, &arg.DescRd);
    }
  }

  p_str = cmd_args.WrListPtr;
  while (p_str != DEF_NULL) {
    sock_id = NetCmd_Sock_SelGetSockID(p_str, p_str);
    if (sock_id != NET_SOCK_ID_NONE) {
      NET_SOCK_DESC_SET(sock_id, &arg.DescWr);
    }
  }

  p_str = cmd_args.ErrListPtr;
  while (p_str != DEF_NULL) {
    sock_id = NetCmd_Sock_SelGetSockID(p_str, p_str);
    if (sock_id != NET_SOCK_ID_NONE) {
      NET_SOCK_DESC_SET(sock_id, &arg.DescErr);
    }
  }

  if (cmd_args.Timeout_sec_Ptr != DEF_NULL) {
    CPU_INT32U timeout;
    RTOS_ERR   err;

    timeout = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.Timeout_sec_Ptr, &err);
    arg.Timeout.timeout_us = 0u;
    arg.Timeout.timeout_sec = timeout;
  } else {
    arg.Timeout.timeout_us = 0u;
    arg.Timeout.timeout_sec = 0u;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (arg);
}
#endif

/****************************************************************************************************//**
 *                                       NetCmd_Sock_SelGetSockID()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_str       $$$$ Add description for 'p_str'
 *
 * @param    p_str_next  $$$$ Add description for 'p_str_next'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
#if 0
static NET_SOCK_ID NetCmd_Sock_SelGetSockID(CPU_CHAR *p_str,
                                            CPU_CHAR *p_str_next)
{
  NET_SOCK_ID val = NET_SOCK_ID_NONE;
  CPU_CHAR    *p_str_copy = p_str;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_str == DEF_NULL) {
    goto exit;
  }

  PP_UNUSED_PARAM(p_str_next);

  p_str_next = DEF_NULL;

  while (p_str_copy != DEF_NULL) {
    switch (*p_str_copy) {
      case ASCII_CHAR_SPACE:
        *p_str_copy = ASCII_CHAR_NULL;
        val = NetCmd_ArgsParserTranslateID_Nbr(p_str, &local_err);
        *p_str_copy = ASCII_CHAR_SPACE;
        p_str_next = p_str_copy + 1;
        goto exit;

      case NET_CMD_ARG_BEGIN:
        goto exit;

      default:
        p_str_copy++;
        break;
    }
  }

exit:
  return (val);
}
#endif

/****************************************************************************************************//**
 *                                           NetCmd_Sock_CmdParse()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_cmd_arg   $$$$ Add description for 'p_str'
 *
 * @param    argc        $$$$ Add description for 'argc'
 *
 * @param    p_argv      $$$$ Add description for 'p_argv'
 *
 * @param    p_err       $$$$ Add description for 'p_err
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static void NetCmd_Sock_CmdParse(NET_CMD_SOCK_ARG *p_cmd_arg,
                                 CPU_INT16U       argc,
                                 CPU_CHAR         *p_argv[],
                                 RTOS_ERR         *p_err)
{
  CPU_INT16U i;

  Mem_Clr(p_cmd_arg, sizeof(NET_CMD_SOCK_ARG));

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_SOCK_ARG_SOCK_ID:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_cmd_arg->IDPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_LEN:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &p_cmd_arg->DataLenPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_DATA:
          p_cmd_arg->DataPtr = p_argv[i];
          break;

        case NET_CMD_SOCK_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_cmd_arg->IF_NbrPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_FAMILY:
          NetCmd_ArgsParserParseSockFamily(&p_argv[i], &p_cmd_arg->SockFamilyPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_TYPE:
          NetCmd_ArgsParserParseSockType(&p_argv[i], &p_cmd_arg->SockTypePtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_ADDR:
          i++;
          p_cmd_arg->AddrPtr = p_argv[i];
          break;

        case NET_CMD_SOCK_ARG_PORT:
          NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_cmd_arg->PortPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          i++;
          break;

        case NET_CMD_SOCK_ARG_Q_SIZE:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_cmd_arg->AcceptQueueSizePtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_FMT:
          i += NetCmd_ArgsParserParseFmt(&p_argv[i], &p_cmd_arg->FmtPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

        case NET_CMD_SOCK_ARG_RETRY:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_cmd_arg->RetryPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            goto exit;
          }
          break;

#if 0
        case NET_CMD_SOCK_ARG_VAL:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &p_cmd_arg->OptValPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_SOCK_ARG_SEL_RD:
          p_cmd_arg->RdListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_WR:
          p_cmd_arg->WrListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_ERR:
          p_cmd_arg->ErrListPtr = p_argv[i] + 1;
          break;

        case NET_CMD_SOCK_ARG_SEL_TIMEOUT:
          p_cmd_arg->Timeout_sec_Ptr = p_argv[i];
          break;
#endif

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          goto exit;
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

exit:
  return;
}

/****************************************************************************************************//**
 *                                       NetCmd_Sock_CmdTranslate()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_args  $$$$ Add description for 'p_args'
 *
 * @param    p_vals  $$$$ Add description for 'p_vals'
 *
 * @param    p_err   $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
static void NetCmd_Sock_CmdTranslate(const NET_CMD_SOCK_ARG *p_args,
                                     NET_CMD_SOCK_VAL       *p_vals,
                                     RTOS_ERR               *p_err)
{
  p_vals->IF = NET_IF_NBR_NONE;
  p_vals->ID = NET_SOCK_ID_NONE;
  p_vals->Port = NET_PORT_NBR_NONE;
  p_vals->AddrFamily = NET_IP_ADDR_FAMILY_NONE;
  p_vals->AcceptQueueSize = 0u;
  p_vals->Fmt = NET_CMD_OUTPUT_FMT_NONE;
#if 0
  p_vals->OptName = 0;
  p_vals->OptValue = 0u;
  p_vals->SelNbrMax = 0u;
  p_vals->SelDescRd = 0u;
  p_vals->SelDescWr = 0u;
  p_vals->SelDescErr = 0u;
  p_vals->SelTimeout.timeout_sec = 0u;
  p_vals->SelTimeout.timeout_us = 0u;
#endif
  Mem_Clr(&p_vals->Addr, NET_IP_MAX_ADDR_SIZE);

  if (p_args->IDPtr != DEF_NULL) {
    p_vals->ID = NetCmd_ArgsParserTranslateID_Nbr(p_args->IDPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->DataLenPtr != DEF_NULL) {
    p_vals->DataLen = NetCmd_ArgsParserTranslateDataLen(p_args->DataLenPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->DataPtr != DEF_NULL) {
    p_vals->DataPtr = (CPU_INT08U *)p_args->DataPtr;
  }

  if (p_args->AddrPtr != DEF_NULL) {
    p_vals->AddrFamily = NetASCII_Str_to_IP(p_args->AddrPtr, &p_vals->Addr, NET_IP_MAX_ADDR_SIZE, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }

    if (p_vals->AddrFamily == NET_IP_ADDR_FAMILY_IPv4) {
      p_vals->AddrLen = NET_IPv4_ADDR_LEN;
      p_vals->AddrSockLen = NET_SOCK_ADDR_IPv4_SIZE;
    } else {
      p_vals->AddrLen = NET_IPv6_ADDR_LEN;
      p_vals->AddrSockLen = NET_SOCK_ADDR_IPv6_SIZE;
    }
  }

  if (p_args->PortPtr != DEF_NULL) {
    p_vals->Port = NetCmd_ArgsParserTranslateVal32U(p_args->PortPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->AcceptQueueSizePtr != DEF_NULL) {
    p_vals->AcceptQueueSize = NetCmd_ArgsParserTranslateVal32U(p_args->AcceptQueueSizePtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->RetryPtr != DEF_NULL) {
    p_vals->Retry = NetCmd_ArgsParserTranslateVal32U(p_args->RetryPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->IF_NbrPtr != DEF_NULL) {
    p_vals->IF = NetCmd_ArgsParserTranslateVal32U(p_args->IF_NbrPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->SockFamilyPtr != DEF_NULL) {
    p_vals->SockFamily = NetCmd_ArgsParserTranslateSockFamily(p_args->SockFamilyPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->SockTypePtr != DEF_NULL) {
    p_vals->SockType = NetCmd_ArgsParserTranslateSockType(p_args->SockTypePtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->FmtPtr != DEF_NULL) {
    p_vals->Fmt = NetCmd_ArgsParserTranslateFmt(p_args->FmtPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

#if 0
  if (p_args->OptNamePtr != DEF_NULL) {
    p_vals->OptName = NetCmd_ArgsParserTranslateSockOptName(p_args->OptNamePtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }

  if (p_args->OptValPtr != DEF_NULL) {
    p_vals->OptValue = p_args->OptValPtr;
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      goto exit;
    }
  }
#endif

  goto exit;

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
