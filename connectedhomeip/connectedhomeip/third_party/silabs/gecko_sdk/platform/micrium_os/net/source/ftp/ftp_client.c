/***************************************************************************//**
 * @file
 * @brief Network - FTP Client
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

#if (defined(RTOS_MODULE_NET_FTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error FTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <cpu/include/cpu.h>

#include  <common/include/lib_def.h>
#include  <common/include/rtos_err.h>
#include  <common/include/lib_str.h>
#include  <common/include/toolchains.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

#include  <common/source/logging/logging_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/kal/kal_priv.h>

#include  <net/include/ftp_client.h>
#include  <net/include/net_tcp.h>
#include  <net/include/net_app.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_fs.h>
#include  <net/include/net_def.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                        (NET, FTP)
#define  RTOS_MODULE_CUR                     RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                       DEFAULT CONFIGURATION DEFINES
 *******************************************************************************************************/

#ifndef FTPc_CFG_DFLT_CTRL_MAX_CONN_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_CTRL_MAX_CONN_TIMEOUT_MS    5000u    // Maximum inactivity time (ms) on CONNECT.
#endif

#ifndef FTPc_CFG_DFLT_CTRL_MAX_RX_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_CTRL_MAX_RX_TIMEOUT_MS      5000u    // Maximum inactivity time (ms) on RX.
#endif

#ifndef FTPc_CFG_DFLT_CTRL_MAX_TX_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_CTRL_MAX_TX_TIMEOUT_MS      5000u    // Maximum inactivity time (ms) on TX.
#endif

#ifndef FTPc_CFG_DFLT_CTRL_MAX_RX_DLY_MS
    #define  FTPc_CFG_DFLT_CTRL_MAX_RX_DLY_MS           100u    // Delay between each retries on RX.
#endif

#ifndef FTPc_CFG_DFLT_CTRL_MAX_TX_RETRY
    #define  FTPc_CFG_DFLT_CTRL_MAX_TX_RETRY              3u    // Maximum number of retries on TX.
#endif

#ifndef FTPc_CFG_DFLT_CTRL_MAX_TX_DLY_MS
    #define  FTPc_CFG_DFLT_CTRL_MAX_TX_DLY_MS           100u    // Delay between each retries on TX.
#endif

#ifndef FTPc_CFG_DFLT_DTP_MAX_CONN_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_DTP_MAX_CONN_TIMEOUT_MS     5000u    // Maximum inactivity time (ms) on CONNECT.
#endif

#ifndef FTPc_CFG_DFLT_DTP_MAX_RX_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_DTP_MAX_RX_TIMEOUT_MS       5000u    // Maximum inactivity time (ms) on RX.
#endif

#ifndef FTPc_CFG_DFLT_DTP_MAX_TX_TIMEOUT_MS
    #define  FTPc_CFG_DFLT_DTP_MAX_TX_TIMEOUT_MS       5000u    // Maximum inactivity time (ms) on TX.
#endif

#ifndef FTPc_CFG_DFLT_DTP_MAX_TX_RETRY
    #define  FTPc_CFG_DFLT_DTP_MAX_TX_RETRY               3u    // Maximum number of retries on TX.
#endif

#ifndef FTPc_CFG_DFLT_DTP_MAX_TX_DLY_MS
    #define  FTPc_CFG_DFLT_DTP_MAX_TX_DLY_MS            100u    // Delay between each retries on TX.
#endif

/********************************************************************************************************
 *                                               FTP COMMANDS
 *******************************************************************************************************/

#define  FTP_CMD_NOOP                                      0
#define  FTP_CMD_QUIT                                      1
#define  FTP_CMD_REIN                                      2
#define  FTP_CMD_SYST                                      3
#define  FTP_CMD_FEAT                                      4
#define  FTP_CMD_HELP                                      5
#define  FTP_CMD_USER                                      6
#define  FTP_CMD_PASS                                      7
#define  FTP_CMD_MODE                                      8
#define  FTP_CMD_TYPE                                      9
#define  FTP_CMD_STRU                                     10
#define  FTP_CMD_PASV                                     11
#define  FTP_CMD_PORT                                     12
#define  FTP_CMD_PWD                                      13
#define  FTP_CMD_CWD                                      14
#define  FTP_CMD_CDUP                                     15
#define  FTP_CMD_MKD                                      16
#define  FTP_CMD_RMD                                      17
#define  FTP_CMD_NLST                                     18
#define  FTP_CMD_LIST                                     19
#define  FTP_CMD_RETR                                     20
#define  FTP_CMD_STOR                                     21
#define  FTP_CMD_APPE                                     22
#define  FTP_CMD_REST                                     23
#define  FTP_CMD_DELE                                     24
#define  FTP_CMD_RNFR                                     25
#define  FTP_CMD_RNTO                                     26
#define  FTP_CMD_SIZE                                     27
#define  FTP_CMD_MDTM                                     28
#define  FTP_CMD_PBSZ                                     29
#define  FTP_CMD_PROT                                     30
#define  FTP_CMD_EPSV                                     31
#define  FTP_CMD_EPRT                                     32
#define  FTP_CMD_MAX                                      33    // This line MUST be the LAST!

/********************************************************************************************************
 *                                           FTP REPLY MESSAGES
 *******************************************************************************************************/

#define  FTP_REPLY_CODE_ALREADYOPEN                      125
#define  FTP_REPLY_CODE_OKAYOPENING                      150
#define  FTP_REPLY_CODE_OKAY                             200
#define  FTP_REPLY_CODE_SYSTEMSTATUS                     211
#define  FTP_REPLY_CODE_FILESTATUS                       213
#define  FTP_REPLY_CODE_HELPMESSAGE                      214
#define  FTP_REPLY_CODE_SYSTEMTYPE                       215
#define  FTP_REPLY_CODE_SERVERREADY                      220
#define  FTP_REPLY_CODE_SERVERCLOSING                    221
#define  FTP_REPLY_CODE_CLOSINGSUCCESS                   226
#define  FTP_REPLY_CODE_ENTERPASVMODE                    227
#define  FTP_REPLY_CODE_ENTEREXTPASVMODE                 229
#define  FTP_REPLY_CODE_LOGGEDIN                         230
#define  FTP_REPLY_CODE_ACTIONCOMPLETE                   250
#define  FTP_REPLY_CODE_PATHNAME                         257
#define  FTP_REPLY_CODE_NEEDPASSWORD                     331
#define  FTP_REPLY_CODE_NEEDMOREINFO                     350
#define  FTP_REPLY_CODE_NOSERVICE                        421
#define  FTP_REPLY_CODE_CANTOPENDATA                     425
#define  FTP_REPLY_CODE_CLOSEDCONNABORT                  426
#define  FTP_REPLY_CODE_PARMSYNTAXERR                    501
#define  FTP_REPLY_CODE_CMDNOSUPPORT                     502
#define  FTP_REPLY_CODE_CMDBADSEQUENCE                   503
#define  FTP_REPLY_CODE_PARMNOSUPPORT                    504
#define  FTP_REPLY_CODE_NOTLOGGEDIN                      530
#define  FTP_REPLY_CODE_NOTFOUND                         550
#define  FTP_REPLY_CODE_ACTIONABORTED                    551
#define  FTP_REPLY_CODE_NOSPACE                          552
#define  FTP_REPLY_CODE_NAMEERR                          553
#define  FTP_REPLY_CODE_PBSZ                             554
#define  FTP_REPLY_CODE_PROT                             555

#define  FTP_REPLY_CODE_LEN                                3
#define  FTP_REPLY_CODE_MULTI_LINE_INDICATOR             '-'

/********************************************************************************************************
 *                                           FTP USASCII CODES
 *******************************************************************************************************/

