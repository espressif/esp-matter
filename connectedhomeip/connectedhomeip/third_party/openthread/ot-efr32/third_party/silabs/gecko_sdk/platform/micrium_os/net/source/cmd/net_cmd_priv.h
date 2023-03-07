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
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_CMD_PRIV_H_
#define  _NET_CMD_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#ifdef  RTOS_MODULE_COMMON_SHELL_AVAIL

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/shell.h>

#include  <net/include/net_cfg_net.h>
#include  <net/include/net_type.h>
#include  <net/include/net_ip.h>

#include  <net/source/tcpip/net_if_priv.h>
#include  <net/source/cmd/net_cmd_output_priv.h>

#ifdef  NET_IF_WIFI_MODULE_EN
#include  <net/include/net_if_wifi.h>
#endif

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CMD_STR_USER_MAX_LEN           256
#define  NET_CMD_STR_PASSWORD_MAX_LEN       256

#define  NET_CMD_STR_ARG_MAX                20

#define  NET_CMD_ARG_BEGIN                  '-'

#define  NET_CMD_ARG_IPv4                   ASCII_CHAR_DIGIT_FOUR
#define  NET_CMD_ARG_IPv6                   ASCII_CHAR_DIGIT_SIX

#define  NET_CMD_ARG_IF                     ASCII_CHAR_LATIN_LOWER_I
#define  NET_CMD_ARG_SOCK_ID                ASCII_CHAR_LATIN_LOWER_I
#define  NET_CMD_ARG_LEN                    ASCII_CHAR_LATIN_LOWER_L
#define  NET_CMD_ARG_CNT                    ASCII_CHAR_LATIN_LOWER_C
#define  NET_CMD_ARG_HW_ADDR                ASCII_CHAR_LATIN_LOWER_H
#define  NET_CMD_ARG_ADDR                   ASCII_CHAR_LATIN_LOWER_A
#define  NET_CMD_ARG_MASK                   ASCII_CHAR_LATIN_LOWER_M
#define  NET_CMD_ARG_FMT                    ASCII_CHAR_LATIN_LOWER_F
#define  NET_CMD_ARG_DATA                   ASCII_CHAR_LATIN_LOWER_D
#define  NET_CMD_ARG_VAL                    ASCII_CHAR_LATIN_LOWER_V
#define  NET_CMD_ARG_TIME                   ASCII_CHAR_LATIN_LOWER_T

#define  NET_CMD_ARG_MTU                    ASCII_CHAR_LATIN_UPPER_M

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  net_cmd_cmd_args NET_CMD_STR_ARGS;

typedef  enum net_cmd_type {
  NET_CMD_TYPE_HELP,
  NET_CMD_TYPE_IF_CONFIG,
  NET_CMD_TYPE_IF_RESET,
  NET_CMD_TYPE_ROUTE_ADD,
  NET_CMD_TYPE_ROUTE_REMOVE,
  NET_CMD_TYPE_PING,
  NET_CMD_TYPE_IP_SETUP,
  NET_CMD_TYPE_IPV6_AUTOCFG,
  NET_CMD_TYPE_NDP_CLR_CACHE,
  NET_CMD_TYPE_NDP_CACHE_STATE,
  NET_CMD_TYPE_NDP_CACHE_IS_ROUTER,
} NET_CMD_TYPE;

typedef  struct  net_cmd_ipv4_cfg {
  NET_IPv4_ADDR Host;
  NET_IPv4_ADDR Mask;
  NET_IPv4_ADDR Gateway;
} NET_CMD_IPv4_CFG;

typedef  struct  net_cmd_ipv6_ascii_cfg {
  CPU_CHAR *HostPtr;
  CPU_CHAR *PrefixLenPtr;
} NET_CMD_IPv6_ASCII_CFG;

typedef  struct  net_cmd_ipv6_cfg {
  NET_IPv6_ADDR Host;
  CPU_INT08U    PrefixLen;
} NET_CMD_IPv6_CFG;

typedef  struct  net_cmd_mac_arg {
  CPU_CHAR *MAC_AddrStr;
} NET_CMD_MAC_CFG;

