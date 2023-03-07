/***************************************************************************//**
 * @file
 * @brief Network Security Port Layer
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
 *
 * Note(s) : (1) The following secure-module-present configuration value MUST be pre-#define'd in
 *               'net_cfg_net.h' PRIOR to all other network modules that require Network Security Layer
 *               configuration (see 'net_cfg_net.h  NETWORK SECURITY MANAGER CONFIGURATION  Note #2b') :
 *
 *                   NET_SECURE_MODULE_EN
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _NET_SECURE_PRIV_H_
#define  _NET_SECURE_PRIV_H_

#include  <net/include/net_cfg_net.h>

#ifdef  NET_SECURE_MODULE_EN

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net_secure.h>
#include  <net/source/tcpip/net_sock_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------- INIT FNCTS --------------
void NetSecure_Init(MEM_SEG  *p_mem_seg,
                    RTOS_ERR *p_err);

void NetSecure_InitSession(NET_SOCK *p_sock,
                           RTOS_ERR *p_err);

//                                                                 ------------ SOCK CFG FNCTS ------------
#if (NET_SECURE_CFG_MAX_NBR_SOCK_SERVER > 0u \
     || NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0u)
CPU_BOOLEAN NetSecure_SockCertKeyCfg(NET_SOCK                     *p_sock,
                                     NET_SOCK_SECURE_TYPE         sock_type,
                                     const CPU_INT08U             *p_buf_cert,
                                     CPU_SIZE_T                   buf_cert_size,
                                     const CPU_INT08U             *p_buf_key,
                                     CPU_SIZE_T                   buf_key_size,
                                     NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                     CPU_BOOLEAN                  cert_chain,
                                     RTOS_ERR                     *p_err);
#endif

#if 0
CPU_BOOLEAN NetSecure_SockServerCertKeyFiles(NET_SOCK                     *p_sock,
                                             const void                   *pfilename_cert,
                                             const void                   *pfilename_key,
                                             NET_SOCK_SECURE_CERT_KEY_FMT fmt,
                                             CPU_BOOLEAN                  cert_chain,
                                             RTOS_ERR                     *p_err);
#endif

#if (NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)
CPU_BOOLEAN NetSecure_ClientCommonNameSet(NET_SOCK *p_sock,
                                          CPU_CHAR *p_common_name,
                                          RTOS_ERR *p_err);

CPU_BOOLEAN NetSecure_ClientTrustCallBackSet(NET_SOCK                   *p_sock,
                                             NET_SOCK_SECURE_TRUST_FNCT p_callback_fnct,
                                             RTOS_ERR                   *p_err);
#endif

//                                                                 ----------- SOCK HANDLER FNCTS ---------
void NetSecure_SockClose(NET_SOCK *p_sock,
                         RTOS_ERR *p_err);

void NetSecure_SockCloseNotify(NET_SOCK *p_sock,
                               RTOS_ERR *p_err);

void NetSecure_SockConn(NET_SOCK *p_sock,
                        RTOS_ERR *p_err);

void NetSecure_SockAccept(NET_SOCK *p_sock_listen,
                          NET_SOCK *p_sock_accept,
                          RTOS_ERR *p_err);

NET_SOCK_RTN_CODE NetSecure_SockRxDataHandler(NET_SOCK   *p_sock,
                                              void       *p_data_buf,
                                              CPU_INT16U data_buf_len,
                                              RTOS_ERR   *p_err);

#if (NET_SOCK_CFG_SEL_EN == DEF_ENABLED)
CPU_BOOLEAN NetSecure_SockRxIsDataPending(NET_SOCK *p_sock,
                                          RTOS_ERR *p_err);
#endif

NET_SOCK_RTN_CODE NetSecure_SockTxDataHandler(NET_SOCK   *p_sock,
                                              void       *p_data_buf,
                                              CPU_INT16U data_buf_len,
                                              RTOS_ERR   *p_err);

void NetSecure_SockDNS_NameSet(NET_SOCK *p_sock,
                               CPU_CHAR *p_dns_name,
                               RTOS_ERR *p_err);

void NetSecure_TraceOut(CPU_CHAR *p_msg);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef NET_SECURE_CFG_MAX_NBR_SOCK_SERVER
#error  "NET_SECURE_CFG_MAX_NBR_SOCK_SERVER        not #define'd in 'net_cfg.h' "
#error  "                                    [MUST be  >= 0                    ]"
#error  "                                    [     &&  <= NET_SOCK_CFG_NBR_SOCK]"

#elif  (NET_SECURE_CFG_MAX_NBR_SOCK_SERVER > 0)

#ifndef  NET_SECURE_CFG_MAX_CERT_LEN
#error  "NET_SECURE_CFG_MAX_CERT_LEN               not #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "

#elif   (DEF_CHK_VAL_MIN(NET_SECURE_CFG_MAX_CERT_LEN, 1) != DEF_OK)
#error  "NET_SECURE_CFG_MAX_CERT_LEN         illegally #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "
#endif

#ifndef  NET_SECURE_CFG_MAX_KEY_LEN
#error  "NET_SECURE_CFG_MAX_KEY_LEN                not #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "

#elif   (DEF_CHK_VAL_MIN(NET_SECURE_CFG_MAX_KEY_LEN, 1) != DEF_OK)
#error  "NET_SECURE_CFG_MAX_KEY_LEN          illegally #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "
#endif

#endif

#ifndef  NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT
#error  "NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT        not #define'd in 'net_cfg.h' "
#error  "                                    [MUST be  >= 0                    ]"
#error  "                                    [     &&  <= NET_TCP_CFG_NBR_CONN ]"

#elif  (NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > 0)

#ifndef  NET_SECURE_CFG_MAX_NBR_CA
#error  "NET_SECURE_CFG_MAX_NBR_CA                 not #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  = 1]                    "

#elif   (NET_SECURE_CFG_MAX_NBR_CA != 1)
#error  "NET_SECURE_CFG_MAX_NBR_CA           illegally #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  = 1]                    "
#endif

#ifndef  NET_SECURE_CFG_MAX_CA_CERT_LEN
#error  "NET_SECURE_CFG_MAX_CA_CERT_LEN            not #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "

#elif   (DEF_CHK_VAL_MIN(NET_SECURE_CFG_MAX_CA_CERT_LEN, 1) != DEF_OK)
#error  "NET_SECURE_CFG_MAX_CA_CERT_LEN      illegally #define'd in 'net_cfg.h'"
#error  "                                    [MUST be  > 0]                    "
#endif

#endif

#if (NET_SECURE_CFG_MAX_NBR_SOCK_SERVER \
     + NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT > NET_SOCK_CFG_SOCK_NBR_TCP)
#error  "NET_SECURE_CFG_MAX_NBR_SOCK_SERVER and/or                              "
#error  "NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT  illegally #define'd in 'net_cfg.h' "
#error  "NET_SECURE_CFG_MAX_NBR_SOCK_SERVER + NET_SECURE_CFG_MAX_NBR_SOCK_CLIENT"
#error  "                                    [MUST be  <= NET_TCP_CFG_NBR_CONN ]"
#endif

#ifndef  NET_SECURE_CFG_MAX_NBR_CA
#error  "NET_SECURE_CFG_MAX_NBR_CA                 not #define'd in 'net_cfg.h'"
#error  "                                    [MUST be   define'd in 'net_cfg.h]"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // NET_SECURE_MODULE_EN
#endif // _NET_SECURE_PRIV_H_