#define  FTP_ASCII_LF                                    '\n'   // Line feed.
#define  FTP_ASCII_CR                                    '\r'   // Carriage return.
#define  FTP_ASCII_SPACE                                 ' '    // Space.

#define  FTP_ASCII_EPSV_PREFIX                           "|||"

#define  FTP_EOL_DELIMITER_LEN                            2    // EOL delimiter is CRLF.

/********************************************************************************************************
 *                                               FTP DATA TYPE
 *******************************************************************************************************/

//                                                                 Data type "IMAGE" supported only.
#define  FTP_TYPE_ASCII                                  'A'
#define  FTP_TYPE_EBCDIC                                 'E'
#define  FTP_TYPE_IMAGE                                  'I'
#define  FTP_TYPE_LOCAL                                  'L'

#define FTP_USER_ANONYMOUS                               "anonymous"

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 This structure is used to build a table of command
//                                                                 codes and their corresponding string.
typedef struct  FTPc_CmdStruct {
  CPU_INT08U     CmdCode;
  const CPU_CHAR *CmdStr;
}  FTPc_CMD_STRUCT;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef  RTOS_MODULE_FS_AVAIL
static const NET_FS_API *FTP_FS_API_Ptr = &NetFS_API_Native;
#endif

/********************************************************************************************************
 *                                       FTP CLIENT CONFIGURATION
 *******************************************************************************************************/

static const FTPc_CFG FTPc_DfltCfg = {
  FTPc_CFG_DFLT_CTRL_MAX_CONN_TIMEOUT_MS,
  FTPc_CFG_DFLT_CTRL_MAX_RX_TIMEOUT_MS,
  FTPc_CFG_DFLT_CTRL_MAX_TX_TIMEOUT_MS,

  FTPc_CFG_DFLT_CTRL_MAX_RX_DLY_MS,

  FTPc_CFG_DFLT_CTRL_MAX_TX_RETRY,
  FTPc_CFG_DFLT_CTRL_MAX_TX_DLY_MS,

  FTPc_CFG_DFLT_DTP_MAX_CONN_TIMEOUT_MS,
  FTPc_CFG_DFLT_DTP_MAX_RX_TIMEOUT_MS,
  FTPc_CFG_DFLT_DTP_MAX_TX_TIMEOUT_MS,

  FTPc_CFG_DFLT_DTP_MAX_TX_RETRY,
  FTPc_CFG_DFLT_DTP_MAX_TX_DLY_MS
};

/********************************************************************************************************
 *                                           INITIALIZED DATA
 *******************************************************************************************************/