typedef  struct  net_cmd_auth_cfg {
  CPU_CHAR *User;
  CPU_CHAR *Password;
} NET_CMD_AUTH_CFG;

typedef  struct  net_cmd_credential {
  CPU_CHAR User[NET_CMD_STR_USER_MAX_LEN];
  CPU_CHAR Password[NET_CMD_STR_PASSWORD_MAX_LEN];
} NET_CMD_CREDENTIAL_CFG;

typedef  struct net_cmd_ping_arg {
  NET_IF_NBR         IF_Nbr;
  NET_IP_ADDR_FAMILY family;
  CPU_INT08U         Addr[16];
  CPU_INT16U         DataLen;
  CPU_INT32U         Cnt;
#if 0
  CPU_BOOLEAN        Background;
#endif
} NET_CMD_PING_ARG;

typedef  struct net_cmd_ping_cmd_arg {
  CPU_CHAR *IF_NbrPtr;
  CPU_CHAR *AddrPtr;
  CPU_CHAR *DataLenPtr;
  CPU_CHAR *CntPtr;
} NET_CMD_PING_CMD_ARG;

#ifdef  NET_IF_WIFI_MODULE_EN
typedef  struct net_cmd_wifi_scan_arg {
  NET_IF_NBR       IF_Nbr;
  NET_IF_WIFI_SSID SSID;
  NET_IF_WIFI_CH   Ch;
} NET_CMD_WIFI_SCAN_ARG;

typedef  struct net_cmd_wifi_join_arg {
  NET_IF_NBR                IF_Nbr;
  NET_IF_WIFI_SSID          SSID;
  NET_IF_WIFI_PSK           PSK;
  NET_IF_WIFI_NET_TYPE      NetType;                                    // Wifi AP net type.
  NET_IF_WIFI_SECURITY_TYPE SecurityType;                               // WiFi AP security type.
} NET_CMD_WIFI_JOIN_ARG;

typedef  struct net_cmd_wifi_create_arg {
  NET_IF_NBR                IF_Nbr;
  NET_IF_WIFI_SSID          SSID;
  NET_IF_WIFI_PSK           PSK;
  NET_IF_WIFI_CH            Ch;
  NET_IF_WIFI_NET_TYPE      NetType;                                    // Wifi AP net type.
  NET_IF_WIFI_SECURITY_TYPE SecurityType;                               // WiFi AP security type.
} NET_CMD_WIFI_CREATE_ARG;
#endif

typedef  struct  net_cmd_args {
  CPU_INT08U             WindowsIF_Nbr;

  CPU_BOOLEAN            IPv4_CfgEn;
  NET_CMD_IPv4_CFG       IPv4;

  CPU_BOOLEAN            IPv6_CfgEn;
  NET_CMD_IPv6_CFG       IPv6;

  CPU_BOOLEAN            MAC_CfgEn;
  NET_CMD_MAC_CFG        MAC_Addr;

  CPU_BOOLEAN            Credential_CfgEn;
  NET_CMD_CREDENTIAL_CFG Credential;

  CPU_BOOLEAN            Telnet_Reqd;
} NET_CMD_ARGS;

typedef struct net_cmd_reset_cmd_arg {
  CPU_CHAR    *IF_NbrPtr;
  CPU_BOOLEAN IPv4_En;
  CPU_BOOLEAN IPv6_En;
} NET_CMD_RESET_CMD_ARG;

typedef  struct net_cmd_reset_arg {
  NET_IF_NBR  IF_Nbr;
  CPU_BOOLEAN IPv4_En;
  CPU_BOOLEAN IPv6_En;
} NET_CMD_RESET_ARG;

typedef  struct net_cmd_route_cmd_arg {
  CPU_CHAR               *IF_NbrPtr;
  NET_IF_IPv4_STATIC_CFG IPv4;
  NET_IF_IPv6_STATIC_CFG IPv6;
} NET_CMD_ROUTE_CMD_ARG;

