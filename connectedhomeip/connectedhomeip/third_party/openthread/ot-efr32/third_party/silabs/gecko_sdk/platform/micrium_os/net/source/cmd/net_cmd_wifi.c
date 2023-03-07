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
 *                                       DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>
#if (defined(RTOS_MODULE_NET_AVAIL) \
  && defined(RTOS_MODULE_COMMON_SHELL_AVAIL))

#include  <net/include/net_cfg_net.h>
#ifdef  NET_IF_WIFI_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/rtos_path.h>
#include  <rtos_description.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
#include  "net_cmd_priv.h"
#include  "net_cmd_args_parser_priv.h"
#endif

#include  <net/include/net_icmp.h>
#include  <net/include/net_app.h>
#include  <net/include/net_if.h>
#include  <net/include/net_if_ether.h>
#include  <net/include/net_ascii.h>
#include  <net/include/net.h>
#include  <net/include/dhcp_client_types.h>

#include  <net/source/tcpip/net_sock_priv.h>
#include  <net/source/tcpip/net_priv.h>
#include  <net/source/tcpip/net_if_ether_priv.h>
#include  <net/source/tcpip/net_if_wifi_priv.h>

#include  <net/include/net_buf.h>

#ifdef  NET_IPv4_MODULE_EN
#include  <net/source/tcpip/net_ipv4_priv.h>
#endif
#ifdef  NET_IPv6_MODULE_EN
#include  <net/source/tcpip/net_ipv6_priv.h>
#include  <net/source/tcpip/net_ndp_priv.h>
#endif

#include  <common/include/rtos_err.h>
#include  <common/include/lib_utils.h>
#include  <common/source/collections/slist_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL
#include  <common/include/shell.h>
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CMD_ARG_WIFI_CHANNEL           'c'
#define  NET_CMD_ARG_WIFI_SSID              's'
#define  NET_CMD_ARG_WIFI_PSK               'p'
#define  NET_CMD_ARG_WIFI_NET_TYPE          't'
#define  NET_CMD_ARG_WIFI_SECURITY_TYPE     's'

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  NET_IF_WIFI_MODULE_EN

#define NET_CMD_NET_TYPE_STR_INFRA                 "infra"
#define NET_CMD_NET_TYPE_STR_ADHOC                 "adhoc"
#define NET_CMD_NET_TYPE_STR_LEN                   5u

static const NET_CMD_DICTIONARY NetCmd_DictionaryNetType[] = {
  { NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE, NET_CMD_NET_TYPE_STR_INFRA, NET_CMD_NET_TYPE_STR_LEN   },
  { NET_IF_WIFI_NET_TYPE_ADHOC, NET_CMD_NET_TYPE_STR_ADHOC, NET_CMD_NET_TYPE_STR_LEN   },
};

#define NET_CMD_SECURITY_TYPE_STR_OPEN             "open"
#define NET_CMD_SECURITY_TYPE_STR_WEP              "wep"
#define NET_CMD_SECURITY_TYPE_STR_WPA              "wpa"
#define NET_CMD_SECURITY_TYPE_STR_WPA2             "wpa2"
#define NET_CMD_SECURITY_TYPE_STR_WPS              "wps"

#define NET_CMD_SECURITY_TYPE_STR_LEN              4u

static const NET_CMD_DICTIONARY NetCmd_DictionarySecurityType[] = {
  { NET_IF_WIFI_SECURITY_OPEN, NET_CMD_SECURITY_TYPE_STR_OPEN, (sizeof(NET_CMD_SECURITY_TYPE_STR_OPEN) - 1) },
  { NET_IF_WIFI_SECURITY_WEP, NET_CMD_SECURITY_TYPE_STR_WEP, (sizeof(NET_CMD_SECURITY_TYPE_STR_WEP)  - 1) },
  { NET_IF_WIFI_SECURITY_WPA2, NET_CMD_SECURITY_TYPE_STR_WPA2, (sizeof(NET_CMD_SECURITY_TYPE_STR_WPA2) - 1) },
  { NET_IF_WIFI_SECURITY_WPA, NET_CMD_SECURITY_TYPE_STR_WPA, (sizeof(NET_CMD_SECURITY_TYPE_STR_WPA)  - 1) },
};
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static NET_CMD_WIFI_SCAN_ARG NetCmd_WiFiScanCmdArgParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err);

