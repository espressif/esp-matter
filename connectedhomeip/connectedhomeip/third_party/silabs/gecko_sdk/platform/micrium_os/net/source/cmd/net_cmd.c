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

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#include  "net_cmd_priv.h"
#include  "net_cmd_args_parser_priv.h"

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_type.h>
#include  <net/include/net_def.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net_if.h>
#include  <net/include/net_ip.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_app.h>
#include  <net/include/net_icmp.h>

#ifdef  NET_IF_ETHER_MODULE_EN
#include  <net/include/net_if_ether.h>
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
#include  <net/include/net_if_wifi.h>
#endif

#include  <net/source/cmd/net_cmd_priv.h>
#include  <net/source/cmd/net_cmd_output_priv.h>
#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/cmd/net_cmd_args_parser_priv.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/include/net_ipv4.h>
#endif

#ifdef  NET_IPv6_MODULE_EN
#include  <net/include/net_ipv6.h>
#include  <net/source/tcpip/net_ipv6_priv.h>
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/shell.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_ascii.h>
#include  <common/include/lib_str.h>
#include  <common/include/lib_utils.h>
#include  <common/include/lib_mem.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_err.h>
#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/include/shell.h>

#include  <net/include/net_stat.h>
#include  <net/include/net_buf.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD NetCmdTbl[] =
{
  { "net_help", NetCmd_Help },
  { "ifconfig", NetCmd_IF_Config },
  { "net_if_reset", NetCmd_IF_Reset },
  { "net_if_set_mtu", NetCmd_IF_SetMTU },
  { "net_route_add", NetCmd_IF_RouteAdd },
  { "net_route_remove", NetCmd_IF_RouteRemove },
  { "net_ping", NetCmd_Ping },
  { "net_if_start", NetCmd_IF_Start },
  { "net_if_stop", NetCmd_IF_Stop },
  { "net_if_restart", NetCmd_IF_Restart },
  { "net_if_buf_rx", NetCmd_IF_BufRxLargeStat },
  { "net_if_buf_tx_l", NetCmd_IF_BufTxLargeStat },
  { "net_if_buf_tx_s", NetCmd_IF_BufTxSmallStat },
#ifdef  NET_IPv4_MODULE_EN
  { "net_ip_setup", NetCmd_IP_Config },
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
  { "net_wifi_scan", NetCmd_WiFi_Scan },
  { "net_wifi_join", NetCmd_WiFi_Join },
  { "net_wifi_create", NetCmd_WiFi_Create },
  { "net_wifi_leave", NetCmd_WiFi_Leave },
  { "net_wifi_peer", NetCmd_WiFi_GetPeerInfo },
#endif

  { "net_sock_open", NetCmd_Sock_Open },
  { "net_sock_close", NetCmd_Sock_Close },
  { "net_sock_bind", NetCmd_Sock_Bind },
  { "net_sock_listen", NetCmd_Sock_Listen },
  { "net_sock_accept", NetCmd_Sock_Accept },
  { "net_sock_connect", NetCmd_Sock_Conn },
  { "net_sock_rx", NetCmd_Sock_Rx },
  { "net_sock_tx", NetCmd_Sock_Tx },
  { "net_sock_opt_set_child", NetCmd_SockOptSetChild },
  { "net_sock_mcast_join", NetCmd_SockMcastJoin },
  { "net_sock_mcast_leave", NetCmd_SockMcastLeave },
  //                                                               {"net_sock_sel",     NetCmd_Sock_Sel},
  { 0, 0 }
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

CPU_BOOLEAN NetCmd_InitDone;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_CMD_RESET_CMD_ARG NetCmd_ResetCmdArgParse(CPU_INT16U argc,
                                                     CPU_CHAR   *p_argv[],
                                                     RTOS_ERR   *p_err);

static NET_CMD_RESET_ARG NetCmd_ResetTranslate(NET_CMD_RESET_CMD_ARG cmd_args,
                                               RTOS_ERR              *p_err);

static NET_CMD_MTU_CMD_ARG NetCmd_MTU_CmdArgParse(CPU_INT16U argc,
                                                  CPU_CHAR   *p_argv[],
                                                  RTOS_ERR   *p_err);

static NET_CMD_MTU_ARG NetCmd_MTU_Translate(NET_CMD_MTU_CMD_ARG cmd_args,
                                            RTOS_ERR            *p_err);

static NET_CMD_ROUTE_CMD_ARG NetCmd_RouteCmdArgParse(CPU_INT16U argc,
                                                     CPU_CHAR   *p_argv[],
                                                     RTOS_ERR   *p_err);

static NET_CMD_ROUTE_ARG NetCmd_RouteTranslate(NET_CMD_ROUTE_CMD_ARG cmd_args,
                                               RTOS_ERR              *p_err);

static CPU_INT16S NetCmd_Ping4(NET_IPv4_ADDR   *p_addr_remote,
                               void            *p_data,
                               CPU_INT16U      data_len,
                               CPU_INT32U      cnt,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S NetCmd_Ping6(NET_IPv6_ADDR   *p_addr_remote,
                               void            *p_data,
                               CPU_INT16U      data_len,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param);

#if 0
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetCmd_IPv6AddrCfgResult(NET_IF_NBR               if_nbr,
                                     NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                     const NET_IPv6_ADDR      *p_addr_cfgd,
                                     NET_IPv6_ADDR_CFG_STATUS addr_cfg_status);
#endif
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          NetCmd_IF_CfgInit()
 *
 * @brief    $$$$ Add function description.
 *
 * @param    p_str_args  $$$$ Add description for 'p_str_args'
 *
 * @param    p_if_cfg    $$$$ Add description for 'p_if_cfg'
 *
 * @param    p_err       $$$$ Add description for 'p_err'
 *
 * @return   $$$$ Add return value description.
 *******************************************************************************************************/
void NetCmd_IF_CfgInit(NET_CMD_STR_ARGS *p_str_args,
                       NET_IF_CFG       *p_if_cfg,
                       RTOS_ERR         *p_err)
{
  if (p_str_args->IF_Cfg.IPv4.Static.Addr != DEF_NULL) {
    p_if_cfg->IPv4.Static.Addr = p_str_args->IF_Cfg.IPv4.Static.Addr;
    p_if_cfg->IPv4.Static.Mask = p_str_args->IF_Cfg.IPv4.Static.Mask;
    p_if_cfg->IPv4.Static.Gateway = p_str_args->IF_Cfg.IPv4.Static.Gateway;
  }

  if (p_str_args->IF_Cfg.IPv6.Static.Addr != DEF_NULL) {
    p_if_cfg->IPv6.Static.Addr = p_str_args->IF_Cfg.IPv6.Static.Addr;
    p_if_cfg->IPv6.Static.PrefixLen = p_str_args->IF_Cfg.IPv6.Static.PrefixLen;
  } else {
    p_if_cfg->IPv6.Static.Addr = DEF_NULL;
  }

  if (p_str_args->IF_Cfg.HW_AddrStr != DEF_NULL) {
    p_if_cfg->HW_AddrStr = p_str_args->IF_Cfg.HW_AddrStr;
  }

  if (p_str_args->IF_Cfg.IPv4.DHCPc.En == DEF_YES) {
    p_if_cfg->IPv4.DHCPc.En = DEF_YES;
  } else {
    p_if_cfg->IPv4.DHCPc.En = DEF_NO;
  }

  PP_UNUSED_PARAM(p_err);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                             NetCmd_Init()
 *
 * @brief    Add Network commands to uC-Shell.
 *
 * @param    p_err   is a pointer to an error code which will be returned to your application.
 *******************************************************************************************************/
void NetCmd_Init(RTOS_ERR *p_err)
{
  NetCmd_InitDone = DEF_NO;
#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
  Shell_CmdTblAdd("net", NetCmdTbl, p_err);
#endif
}

/****************************************************************************************************//**
 *                                             NetCmd_Help()
 *
 * @brief    Command function to print out Net Commands help.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed
 *           SHELL_OUT_ERR,                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_Help(CPU_INT16U      argc,
                       CPU_CHAR        *p_argv[],
                       SHELL_OUT_FNCT  out_fnct,
                       SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  ret_val = NetCmd_OutputCmdTbl(NetCmdTbl, out_fnct, p_cmd_param);

  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_IF_Config()
 *
 * @brief    Command function to print out interfaces information.
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
#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
CPU_INT16S NetCmd_IF_Config(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_IF     *p_if;
  NET_IF_NBR if_nbr;
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR_OBJ *p_ipv4_addr_obj;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_IPv6_ADDR_OBJ *p_ipv6_addr_obj;
#endif
  CPU_CHAR   addr_ip_str[NET_ASCII_LEN_MAX_ADDR_IP];
  CPU_CHAR   str_output[DEF_INT_32U_NBR_DIG_MAX + 1];
  CPU_INT16S ret_val;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (argc > 1) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  for (if_nbr = 1; if_nbr <= NET_IF_CFG_MAX_NBR_IF; if_nbr++) {
    p_if = NetIF_Get(if_nbr, &local_err);

    ret_val = NetCmd_OutputMsg("Interface ID : ", DEF_YES, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    (void)Str_FmtNbr_Int32U(if_nbr,
                            DEF_INT_32U_NBR_DIG_MAX,
                            DEF_NBR_BASE_DEC,
                            DEF_NULL,
                            DEF_NO,
                            DEF_YES,
                            str_output);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

#ifdef  NET_IPv4_MODULE_EN
    SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv4_AddrListPtr, p_ipv4_addr_obj, NET_IPv4_ADDR_OBJ, ListNode) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      ret_val = NetCmd_OutputMsg("Host Address : ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      NetASCII_IPv4_to_Str(p_ipv4_addr_obj->AddrHost, addr_ip_str, DEF_NO, &local_err);
      ret_val = NetCmd_OutputMsg(addr_ip_str, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      ret_val = NetCmd_OutputMsg("Mask         : ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      NetASCII_IPv4_to_Str(p_ipv4_addr_obj->AddrHostSubnetMask, addr_ip_str, DEF_NO, &local_err);
      ret_val = NetCmd_OutputMsg(addr_ip_str, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      ret_val = NetCmd_OutputMsg("Gateway      : ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      NetASCII_IPv4_to_Str(p_ipv4_addr_obj->AddrDfltGateway, addr_ip_str, DEF_NO, &local_err);
      ret_val = NetCmd_OutputMsg(addr_ip_str, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    }
#endif

#ifdef  NET_IPv6_MODULE_EN
    SLIST_FOR_EACH_ENTRY(p_if->IP_Obj->IPv6_AddrListPtr, p_ipv6_addr_obj, NET_IPv6_ADDR_OBJ, ListNode) {
      RTOS_ERR_SET(local_err, RTOS_ERR_NONE);
      ret_val = NetCmd_OutputMsg("Host Address : ", DEF_YES, DEF_NO, DEF_YES, out_fnct, p_cmd_param);
      NetASCII_IPv6_to_Str(&p_ipv6_addr_obj->AddrHost, addr_ip_str, DEF_NO, DEF_NO, &local_err);
      ret_val = NetCmd_OutputMsg(addr_ip_str, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

      ret_val = NetCmd_OutputMsg("%", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
      (void)Str_FmtNbr_Int32U(p_ipv6_addr_obj->AddrHostPrefixLen,
                              DEF_INT_32U_NBR_DIG_MAX,
                              DEF_NBR_BASE_DEC,
                              DEF_NULL,
                              DEF_NO,
                              DEF_YES,
                              str_output);
      ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    }
#endif
  }

  ret_val = NetCmd_OutputMsg(" ", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

  PP_UNUSED_PARAM(p_argv);

  return (ret_val);
}
#endif

/****************************************************************************************************//**
 *                                           NetCmd_IF_Reset()
 *
 * @brief    Command function to reset interface(s) : remove all configured addresses, IPv4 or IPv6 or both,
 *           on specified interface. If no interface is specified, all existing interfaces will be reset.
 *
 * @param    argc            is a count of the arguments supplied.
 *
 * @param    p_argv          an array of pointers to the strings which are those arguments. (see Note #1)
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
 *
 * @note         (1) This function takes as arguments :
 *                   -i if_nbr     Specified on which interface the reset will occur.
 *                   -4            Specified to clear only the IPv4 addresses of the interface.
 *                   -6            Specified to clear only the IPv6 addresses of the interface.
 *
 *               If no arguments are passed, the function will reset all interfaces and both IPv4 and IPv6
 *               addresses.
 *******************************************************************************************************/
CPU_INT16S NetCmd_IF_Reset(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT08U            if_nbr;
  CPU_INT16S            ret_val;
  CPU_BOOLEAN           result;
  NET_CMD_RESET_CMD_ARG cmd_args;
  NET_CMD_RESET_ARG     args;
  RTOS_ERR              local_err;

  cmd_args = NetCmd_ResetCmdArgParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  args = NetCmd_ResetTranslate(cmd_args, &local_err);

#ifdef  NET_IPv4_MODULE_EN
  if (args.IPv4_En == DEF_YES) {
    if (args.IF_Nbr != NET_IF_NBR_NONE) {
      result = NetIPv4_CfgAddrRemoveAll(args.IF_Nbr, &local_err);
    } else {
      for (if_nbr = 1; if_nbr <= NET_IF_CFG_MAX_NBR_IF; if_nbr++) {
        result = NetIPv4_CfgAddrRemoveAll(if_nbr, &local_err);
        if (result != DEF_OK) {
          break;
        }
      }
    }
    if (result != DEF_OK) {
      ret_val = NetCmd_OutputError("Failed to reset Interface for IPv4", out_fnct, p_cmd_param);
      return (ret_val);
    } else {
      ret_val = NetCmd_OutputMsg("Reset Interface for IPv4", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    }
  }
#endif

#ifdef  NET_IPv6_MODULE_EN
  if (args.IPv6_En == DEF_YES) {
    if (args.IF_Nbr != NET_IF_NBR_NONE) {
      result = NetIPv6_CfgAddrRemoveAll(args.IF_Nbr, &local_err);
    } else {
      for (if_nbr = 1; if_nbr <= NET_IF_CFG_MAX_NBR_IF; if_nbr++) {
        result = NetIPv6_CfgAddrRemoveAll(if_nbr, &local_err);
        if (result != DEF_OK) {
          break;
        }
      }
    }
    if (result != DEF_OK) {
      ret_val = NetCmd_OutputError("Failed to reset Interface for IPv6", out_fnct, p_cmd_param);
      return (ret_val);
    } else {
      ret_val = NetCmd_OutputMsg("Reset Interface for IPv6", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    }
  }
#endif

  ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_IF_SetMTU()
 *
 * @brief    Command function to configure MTU of given Interface.
 *
 * @param    argc            is a count of the arguments supplied.
 *
 * @param    p_argv          an array of pointers to the strings which are those arguments. (see Note #1)
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
 *
 * @note         (1) This function takes as arguments :
 *                   -i if_nbr     Specified on which interface the reset will occur.
 *                   -M mtu        Specified the new MTU to configure.
 *******************************************************************************************************/
CPU_INT16S NetCmd_IF_SetMTU(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_MTU_CMD_ARG cmd_args;
  NET_CMD_MTU_ARG     args;
  CPU_INT16S          ret_val;
  RTOS_ERR            local_err;

  ret_val = 0u;

  cmd_args = NetCmd_MTU_CmdArgParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  args = NetCmd_MTU_Translate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  NetIF_MTU_Set(args.IF_Nbr,
                args.MTU,
                &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputError("Failed to configure Interface MTU", out_fnct, p_cmd_param);
    return (ret_val);
  }

  ret_val = NetCmd_OutputMsg("Configured Interface MTU", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

  ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                         NetCmd_IF_RouteAdd()
 *
 * @brief    Command function to add IP address route.
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
CPU_INT16S NetCmd_IF_RouteAdd(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_ROUTE_CMD_ARG cmd_args;
  NET_CMD_ROUTE_ARG     args;
  CPU_INT16S            ret_val;
  CPU_BOOLEAN           addr_added;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;
  addr_added = DEF_NO;

  if (argc < 6) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  cmd_args = NetCmd_RouteCmdArgParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  args = NetCmd_RouteTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  if (args.IF_Nbr == NET_IF_NBR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

#ifdef  NET_IPv4_MODULE_EN
  if (args.IPv4_En == DEF_YES) {
    NetCmd_IF_IPv4AddrCfgStatic(args.IF_Nbr, &args.IPv4Cfg, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Failed to configure IPv4 static address", out_fnct, p_cmd_param);
      ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
      return (ret_val);
    }
    ret_val = NetCmd_OutputMsg("Added IPv4 static address", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    addr_added = DEF_YES;
  }
#endif

#ifdef  NET_IPv6_MODULE_EN
  if (args.IPv6_En == DEF_YES) {
    NetCmd_IF_IPv6AddrCfgStatic(args.IF_Nbr, &args.IPv6Cfg, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Failed to configure IPv6 static address", out_fnct, p_cmd_param);
      ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
      return (ret_val);
    }
    ret_val = NetCmd_OutputMsg("Added IPv6 static address", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    addr_added = DEF_YES;
  }
#endif

  if (addr_added == DEF_NO) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_IF_RouteRemove()
 *
 * @brief    Command function to remove a route (previously added using NetCmd_IF_RouteAdd()).
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
CPU_INT16S NetCmd_IF_RouteRemove(CPU_INT16U      argc,
                                 CPU_CHAR        *p_argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param)
{
  NET_CMD_ROUTE_CMD_ARG cmd_args;
  NET_CMD_ROUTE_ARG     args;
  CPU_INT16S            ret_val;
  CPU_BOOLEAN           addr_removed;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;
  addr_removed = DEF_NO;

  cmd_args = NetCmd_RouteCmdArgParse(argc, p_argv, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  args = NetCmd_RouteTranslate(cmd_args, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  if (args.IF_Nbr == NET_IF_NBR_NONE) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

#ifdef  NET_IPv4_MODULE_EN
  if (args.IPv4_En == DEF_YES) {
    NetCmd_IF_IPv4AddrRemove(args.IF_Nbr, &args.IPv4Cfg, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Failed to remove IPv4 static address", out_fnct, p_cmd_param);
      return (ret_val);
    }
    ret_val = NetCmd_OutputMsg("Removed IPv4 static address", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    addr_removed = DEF_YES;
  }
#endif

#ifdef  NET_IPv6_MODULE_EN
  if (args.IPv6_En == DEF_YES) {
    NetCmd_IF_IPv6AddrRemove(args.IF_Nbr, &args.IPv6Cfg, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Failed to remove IPv6 static address", out_fnct, p_cmd_param);
      return (ret_val);
    }
    ret_val = NetCmd_OutputMsg("Removed IPv6 static address", DEF_YES, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    addr_removed = DEF_YES;
  }
#endif

  if (addr_removed == DEF_NO) {
    ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
    return (ret_val);
  }

  ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                     NetCmd_IF_IPv4AddrCfgStatic()
 *
 * @brief    Add a static IPv4 address on an interface.
 *
 * @param    if_id       Network interface number.
 *
 * @param    p_ip_cfg    Pointer to IPv4 address configuration structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv4_MODULE_EN
void NetCmd_IF_IPv4AddrCfgStatic(NET_IF_NBR       if_id,
                                 NET_CMD_IPv4_CFG *p_ip_cfg,
                                 RTOS_ERR         *p_err)
{
  (void)NetIPv4_CfgAddrAdd(if_id,
                           p_ip_cfg->Host,
                           p_ip_cfg->Mask,
                           p_ip_cfg->Gateway,
                           p_err);
}
#endif

/****************************************************************************************************//**
 *                                     NetCmd_IF_IPv6AddrCfgStatic()
 *
 * @brief    Add a static IPv6 address on an interface.
 *
 * @param    if_id       Network interface number.
 *
 * @param    p_ip_cfg    Pointer to IPv6 address configuration structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv6_MODULE_EN
void NetCmd_IF_IPv6AddrCfgStatic(NET_IF_NBR       if_id,
                                 NET_CMD_IPv6_CFG *p_ip_cfg,
                                 RTOS_ERR         *p_err)
{
  NET_FLAGS ipv6_flags;

  ipv6_flags = 0;
  DEF_BIT_CLR(ipv6_flags, (NET_FLAGS)NET_IPv6_FLAG_BLOCK_EN);
  DEF_BIT_SET(ipv6_flags, NET_IPv6_FLAG_DAD_EN);

  (void)NetIPv6_CfgAddrAdd(if_id,
                           &p_ip_cfg->Host,
                           p_ip_cfg->PrefixLen,
                           ipv6_flags,
                           p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NOT_AVAIL:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    default:
      break;
  }
}
#endif

/****************************************************************************************************//**
 *                                      NetCmd_IF_IPv4AddrRemove()
 *
 * @brief    Remove an IPv4 address.
 *
 * @param    if_id       Network interface number.
 *
 * @param    p_ip_cfg    Pointer to IPv4 address configuration structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv4_MODULE_EN
void NetCmd_IF_IPv4AddrRemove(NET_IF_NBR       if_id,
                              NET_CMD_IPv4_CFG *p_ip_cfg,
                              RTOS_ERR         *p_err)
{
  (void)NetIPv4_CfgAddrRemove(if_id,
                              p_ip_cfg->Host,
                              p_err);
}
#endif

/****************************************************************************************************//**
 *                                      NetCmd_IF_IPv6AddrRemove()
 *
 * @brief    Remove an IPv6 address.
 *
 * @param    if_id       Network interface number.
 *
 * @param    p_ip_cfg    Pointer to IPv6 address configuration structure.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
#ifdef  NET_IPv6_MODULE_EN
void NetCmd_IF_IPv6AddrRemove(NET_IF_NBR       if_id,
                              NET_CMD_IPv6_CFG *p_ip_cfg,
                              RTOS_ERR         *p_err)
{
  (void)NetIPv6_CfgAddrRemove(if_id,
                              &p_ip_cfg->Host,
                              p_err);
}
#endif

/****************************************************************************************************//**
 *                                             NetCmd_Ping()
 *
 * @brief    Function command to ping another host.
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
CPU_INT16S NetCmd_Ping(CPU_INT16U      argc,
                       CPU_CHAR        *p_argv[],
                       SHELL_OUT_FNCT  out_fnct,
                       SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_CHAR             *p_data;
  CPU_INT16S           ret_val;
  NET_CMD_PING_CMD_ARG cmd_arg;
  NET_CMD_PING_ARG     args;
  CPU_INT16U           data_len;
  CPU_INT32U           cnt;
  RTOS_ERR             local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;
  cmd_arg = NetCmd_PingCmdArgParse(argc, p_argv, &local_err);

  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_INVALID_ARG:
    default:
      ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
      return (ret_val);
  }

  args = NetCmd_PingCmdArgTranslate(&cmd_arg, &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_INVALID_ARG:
    default:
      ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
      return (ret_val);
  }

  if (args.DataLen == 0) {
    data_len = Str_Len(cmd_arg.AddrPtr);
    p_data = cmd_arg.AddrPtr;
  } else {
    data_len = args.DataLen;
    p_data = (CPU_CHAR *)Mem_SegAlloc("Cmd data", DEF_NULL, data_len, &local_err);
  }

  if (args.Cnt == 0) {
    cnt = 1u;
  } else {
    cnt = args.Cnt;
  }

  switch (args.family) {
    case NET_IP_ADDR_FAMILY_IPv4:
      ret_val = NetCmd_Ping4((NET_IPv4_ADDR *)&args.Addr,
                             p_data,
                             data_len,
                             cnt,
                             out_fnct,
                             p_cmd_param);
      break;

    case NET_IP_ADDR_FAMILY_IPv6:
      ret_val = NetCmd_Ping6((NET_IPv6_ADDR *)&args.Addr,
                             p_data,
                             data_len,
                             out_fnct,
                             p_cmd_param);
      break;

    default:
      ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
      return (ret_val);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                       NetCmd_PingCmdArgParse()
 *
 * @brief    Parse ping command line.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   Ping arguments parsed.
 *******************************************************************************************************/
NET_CMD_PING_CMD_ARG NetCmd_PingCmdArgParse(CPU_INT16U argc,
                                            CPU_CHAR   *p_argv[],
                                            RTOS_ERR   *p_err)
{
  NET_CMD_PING_CMD_ARG cmd_args;
  CPU_BOOLEAN          dig_hex;
  CPU_INT16U           i;

  cmd_args.IF_NbrPtr = DEF_NULL;
  cmd_args.AddrPtr = DEF_NULL;
  cmd_args.DataLenPtr = DEF_NULL;
  cmd_args.CntPtr = DEF_NULL;

  if ((argc > 10) || (argc < 2)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.IF_NbrPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_LEN:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &cmd_args.DataLenPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_CNT:
          i += NetCmd_ArgsParserParseDataLen(&p_argv[i], &cmd_args.CntPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    } else {
      dig_hex = ASCII_IS_DIG_HEX(*p_argv[i]);
      if (dig_hex == DEF_NO) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }

      cmd_args.AddrPtr = p_argv[i];
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                     NetCmd_PingCmdArgTranslate()
 *
 * @brief    Translate ping arguments.
 *
 * @param    cmd_arg     Pointer to the ping argument to translate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Ping argument converted.
 *******************************************************************************************************/
NET_CMD_PING_ARG NetCmd_PingCmdArgTranslate(NET_CMD_PING_CMD_ARG *p_cmd_args,
                                            RTOS_ERR             *p_err)
{
  NET_CMD_PING_ARG args;
  RTOS_ERR         local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  if (p_cmd_args->IF_NbrPtr != DEF_NULL) {
    args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(p_cmd_args->IF_NbrPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (args);
    }
  } else {
    args.IF_Nbr = NET_IF_NBR_NONE;
  }

  if (p_cmd_args->DataLenPtr != DEF_NULL) {
    args.DataLen = NetCmd_ArgsParserTranslateDataLen(p_cmd_args->DataLenPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (args);
    }
  } else {
    args.DataLen = 0;
  }

  if (p_cmd_args->CntPtr != DEF_NULL) {
    args.Cnt = NetCmd_ArgsParserTranslateDataLen(p_cmd_args->CntPtr, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (args);
    }
  } else {
    args.Cnt = 0;
  }

  args.family = NetASCII_Str_to_IP((CPU_CHAR *)p_cmd_args->AddrPtr,
                                   &args.Addr,
                                   sizeof(args.Addr),
                                   &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (args);
  }

#if 0
  args.Background = p_cmd_args->Background;
#endif

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (args);
}

/****************************************************************************************************//**
 *                                          NetCmd_IP_Config()
 *
 * @brief    Command function to configure an IPv4 address.
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
#ifdef  NET_IPv4_MODULE_EN
CPU_INT16S NetCmd_IP_Config(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  NET_IF_NBR             if_nbr;
  NET_IF_IPv4_STATIC_CFG ip_str_cfg;
  NET_CMD_IPv4_CFG       ip_cfg;
  CPU_INT16S             ret_val;
  CPU_INT32U             arg_ix;
  RTOS_ERR               local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  //                                                               Initializing address configuration parameters.
  if_nbr = NET_IF_NBR_NONE;
  ret_val = 0u;

  ip_str_cfg.Addr = DEF_NULL;
  ip_str_cfg.Mask = DEF_NULL;
  ip_str_cfg.Gateway = DEF_NULL;

  for (arg_ix = 1; arg_ix < argc; arg_ix++) {
    if (*p_argv[arg_ix] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[arg_ix] + 1)) {
        case NET_CMD_ARG_IF:
          arg_ix++;
          if_nbr = (NET_IF_NBR)Str_ParseNbr_Int32U(p_argv[arg_ix],
                                                   DEF_NULL,
                                                   10);
          break;

        case NET_CMD_ARG_ADDR:
          arg_ix++;
          ip_str_cfg.Addr = p_argv[arg_ix];
          break;

        case NET_CMD_ARG_MASK:
          arg_ix++;
          ip_str_cfg.Mask = p_argv[arg_ix];
          break;

        default:
          RTOS_ERR_SET(local_err, RTOS_ERR_INVALID_ARG);
          NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
          return (0);
      }
    }
  }

  ip_cfg.Host = NetASCII_Str_to_IPv4((CPU_CHAR *)ip_str_cfg.Addr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    return (ret_val);
  }

  ip_cfg.Mask = NetASCII_Str_to_IPv4((CPU_CHAR *)ip_str_cfg.Mask, &local_err);

  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
    return (ret_val);
  }

  ip_cfg.Gateway = NET_IPv4_ADDR_NONE;

  NetCmd_IF_IPv4AddrCfgStatic(if_nbr, &ip_cfg, &local_err);

  if ((RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE)
      || (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_ALREADY_EXISTS)) {
    ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  }

  return (ret_val);
}
#endif

/****************************************************************************************************//**
 *                                          NetCmd_IF_Start()
 *
 * @brief    Command function to start an Network interface.
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
CPU_INT16S NetCmd_IF_Start(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S  ret_val;
  NET_IF_NBR  if_nbr = NET_IF_NBR_NONE;
  NET_IF_TYPE if_type;
  CPU_INT08U  i;
  CPU_CHAR    *p_if_str;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc == 3u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
        }
      }
    }

    if_type = NetIF_TypeGet(if_nbr, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Interface cannot be started.",
                                   out_fnct,
                                   p_cmd_param);
      return (ret_val);
    }

    switch (if_type) {
#ifdef  NET_IF_ETHER_MODULE_EN
      case NET_IF_TYPE_ETHER:
        NetIF_Ether_Start(if_nbr, DEF_NULL, &local_err);
        break;
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
      case NET_IF_TYPE_WIFI:
        NetIF_WiFi_Start(if_nbr, DEF_NULL, &local_err);
        break;
#endif

      default:
        ret_val = NetCmd_OutputError("Interface cannot be started.",
                                     out_fnct,
                                     p_cmd_param);
        return (ret_val);
    }

    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      NetCmd_OutputSuccess(out_fnct,
                           p_cmd_param);
    } else if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_INVALID_STATE) {
      ret_val = NetCmd_OutputError("The interface is already started.",
                                   out_fnct,
                                   p_cmd_param);
    } else {
      ret_val = NetCmd_OutputError("Interface cannot be started.",
                                   out_fnct,
                                   p_cmd_param);
    }
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_start [-i interface_nbr]",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_IF_Stop()
 *
 * @brief    Command function to stop an Network interface.
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
CPU_INT16S NetCmd_IF_Stop(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param)

{
  CPU_INT16S ret_val;
  NET_IF_NBR if_nbr = NET_IF_NBR_NONE;
  CPU_INT08U i;
  CPU_CHAR   *p_if_str;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc == 3u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }

          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
        }
      }
    }

    NetIF_Stop(if_nbr, &local_err);

    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      NetCmd_OutputSuccess(out_fnct,
                           p_cmd_param);
    } else if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_INVALID_STATE) {
      ret_val = NetCmd_OutputError("The interface is already stopped.",
                                   out_fnct,
                                   p_cmd_param);
    } else {
      ret_val = NetCmd_OutputError("Interface cannot be stopped.",
                                   out_fnct,
                                   p_cmd_param);
    }
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_stop [-i interface_nbr]",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                          NetCmd_IF_Restart()
 *
 * @brief    Command function to restart an Network interface.
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
CPU_INT16S NetCmd_IF_Restart(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)

{
  CPU_INT16S  ret_val;
  NET_IF_NBR  if_nbr = NET_IF_NBR_NONE;
  NET_IF_TYPE if_type;
  CPU_INT08U  i;
  CPU_INT16U  time_sec = 10;
  CPU_INT32U  time_ms;
  CPU_CHAR    *p_if_str;
  CPU_BOOLEAN found = DEF_NO;
  RTOS_ERR    local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc <= 5u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }

          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            return (ret_val);
          }

          found = DEF_YES;
        } else if (*(p_argv[i] + 1) == NET_CMD_ARG_TIME) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }

          time_sec = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }

          found = DEF_YES;
        }
      }
    }

    if (found != DEF_YES) {
      ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
      return (ret_val);
    }

    NetIF_Stop(if_nbr, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_INVALID_STATE) {
      ret_val = NetCmd_OutputError("The interface is already stopped.",
                                   out_fnct,
                                   p_cmd_param);
      goto exit;
    } else if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Interface cannot be stopped.",
                                   out_fnct,
                                   p_cmd_param);
      goto exit;
    }

    time_ms = time_sec * 1000;
    KAL_Dly(time_ms);

    if_type = NetIF_TypeGet(if_nbr, &local_err);
    if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
      ret_val = NetCmd_OutputError("Interface cannot be re-started.",
                                   out_fnct,
                                   p_cmd_param);
      return (ret_val);
    }

    switch (if_type) {
#ifdef  NET_IF_ETHER_MODULE_EN
      case NET_IF_TYPE_ETHER:
        NetIF_Ether_Start(if_nbr, DEF_NULL, &local_err);
        break;
#endif

#ifdef  NET_IF_WIFI_MODULE_EN
      case NET_IF_TYPE_WIFI:
        NetIF_WiFi_Start(if_nbr, DEF_NULL, &local_err);
        break;
#endif

      default:
        ret_val = NetCmd_OutputError("Interface cannot be started.",
                                     out_fnct,
                                     p_cmd_param);
        return (ret_val);
    }

    if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
      KAL_Dly(300);
      NetCmd_OutputSuccess(out_fnct,
                           p_cmd_param);
    } else if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_INVALID_STATE) {
      ret_val = NetCmd_OutputError("The interface is already started.",
                                   out_fnct,
                                   p_cmd_param);
    } else {
      ret_val = NetCmd_OutputError("Interface cannot be started.",
                                   out_fnct,
                                   p_cmd_param);
    }
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_restart -i interface_nbr -t wait_time",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

exit:
  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_DictionaryGet()
 *
 * @brief    Find dictionary key by comparing string with dictionary string entries.
 *
 * @param    p_dictionary_tbl    Pointer on the dictionary table.
 *
 * @param    dictionary_size     Size of the dictionary in octet.
 *
 * @param    p_str_cmp           Pointer to string to find key.
 *                               Argument validated by callers.
 *
 * @param    str_len             Length of the string.
 *******************************************************************************************************/
CPU_INT32U NetCmd_DictionaryGet(const NET_CMD_DICTIONARY *p_dictionary_tbl,
                                CPU_INT32U               dictionary_size,
                                const CPU_CHAR           *p_str_cmp,
                                CPU_INT32U               str_len)
{
  CPU_INT32U         nbr_entry;
  CPU_INT32U         ix;
  CPU_INT32U         len;
  CPU_INT16S         cmp;
  NET_CMD_DICTIONARY *p_srch;

  nbr_entry = dictionary_size / sizeof(NET_CMD_DICTIONARY);
  p_srch = (NET_CMD_DICTIONARY *)p_dictionary_tbl;
  for (ix = 0; ix < nbr_entry; ix++) {
    len = DEF_MIN(str_len, p_srch->StrLen);
    cmp = Str_Cmp_N(p_str_cmp, p_srch->StrPtr, len);
    if (cmp == 0) {
      return (p_srch->Key);
    }
    p_srch++;
  }

  return (NET_CMD_DICTIONARY_KEY_INVALID);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetCmd_ResetCmdArgParse()
 *
 * @brief    Parse Reset command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   Reset argument parsed.
 *******************************************************************************************************/
static NET_CMD_RESET_CMD_ARG NetCmd_ResetCmdArgParse(CPU_INT16U argc,
                                                     CPU_CHAR   *p_argv[],
                                                     RTOS_ERR   *p_err)
{
  NET_CMD_RESET_CMD_ARG cmd_args;
  CPU_INT16U            i;

  cmd_args.IF_NbrPtr = DEF_NULL;
  cmd_args.IPv4_En = DEF_NO;
  cmd_args.IPv6_En = DEF_NO;

  if (argc > 5) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.IF_NbrPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_IPv4:
          cmd_args.IPv4_En = DEF_YES;
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_IPv6:
          cmd_args.IPv6_En = DEF_YES;
          break;

        default:
          RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
          return (cmd_args);
      }
    }
  }

  if ((cmd_args.IPv4_En == DEF_NO)
      && (cmd_args.IPv6_En == DEF_NO)) {
    cmd_args.IPv4_En = DEF_YES;
    cmd_args.IPv6_En = DEF_YES;
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                        NetCmd_ResetTranslate()
 *
 * @brief    Translate reset argument.
 *
 * @param    cmd_arg     Pointer to the reset argument to translate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Reset argument converted.
 *******************************************************************************************************/
static NET_CMD_RESET_ARG NetCmd_ResetTranslate(NET_CMD_RESET_CMD_ARG cmd_args,
                                               RTOS_ERR              *p_err)
{
  NET_CMD_RESET_ARG args;

  args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.IF_NbrPtr, p_err);

  if (cmd_args.IPv4_En == DEF_YES) {
    args.IPv4_En = DEF_YES;
  } else {
    args.IPv4_En = DEF_NO;
  }

  if (cmd_args.IPv6_En == DEF_YES) {
    args.IPv6_En = DEF_YES;
  } else {
    args.IPv6_En = DEF_NO;
  }

  return (args);
}

/****************************************************************************************************//**
 *                                      NetCmd_MTU_CmdArgParse()
 *
 * @brief    Parse set MTU command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   Set MTU argument parsed.
 *******************************************************************************************************/
static NET_CMD_MTU_CMD_ARG NetCmd_MTU_CmdArgParse(CPU_INT16U argc,
                                                  CPU_CHAR   *p_argv[],
                                                  RTOS_ERR   *p_err)
{
  NET_CMD_MTU_CMD_ARG cmd_args;
  CPU_INT16U          i;

  cmd_args.IF_NbrPtr = DEF_NULL;
  cmd_args.MTU_Ptr = DEF_NULL;

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_args.IF_NbrPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_MTU:
          i += NetCmd_ArgsParserParseMTU(&p_argv[i], &cmd_args.MTU_Ptr, p_err);
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
 *                                      NetCmd_MTU_Translate()
 *
 * @brief    Translate set MTU argument.
 *
 * @param    cmd_arg     Pointer to the set MTU argument to translate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Set MTU argument converted.
 *******************************************************************************************************/
static NET_CMD_MTU_ARG NetCmd_MTU_Translate(NET_CMD_MTU_CMD_ARG cmd_args,
                                            RTOS_ERR            *p_err)
{
  NET_CMD_MTU_ARG args;

  args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.IF_NbrPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (args);
  }

  args.MTU = NetCmd_ArgsParserTranslateMTU(cmd_args.MTU_Ptr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (args);
  }

  return (args);
}

/****************************************************************************************************//**
 *                                       NetCmd_RouteCmdArgParse()
 *
 * @brief    Parse Route command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   Route argument parsed.
 *******************************************************************************************************/
static NET_CMD_ROUTE_CMD_ARG NetCmd_RouteCmdArgParse(CPU_INT16U argc,
                                                     CPU_CHAR   *p_argv[],
                                                     RTOS_ERR   *p_err)
{
  NET_CMD_ROUTE_CMD_ARG cmd_arg;
  CPU_INT16U            i;

  cmd_arg.IF_NbrPtr = DEF_NULL;
  cmd_arg.IPv4.Addr = DEF_NULL;
  cmd_arg.IPv4.Mask = DEF_NULL;
  cmd_arg.IPv4.Gateway = DEF_NULL;
  cmd_arg.IPv6.Addr = DEF_NULL;
  cmd_arg.IPv6.PrefixLen = 0;

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &cmd_arg.IF_NbrPtr, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_arg);
          }
          break;

        case NET_CMD_ARG_IPv4:
          i += NetCmd_ArgsParserParseIPv4(&p_argv[i], &cmd_arg.IPv4, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_arg);
          }
          break;

        case NET_CMD_ARG_IPv6:
          i += NetCmd_ArgsParserParseIPv6(&p_argv[i], &cmd_arg.IPv6, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_arg);
          }
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
 *                                        NetCmd_RouteTranslate()
 *
 * @brief    Translate route argument.
 *
 * @param    cmd_arg     Pointer to the set MTU argument to translate.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   ROUTE argument converted.
 *******************************************************************************************************/
static NET_CMD_ROUTE_ARG NetCmd_RouteTranslate(NET_CMD_ROUTE_CMD_ARG cmd_args,
                                               RTOS_ERR              *p_err)
{
  NET_CMD_ROUTE_ARG args;

  args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(cmd_args.IF_NbrPtr, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return (args);
  }

#ifdef  NET_IPv4_MODULE_EN
  if (cmd_args.IPv4.Addr != DEF_NULL) {
    args.IPv4Cfg = NetCmd_ArgsParserTranslateIPv4(&cmd_args.IPv4, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (args);
    }

    args.IPv4_En = DEF_YES;
  } else {
    args.IPv4_En = DEF_NO;
  }
#endif

#ifdef  NET_IPv6_MODULE_EN
  if (cmd_args.IPv6.Addr != DEF_NULL) {
    args.IPv6Cfg = NetCmd_ArgsParserTranslateIPv6(&cmd_args.IPv6, p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      return (args);
    }

    args.IPv6_En = DEF_YES;
  } else {
    args.IPv6_En = DEF_NO;
  }
#endif

  return (args);
}

/****************************************************************************************************//**
 *                                            NetCmd_Ping4()
 *
 * @brief    Ping using an IPv4 address.
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
static CPU_INT16S NetCmd_Ping4(NET_IPv4_ADDR   *p_addr_remote,
                               void            *p_data,
                               CPU_INT16U      data_len,
                               CPU_INT32U      cnt,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;
#ifdef  NET_IPv4_MODULE_EN
  CPU_INT32U ix;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  for (ix = 0; ix < cnt; ix++) {
    (void)NetICMP_TxEchoReq((CPU_INT08U *)p_addr_remote,
                            sizeof(NET_IPv4_ADDR),
                            1000,
                            (void *)p_data,
                            data_len,
                            &local_err);
  }

  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;

    case RTOS_ERR_TIMEOUT:
      ret_val = NetCmd_OutputError("Timeout", out_fnct, p_cmd_param);
      return (ret_val);

    case RTOS_ERR_NET_IF_LINK_DOWN:
      ret_val = NetCmd_OutputError("Link down", out_fnct, p_cmd_param);
      return (ret_val);

    default:
      ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
      return (ret_val);
  }

  return (ret_val);
#else
  PP_UNUSED_PARAM(p_addr_remote);
  PP_UNUSED_PARAM(p_data);
  PP_UNUSED_PARAM(data_len);
  PP_UNUSED_PARAM(cnt);

  ret_val = out_fnct("IXANVL ping: IPv4 not present\n\rFAILED\n\r\n\r",
                     43,
                     p_cmd_param->OutputOptPtr);
  return (ret_val);
#endif
}

/****************************************************************************************************//**
 *                                            NetCmd_Ping6()
 *
 * @brief    Ping using an IPv6 address.
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
static CPU_INT16S NetCmd_Ping6(NET_IPv6_ADDR   *p_addr_remote,
                               void            *p_data,
                               CPU_INT16U      data_len,
                               SHELL_OUT_FNCT  out_fnct,
                               SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;
#ifdef  NET_IPv6_MODULE_EN
  RTOS_ERR local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  (void)NetICMP_TxEchoReq((CPU_INT08U *)p_addr_remote,
                          sizeof(NET_IPv6_ADDR),
                          1000u,
                          p_data,
                          data_len,
                          &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      ret_val = NetCmd_OutputSuccess(out_fnct, p_cmd_param);
      break;

    case RTOS_ERR_TIMEOUT:
      ret_val = NetCmd_OutputError("Timeout", out_fnct, p_cmd_param);
      return (ret_val);

    case RTOS_ERR_NET_IF_LINK_DOWN:
      ret_val = NetCmd_OutputError("Link down", out_fnct, p_cmd_param);
      return (ret_val);

    default:
      ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
      return (ret_val);
  }

#else
  ret_val = NetCmd_OutputError("IPv6 not present", out_fnct, p_cmd_param);
#endif

  PP_UNUSED_PARAM(p_addr_remote);
  PP_UNUSED_PARAM(p_data);
  PP_UNUSED_PARAM(data_len);

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_IPv6AddrCfgResult()
 *
 * @brief   Hook function called when auto-config is completed.
 *******************************************************************************************************/
#if 0
#ifdef  NET_IPv6_ADDR_AUTO_CFG_MODULE_EN
static void NetCmd_IPv6AddrCfgResult(NET_IF_NBR               if_nbr,
                                     NET_IPv6_CFG_ADDR_TYPE   addr_type,
                                     const NET_IPv6_ADDR      *p_addr_cfgd,
                                     NET_IPv6_ADDR_CFG_STATUS addr_cfg_status)
{
  CPU_BOOLEAN log_addr = DEF_NO;

  switch (addr_cfg_status) {
    case NET_IPv6_ADDR_CFG_STATUS_IN_PROGRESS:
      NET_CMD_TRACE_INFO(("Starting configuration an IPv6 address\r\n"));
      break;

    case NET_IPv6_ADDR_CFG_STATUS_SUCCEED:
      NET_CMD_TRACE_INFO(("IPv6 Address configured successfully\r\n"));
      log_addr = DEF_YES;
      break;

    case NET_IPv6_ADDR_CFG_STATUS_NOT_DONE:
      NET_CMD_TRACE_INFO(("IPv6 Address configuration not done\r\n"));
      log_addr = DEF_YES;
      break;

    case NET_IPv6_ADDR_CFG_STATUS_FAIL:
      NET_CMD_TRACE_INFO(("Failed to configure an IPv6 address\r\n"));
      break;

    case NET_IPv6_ADDR_CFG_STATUS_DUPLICATE:
      NET_CMD_TRACE_INFO(("Duplicate address detected\r\n"));
      break;

    default:
      NET_CMD_TRACE_INFO(("Address Status = INVALID TYPE\r\n"));
      goto exit;
  }

  PP_UNUSED_PARAM(if_nbr);
  PP_UNUSED_PARAM(addr_type);
  PP_UNUSED_PARAM(p_addr_cfgd);

exit: return;
}
#endif
#endif

/****************************************************************************************************//**
 *                                       NetCmd_IF_BufRxLargeStat()
 *
 * @brief    Get Rx large Buffer stat
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
CPU_INT16S NetCmd_IF_BufRxLargeStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S    ret_val;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;
  CPU_INT08U    i;
  CPU_CHAR      *p_if_str;
  NET_STAT_POOL stat;
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc == 3u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
        }
      }
    }

    stat = NetBuf_RxLargePoolStatGet(if_nbr);
    ret_val += NetCmd_OutputNetStat(&stat, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_buf_rx [-i interface_nbr]",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                       NetCmd_IF_BufTxLargeStat()
 *
 * @brief    Get Tx large Buffer stat
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
CPU_INT16S NetCmd_IF_BufTxLargeStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S    ret_val;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;
  CPU_INT08U    i;
  CPU_CHAR      *p_if_str;
  NET_STAT_POOL stat;
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc == 3u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
        }
      }
    }

    stat = NetBuf_TxLargePoolStatGet(if_nbr);
    ret_val += NetCmd_OutputNetStat(&stat, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_buf_rx [-i interface_nbr]",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                       NetCmd_IF_BufTxSmallStat()
 *
 * @brief    Get Tx small Buffer stat
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
CPU_INT16S NetCmd_IF_BufTxSmallStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S    ret_val;
  NET_IF_NBR    if_nbr = NET_IF_NBR_NONE;
  CPU_INT08U    i;
  CPU_CHAR      *p_if_str;
  NET_STAT_POOL stat;
  RTOS_ERR      local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;

  if (argc == 3u) {
    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == NET_CMD_ARG_IF) {
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
          if_nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, &local_err);
          if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
            ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
            return (ret_val);
          }
        }
      }
    }

    stat = NetBuf_TxSmallPoolStatGet(if_nbr);
    ret_val += NetCmd_OutputNetStat(&stat, out_fnct, p_cmd_param);
    ret_val += NetCmd_OutputSuccess(out_fnct, p_cmd_param);
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_if_buf_rx [-i interface_nbr]",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
  }

  return (ret_val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