//                                                                 This structure is used to build a table of command
//                                                                 codes and their corresponding string.  The context
//                                                                 is the state(s) in which the command is allowed.
static const FTPc_CMD_STRUCT FTPc_Cmd[] = {
  { FTP_CMD_NOOP, (const  CPU_CHAR *)"NOOP" },
  { FTP_CMD_QUIT, (const  CPU_CHAR *)"QUIT" },
  { FTP_CMD_REIN, (const  CPU_CHAR *)"REIN" },
  { FTP_CMD_SYST, (const  CPU_CHAR *)"SYST" },
  { FTP_CMD_FEAT, (const  CPU_CHAR *)"FEAT" },
  { FTP_CMD_HELP, (const  CPU_CHAR *)"HELP" },
  { FTP_CMD_USER, (const  CPU_CHAR *)"USER" },
  { FTP_CMD_PASS, (const  CPU_CHAR *)"PASS" },
  { FTP_CMD_MODE, (const  CPU_CHAR *)"MODE" },
  { FTP_CMD_TYPE, (const  CPU_CHAR *)"TYPE" },
  { FTP_CMD_STRU, (const  CPU_CHAR *)"STRU" },
  { FTP_CMD_PASV, (const  CPU_CHAR *)"PASV" },
  { FTP_CMD_PORT, (const  CPU_CHAR *)"PORT" },
  { FTP_CMD_PWD, (const  CPU_CHAR *)"PWD"  },
  { FTP_CMD_CWD, (const  CPU_CHAR *)"CWD"  },
  { FTP_CMD_CDUP, (const  CPU_CHAR *)"CDUP" },
  { FTP_CMD_MKD, (const  CPU_CHAR *)"MKD"  },
  { FTP_CMD_RMD, (const  CPU_CHAR *)"RMD"  },
  { FTP_CMD_NLST, (const  CPU_CHAR *)"NLST" },
  { FTP_CMD_LIST, (const  CPU_CHAR *)"LIST" },
  { FTP_CMD_RETR, (const  CPU_CHAR *)"RETR" },
  { FTP_CMD_STOR, (const  CPU_CHAR *)"STOR" },
  { FTP_CMD_APPE, (const  CPU_CHAR *)"APPE" },
  { FTP_CMD_REST, (const  CPU_CHAR *)"REST" },
  { FTP_CMD_DELE, (const  CPU_CHAR *)"DELE" },
  { FTP_CMD_RNFR, (const  CPU_CHAR *)"RNFR" },
  { FTP_CMD_RNTO, (const  CPU_CHAR *)"RNTO" },
  { FTP_CMD_SIZE, (const  CPU_CHAR *)"SIZE" },
  { FTP_CMD_MDTM, (const  CPU_CHAR *)"MDTM" },
  { FTP_CMD_PBSZ, (const  CPU_CHAR *)"PBSZ" },
  { FTP_CMD_PROT, (const  CPU_CHAR *)"PROT" },
  { FTP_CMD_PASV, (const  CPU_CHAR *)"EPSV" },
  { FTP_CMD_EPRT, (const  CPU_CHAR *)"EPRT" },
  { FTP_CMD_MAX, (const  CPU_CHAR *)"MAX"  }                    // This line MUST be the LAST!
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static CPU_INT32U FTPc_WaitForStatus(FTPc_CONN  *p_conn,
                                     CPU_CHAR   *p_ctrl_data,
                                     CPU_INT16U ctrl_data_len,
                                     RTOS_ERR   *p_err);

static CPU_INT32S FTPc_RxReply(CPU_INT32S sock_id,
                               CPU_CHAR   *p_data,
                               CPU_INT16U data_len,
                               CPU_INT32U dly_ms,
                               RTOS_ERR   *p_err);

static CPU_BOOLEAN FTPc_Tx(CPU_INT32S sock_id,
                           CPU_CHAR   *p_data,
                           CPU_INT16U data_len,
                           CPU_INT32U timeout_ms,
                           CPU_INT16U retry_max,
                           CPU_INT32U time_dly_ms,
                           RTOS_ERR   *p_err);

static NET_SOCK_ID FTPc_Conn(FTPc_CONN  *p_conn,
                             CPU_CHAR   *p_ctrl_buf,
                             CPU_INT16U ctrl_buf_size,
                             RTOS_ERR   *p_err);

/****************************************************************************************************//**
 *                                              FTPc_Open()
 *
 * @brief    Open connection to an FTP server.
 *
 * @param    p_conn          Pointer to FTP Client Connection object.
 *
 * @param    p_cfg           Pointer to FTPc Configuration object.
 *                           DEF_NULL to use internal default configuration.
 *
 * @param    p_secure_cfg    Pointer to a secure configuration structure if secured connection is     required or
 *                           DEF_NULL                                    if secured connection is not required.
 *
 * @param    p_host_server   Pointer to hostname/IP address string of the server.
 *
 * @param    port_nbr        IP port of the server.
 *
 * @param    p_user          Pointer to account username on the server.
 *
 * @param    p_pass          Pointer to account password on the server.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s) from this function:
 *                               - RTOS_ERR_NONE
 *                               - RTOS_ERR_INVALID_TYPE
 *                               - RTOS_ERR_NET_RETRY_MAX
 *                               - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                               - RTOS_ERR_SEG_OVF
 *                               - RTOS_ERR_NOT_SUPPORTED
 *                               - RTOS_ERR_NET_CONN_CLOSE_RX
 *                               - RTOS_ERR_OS_SCHED_LOCKED
 *                               - RTOS_ERR_NOT_AVAIL
 *                               - RTOS_ERR_FAIL
 *                               - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                               - RTOS_ERR_WOULD_OVF
 *                               - RTOS_ERR_NET_IF_LINK_DOWN
 *                               - RTOS_ERR_INVALID_HANDLE
 *                               - RTOS_ERR_WOULD_BLOCK
 *                               - RTOS_ERR_INVALID_STATE
 *                               - RTOS_ERR_ABORT
 *                               - RTOS_ERR_TIMEOUT
 *                               - RTOS_ERR_NET_OP_IN_PROGRESS
 *                               - RTOS_ERR_TX
 *                               - RTOS_ERR_ALREADY_EXISTS
 *                               - RTOS_ERR_NOT_FOUND
 *                               - RTOS_ERR_NET_INVALID_CONN
 *                               - RTOS_ERR_RX
 *                               - RTOS_ERR_NOT_READY
 *                               - RTOS_ERR_POOL_EMPTY
 *                               - RTOS_ERR_OS_OBJ_DEL
 *                               - RTOS_ERR_NET_ADDR_UNRESOLVED
 *                               - RTOS_ERR_NET_NEXT_HOP
 *                               - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_FAIL        connection failed.
 *           DEF_OK          connection successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_Open(FTPc_CONN             *p_conn,
                      const FTPc_CFG        *p_cfg,
                      const FTPc_SECURE_CFG *p_secure_cfg,
                      CPU_CHAR              *p_host_server,
                      NET_PORT_NBR          port_nbr,
                      CPU_CHAR              *p_user,
                      CPU_CHAR              *p_pass,
                      RTOS_ERR              *p_err)
{
  CPU_CHAR     *p_username;
  CPU_CHAR     *p_password;
  NET_PORT_NBR port;
  CPU_CHAR     ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U   ctrl_buf_size;
  CPU_INT32U   buf_size;
  CPU_INT32U   reply_code;
  CPU_BOOLEAN  rtn_code;
  RTOS_ERR     err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_host_server != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  if (p_cfg == DEF_NULL) {
    p_conn->CfgPtr = &FTPc_DfltCfg;
  } else {
    p_conn->CfgPtr = (FTPc_CFG *)p_cfg;
  }

#ifdef  NET_SECURE_MODULE_EN
  p_conn->SecureCfgPtr = p_secure_cfg;
#else
  RTOS_ASSERT_DBG_ERR_SET((p_secure_cfg == DEF_NULL), *p_err, RTOS_ERR_NOT_AVAIL, DEF_FAIL);
#endif

  ctrl_buf_size = sizeof(ctrl_buf);
  LOG_VRB(("FTPc - Open connection with: ", (s)p_host_server));

  if (port_nbr == 0u) {
    port = FTPc_PORT_DFLT;
  } else {
    port = port_nbr;
  }

  p_conn->SockAddrFamily = NetApp_ClientStreamOpenByHostname(&p_conn->SockID,
                                                             p_host_server,
                                                             port,
                                                             &p_conn->SockAddr,
                                                             (NET_APP_SOCK_SECURE_CFG *)p_secure_cfg,
                                                             p_conn->CfgPtr->CtrlConnMaxTimout_ms,
                                                             p_err);
  switch (RTOS_ERR_CODE_GET(*p_err)) {
    case RTOS_ERR_NONE:
      break;

    default:
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetApp_ClientStreamOpenByHostname() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
      rtn_code = DEF_FAIL;
      goto exit;
  }

  Mem_Clr(p_conn->Buf, FTPc_CTRL_NET_BUF_SIZE);

  //                                                               Receive status lines until "server ready status".
  while (DEF_OK) {
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
    if (reply_code != FTP_REPLY_CODE_SERVERREADY) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected SERVER_READY and received:", (u)reply_code));
      if (p_err != RTOS_ERR_NONE) {
        goto exit_close_sock;
      }
      continue;
    }
    break;
  }

  //                                                              ******* USERNAME ***********************************
  //                                                               Send USER command.
  if (p_user == DEF_NULL) {
    p_username = FTP_USER_ANONYMOUS;
  } else {
    p_username = p_user;
  }

  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_USER].CmdStr, p_username);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit_close_sock;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_NEEDPASSWORD) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected NEED_PASSWORD and received:", (u)reply_code));
    goto exit_close_sock;
  }

  //                                                              ******* PASSWORD ***********************************
  //                                                               Send PASS command.
  if (p_pass == DEF_NULL) {
    p_password = "";
  } else {
    p_password = p_pass;
  }

  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_PASS].CmdStr, p_password);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : PASSWORD"));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit_close_sock;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_LOGGEDIN) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected LOGGEDIN and received:", (u)reply_code));
    RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
    goto exit_close_sock;
  }

#ifdef  NET_SECURE_MODULE_EN
  if (p_conn->SecureCfgPtr != DEF_NULL) {                      // See Note #2.
                                                               //  Send PBSZ command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_PBSZ].CmdStr, (CPU_CHAR *)"0");
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      goto exit_close_sock;
    }

    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
    if (reply_code != FTP_REPLY_CODE_OKAY) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_close_sock;
    }
    //                                                             Send PROT command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_PROT].CmdStr, "P");
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      goto exit_close_sock;
    }

    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
    if (reply_code != FTP_REPLY_CODE_OKAY) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      goto exit_close_sock;
    }
  }
#endif

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));
  goto exit;

exit_close_sock:
  NetSock_Close(p_conn->SockID, &err_local);
  PP_UNUSED_PARAM(err_local);

  p_conn->SockID = NET_SOCK_ID_NONE;
  rtn_code = DEF_FAIL;
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": EXIT WITH ERROR"));

exit:
  return (rtn_code);
}

/****************************************************************************************************//**
 *                                             FTPc_Close()
 *
 * @brief    Close FTP connection.
 *
 * @param    p_conn  Pointer to FTP Client Connection object.
 *
 * @param    p_err   Pointer to the variable that will receive one of the following error code(s) from this function:
 *                       - RTOS_ERR_NONE
 *                       - RTOS_ERR_INVALID_TYPE
 *                       - RTOS_ERR_NET_RETRY_MAX
 *                       - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                       - RTOS_ERR_SEG_OVF
 *                       - RTOS_ERR_NOT_SUPPORTED
 *                       - RTOS_ERR_NET_CONN_CLOSE_RX
 *                       - RTOS_ERR_OS_SCHED_LOCKED
 *                       - RTOS_ERR_NOT_AVAIL
 *                       - RTOS_ERR_FAIL
 *                       - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                       - RTOS_ERR_WOULD_OVF
 *                       - RTOS_ERR_NET_IF_LINK_DOWN
 *                       - RTOS_ERR_INVALID_HANDLE
 *                       - RTOS_ERR_WOULD_BLOCK
 *                       - RTOS_ERR_INVALID_STATE
 *                       - RTOS_ERR_ABORT
 *                       - RTOS_ERR_TIMEOUT
 *                       - RTOS_ERR_TX
 *                       - RTOS_ERR_NOT_FOUND
 *                       - RTOS_ERR_ALREADY_EXISTS
 *                       - RTOS_ERR_NET_INVALID_CONN
 *                       - RTOS_ERR_RX
 *                       - RTOS_ERR_NOT_READY
 *                       - RTOS_ERR_POOL_EMPTY
 *                       - RTOS_ERR_OS_OBJ_DEL
 *                       - RTOS_ERR_NET_NEXT_HOP
 *                       - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_FAIL        FTP connection close failed.
 *           DEF_OK          FTP connection close successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_Close(FTPc_CONN *p_conn,
                       RTOS_ERR  *p_err)
{
  CPU_CHAR    ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U  ctrl_buf_size;
  CPU_INT32U  buf_size;
  CPU_INT32U  reply_code;
  CPU_BOOLEAN rtn_code;
  RTOS_ERR    err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  ctrl_buf_size = sizeof(ctrl_buf);
  //                                                               Send QUIT command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s\r\n", FTPc_Cmd[FTP_CMD_QUIT].CmdStr);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_SERVERCLOSING) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected SERVERCLOSING and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:                                                           // Close socket.
  NetSock_Close(p_conn->SockID, &err_local);
  PP_UNUSED_PARAM(err_local);

  return (DEF_OK);
}

/****************************************************************************************************//**
 *                                            FTPc_RecvBuf()
 *
 * @brief    Receive a file from an FTP server into a memory buffer.
 *
 * @param    p_conn              Pointer to FTPc Connection object.
 *
 * @param    p_remote_file_name  Pointer to name of the file in FTP server.
 *
 * @param    p_buf               Pointer to memory buffer to hold received file.
 *
 * @param    buf_len             Size of the memory buffer.
 *
 * @param    p_file_size         Pointer to a variable that will received the size of the file received.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_NET_RETRY_MAX
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_NET_CONN_CLOSE_RX
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_FAIL
 *                                   - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_NET_OP_IN_PROGRESS
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NET_NEXT_HOP
 *                                   - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_FAIL        reception failed.
 *           DEF_OK          reception successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_RecvBuf(FTPc_CONN  *p_conn,
                         CPU_CHAR   *p_remote_file_name,
                         CPU_INT08U *p_buf,
                         CPU_INT32U buf_len,
                         CPU_INT32U *p_file_size,
                         RTOS_ERR   *p_err)
{
  NET_SOCK_ID sock_dtp_id;
  CPU_CHAR    ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U  ctrl_buf_size;
  CPU_INT32U  buf_size;
  CPU_INT32U  reply_code;
  CPU_BOOLEAN rtn_code;
  CPU_CHAR    *tmp_buf;
  CPU_INT32S  tmp_val;
  CPU_INT32U  rx_len;
  CPU_INT32U  bytes_recv;
  CPU_INT32U  got_file_size;
  RTOS_ERR    err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_remote_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_file_size != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  rtn_code = DEF_OK;
  ctrl_buf_size = sizeof(ctrl_buf);

  //                                                               Send TYPE command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_OKAY) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  //                                                               Send SIZE command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_SIZE].CmdStr, p_remote_file_name);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, ctrl_buf, ctrl_buf_size, p_err);
  if (reply_code != FTP_REPLY_CODE_FILESTATUS) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected FILESTATUS and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  tmp_buf = ctrl_buf;
  tmp_val = Str_ParseNbr_Int32S(tmp_buf, &tmp_buf, 10);        // Skip result code.
  tmp_buf++;

  tmp_val = Str_ParseNbr_Int32S(tmp_buf, &tmp_buf, 10);        // Get file size.
  tmp_buf++;
  got_file_size = tmp_val;
  if (got_file_size > buf_len) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_WOULD_OVF);
    rtn_code = DEF_FAIL;
    goto exit;
  }

  LOG_VRB(("FTPc - Opening DTP Socket"));
  sock_dtp_id = FTPc_Conn(p_conn, ctrl_buf, ctrl_buf_size, p_err);
  if (sock_dtp_id == NET_SOCK_ID_NONE) {
    rtn_code = DEF_FAIL;
    goto exit;
  }

  //                                                               Send RETR command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_RETR].CmdStr, p_remote_file_name);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit_close_dtp_sock;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if ((reply_code != FTP_REPLY_CODE_ALREADYOPEN)
      && (reply_code != FTP_REPLY_CODE_OKAYOPENING)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected ALREADYOPEN or OKAYOPENING and received:", (u)reply_code));
    goto exit_close_dtp_sock;
  }

  NetSock_CfgTimeoutRxQ_Set(sock_dtp_id, p_conn->CfgPtr->DTP_RxMaxTimout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit_close_dtp_sock;
  }

  bytes_recv = 0;
  while (bytes_recv < got_file_size) {
    tmp_buf = ((CPU_CHAR *)p_buf) + bytes_recv;
    rx_len = got_file_size - bytes_recv;
    if (rx_len > DEF_INT_16S_MAX_VAL) {
      rx_len = DEF_INT_16S_MAX_VAL;
    }

    tmp_val = NetSock_RxData(sock_dtp_id, tmp_buf, rx_len, NET_SOCK_FLAG_NONE, p_err);
    if (tmp_val > 0) {
      bytes_recv += tmp_val;
    }
    if ((RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE)
        && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_WOULD_BLOCK)
        && (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_TIMEOUT)    ) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_RxData() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
      break;
    }

    //                                                             In this case, a timeout represents an end-of-file
    //                                                             condition.
    if ((RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_WOULD_BLOCK)
        || (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_TIMEOUT)) {
      break;
    }
  }

  *p_file_size = got_file_size;

exit_close_dtp_sock:

  LOG_VRB(("Closing DTP socket"));
  //                                                               Close socket.
  NetSock_Close(sock_dtp_id, &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_Close() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
  }

  if (RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE) {
    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, &err_local);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected CLOSINGSUCCESS and received:", (u)reply_code));
      goto exit;
    }
  } else {
    goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:
  return (rtn_code);
}

/****************************************************************************************************//**
 *                                            FTPc_SendBuf()
 *
 * @brief    Send a memory buffer to an FTP server.
 *
 * @param    p_conn              Pointer to FTPc Connection object.
 *
 * @param    p_remote_file_name  File path on the server
 *
 * @param    p_buf               Pointer to memory buffer to send.
 *
 * @param    buf_len             Size of the memory buffer.
 *
 * @param    append              if DEF_YES, existing file on FTP server will be appended with
 *                               memory buffer. If file doesn't exist on FTP server, it will be created.
 *                               if DEF_NO, existing file on FTP server will be overwritten.
 *                               If file doesn't exist on FTP server, it will be created.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_INVALID_TYPE
 *                                   - RTOS_ERR_NET_RETRY_MAX
 *                                   - RTOS_ERR_BLK_ALLOC_CALLBACK
 *                                   - RTOS_ERR_SEG_OVF
 *                                   - RTOS_ERR_NOT_SUPPORTED
 *                                   - RTOS_ERR_NET_CONN_CLOSE_RX
 *                                   - RTOS_ERR_OS_SCHED_LOCKED
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_FAIL
 *                                   - RTOS_ERR_NET_INVALID_ADDR_SRC
 *                                   - RTOS_ERR_WOULD_OVF
 *                                   - RTOS_ERR_NET_IF_LINK_DOWN
 *                                   - RTOS_ERR_INVALID_HANDLE
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_INVALID_ARG
 *                                   - RTOS_ERR_INVALID_STATE
 *                                   - RTOS_ERR_ABORT
 *                                   - RTOS_ERR_TIMEOUT
 *                                   - RTOS_ERR_NET_OP_IN_PROGRESS
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_ALREADY_EXISTS
 *                                   - RTOS_ERR_NET_INVALID_CONN
 *                                   - RTOS_ERR_RX
 *                                   - RTOS_ERR_NOT_READY
 *                                   - RTOS_ERR_POOL_EMPTY
 *                                   - RTOS_ERR_OS_OBJ_DEL
 *                                   - RTOS_ERR_NET_NEXT_HOP
 *                                   - RTOS_ERR_NET_CONN_CLOSED_FAULT
 *
 * @return   DEF_FAIL        transmission failed.
 *           DEF_OK          transmission successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_SendBuf(FTPc_CONN   *p_conn,
                         CPU_CHAR    *p_remote_file_name,
                         CPU_INT08U  *p_buf,
                         CPU_INT32U  buf_len,
                         CPU_BOOLEAN append,
                         RTOS_ERR    *p_err)
{
  NET_SOCK_ID sock_dtp_id;
  CPU_CHAR    ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U  ctrl_buf_size;
  CPU_INT32U  buf_size;
  CPU_INT32U  reply_code;
  CPU_BOOLEAN rtn_code;
  CPU_INT32U  bytes_sent;
  CPU_INT32U  tx_pkt_cnt;
  RTOS_ERR    err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_remote_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ASSERT_DBG_ERR_SET((p_buf != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR, DEF_FAIL);
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  ctrl_buf_size = sizeof(ctrl_buf);
  //                                                               Send TYPE command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_OKAY) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  LOG_VRB(("FTPc - Opening DTP Socket"));
  sock_dtp_id = FTPc_Conn(p_conn, ctrl_buf, ctrl_buf_size, p_err);
  if (sock_dtp_id == NET_SOCK_ID_NONE) {
    rtn_code = DEF_FAIL;
    goto exit;
  }

  if (append == DEF_YES) {
    //                                                             Send APPE command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_APPE].CmdStr, p_remote_file_name);
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      goto exit_close_dtp_sock;
    }
  } else {
    //                                                             Send STOR command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_STOR].CmdStr, p_remote_file_name);
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      goto exit_close_dtp_sock;
    }
  }
  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if ((reply_code != FTP_REPLY_CODE_ALREADYOPEN)
      && (reply_code != FTP_REPLY_CODE_OKAYOPENING)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAYOPENING and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit_close_dtp_sock;
  }

  NetSock_CfgTimeoutRxQ_Set(sock_dtp_id, p_conn->CfgPtr->DTP_TxMaxTimout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    rtn_code = DEF_FAIL;
    goto exit_close_dtp_sock;
  }

  bytes_sent = 0;
  tx_pkt_cnt = 0;
  while (bytes_sent < buf_len) {
    buf_size = buf_len - bytes_sent;
    if (buf_size > FTPc_DTP_NET_BUF_SIZE) {
      buf_size = FTPc_DTP_NET_BUF_SIZE;
    }

    rtn_code = FTPc_Tx(sock_dtp_id,
                       (CPU_CHAR *)p_buf,
                       buf_size,
                       0,
                       p_conn->CfgPtr->DTP_TxMaxRetry,
                       p_conn->CfgPtr->DTP_TxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      break;
    }

    bytes_sent += buf_size;
    p_buf += buf_size;
    tx_pkt_cnt++;
  }

exit_close_dtp_sock:
  //                                                               Close socket.
  NetSock_Close(sock_dtp_id, &err_local);

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, &err_local);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected CLOSINGSUCCESS and received:", (u)reply_code));
      rtn_code = DEF_FAIL;
      goto exit;
    }

    rtn_code = DEF_OK;
  } else {
    goto exit;
  }

  PP_UNUSED_PARAM(err_local);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:
  return (rtn_code);
}

/****************************************************************************************************//**
 *                                            FTPc_RecvFile()
 *
 * @brief    Receive a file from an FTP server to the file system.
 *
 * @param    p_conn              Pointer to FTPc Connection object.
 *
 * @param    p_remote_file_name  Remote File path.
 *
 * @param    p_local_file_name   Local File path.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_INVALID_STATE
 *
 * @return   DEF_FAIL        reception failed.
 *           DEF_OK          reception successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_RecvFile(FTPc_CONN *p_conn,
                          CPU_CHAR  *p_remote_file_name,
                          CPU_CHAR  *p_local_file_name,
                          RTOS_ERR  *p_err)
{
#ifdef  RTOS_MODULE_FS_AVAIL
  NET_SOCK_ID sock_dtp_id;
  CPU_CHAR    data_buf[FTPc_DTP_NET_BUF_SIZE];
  CPU_CHAR    ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U  ctrl_buf_size;
  CPU_INT32U  buf_size;
  CPU_INT32U  reply_code;
  CPU_BOOLEAN rtn_code;
  void        *p_file;
  CPU_SIZE_T  fs_len;
  RTOS_ERR    err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_remote_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_local_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));
  ctrl_buf_size = sizeof(ctrl_buf);
  //                                                               Send TYPE command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_OKAY) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  LOG_VRB(("FTPc - Opening DTP Socket"));
  sock_dtp_id = FTPc_Conn(p_conn, ctrl_buf, ctrl_buf_size, p_err);
  if (sock_dtp_id == NET_SOCK_ID_NONE) {
    rtn_code = DEF_FAIL;
    goto exit;
  }

  //                                                               Send RETR command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_RETR].CmdStr, p_remote_file_name);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit_close_dtp_sock;
  }
  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  switch (reply_code) {
    case FTP_REPLY_CODE_ALREADYOPEN:
    case FTP_REPLY_CODE_OKAYOPENING:
      break;

    case FTP_REPLY_CODE_NOTFOUND:
      LOG_DBG(("FTPc - File not found"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
      rtn_code = DEF_FAIL;
      goto exit_close_dtp_sock;

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected ALREADYOPEN, OKAYOPENING or NOTFOUND and received:", (u)reply_code));
      rtn_code = DEF_FAIL;
      goto exit_close_dtp_sock;
  }

  LOG_VRB(("FTPc - Opening DTP Socket"));
  p_file = FTP_FS_API_Ptr->Open(p_local_file_name,
                                NET_FS_FILE_MODE_CREATE,
                                NET_FS_FILE_ACCESS_RD_WR);
  if (p_file == DEF_NULL) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : File Open returned an error line ", (u)__LINE__));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    rtn_code = DEF_FAIL;
    goto exit_close_dtp_sock;
  }

  NetSock_CfgTimeoutRxQ_Set(sock_dtp_id,
                            p_conn->CfgPtr->DTP_RxMaxTimout_ms,
                            p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    rtn_code = DEF_FAIL;
    goto exit_close_file;
  }

  while (DEF_TRUE) {
    buf_size = NetSock_RxData(sock_dtp_id, data_buf, sizeof(data_buf), NET_SOCK_FLAG_NONE, p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
      case RTOS_ERR_WOULD_BLOCK:
        break;

      case RTOS_ERR_TIMEOUT:
      case RTOS_ERR_NET_CONN_CLOSE_RX:
        //                                                         In this case, a timeout represents an end-of-file
        //                                                         condition.
        RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
        goto exit_close_file;

      default:
        LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : File Open returned an error line ", (u)__LINE__));
        goto exit_close_file;
    }

    (void)FTP_FS_API_Ptr->Wr((void *) p_file,
                             (void *) data_buf,
                             (CPU_SIZE_T) buf_size,
                             (CPU_SIZE_T *)&fs_len);
    if (fs_len != buf_size) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : File Write returned an error line ", (u)__LINE__));
      goto exit_close_file;
    }
  }

exit_close_file:
  FTP_FS_API_Ptr->Close(p_file);

exit_close_dtp_sock:
  //                                                               Close socket.
  NetSock_Close(sock_dtp_id, &err_local);

  if (RTOS_ERR_CODE_GET(err_local) == RTOS_ERR_NONE) {
    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, &err_local);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected CLOSINGSUCCESS and received:", (u)reply_code));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      rtn_code = DEF_FAIL;
      goto exit;
    }

    rtn_code = DEF_OK;
  } else {
    goto exit;
  }

  PP_UNUSED_PARAM(err_local);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:
  return (rtn_code);

#else
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_remote_file_name);
  PP_UNUSED_PARAM(p_local_file_name);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);

  return (DEF_FAIL);
#endif
}

/****************************************************************************************************//**
 *                                            FTPc_SendFile()
 *
 * @brief    Send a file located in the file system to an FTP server.
 *
 * @param    p_conn              Pointer to FTPc Connection object.
 *
 * @param    p_remote_file_name  Remote File path.
 *
 * @param    p_local_file_name   Local File path.
 *
 * @param    append              if DEF_YES, existing file on FTP server will be appended with
 *                               local file.  If file doesn't exist on FTP server, it will be created.
 *                               if DEF_NO, existing file on FTP server will be overwritten.
 *                               If file doesn't exist on FTP server, it will be created.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_NOT_AVAIL
 *                                   - RTOS_ERR_NOT_FOUND
 *                                   - RTOS_ERR_INVALID_STATE
 *
 * @return   DEF_FAIL        transmission failed.
 *           DEF_OK          transmission successful.
 *******************************************************************************************************/
CPU_BOOLEAN FTPc_SendFile(FTPc_CONN   *p_conn,
                          CPU_CHAR    *p_remote_file_name,
                          CPU_CHAR    *p_local_file_name,
                          CPU_BOOLEAN append,
                          RTOS_ERR    *p_err)
{
#ifdef  RTOS_MODULE_FS_AVAIL
  NET_SOCK_ID sock_dtp_id;
  CPU_CHAR    data_buf[FTPc_DTP_NET_BUF_SIZE];
  CPU_CHAR    ctrl_buf[FTPc_CTRL_NET_BUF_SIZE];
  CPU_INT32U  ctrl_buf_size;
  CPU_INT32U  buf_size;
  CPU_INT32U  reply_code;
  CPU_BOOLEAN rtn_code;
  void        *p_file;
  CPU_SIZE_T  fs_len;
  CPU_BOOLEAN fs_err;
  RTOS_ERR    err_local;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_conn != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_remote_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_local_file_name != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  ctrl_buf_size = sizeof(ctrl_buf);
  //                                                               Send TYPE command.
  buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %c\r\n", FTPc_Cmd[FTP_CMD_TYPE].CmdStr, FTP_TYPE_IMAGE);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     p_err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if (reply_code != FTP_REPLY_CODE_OKAY) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAY and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit;
  }

  sock_dtp_id = FTPc_Conn(p_conn, ctrl_buf, ctrl_buf_size, p_err);
  if (sock_dtp_id == NET_SOCK_ID_NONE) {
    rtn_code = DEF_FAIL;
    goto exit;
  }

  if (append == DEF_YES) {
    //                                                             Send APPE command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_APPE].CmdStr, p_remote_file_name);
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      rtn_code = DEF_FAIL;
      goto exit_close_dtp_sock;
    }
  } else {
    //                                                             Send STOR command.
    buf_size = Str_Snprintf((char *)ctrl_buf, ctrl_buf_size, "%s %s\r\n", FTPc_Cmd[FTP_CMD_STOR].CmdStr, p_remote_file_name);
    LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)ctrl_buf));
    rtn_code = FTPc_Tx(p_conn->SockID,
                       ctrl_buf,
                       buf_size,
                       p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                       p_conn->CfgPtr->CtrlTxMaxRetry,
                       p_conn->CfgPtr->CtrlTxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      rtn_code = DEF_FAIL;
      goto exit_close_dtp_sock;
    }
  }
  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, 0, 0, p_err);
  if ((reply_code != FTP_REPLY_CODE_ALREADYOPEN)
      && (reply_code != FTP_REPLY_CODE_OKAYOPENING)) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Received wrong reply code, expected OKAYOPENING and received:", (u)reply_code));
    rtn_code = DEF_FAIL;
    goto exit_close_dtp_sock;
  }

  p_file = FTP_FS_API_Ptr->Open(p_local_file_name,
                                NET_FS_FILE_MODE_OPEN,
                                NET_FS_FILE_ACCESS_RD);
  if (p_file == DEF_NULL) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : File Open returned an error line ", (u)__LINE__));
    RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_FOUND);
    rtn_code = DEF_FAIL;
    goto exit_close_dtp_sock;
  }

  NetSock_CfgTimeoutRxQ_Set(sock_dtp_id, p_conn->CfgPtr->DTP_TxMaxTimout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    rtn_code = DEF_FAIL;
    goto exit_close_file;
  }

  while (DEF_TRUE) {
    fs_err = FTP_FS_API_Ptr->Rd(p_file,
                                data_buf,
                                sizeof(data_buf),
                                &fs_len);

    if (fs_err != DEF_OK) {
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : File Read returned an error line ", (u)__LINE__));
      goto exit_close_file;
    }
    rtn_code = FTPc_Tx(sock_dtp_id,
                       data_buf,
                       fs_len,
                       0,
                       p_conn->CfgPtr->DTP_TxMaxRetry,
                       p_conn->CfgPtr->DTP_TxMaxDly_ms,
                       p_err);
    if (rtn_code == DEF_FAIL) {
      goto exit_close_file;
    }

    if (fs_len != sizeof(data_buf)) {
      break;
    }
  }