static NET_CMD_WIFI_JOIN_ARG NetCmd_WiFiJoinCmdArgParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err);
static NET_CMD_WIFI_CREATE_ARG NetCmd_WiFiCreateCmdArgParse(CPU_INT16U argc,
                                                            CPU_CHAR   *p_argv[],
                                                            RTOS_ERR   *p_err);

/****************************************************************************************************//**
 *                                        NetCmd_WiFi_Scan()
 *
 * @brief    Command function to Scan for available WiFi SSID.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_WiFi_Scan(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S            ret_val;
  NET_CMD_WIFI_SCAN_ARG args;
  NET_IF_WIFI_AP        ap[30];
  CPU_INT16U            ctn;
  CPU_INT16U            i;
  CPU_CHAR              str_output[NET_IF_WIFI_STR_LEN_MAX_SSID + 1];
  CPU_INT08U            ssid_len;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  ret_val = 0u;
  if (argc >= 3u) {
    args = NetCmd_WiFiScanCmdArgParse(argc, p_argv, &local_err);
    switch (RTOS_ERR_CODE_GET(local_err)) {
      case RTOS_ERR_NONE:
        break;

      case RTOS_ERR_INVALID_ARG:
      default:
        ret_val = NetCmd_OutputCmdArgInvalid(out_fnct, p_cmd_param);
        return (ret_val);
    }
  } else {
    ret_val = NetCmd_OutputMsg("Usage: net_wifi_scan wanted_SSID [-i interface_nbr] [-c channel] ",
                               DEF_YES,
                               DEF_YES,
                               DEF_NO,
                               out_fnct,
                               p_cmd_param);
    return (ret_val);
  }
  //                                                               ------------ SCAN FOR WIRELESS NETWORKS ------------
  ctn = NetIF_WiFi_Scan(args.IF_Nbr,
                        ap,                                      // Access point table location.
                        30,                                      // Access point table size.
                        &args.SSID,
                        args.Ch,
                        &local_err);

  if (RTOS_ERR_CODE_GET(local_err) != RTOS_ERR_NONE) {
    ret_val = NetCmd_OutputError("The Scan has failed.",
                                 out_fnct,
                                 p_cmd_param);
    return (ret_val);
  }

  ret_val = NetCmd_OutputMsg("Number of Access point found: ", DEF_YES, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
  (void)Str_FmtNbr_Int32U(ctn,
                          DEF_INT_32U_NBR_DIG_MAX,
                          DEF_NBR_BASE_DEC,
                          DEF_NULL,
                          DEF_NO,
                          DEF_YES,
                          str_output);

  ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);

  if (ctn == 0) {
    return (ret_val);
  }

  ret_val = NetCmd_OutputMsg("    SSID                             BSSID              Ch  Si   Type   Security",
                             DEF_YES,
                             DEF_YES,
                             DEF_NO,
                             out_fnct,
                             p_cmd_param);
  ret_val = NetCmd_OutputMsg(" -------------------------------------------------------------------------------",
                             DEF_NO,
                             DEF_YES,
                             DEF_NO,
                             out_fnct,
                             p_cmd_param);

  //                                                               --------- ANALYSE WIRELESS NETWORKS FOUND ----------

  for (i = 0u; i < ctn; i++) {
    (void)Str_FmtNbr_Int32U(i + 1,
                            2u,
                            DEF_NBR_BASE_DEC,
                            ' ',
                            DEF_NO,
                            DEF_YES,
                            str_output);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg("- ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    ssid_len = Str_Len(ap[i].SSID.SSID);
    Mem_Copy(str_output, ap[i].SSID.SSID, ssid_len);
    Mem_Set(&str_output[ssid_len], ' ', NET_IF_WIFI_STR_LEN_MAX_SSID - ssid_len);
    str_output[NET_IF_WIFI_STR_LEN_MAX_SSID] = 0u;
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg(" ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    NetASCII_MAC_to_Str(ap[i].BSSID.BSSID,
                        str_output,
                        DEF_YES,
                        DEF_YES,
                        &local_err);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    ret_val = NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    (void)Str_FmtNbr_Int32U(ap[i].Ch,
                            2u,
                            DEF_NBR_BASE_DEC,
                            ' ',
                            DEF_NO,
                            DEF_YES,
                            str_output);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg(", -", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    (void)Str_FmtNbr_Int32U(ap[i].SignalStrength,
                            2u,
                            DEF_NBR_BASE_DEC,
                            ' ',
                            DEF_NO,
                            DEF_YES,
                            str_output);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    if (ap[i].NetType == NET_IF_WIFI_NET_TYPE_INFRASTRUCTURE) {
      Str_Copy(str_output, "INFRA");
    } else if (ap[i].NetType == NET_IF_WIFI_NET_TYPE_ADHOC) {
      Str_Copy(str_output, "ADHOC");
    } else {
      Str_Copy(str_output, "UNKWN");
    }
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg(", ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);

    switch (ap[i].SecurityType) {
      case NET_IF_WIFI_SECURITY_OPEN:
        Str_Copy(str_output, "OPEN");
        break;

      case NET_IF_WIFI_SECURITY_WEP:
        Str_Copy(str_output, "WEP");
        break;

      case NET_IF_WIFI_SECURITY_WPA:
        Str_Copy(str_output, "WPA");
        break;

      case NET_IF_WIFI_SECURITY_WPA2:
        Str_Copy(str_output, "WPA2");
        break;

      default:
        Str_Copy(str_output, "UNKNOWN");
        break;
    }

    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_WiFi_Join()
 *
 * @brief    Command function to Join an WiFi Access Point.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_WiFi_Join(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S            ret_val;
  NET_CMD_WIFI_JOIN_ARG cmd_args;
  RTOS_ERR              local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  cmd_args = NetCmd_WiFiJoinCmdArgParse(argc, p_argv, &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_INVALID_ARG:
    default:
      ret_val = NetCmd_OutputMsg("Usage: net_if_join SSID [-p password] [-i interface_nbr] [-t net_type] [-s security_type]",
                                 DEF_YES,
                                 DEF_YES,
                                 DEF_NO,
                                 out_fnct,
                                 p_cmd_param);
      return (ret_val);
  }

  NetIF_WiFi_Join(cmd_args.IF_Nbr,
                  cmd_args.NetType,
                  NET_IF_WIFI_DATA_RATE_AUTO,
                  cmd_args.SecurityType,
                  NET_IF_WIFI_PWR_LEVEL_HI,
                  cmd_args.SSID,
                  cmd_args.PSK,
                  &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    NetCmd_OutputSuccess(out_fnct,
                         p_cmd_param);
  } else {
    ret_val = NetCmd_OutputError("Impossible to Join the specified Access Point.",
                                 out_fnct,
                                 p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_WiFi_Create()
 *
 * @brief    Command function to Create an WiFi Access point.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_WiFi_Create(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S              ret_val;
  NET_CMD_WIFI_CREATE_ARG cmd_args;
  RTOS_ERR                local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  cmd_args = NetCmd_WiFiCreateCmdArgParse(argc, p_argv, &local_err);
  switch (RTOS_ERR_CODE_GET(local_err)) {
    case RTOS_ERR_NONE:
      break;

    case RTOS_ERR_INVALID_ARG:
    default:
      ret_val = NetCmd_OutputMsg("Usage: net_if_create SSID [-p password] [-c channel] [-i interface_nbr] [-t net_type] [-s security_type]",
                                 DEF_YES,
                                 DEF_YES,
                                 DEF_NO,
                                 out_fnct,
                                 p_cmd_param);
      return (ret_val);
  }

  NetIF_WiFi_CreateAP(cmd_args.IF_Nbr,
                      cmd_args.NetType,
                      NET_IF_WIFI_DATA_RATE_AUTO,
                      cmd_args.SecurityType,
                      NET_IF_WIFI_PWR_LEVEL_HI,
                      cmd_args.Ch,
                      cmd_args.SSID,
                      cmd_args.PSK,
                      &local_err);

  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    NetCmd_OutputSuccess(out_fnct,
                         p_cmd_param);
  } else {
    ret_val = NetCmd_OutputError("Impossible to Create the specified Access Point.",
                                 out_fnct,
                                 p_cmd_param);
    ret_val += NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                        NetCmd_WiFi_Leave()
 *
 * @brief    Command function to Leave an WiFi Access point.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_WiFi_Leave(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;
  CPU_INT16U i;
  CPU_CHAR   *p_if_str;
  NET_IF_NBR if_nbr;
  RTOS_ERR   local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:

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
          break;
      }
    }
  }

  NetIF_WiFi_Leave(if_nbr, &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    NetCmd_OutputSuccess(out_fnct,
                         p_cmd_param);
  } else {
    ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  }

  return (ret_val);
}

/****************************************************************************************************//**
 *                                       NetCmd_WiFi_GetPeerInfo()
 *
 * @brief    Command function to output the peer information when acting as an WiFi Access point.
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
 *           SHELL_OUT_RTN_CODE_CONN_CLOSED,                 if implemented connection closed
 *           SHELL_OUT_ERR,                                  otherwise
 *******************************************************************************************************/