typedef  struct net_cmd_route_arg {
  NET_IF_NBR       IF_Nbr;
  CPU_BOOLEAN      IPv4_En;
  NET_CMD_IPv4_CFG IPv4Cfg;
  CPU_BOOLEAN      IPv6_En;
  NET_CMD_IPv6_CFG IPv6Cfg;
} NET_CMD_ROUTE_ARG;

typedef struct net_cmd_mtu_cmd_arg {
  CPU_CHAR *IF_NbrPtr;
  CPU_CHAR *MTU_Ptr;
} NET_CMD_MTU_CMD_ARG;

typedef struct net_cmd_mtu_arg {
  NET_IF_NBR IF_Nbr;
  CPU_INT16U MTU;
} NET_CMD_MTU_ARG;

typedef  struct  net_cmd_sock_open_cmd_arg {
  CPU_CHAR *FamilyPtr;
  CPU_CHAR *TypePtr;
} NET_CMD_SOCK_OPEN_CMD_ARG;

typedef  struct  net_cmd_sock_open_arg {
  NET_SOCK_PROTOCOL_FAMILY Family;
  NET_SOCK_TYPE            Type;
} NET_CMD_SOCK_OPEN_ARG;

typedef  struct  net_cmd_sock_id_cmd_arg {
  CPU_CHAR *SockIDPtr;
} NET_CMD_SOCK_ID_CMD_ARG;

typedef  struct  net_cmd_sock_id_arg {
  NET_SOCK_ID SockID;
} NET_CMD_SOCK_ID_ARG;

typedef  struct  net_cmd_sock_bind_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *PortPtr;
  CPU_CHAR *FamilyPtr;
} NET_CMD_SOCK_BIND_CMD_ARG;

typedef  struct  net_cmd_sock_bind_arg {
  NET_SOCK_ID              SockID;
  NET_PORT_NBR             Port;
  NET_SOCK_PROTOCOL_FAMILY Family;
} NET_CMD_SOCK_BIND_ARG;

typedef  struct  net_cmd_sock_listen_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *QueueSizePtr;
} NET_CMD_SOCK_LISTEN_CMD_ARG;

typedef  struct  net_cmd_sock_listen_arg {
  NET_SOCK_ID SockID;
  CPU_INT16U  QueueSize;
} NET_CMD_SOCK_LISTEN_ARG;

typedef  struct  net_cmd_sock_conn_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *AddrPtr;
  CPU_CHAR *PortPtr;
} NET_CMD_SOCK_CONN_CMD_ARG;

typedef  struct  net_cmd_sock_conn_arg {
  NET_SOCK_ID  SockID;
  NET_PORT_NBR Port;
  CPU_INT08U   Addr[NET_IP_MAX_ADDR_SIZE];
  CPU_INT08U   AddrLen;
} NET_CMD_SOCK_CONN_ARG;

typedef  struct  net_cmd_sock_rx_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *DataLenPtr;
  CPU_CHAR *OutputFmtPtr;
} NET_CMD_SOCK_RX_CMD_ARG;

typedef  struct  net_cmd_sock_rx_arg {
  NET_SOCK_ID        SockID;
  CPU_INT16U         DataLen;
  NET_CMD_OUTPUT_FMT OutputFmt;
} NET_CMD_SOCK_RX_ARG;

typedef  struct  net_cmd_sock_tx_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *DataLenPtr;
  CPU_CHAR *DataPtr;
} NET_CMD_SOCK_TX_CMD_ARG;

typedef  struct  net_cmd_sock_tx_arg {
  NET_SOCK_ID SockID;
  CPU_INT16U  DataLen;
  CPU_INT08U  *DataPtr;
} NET_CMD_SOCK_TX_ARG;

typedef  struct  net_cmd_sock_sel_cmd_arg {
  CPU_CHAR *RdListPtr;
  CPU_CHAR *WrListPtr;
  CPU_CHAR *ErrListPtr;
  CPU_CHAR *Timeout_sec_Ptr;
} NET_CMD_SOCK_SEL_CMD_ARG;

typedef  struct  net_cmd_sock_sel_arg {
  CPU_INT16S       SockNbrMax;
  NET_SOCK_DESC    DescRd;
  NET_SOCK_DESC    DescWr;
  NET_SOCK_DESC    DescErr;
  NET_SOCK_TIMEOUT Timeout;
} NET_CMD_SOCK_SEL_ARG;