exit_close_file:
  FTP_FS_API_Ptr->Close(p_file);

exit_close_dtp_sock:
  //                                                               Close socket.
  NetSock_Close(sock_dtp_id, &err_local);
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": CLOSE DTP socket"));

  if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
    //                                                             Receive status line.
    reply_code = FTPc_WaitForStatus(p_conn, 0, 0, &err_local);
    if (reply_code != FTP_REPLY_CODE_CLOSINGSUCCESS) {
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
      rtn_code = DEF_FAIL;
      goto exit;
    }

    rtn_code = DEF_OK;
  } else {
    goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));
exit:
  return (rtn_code);

#else
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_remote_file_name);
  PP_UNUSED_PARAM(p_local_file_name);
  PP_UNUSED_PARAM(append);

  RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_AVAIL);
  return (DEF_FAIL);
#endif
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                         FTPc_WaitForStatus()
 *
 * @brief    Receive FTP server reply after a request and analyze it to find the server status reply.
 *
 * @param    p_conn          Pointer to FTPc Connection object.
 *
 * @param    p_ctrl_data     Pointer to  buffer  that will receive control reply data.
 *
 * @param    ctrl_data_len   Size of the buffer  that will receive control reply data.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s) from this function.
 *
 * @return   FTP server status code   if no error;
 *           0                        otherwise.
 *
 * @note         (1) Even though the server reply code can be retrieved from the first 3 octets in the
 *               received message, it is mandatory that the whole response be taken from the socket
 *               receive queue in order to make sure no "left over" messages will be present at the
 *               socket at a later call to this function.
 *******************************************************************************************************/
