/***************************************************************************//**
 * @file
 * @brief Network - Ftp Client Module
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

/****************************************************************************************************//**
 * @defgroup NET_FTP FTP Client API
 * @ingroup  NET
 * @brief    FTP Client API
 *
 * @addtogroup NET_FTP
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _FTP_CLIENT_H_
#define  _FTP_CLIENT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#include  <cpu/include/cpu.h>

#include  <net/include/net_ip.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_type.h>
#include  <net/include/net_cfg_net.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  FTPc_CTRL_NET_BUF_SIZE                         1024    // Ctrl buffer size.
#define  FTPc_DTP_NET_BUF_SIZE                          1460    // Dtp buffer size.

#define  FTPc_PORT_DFLT                                   21

/********************************************************************************************************
 *                                       FTP SECURE CFG DATA TYPE
 * @brief FTP Client secure configuration
 *******************************************************************************************************/

typedef  struct  ftpc_secure_cfg {
  CPU_CHAR                   *CommonName;
  NET_SOCK_SECURE_TRUST_FNCT TrustCallback;
} FTPc_SECURE_CFG;

/********************************************************************************************************
 *                                    FTPc CONFIGURATION DATA TYPE
 * @brief FTP Client  configuration
 *******************************************************************************************************/

typedef  struct  ftpc_cfg {
  CPU_INT32U CtrlConnMaxTimout_ms;
  CPU_INT32U CtrlRxMaxTimout_ms;
  CPU_INT32U CtrlTxMaxTimout_ms;

  CPU_INT32U CtrlRxMaxDly_ms;

  CPU_INT32U CtrlTxMaxRetry;
  CPU_INT32U CtrlTxMaxDly_ms;

  CPU_INT32U DTP_ConnMaxTimout_ms;
  CPU_INT32U DTP_RxMaxTimout_ms;
  CPU_INT32U DTP_TxMaxTimout_ms;

  CPU_INT32U DTP_TxMaxRetry;
  CPU_INT32U DTP_TxMaxDly_ms;
} FTPc_CFG;

typedef  struct  ftpc_conn {
  NET_SOCK_ID           SockID;
  NET_SOCK_ADDR         SockAddr;
  NET_IP_ADDR_FAMILY    SockAddrFamily;
#ifdef  NET_SECURE_MODULE_EN
  const FTPc_SECURE_CFG *SecureCfgPtr;
#endif
  CPU_INT08U            Buf[FTPc_CTRL_NET_BUF_SIZE];
  const FTPc_CFG        *CfgPtr;
} FTPc_CONN;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

CPU_BOOLEAN FTPc_Open(FTPc_CONN             *p_conn,
                      const FTPc_CFG        *p_cfg,
                      const FTPc_SECURE_CFG *p_secure_cfg,
                      CPU_CHAR              *p_host_server,
                      NET_PORT_NBR          port_nbr,
                      CPU_CHAR              *p_user,
                      CPU_CHAR              *p_pass,
                      RTOS_ERR              *p_err);

CPU_BOOLEAN FTPc_Close(FTPc_CONN *p_conn,
                       RTOS_ERR  *p_err);

CPU_BOOLEAN FTPc_RecvBuf(FTPc_CONN  *p_conn,
                         CPU_CHAR   *p_remote_file_name,
                         CPU_INT08U *p_buf,
                         CPU_INT32U buf_len,
                         CPU_INT32U *p_file_size,
                         RTOS_ERR   *p_err);

CPU_BOOLEAN FTPc_SendBuf(FTPc_CONN   *p_conn,
                         CPU_CHAR    *p_remote_file_name,
                         CPU_INT08U  *p_buf,
                         CPU_INT32U  buf_len,
                         CPU_BOOLEAN append,
                         RTOS_ERR    *p_err);

CPU_BOOLEAN FTPc_RecvFile(FTPc_CONN *p_conn,
                          CPU_CHAR  *p_remote_file_name,
                          CPU_CHAR  *p_local_file_name,
                          RTOS_ERR  *p_err);

CPU_BOOLEAN FTPc_SendFile(FTPc_CONN   *p_conn,
                          CPU_CHAR    *p_remote_file_name,
                          CPU_CHAR    *p_local_file_name,
                          CPU_BOOLEAN append,
                          RTOS_ERR    *p_net);

#ifdef __cplusplus
}
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                         CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef NET_TCP_MODULE_EN
#error  "NET_TCP_CFG_EN illegally #define'd in 'net_cfg.h' [MUST be  DEF_ENABLED]"
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _FTP_CLIENT_H_