typedef  struct  net_cmd_sock_opt_cmd_arg {
  CPU_CHAR *SockIDPtr;
  CPU_CHAR *ValPtr;
} NET_CMD_SOCK_OPT_CMD_ARG;

typedef  struct  net_cmd_sock_opt_arg {
  NET_SOCK_ID SockID;
  CPU_INT32U  Value;
} NET_CMD_SOCK_OPT_ARG;

typedef  struct  net_cmd_sock_arg {
  CPU_CHAR *IF_NbrPtr;
  CPU_CHAR *IDPtr;
  CPU_CHAR *DataLenPtr;
  CPU_CHAR *DataPtr;
  CPU_CHAR *AddrPtr;
  CPU_CHAR *PortPtr;
  CPU_CHAR *AcceptQueueSizePtr;
  CPU_CHAR *SockFamilyPtr;
  CPU_CHAR *SockTypePtr;
  CPU_CHAR *FmtPtr;
  CPU_CHAR *RetryPtr;
#if 0
  CPU_CHAR *OptNamePtr;
  CPU_CHAR *OptValPtr;
  CPU_CHAR *RdListPtr;
  CPU_CHAR *WrListPtr;
  CPU_CHAR *ErrListPtr;
  CPU_CHAR *Timeout_sec_Ptr;
#endif
} NET_CMD_SOCK_ARG;

typedef  struct  net_arg {
  NET_IF_NBR         IF;
  NET_SOCK_ID        ID;
  NET_PORT_NBR       Port;
  CPU_INT08U         Addr[NET_IP_MAX_ADDR_SIZE];
  NET_IP_ADDR_FAMILY AddrFamily;
  NET_SOCK_ADDR_LEN  AddrLen;
  NET_SOCK_ADDR_LEN  AddrSockLen;
  CPU_INT08U         SockFamily;
  CPU_INT08U         SockType;
  CPU_INT16U         AcceptQueueSize;
  NET_CMD_OUTPUT_FMT Fmt;
  CPU_INT32U         DataLen;
  CPU_INT08U         *DataPtr;
  CPU_INT32U         Retry;
#if 0
  NET_SOCK_OPT_NAME  OptName;
  CPU_INT32U         OptValue;
  CPU_INT16S         SelNbrMax;
  NET_SOCK_DESC      SelDescRd;
  NET_SOCK_DESC      SelDescWr;
  NET_SOCK_DESC      SelDescErr;
  NET_SOCK_TIMEOUT   SelTimeout;
#endif
} NET_CMD_SOCK_VAL;

typedef  CPU_INT32U NET_CMD_DICTIONARY_KEY;

#define  NET_CMD_DICTIONARY_KEY_INVALID            DEF_INT_32U_MAX_VAL

typedef  struct  net_cmd_dictionary {
  const NET_CMD_DICTIONARY_KEY Key;
  const CPU_CHAR               *StrPtr;
  const CPU_INT32U             StrLen;
} NET_CMD_DICTIONARY;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern CPU_BOOLEAN NetCmd_InitDone;