static CPU_INT32U FTPc_WaitForStatus(FTPc_CONN  *p_conn,
                                     CPU_CHAR   *p_ctrl_data,
                                     CPU_INT16U ctrl_data_len,
                                     RTOS_ERR   *p_err)
{
  CPU_CHAR    *p_buf;
  CPU_CHAR    *p_str_multiline;
  CPU_CHAR    *p_search_str;
  CPU_CHAR    end_of_cmd[FTP_EOL_DELIMITER_LEN + FTP_REPLY_CODE_LEN + 2];
  CPU_INT16U  reply_code;
  CPU_INT32U  len;
  CPU_INT32U  buf_len;
  CPU_INT32S  rx_reply_tot_len;
  CPU_INT32S  rx_reply_pkt_len;
  CPU_BOOLEAN find_eof;
  CPU_BOOLEAN parse_done;
  CPU_BOOLEAN rx_reply;
  RTOS_ERR    err_local;

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  end_of_cmd[0] = FTP_ASCII_CR;
  end_of_cmd[1] = FTP_ASCII_LF;
  end_of_cmd[FTP_EOL_DELIMITER_LEN + FTP_REPLY_CODE_LEN] = FTP_ASCII_SPACE;
  end_of_cmd[FTP_EOL_DELIMITER_LEN + FTP_REPLY_CODE_LEN + 1] = (CPU_CHAR)'\0';
  reply_code = 0u;
  rx_reply_tot_len = 0;
  find_eof = DEF_NO;
  parse_done = DEF_NO;
  rx_reply = DEF_YES;
  p_buf = (CPU_CHAR *)&p_conn->Buf[0];
  buf_len = sizeof(p_conn->Buf);

  if (*p_buf != (CPU_CHAR)'\0') {
    rx_reply = DEF_NO;
  }

  NetSock_CfgTimeoutRxQ_Set(p_conn->SockID, p_conn->CfgPtr->CtrlRxMaxTimout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit;
  }

  while (parse_done == DEF_NO) {
    if (rx_reply == DEF_YES) {
      rx_reply_pkt_len = FTPc_RxReply(p_conn->SockID,
                                      p_buf   + rx_reply_tot_len,
                                      buf_len - rx_reply_tot_len - 1,
                                      p_conn->CfgPtr->CtrlRxMaxDly_ms,
                                      p_err);
      switch (RTOS_ERR_CODE_GET(*p_err)) {
        case RTOS_ERR_NONE:
        case RTOS_ERR_WOULD_BLOCK:
          break;

        default:
          goto exit;
      }
    }

    if ((rx_reply == DEF_NO)
        || (rx_reply_pkt_len > 0)) {
      if (rx_reply == DEF_YES) {
        rx_reply_tot_len += rx_reply_pkt_len;
        p_buf[rx_reply_tot_len] = (CPU_CHAR)'\0';             // Append termination char.
      }

      p_search_str = p_buf;
      p_str_multiline = p_buf + FTP_REPLY_CODE_LEN;
      if (*p_str_multiline == FTP_REPLY_CODE_MULTI_LINE_INDICATOR) {
        //                                                         Build end of cmd.
        Str_Copy_N((CPU_CHAR *)end_of_cmd + FTP_EOL_DELIMITER_LEN,
                   (CPU_CHAR *)p_buf,
                   (CPU_SIZE_T)FTP_REPLY_CODE_LEN);
        //                                                         If end of cmd sent into the same packet.
        p_search_str = Str_Str_N(p_buf, end_of_cmd, buf_len);
        if (p_search_str != DEF_NULL) {                         // If delimiter found ...
          p_search_str += 2;
          find_eof = DEF_YES;
        }
      } else {
        find_eof = DEF_YES;
      }
      //                                                           Rx pkt until end of file is not rx'd.
      if (find_eof == DEF_YES) {
        len = buf_len - (p_search_str - p_buf);
        p_search_str = Str_Char_N(p_search_str, len, FTP_ASCII_CR);

        if ( (p_search_str != DEF_NULL)
             && (*(p_search_str + 1) == FTP_ASCII_LF)) {
          parse_done = DEF_YES;
          reply_code = Str_ParseNbr_Int32U(p_buf, 0, 10);               // Chk rep code.
          p_search_str += 2;

          if (*p_search_str != (CPU_CHAR)'\0') {
            len = buf_len - (p_search_str - p_buf);
            Str_Copy_N(p_buf, p_search_str, len);

            if (p_ctrl_data != DEF_NULL) {
              if (len > ctrl_data_len) {
                len = ctrl_data_len;
              }
              Str_Copy_N(p_ctrl_data, p_buf, len);
            }
          } else {
            if (p_ctrl_data != DEF_NULL) {
              Str_Copy_N(p_ctrl_data, p_buf, ctrl_data_len);
            }

            Mem_Clr(&p_conn->Buf[0], buf_len);
          }
        } else if (rx_reply == DEF_NO) {
          rx_reply = DEF_YES;
          rx_reply_tot_len = Str_Len_N(p_buf, buf_len);
        }
      }
    } else if (rx_reply_pkt_len < 0) {
      return (0);
    }
  }

  NetSock_CfgTimeoutRxQ_Set(p_conn->SockID, NET_TMR_TIME_INFINITE, &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set(NET_TMR_TIME_INFINITE) error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:
  return (reply_code);
}

/****************************************************************************************************//**
 *                                             FTPc_RxReply()
 *
 * @brief    Receive reply data.
 *
 * @param    sock_id     TCP socket ID.
 *
 * @param    p_data      Pointer to  buffer  that will receive data.
 *
 * @param    data_len    Size of the buffer  that will receive data.
 *
 * @param    p_err       Pointer to the variable that will receive one of the following error code(s) from this function.
 *
 * @return   Number of positive data octets received, if NO errors;
 *           0                                        otherwise.
 *******************************************************************************************************/
static CPU_INT32S FTPc_RxReply(CPU_INT32S sock_id,
                               CPU_CHAR   *p_data,
                               CPU_INT16U data_len,
                               CPU_INT32U dly_ms,
                               RTOS_ERR   *p_err)
{
  CPU_INT32S pkt_size;

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  pkt_size = 0;

  while (DEF_ON) {
    pkt_size = NetSock_RxData(sock_id,
                              p_data,
                              data_len,
                              NET_SOCK_FLAG_NONE,
                              p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        goto exit;

      case RTOS_ERR_WOULD_BLOCK:                                          // If transitory rx err(s), ...
        KAL_Dly(dly_ms);
        break;

      default:
        pkt_size = 0;
        LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_RxData() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
        goto exit_fail;
    }
  }

exit:
  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));
