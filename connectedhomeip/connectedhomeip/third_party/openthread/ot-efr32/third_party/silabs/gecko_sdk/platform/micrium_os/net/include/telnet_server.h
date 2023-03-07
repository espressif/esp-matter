/***************************************************************************//**
 * @file
 * @brief Network - Telnet Server Module
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
 * @defgroup NET_TELNET Telnet Server API
 * @ingroup  NET
 * @brief    Telnet Server API
 *
 * @addtogroup NET_TELNET
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _TELNET_SERVER_H_
#define  _TELNET_SERVER_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/net.h>
#include  <net/include/net_sock.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/collections/slist_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                              TELNET SERVER DEFAULT CONFIGURATION
 *
 * Note(s) : (1) The inactivity timeout defines the maximum amount of time the server waits for a client
 *               to transmit data.  Once this time has expired, the server closes the connection and waits
 *               for other client to connect.
 *
 *           (2) The receive buffer is used to hold the data as well as the incoming option requests and
 *               replies.  Its size should hence be slightly larger than the NVT buffer.  This later should
 *               be defined to be as large as the longest line of data transmitted by the client.
 *
 *           (3) The maximum path name configuration defined should be set in accordance with the file
 *               system being used on your target, and only applies if a file system is configured with
 *               this module.  This default setting is the appropriate one for Micrium OS File System.
 *******************************************************************************************************/

#define  TELNET_SERVER_CFG_SOCK_FAMILY_DFLT                     TELNETs_IP_TYPE_IPv4
#define  TELNET_SERVER_CFG_WELCOME_MSG_STR_DFLT                 "\r\nWelcome to Micrium Telnet Server\r\n\r\n"
#define  TELNET_SERVER_CFG_SERVER_PORT_DFLT                     23u
#define  TELNET_SERVER_CFG_INACTIVITY_TIMEOUT_SEC_DFLT          3600u
#define  TELNET_SERVER_CFG_TX_TRIES_NBR_MAX_DFLT                3u
#define  TELNET_SERVER_CFG_RX_BUF_LEN_DFLT                      512u
#define  TELNET_SERVER_CFG_NVT_BUF_LEN_DFLT                     512u
#define  TELNET_SERVER_CFG_USERNAME_STR_LEN_MAX_DFLT            32u
#define  TELNET_SERVER_CFG_PASSWORD_STR_LEN_MAX_DFLT            32u
#define  TELNET_SERVER_CFG_LOGIN_TRIES_NBR_MAX_DFLT             3u
#define  TELNET_SERVER_CFG_FS_EN_DFLT                           DEF_ENABLED
#define  TELNET_SERVER_CFG_ECHO_EN_DFLT                         DEF_DISABLED
#define  TELNET_SERVER_CFG_FS_PATH_LEN_MAX_DFLT                 128u

#define  TELNET_SERVER_TASK_CFG_STK_SIZE_ELEMENTS_DFLT          256u
#define  TELNET_SERVER_TASK_CFG_STK_PTR_DFLT                    DEF_NULL
#define  TELNET_SERVER_SESSION_TASK_CFG_STK_SIZE_ELEMENTS_DFLT  512u
#define  TELNET_SERVER_SESSION_TASK_CFG_STK_PTR_DFLT            DEF_NULL

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                              TELNET OUTPUT FUNCTION POINTER DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16S (*TELNET_OUT_FNCT)(CPU_CHAR *,
                                       CPU_INT16U,
                                       void *);

/********************************************************************************************************
 *                                      TELNET HOOKS DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16S (*TELNET_CMD_HANDLER_EXT_HOOK) (CPU_CHAR *,
                                                    void *,
                                                    CPU_BOOLEAN *,
                                                    void *,
                                                    TELNET_OUT_FNCT,
                                                    RTOS_ERR *);

/********************************************************************************************************
 *                                    SOCKET FAMILY TYPE ENUMERATION
 *******************************************************************************************************/