/********************************************************************************************************
 ********************************************************************************************************
 *                                         FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

void NetCmd_Init(RTOS_ERR *p_err);

void NetCmd_IF_CfgInit(NET_CMD_STR_ARGS *p_str_args,
                       NET_IF_CFG       *p_if_cfg,
                       RTOS_ERR         *p_err);

CPU_INT16S NetCmd_Help(CPU_INT16U      argc,
                       CPU_CHAR        *p_argv[],
                       SHELL_OUT_FNCT  out_fnct,
                       SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_Config(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_Reset(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_SetMTU(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_RouteAdd(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_RouteRemove(CPU_INT16U      argc,
                                 CPU_CHAR        *p_argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param);

void NetCmd_IF_IPv4AddrCfgStatic(NET_IF_NBR       if_id,
                                 NET_CMD_IPv4_CFG *p_ip_cfg,
                                 RTOS_ERR         *p_err);

void NetCmd_IF_IPv6AddrCfgStatic(NET_IF_NBR       if_id,
                                 NET_CMD_IPv6_CFG *p_ip_cfg,
                                 RTOS_ERR         *p_err);

void NetCmd_IF_IPv4AddrRemove(NET_IF_NBR       if_id,
                              NET_CMD_IPv4_CFG *p_ip_cfg,
                              RTOS_ERR         *p_err);

void NetCmd_IF_IPv6AddrRemove(NET_IF_NBR       if_id,
                              NET_CMD_IPv6_CFG *p_ip_cfg,
                              RTOS_ERR         *p_err);

CPU_INT16S NetCmd_Ping(CPU_INT16U      argc,
                       CPU_CHAR        *p_argv[],
                       SHELL_OUT_FNCT  out_fnct,
                       SHELL_CMD_PARAM *p_cmd_param);

#ifdef  NET_IPv4_MODULE_EN
CPU_INT16S NetCmd_IP_Config(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);
#endif

CPU_INT16S NetCmd_IF_Start(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_Stop(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_Restart(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_BufRxLargeStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_BufTxLargeStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_IF_BufTxSmallStat(CPU_INT16U      argc,
                                    CPU_CHAR        *p_argv[],
                                    SHELL_OUT_FNCT  out_fnct,
                                    SHELL_CMD_PARAM *p_cmd_param);

#ifdef  NET_IF_WIFI_MODULE_EN
CPU_INT16S NetCmd_WiFi_Scan(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_WiFi_Join(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_WiFi_Create(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_WiFi_Leave(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_WiFi_GetPeerInfo(CPU_INT16U      argc,
                                   CPU_CHAR        *p_argv[],
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_WiFi_Status(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);
#endif

CPU_INT16S NetCmd_Sock_Open(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Close(CPU_INT16U      argc,
                             CPU_CHAR        *p_argv[],
                             SHELL_OUT_FNCT  out_fnct,
                             SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Bind(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Listen(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Accept(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Conn(CPU_INT16U      argc,
                            CPU_CHAR        *p_argv[],
                            SHELL_OUT_FNCT  out_fnct,
                            SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Rx(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_Sock_Tx(CPU_INT16U      argc,
                          CPU_CHAR        *p_argv[],
                          SHELL_OUT_FNCT  out_fnct,
                          SHELL_CMD_PARAM *p_cmd_param);

#if 0
CPU_INT16S NetCmd_Sock_Sel(CPU_INT16U      argc,
                           CPU_CHAR        *p_argv[],
                           SHELL_OUT_FNCT  out_fnct,
                           SHELL_CMD_PARAM *p_cmd_param);
#endif

CPU_INT16S NetCmd_SockOptSetChild(CPU_INT16U      argc,
                                  CPU_CHAR        *p_argv[],
                                  SHELL_OUT_FNCT  out_fnct,
                                  SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_SockMcastJoin(CPU_INT16U      argc,
                                CPU_CHAR        *p_argv[],
                                SHELL_OUT_FNCT  out_fnct,
                                SHELL_CMD_PARAM *p_cmd_param);

CPU_INT16S NetCmd_SockMcastLeave(CPU_INT16U      argc,
                                 CPU_CHAR        *p_argv[],
                                 SHELL_OUT_FNCT  out_fnct,
                                 SHELL_CMD_PARAM *p_cmd_param);

NET_CMD_PING_CMD_ARG NetCmd_PingCmdArgParse(CPU_INT16U argc,
                                            CPU_CHAR   *p_argv[],
                                            RTOS_ERR   *p_err);

NET_CMD_PING_ARG NetCmd_PingCmdArgTranslate(NET_CMD_PING_CMD_ARG *p_cmd_args,
                                            RTOS_ERR             *p_err);

CPU_INT32U NetCmd_DictionaryGet(const NET_CMD_DICTIONARY *p_dictionary_tbl,
                                CPU_INT32U               dictionary_size,
                                const CPU_CHAR           *p_str_cmp,
                                CPU_INT32U               str_len);

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_COMMON_SHELL_AVAIL
#endif // _NET_CMD_PRIV_H_