exit_fail:
  return (pkt_size);
}

/****************************************************************************************************//**
 *                                               FTPc_Tx()
 *
 * @brief    Transmit data to TCP socket, handling transient errors and incomplete buffer transmit.
 *
 * @param    sock_id         Socket descriptor/handle identifier of socket to transmit application data.
 *
 * @param    p_data          Pointer to application data to transmit.
 *
 * @param    data_len        Length  of data to transmit (in octets).
 *
 * @param    timeout_ms      Transmit timeout value per attempt/retry :
 *                           0,                          if current configured timeout value desired
 *                           [or NO timeout for datagram sockets
 *                           (see Note #5)].
 *                           NET_TMR_TIME_INFINITE,      if infinite (i.e. NO timeout) value desired.
 *                           In number of milliseconds,  otherwise.
 *
 * @param    retry_max       Maximum number of consecutive socket transmit retries.
 *
 * @param    time_dly_ms     Transitory transmit delay value, in milliseconds.
 *
 * @param    p_err           Pointer to the variable that will receive one of the following error code(s) from this function.
 *
 * @return   DEF_FAIL        transmission failed.
 *           DEF_OK          transmission successful.
 *******************************************************************************************************/
static CPU_BOOLEAN FTPc_Tx(CPU_INT32S sock_id,
                           CPU_CHAR   *p_data,
                           CPU_INT16U data_len,
                           CPU_INT32U timeout_ms,
                           CPU_INT16U retry_max,
                           CPU_INT32U time_dly_ms,
                           RTOS_ERR   *p_err)
{
  CPU_CHAR    *tx_buf;
  CPU_INT16S  tx_buf_len;
  CPU_INT16S  tx_len;
  CPU_INT16S  tx_len_tot;
  CPU_INT32U  timeout_ms_cfgd;
  CPU_INT32U  tx_retry_cnt;
  CPU_BOOLEAN rtn_value = DEF_FAIL;
  CPU_BOOLEAN tx_dly;
  RTOS_ERR    err_local;

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));
  timeout_ms_cfgd = NetSock_CfgTimeoutTxQ_Get_ms(sock_id, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutTxQ_Get_ms() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit;
  }

  NetSock_CfgTimeoutTxQ_Set(sock_id, timeout_ms, p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutTxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit;
  }

  tx_len_tot = 0;
  tx_retry_cnt = 0;
  tx_dly = DEF_NO;
  while (tx_len_tot < data_len ) {                              // While tx tot len < buf len ...
    if (tx_dly == DEF_YES) {                                    // Dly tx, if req'd.
      KAL_Dly(time_dly_ms);
    }

    tx_buf = p_data   + tx_len_tot;
    tx_buf_len = data_len - tx_len_tot;
    tx_len = NetSock_TxData(sock_id,                            // ... tx data.
                            tx_buf,
                            tx_buf_len,
                            NET_SOCK_FLAG_NONE,
                            p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        if (tx_len > 0) {                                       // If          tx len > 0, ...
          tx_len_tot += tx_len;                                 // ... inc tot tx len.
          tx_dly = DEF_NO;
        } else {                                                // Else dly next tx.
          tx_dly = DEF_YES;
        }
        tx_retry_cnt = 0;
        break;

      case RTOS_ERR_WOULD_BLOCK:                                // If transitory tx err(s), ...
        tx_dly = DEF_YES;                                       // ... dly next tx.
        tx_retry_cnt++;
        if (tx_retry_cnt >= retry_max) {
          LOG_DBG(("FTPc - ",
                   (s)__FUNCTION__,
                   " : NetSock_TxData() maximum retry count reached, last error: ",
                   (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
          goto exit;
        }
        break;

      default:
        LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_TxData() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
        goto exit;
    }
  }

  rtn_value = DEF_OK;

  NetSock_CfgTimeoutRxQ_Set(sock_id, timeout_ms_cfgd, &err_local);
  if (RTOS_ERR_CODE_GET(err_local) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetSock_CfgTimeoutRxQ_Set() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(err_local))));
    goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));