CPU_INT16S NetCmd_WiFi_GetPeerInfo(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S       ret_val;
  CPU_INT16U       i;
  CPU_CHAR         *p_if_str;
  NET_IF_NBR       if_nbr;
  NET_IF_WIFI_PEER buf_peer_info[5];
  CPU_INT16U       ctn;
  CPU_CHAR         str_output[32];
  RTOS_ERR         local_err;

  RTOS_ERR_SET(local_err, RTOS_ERR_NONE);

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:

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
          break;
      }
    }
  }

  ctn = NetIF_WiFi_GetPeerInfo(if_nbr,
                               buf_peer_info,
                               10,
                               &local_err);
  if (RTOS_ERR_CODE_GET(local_err) == RTOS_ERR_NONE) {
    (void)Str_FmtNbr_Int32U(ctn,
                            2u,
                            DEF_NBR_BASE_DEC,
                            ' ',
                            DEF_NO,
                            DEF_YES,
                            str_output);
    ret_val = NetCmd_OutputMsg("Nb of Peer : ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
    ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    for (i = 0; i < ctn; i++) {
      (void)Str_FmtNbr_Int32U(i + 1,
                              2u,
                              DEF_NBR_BASE_DEC,
                              ' ',
                              DEF_NO,
                              DEF_YES,
                              str_output);
      ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
      ret_val = NetCmd_OutputMsg(" - ", DEF_NO, DEF_NO, DEF_NO, out_fnct, p_cmd_param);
      NetASCII_MAC_to_Str((CPU_INT08U *)buf_peer_info[i].HW_Addr,
                          str_output,
                          DEF_NO,
                          DEF_YES,
                          &local_err);
      ret_val = NetCmd_OutputMsg(str_output, DEF_NO, DEF_YES, DEF_NO, out_fnct, p_cmd_param);
    }
  } else {
    ret_val = NetCmd_OutputErrorStr(local_err, out_fnct, p_cmd_param);
  }

  return (ret_val);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                       NetCmd_WiFiScanCmdArgParse ()
 *
 * @brief    Parse WiFi Scan command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   WiFi Scan argument parsed.
 *******************************************************************************************************/
static NET_CMD_WIFI_SCAN_ARG NetCmd_WiFiScanCmdArgParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err)
{
  NET_CMD_WIFI_SCAN_ARG cmd_args;
  CPU_BOOLEAN           is_graph;
  CPU_INT16U            i;
  CPU_CHAR              *p_if_str;
  CPU_INT32U            ch;
  CPU_INT08U            j;

  cmd_args.Ch = NET_IF_WIFI_CH_ALL;
  cmd_args.IF_Nbr = 0u;
  Mem_Set(cmd_args.SSID.SSID, 0x00, NET_IF_WIFI_STR_LEN_MAX_SSID);

  if (argc > 10) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:

          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }

          cmd_args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_WIFI_CHANNEL:
          ch = Str_ParseNbr_Int32U(p_argv[i + 1], DEF_NULL, DEF_NBR_BASE_DEC);
          if ((ch > 0u) && (ch < 14u)) {
            cmd_args.Ch = ch;
          } else {
            cmd_args.Ch = 0u;
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;
      }
    } else {
      if (cmd_args.SSID.SSID[0] != DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }

      j = 0;
      while (j < NET_IF_WIFI_STR_LEN_MAX_SSID + 1) {
        is_graph = ASCII_IS_GRAPH(*(p_argv[i] + j));
        if (is_graph == DEF_FALSE) {
          break;
        }
        j++;
      }

      if (j < NET_IF_WIFI_STR_LEN_MAX_SSID ) {
        Mem_Copy(cmd_args.SSID.SSID, p_argv[i], j);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                     NetCmd_WiFiJoinCmdArgParse()
 *
 * @brief    Parse WiFi Join command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   WiFi Join argument parsed.
 *******************************************************************************************************/
static NET_CMD_WIFI_JOIN_ARG NetCmd_WiFiJoinCmdArgParse(CPU_INT16U argc,
                                                        CPU_CHAR   *p_argv[],
                                                        RTOS_ERR   *p_err)
{
  NET_CMD_WIFI_JOIN_ARG cmd_args;
  CPU_BOOLEAN           is_graph;
  CPU_INT16U            i;
  CPU_CHAR              *p_if_str;
  CPU_INT08U            j = 0u;
  CPU_INT32U            net_type_result;
  CPU_INT32U            security_type_result;

  cmd_args.IF_Nbr = 0u;
  Mem_Set(cmd_args.SSID.SSID, 0x00, NET_IF_WIFI_STR_LEN_MAX_SSID);

  if ((argc > 10) || (argc <= 3)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:
          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }

          cmd_args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_WIFI_PSK:
          while (j < NET_IF_WIFI_STR_LEN_MAX_PSK + 1) {
            is_graph = ASCII_IS_GRAPH(*(p_argv[i + 1] + j));
            if (is_graph == DEF_FALSE) {
              break;
            }
            j++;
          }
          if (j < NET_IF_WIFI_STR_LEN_MAX_PSK ) {
            Mem_Copy(cmd_args.PSK.PSK, p_argv[i + 1], j);
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_ARG_WIFI_NET_TYPE:
          net_type_result = NetCmd_DictionaryGet(NetCmd_DictionaryNetType,
                                                 sizeof(NetCmd_DictionaryNetType),
                                                 (const CPU_CHAR *) p_argv[i + 1],
                                                 NET_CMD_NET_TYPE_STR_LEN);

          if (net_type_result != NET_CMD_DICTIONARY_KEY_INVALID) {
            cmd_args.NetType = net_type_result;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_ARG_WIFI_SECURITY_TYPE:
          security_type_result = NetCmd_DictionaryGet(NetCmd_DictionarySecurityType,
                                                      sizeof(NetCmd_DictionarySecurityType),
                                                      (const CPU_CHAR *) p_argv[i + 1],
                                                      NET_CMD_SECURITY_TYPE_STR_LEN);

          if (security_type_result != NET_CMD_DICTIONARY_KEY_INVALID) {
            cmd_args.SecurityType = security_type_result;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;
      }
    } else {
      if (cmd_args.SSID.SSID[0] != DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }
      j = 0;
      while (j < NET_IF_WIFI_STR_LEN_MAX_SSID + 1) {
        is_graph = ASCII_IS_GRAPH(*(p_argv[i] + j));
        if (is_graph == DEF_FALSE) {
          break;
        }
        j++;
      }
      if (j < NET_IF_WIFI_STR_LEN_MAX_SSID ) {
        Mem_Copy(cmd_args.SSID.SSID, p_argv[i], j);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/****************************************************************************************************//**
 *                                    NetCmd_WiFiCreateCmdArgParse()
 *
 * @brief    Parse WiFi Create command arguments.
 *
 * @param    argc    is a count of the arguments supplied.
 *
 * @param    p_argv  an array of pointers to the strings which are those arguments.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function
 *
 * @return   WiFi Create argument parsed.
 *******************************************************************************************************/
static NET_CMD_WIFI_CREATE_ARG NetCmd_WiFiCreateCmdArgParse(CPU_INT16U argc,
                                                            CPU_CHAR   *p_argv[],
                                                            RTOS_ERR   *p_err)
{
  NET_CMD_WIFI_CREATE_ARG cmd_args;
  CPU_BOOLEAN             is_graph;
  CPU_INT16U              i;
  CPU_CHAR                *p_if_str;
  CPU_INT08U              j = 0u;
  CPU_INT32U              net_type_result;
  CPU_INT32U              security_type_result;
  CPU_INT32U              ch;

  cmd_args.IF_Nbr = 0u;
  Mem_Set(cmd_args.SSID.SSID, 0x00, NET_IF_WIFI_STR_LEN_MAX_SSID);

  if ((argc > 15) || (argc <= 3)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
    return (cmd_args);
  }

  for (i = 1; i < argc; i++) {
    if (*p_argv[i] == NET_CMD_ARG_BEGIN) {
      switch (*(p_argv[i] + 1)) {
        case NET_CMD_ARG_IF:

          i += NetCmd_ArgsParserParseID_Nbr(&p_argv[i], &p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }

          cmd_args.IF_Nbr = NetCmd_ArgsParserTranslateID_Nbr(p_if_str, p_err);
          if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
            return (cmd_args);
          }
          break;

        case NET_CMD_ARG_WIFI_PSK:
          while (j < NET_IF_WIFI_STR_LEN_MAX_PSK + 1) {
            is_graph = ASCII_IS_GRAPH(*(p_argv[i + 1] + j));
            if (is_graph == DEF_FALSE) {
              break;
            }
            j++;
          }
          if (j < NET_IF_WIFI_STR_LEN_MAX_PSK ) {
            Mem_Copy(cmd_args.PSK.PSK, p_argv[i + 1], j);
            cmd_args.PSK.PSK[j] = 0;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_ARG_WIFI_NET_TYPE:
          net_type_result = NetCmd_DictionaryGet(NetCmd_DictionaryNetType,
                                                 sizeof(NetCmd_DictionaryNetType),
                                                 (const CPU_CHAR *) p_argv[i + 1],
                                                 NET_CMD_NET_TYPE_STR_LEN);

          if (net_type_result != NET_CMD_DICTIONARY_KEY_INVALID) {
            cmd_args.NetType = net_type_result;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_ARG_WIFI_SECURITY_TYPE:
          security_type_result = NetCmd_DictionaryGet(NetCmd_DictionarySecurityType,
                                                      sizeof(NetCmd_DictionarySecurityType),
                                                      (const CPU_CHAR *) p_argv[i + 1],
                                                      NET_CMD_SECURITY_TYPE_STR_LEN);

          if (security_type_result != NET_CMD_DICTIONARY_KEY_INVALID) {
            cmd_args.SecurityType = security_type_result;
          } else {
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;

        case NET_CMD_ARG_WIFI_CHANNEL:
          ch = Str_ParseNbr_Int32U(p_argv[i + 1], DEF_NULL, DEF_NBR_BASE_DEC);
          if ((ch > 0u) && (ch < 14u)) {
            cmd_args.Ch = ch;
          } else {
            cmd_args.Ch = 0u;
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
            return (cmd_args);
          }
          i++;
          break;
      }
    } else {
      if (cmd_args.SSID.SSID[0] != DEF_NULL) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }

      j = 0;
      while (j < NET_IF_WIFI_STR_LEN_MAX_SSID + 1) {
        is_graph = ASCII_IS_GRAPH(*(p_argv[i] + j));
        if (is_graph == DEF_FALSE) {
          break;
        }
        j++;
      }

      if (j < NET_IF_WIFI_STR_LEN_MAX_SSID ) {
        Mem_Copy(cmd_args.SSID.SSID, p_argv[i], j);
      } else {
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
        return (cmd_args);
      }
    }
  }

  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  return (cmd_args);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_IF_WIFI_MODULE_EN
#endif // RTOS_MODULE_NET_AVAIL && RTOS_MODULE_COMMON_SHELL_AVAIL