typedef enum telnets_sock_sel {
  TELNETs_IP_TYPE_IPv4,
  TELNETs_IP_TYPE_IPv6,
  TELNETs_IP_TYPE_IPv4_IPv6
} TELNETs_IP_TYPE;

/********************************************************************************************************
 *                            TELNETs SECURE SESSION CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  TELNETs_SecureCfg {
  CPU_CHAR                     *CertPtr;
  CPU_INT32U                   CertLen;
  CPU_CHAR                     *KeyPtr;
  CPU_INT32U                   KeyLen;
  NET_SOCK_SECURE_CERT_KEY_FMT Fmt;
  CPU_BOOLEAN                  CertChain;
} TELNETs_SECURE_CFG;

/********************************************************************************************************
 *                                   TELNETs CONFIGURATION DATA TYPE
 *******************************************************************************************************/

typedef  struct  telnets_cfg {
  CPU_INT16U         Port;
  TELNETs_IP_TYPE    IP_Type;
  CPU_INT32U         ConnInactivityTimeout_s;
  CPU_INT08U         TxTriesMaxNbr;
  CPU_INT16U         RxBufLen;
  CPU_INT16U         NVT_BufLen;
  CPU_INT16U         UsernameStrLenMax;
  CPU_INT16U         PasswordStrLenMax;
  CPU_INT08U         LoginTriesMaxNbr;
  CPU_BOOLEAN        EchoEn;
  CPU_INT16U         FS_PathLenMax;
  CPU_CHAR           *WelcomeMsgStr;
  TELNETs_SECURE_CFG *SecureCfgPtr;
} TELNETs_CFG;

typedef  struct  telnets_init_cfg {
  MEM_SEG *MemSegPtr;                                           // Pointer to the Telnet server memory segment.
} TELNETs_INIT_CFG;

/********************************************************************************************************
 *                                     TELNETS INSTANCE DATA TYPE
 *******************************************************************************************************/

typedef  struct  telnets_instance {
  const TELNETs_CFG   *CfgPtr;
  const RTOS_TASK_CFG *TaskSrvCfgPtr;
  const RTOS_TASK_CFG *TaskSessionCfgPtr;

  KAL_TASK_HANDLE     TaskSrvHandle;
  KAL_TASK_HANDLE     TaskSessionHandle;

  NET_SOCK_ID         SockID;

  CPU_INT32U          NbrActiveSession;

  SLIST_MEMBER        *SessionListPtr;

  MEM_DYN_POOL        SessionPool;
  MEM_DYN_POOL        RxBufPool;
  MEM_DYN_POOL        NVT_BufPool;
  MEM_DYN_POOL        UserNamePool;
  MEM_DYN_POOL        PasswordPool;
  MEM_DYN_POOL        WorkingDirPool;
} TELNETs_INSTANCE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const TELNETs_INIT_CFG TELNETs_InitCfgDflt;
#endif

extern const TELNETs_CFG   TELNETs_CfgDflt;
extern const RTOS_TASK_CFG TELNETs_TaskSrvCfgDflt;
extern const RTOS_TASK_CFG TELNETs_TaskSessionCfgDflt;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void TELNETs_ConfigureMemSeg(MEM_SEG *p_mem_seg);
#endif

void TELNETs_Init(RTOS_ERR *p_err);

TELNETs_INSTANCE *TELNETs_InstanceInit(const TELNETs_CFG   *p_cfg,
                                       const RTOS_TASK_CFG *p_task_srv_cfg,
                                       const RTOS_TASK_CFG *p_task_session_cfg,
                                       RTOS_ERR            *p_err);

void TELNETs_InstanceStart(TELNETs_INSTANCE *p_instance,
                           RTOS_ERR         *p_err);

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

/********************************************************************************************************
 ********************************************************************************************************
 *                                       MODULE AVAILABLE ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  RTOS_MODULE_COMMON_SHELL_AVAIL
#error  "RTOS_MODULE_COMMON_SHELL_AVAIL must exist for the Telnet server module to work."
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _TELNET_SERVER_H_