exit:
  return (rtn_value);
}

/****************************************************************************************************//**
 *                                             FTPc_Conn()
 *
 * @brief    Connect a data socket in PASV or EPSV mode.
 *
 * @param    p_conn          Pointer to FTPc Connection object.
 *
 * @param    p_ctrl_buf      Pointer to buffer that contain the result of status message.
 *
 * @param    ctrl_buf_size   Size of control buffer.
 *
 * @param    p_err           Pointer to variable that will receive the return error code from this function.
 *
 * @return   Data socket ID,   if no error.
 *           NET_SOCK_ID_NONE, otherwise.
 *******************************************************************************************************/
static NET_SOCK_ID FTPc_Conn(FTPc_CONN  *p_conn,
                             CPU_CHAR   *p_ctrl_buf,
                             CPU_INT16U ctrl_buf_size,
                             RTOS_ERR   *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_IPv4_ADDR server_ipv4;
  CPU_INT32S    tmp_val;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_SOCK_ADDR_IPv6 *p_sock_addr_ipv6;
#endif
  NET_APP_SOCK_SECURE_CFG *p_secure = DEF_NULL;
  CPU_INT08U              *p_addr;
  NET_IP_ADDR_FAMILY      ip_addr_family;
  NET_SOCK_ID             sock_dtp_id = NET_SOCK_ID_NONE;
  CPU_INT32U              buf_size;
  CPU_INT16U              server_port;
  CPU_CHAR                *tmp_buf;
  CPU_INT32U              reply_code;
  CPU_BOOLEAN             rtn_code;
  RTOS_ERR                err;

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Start"));

  switch (p_conn->SockAddrFamily) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      buf_size = Str_Snprintf((char *)p_ctrl_buf, ctrl_buf_size, "%s\r\n", FTPc_Cmd[FTP_CMD_PASV].CmdStr);
      break;
#endif

    case NET_IP_ADDR_FAMILY_IPv6:
      buf_size = Str_Snprintf((char *)p_ctrl_buf, ctrl_buf_size, "%s\r\n", FTPc_Cmd[FTP_CMD_EPSV].CmdStr);
      break;

    default:
      LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Invalid Address family:"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, " : transmitting : ", (s)p_ctrl_buf));
  rtn_code = FTPc_Tx(p_conn->SockID,
                     p_ctrl_buf,
                     buf_size,
                     p_conn->CfgPtr->CtrlTxMaxTimout_ms,
                     p_conn->CfgPtr->CtrlTxMaxRetry,
                     p_conn->CfgPtr->CtrlTxMaxDly_ms,
                     &err);
  if (rtn_code == DEF_FAIL) {
    goto exit;
  }

  //                                                               Receive status line.
  reply_code = FTPc_WaitForStatus(p_conn, p_ctrl_buf, ctrl_buf_size, &err);

  switch (p_conn->SockAddrFamily) {
#ifdef  NET_IPv4_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv4:
      if (reply_code != FTP_REPLY_CODE_ENTERPASVMODE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Doesn't support passive mode"));
        goto exit;
      }

      server_ipv4 = 0;
      server_port = 0;
      tmp_buf = Str_Char(p_ctrl_buf, '(');
      tmp_buf++;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);              // Get IP Address MSB.
      tmp_buf++;
      server_ipv4 += tmp_val << 24;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);
      tmp_buf++;
      server_ipv4 += tmp_val << 16;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);
      tmp_buf++;
      server_ipv4 += tmp_val << 8;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);              // Get IP Address LSB.
      tmp_buf++;
      server_ipv4 += tmp_val << 0;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);              // Get IP Port MSB.
      tmp_buf++;
      server_port += tmp_val << 8;

      tmp_val = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);              // Get IP Port LSB.
      tmp_buf++;
      server_port += tmp_val << 0;

      ip_addr_family = NET_IP_ADDR_FAMILY_IPv4;
      p_addr = (CPU_INT08U *)&server_ipv4;
      break;
