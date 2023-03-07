/***************************************************************************//**
 * @file
 * @brief Network Command Argument Parsing Utilities
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

#ifndef  _NET_CMD_ARGS_PARSER_PRIV_H_
#define  _NET_CMD_ARGS_PARSER_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_sock.h>
#include  <net/include/net_if.h>
#include  <net/include/dhcp_client_types.h>
#include  <net/source/cmd/net_cmd_priv.h>
#include  <net/source/cmd/net_cmd_output_priv.h>
#include  <net/source/tcpip/net_if_priv.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

struct  net_cmd_cmd_args {
  NET_IF_CFG       IF_Cfg;
  CPU_CHAR         *WindowsIF_Nbr_Ptr;
  NET_CMD_AUTH_CFG AuthCfg;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                              MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  NET_CMD_ARGS_PARSER_CMD_ARGS_INIT(cmd_arg)   {      \
    cmd_arg.WindowsIF_Nbr_Ptr = DEF_NULL;                    \
    cmd_arg.IF_Cfg.IPv4.Static.Addr = DEF_NULL;              \
    cmd_arg.IF_Cfg.IPv4.Static.Mask = DEF_NULL;              \
    cmd_arg.IF_Cfg.IPv4.Static.Gateway = DEF_NULL;           \
    cmd_arg.IF_Cfg.IPv4.LinkLocal.En = DEF_NO;               \
    cmd_arg.IF_Cfg.IPv4.LinkLocal.OnCompleteHook = DEF_NULL; \
    cmd_arg.IF_Cfg.IPv4.DHCPc.En = DEF_NO;                   \
    cmd_arg.IF_Cfg.IPv4.DHCPc.OnCompleteHook = DEF_NULL;     \
    DHCPc_CFG_DFLT_INIT(cmd_arg.IF_Cfg.IPv4.DHCPc.Cfg);      \
    cmd_arg.IF_Cfg.IPv4.LinkLocal.En = DEF_NO;               \
    cmd_arg.IF_Cfg.IPv4.LinkLocal.OnCompleteHook = DEF_NULL; \
    cmd_arg.IF_Cfg.IPv6.Static.Addr = DEF_NULL;              \
    cmd_arg.IF_Cfg.IPv6.Static.PrefixLen = 0;                \
    cmd_arg.IF_Cfg.IPv6.Static.DAD_En = DEF_NO;              \
    cmd_arg.IF_Cfg.IPv6.AutoCfg.En = DEF_NO;                 \
    cmd_arg.IF_Cfg.IPv6.AutoCfg.DAD_En = DEF_YES;            \
    cmd_arg.IF_Cfg.IPv6.Hook = DEF_NULL;                     \
    cmd_arg.IF_Cfg.HW_AddrStr = DEF_NULL;                    \
    cmd_arg.AuthCfg.User = DEF_NULL;                         \
    cmd_arg.AuthCfg.Password = DEF_NULL;                     \
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

NET_CMD_STR_ARGS NetCmd_ArgsParserCmdParse(CPU_INT16U argc,
                                           CPU_CHAR   *p_argv[],
                                           RTOS_ERR   *p_err);

CPU_INT08U NetCmd_ArgsParserParseID_Nbr(CPU_CHAR *p_argv[],
                                        CPU_CHAR **p_str_if_nbr,
                                        RTOS_ERR *p_err);

CPU_INT16U NetCmd_ArgsParserTranslateID_Nbr(CPU_CHAR *p_str_if_nbr,
                                            RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseIPv4(CPU_CHAR               *p_argv[],
                                      NET_IF_IPv4_STATIC_CFG *p_ip_cfg,
                                      RTOS_ERR               *p_err);

NET_CMD_IPv4_CFG NetCmd_ArgsParserTranslateIPv4(NET_IF_IPv4_STATIC_CFG *p_ip_cfg,
                                                RTOS_ERR               *p_err);

CPU_INT08U NetCmd_ArgsParserParseIPv6(CPU_CHAR               *p_argv[],
                                      NET_IF_IPv6_STATIC_CFG *p_ip_cfg,
                                      RTOS_ERR               *p_err);

NET_CMD_IPv6_CFG NetCmd_ArgsParserTranslateIPv6(NET_IF_IPv6_STATIC_CFG *p_ip_cfg,
                                                RTOS_ERR               *p_err);

CPU_INT08U NetCmd_ArgsParserParseMAC(CPU_CHAR *p_argv[],
                                     CPU_CHAR **p_str_mac,
                                     RTOS_ERR *p_err);

NET_CMD_MAC_CFG NetCmd_ArgsParserTranslateMAC(CPU_CHAR *p_str_mac,
                                              RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseDataLen(CPU_CHAR *p_argv[],
                                         CPU_CHAR **p_str_len,
                                         RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseFmt(CPU_CHAR *p_argv[],
                                     CPU_CHAR **p_str_len,
                                     RTOS_ERR *p_err);

NET_CMD_OUTPUT_FMT NetCmd_ArgsParserTranslateFmt(CPU_CHAR *p_char_type,
                                                 RTOS_ERR *p_err);

CPU_INT16U NetCmd_ArgsParserTranslateDataLen(CPU_CHAR *p_str_len,
                                             RTOS_ERR *p_err);

CPU_INT32U NetCmd_ArgsParserTranslateVal32U(CPU_CHAR *p_str_len,
                                            RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseCredential(CPU_CHAR         *p_argv[],
                                            NET_CMD_AUTH_CFG *p_credential,
                                            RTOS_ERR         *p_err);

NET_CMD_CREDENTIAL_CFG NetCmd_ArgsParserTranslateCredential(NET_CMD_AUTH_CFG *p_credential,
                                                            RTOS_ERR         *p_err);

CPU_INT08U NetCmd_ArgsParserParseMTU(CPU_CHAR *p_argv[],
                                     CPU_CHAR **p_str_len,
                                     RTOS_ERR *p_err);

CPU_INT16U NetCmd_ArgsParserTranslateMTU(CPU_CHAR *p_str_len,
                                         RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseSockFamily(CPU_CHAR *p_argv[],
                                            CPU_CHAR **p_str_family,
                                            RTOS_ERR *p_err);

NET_SOCK_PROTOCOL_FAMILY NetCmd_ArgsParserTranslateSockFamily(CPU_CHAR *p_char_family,
                                                              RTOS_ERR *p_err);

CPU_INT08U NetCmd_ArgsParserParseSockType(CPU_CHAR *p_argv[],
                                          CPU_CHAR **p_str_len,
                                          RTOS_ERR *p_err);

NET_SOCK_TYPE NetCmd_ArgsParserTranslateSockType(CPU_CHAR *p_char_family,
                                                 RTOS_ERR *p_err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _NET_CMD_ARGS_PARSER_PRIV_H_