#endif

#ifdef  NET_IPv6_MODULE_EN
    case NET_IP_ADDR_FAMILY_IPv6:
      if (reply_code != FTP_REPLY_CODE_ENTEREXTPASVMODE) {
        RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
        LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : Doesn't support passive mode"));
        goto exit;
      }

      tmp_buf = Str_Str(p_ctrl_buf, FTP_ASCII_EPSV_PREFIX);
      tmp_buf = tmp_buf + Str_Len(FTP_ASCII_EPSV_PREFIX);
      server_port = Str_ParseNbr_Int32U(tmp_buf, &tmp_buf, 10);
      ip_addr_family = NET_IP_ADDR_FAMILY_IPv6;
      p_sock_addr_ipv6 = (NET_SOCK_ADDR_IPv6 *)&p_conn->SockAddr;
      p_addr = (CPU_INT08U *)&p_sock_addr_ipv6->Addr;
      break;
#endif

    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_ARG);
      goto exit;
  }

#ifdef  NET_SECURE_MODULE_EN
  p_secure = (NET_APP_SOCK_SECURE_CFG *)p_conn->SecureCfgPtr;
#endif
  sock_dtp_id = NetApp_ClientStreamOpen(p_addr,
                                        ip_addr_family,
                                        server_port,
                                        DEF_NULL,
                                        p_secure,
                                        p_conn->CfgPtr->DTP_ConnMaxTimout_ms,
                                        p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    LOG_DBG(("FTPc - ", (s)__FUNCTION__, " : NetApp_ClientStreamOpen() error returned: ", (s)RTOS_ERR_STR_GET(RTOS_ERR_CODE_GET(*p_err))));
    goto exit;
  }

  LOG_VRB(("FTPc - ", (s)__FUNCTION__, ": Normal exit"));

exit:
  return (sock_dtp_id);
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_FTP_CLIENT_AVAIL
