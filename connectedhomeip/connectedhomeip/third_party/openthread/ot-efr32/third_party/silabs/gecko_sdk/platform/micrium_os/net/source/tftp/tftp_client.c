/***************************************************************************//**
 * @file
 * @brief Network - TFTP Client Module
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

#include <rtos_description.h>

#if (defined(RTOS_MODULE_NET_TFTP_CLIENT_AVAIL))

#if (!defined(RTOS_MODULE_NET_AVAIL))
#error TFTP Client Module requires Network Core module. Make sure it is part of your project \
  and that RTOS_MODULE_NET_AVAIL is defined in rtos_description.h.
#endif

#if (!defined(RTOS_MODULE_FS_AVAIL))
#error TFTP Client Module requires File System module. Make sure it is part of your project \
  and that RTOS_MODULE_FS_AVAIL is defined in rtos_description.h.
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                               INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <net/include/tftp_client.h>
#include  <net/include/net.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_fs.h>
#include  <net/include/net_util.h>
#include  <net/include/net_app.h>

#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>

#include  <common/source/kal/kal_priv.h>
#include  <common/source/rtos/rtos_utils_priv.h>
#include  <common/source/logging/logging_priv.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  LOG_DFLT_CH                                    (NET, TFTP)
#define  RTOS_MODULE_CUR                                 RTOS_CFG_MODULE_NET

/********************************************************************************************************
 *                                       FILE OPEN MODE DEFINES
 *******************************************************************************************************/

#define  TFTPc_FILE_OPEN_RD                                0
#define  TFTPc_FILE_OPEN_WR                                1

/********************************************************************************************************
 *                                           TFTP OPCODE DEFINES
 *******************************************************************************************************/

#define  TFTP_OPCODE_RRQ                                   1
#define  TFTP_OPCODE_WRQ                                   2
#define  TFTP_OPCODE_DATA                                  3
#define  TFTP_OPCODE_ACK                                   4
#define  TFTP_OPCODE_ERR                                   5

/********************************************************************************************************
 *                                       TFTP PKT OFFSET DEFINES
 *******************************************************************************************************/

#define  TFTP_PKT_OFFSET_OPCODE                            0
#define  TFTP_PKT_OFFSET_FILENAME                          2
#define  TFTP_PKT_OFFSET_BLK_NBR                           2
#define  TFTP_PKT_OFFSET_ERR_CODE                          2
#define  TFTP_PKT_OFFSET_ERR_MSG                           4
#define  TFTP_PKT_OFFSET_DATA                              4

/********************************************************************************************************
 *                                       TFTP PKT FIELD SIZE DEFINES
 *******************************************************************************************************/

#define  TFTP_PKT_SIZE_OPCODE                              2
#define  TFTP_PKT_SIZE_BLK_NBR                             2
#define  TFTP_PKT_SIZE_ERR_CODE                            2
#define  TFTP_PKT_SIZE_NULL                                1

/********************************************************************************************************
 *                                       TFTP TRANSFER MODE DEFINES
 *******************************************************************************************************/

#define  TFTP_MODE_NETASCII_STR                    "netascii"
#define  TFTP_MODE_NETASCII_STR_LEN                        8
#define  TFTP_MODE_BINARY_STR                         "octet"
#define  TFTP_MODE_BINARY_STR_LEN                          5

/********************************************************************************************************
 *                                           TFTP PKT DEFINES
 *******************************************************************************************************/

#define  TFTPc_DATA_BLOCK_SIZE                           512
#define  TFTPc_PKT_BUF_SIZE                     (TFTPc_DATA_BLOCK_SIZE + TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR)

#define  TFTPc_MAX_NBR_TX_RETRY                            3

/********************************************************************************************************
 *                                           TFTP ERROR CODES
 *******************************************************************************************************/

#define  TFTP_ERR_CODE_NOT_DEF                             0    // Not defined, see err message (if any).
#define  TFTP_ERR_CODE_FILE_NOT_FOUND                      1    // File not found.
#define  TFTP_ERR_CODE_ACCESS_VIOLATION                    2    // Access violation.
#define  TFTP_ERR_CODE_DISK_FULL                           3    // Disk full of allocation exceeded.
#define  TFTP_ERR_CODE_ILLEGAL_OP                          4    // Illegal TFTP operation.
#define  TFTP_ERR_CODE_UNKNOWN_ID                          5    // Unknown transfer ID.
#define  TFTP_ERR_CODE_FILE_EXISTS                         6    // File already exists.
#define  TFTP_ERR_CODE_NO_USER                             7    // No such user.

/********************************************************************************************************
 *                                           TFTPc ERROR MESSAGES
 *******************************************************************************************************/

#define  TFTPc_ERR_MSG_WR_ERR              "File write error"
#define  TFTPc_ERR_MSG_RD_ERR              "File read error"

/********************************************************************************************************
 *                                       TFTPc CLIENT STATE DEFINES
 *******************************************************************************************************/

#define  TFTPc_STATE_DATA_GET                              1
#define  TFTPc_STATE_DATA_PUT                              2
#define  TFTPc_STATE_DATA_PUT_WAIT_LAST_ACK                3
#define  TFTPc_STATE_TRANSFER_COMPLETE                     4

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       FILE ACCESS MODE DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U TFTPc_FILE_ACCESS;

/********************************************************************************************************
 *                                       TFTPc BLOCK NUMBER DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT16U TFTPc_BLK_NBR;

/********************************************************************************************************
 *                                   TFTPc SERVER OBJECT DATA TYPE
 *******************************************************************************************************/

typedef  struct  tftpc_conn_obj {
  const TFTPc_CFG    *CfgPtr;
  NET_IP_ADDR_FAMILY AddrFamily;

  CPU_INT16U         RxBlkNbrNext;                                     // Next rx'd blk nbr expected.

  CPU_INT08U         RxPktBuf[TFTPc_PKT_BUF_SIZE];                     // Last rx'd pkt buf.
  CPU_INT32S         RxPktLen;                                         // Last rx'd pkt len.
  CPU_INT16U         RxPktOpcode;                                      // Last rx'd pkt opcode.

  CPU_INT16U         TxPktBlkNbr;                                      // Last tx'd pkt blk nbr.
  CPU_INT08U         TxPktBuf[TFTPc_PKT_BUF_SIZE];                     // Last tx'd pkt buf.
  CPU_INT16U         TxPktLen;                                         // Last tx'd pkt len.
  CPU_INT08U         TxPktRetry;                                       // Nbr of time last tx'd pkt had been sent.

  NET_SOCK_ID        SockID;                                           // Client sock id.
  NET_SOCK_ADDR      SockAddr;                                         // Server sock addr IP.

  CPU_BOOLEAN        TID_Set;                                          // Indicates whether the terminal ID is set or not.

  CPU_INT08U         State;                                            // Cur state of TFTPc state machine.

  void               *FileHandle;                                      // Handle to cur opened file.
} TFTPc_CONN_OBJ;

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

const TFTPc_CFG TFTPc_CfgDflt = {
  .ServerPortNbr = TFTP_CLIENT_CFG_PORT_NBR_DFLT,
  .RxInactivityTimeout_ms = TFTP_CLIENT_CFG_RX_TIMEOUT_MS_DFLT,
  .TxInactivityTimeout_ms = TFTP_CLIENT_CFG_TX_TIMEOUT_MS_DFLT
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static const NET_FS_API *TFTPc_FS_API_Ptr = &NetFS_API_Native;

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 -------------------- INIT FNCT ---------------------
static CPU_BOOLEAN TFTPc_SockInit(TFTPc_CONN_OBJ     *p_conn,
                                  CPU_CHAR           *p_server_hostname,
                                  NET_PORT_NBR       server_port,
                                  NET_IP_ADDR_FAMILY *p_ip_family,
                                  RTOS_ERR           *p_err);

//                                                                 ----------------- PROCESSING FNCTS -----------------
static void TFTPc_Processing(TFTPc_CONN_OBJ *p_conn,
                             RTOS_ERR       *p_err);

static void TFTPc_StateDataGet(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err);

static void TFTPc_StateDataPut(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err);

static CPU_INT16U TFTPc_GetRxBlkNbr(TFTPc_CONN_OBJ *p_conn);

//                                                                 ---------------- FILE ACCESS FNCTS -----------------
static void *TFTPc_FileOpenMode(CPU_CHAR          *p_filename,
                                TFTPc_FILE_ACCESS file_access);

static CPU_INT16U TFTPc_DataWr(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err);

static CPU_INT16U TFTPc_DataRd(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err);

//                                                                 --------------------- RX FNCTS ---------------------
static NET_SOCK_RTN_CODE TFTPc_RxPkt(TFTPc_CONN_OBJ *p_conn,
                                     void           *p_pkt,
                                     CPU_INT16U     pkt_len,
                                     RTOS_ERR       *p_err);

//                                                                 --------------------- TX FNCTS ---------------------
static void TFTPc_TxReq(TFTPc_CONN_OBJ *p_conn,
                        CPU_INT16U     req_opcode,
                        CPU_CHAR       *p_filename,
                        TFTPc_MODE     mode,
                        RTOS_ERR       *p_err);

static void TFTPc_TxData(TFTPc_CONN_OBJ *p_conn,
                         TFTPc_BLK_NBR  blk_nbr,
                         CPU_INT16U     data_len,
                         RTOS_ERR       *p_err);

static void TFTPc_TxAck(TFTPc_CONN_OBJ *p_conn,
                        TFTPc_BLK_NBR  blk_nbr,
                        RTOS_ERR       *p_err);

static void TFTPc_TxErr(TFTPc_CONN_OBJ *p_conn,
                        CPU_INT16U     err_code,
                        CPU_CHAR       *p_err_msg,
                        RTOS_ERR       *p_err);

static void TFTPc_Terminate(TFTPc_CONN_OBJ *p_conn);

/********************************************************************************************************
 ********************************************************************************************************
 *                                           PUBLIC FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               TFTPc_Get()
 *
 * @brief    Gets a file from the TFTP server.
 *
 * @param    p_host_server       Server host name.
 *
 * @param    p_cfg               Pointer to TFTP client configuration to use.
 *                               DEF_NULL, if the default configuration must be used.
 *
 * @param    p_filename_local    Pointer to name of the file to be written by   the client.
 *
 * @param    p_filename_remote   Pointer to name of the file to be read    from the server.
 *
 * @param    mode                TFTP transfer mode :
 *                                   - TFTPc_MODE_NETASCII     ASCII  mode.
 *                                   - TFTPc_MODE_OCTET        Binary mode.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void TFTPc_Get(CPU_CHAR        *p_host_server,
               const TFTPc_CFG *p_cfg,
               CPU_CHAR        *p_filename_local,
               CPU_CHAR        *p_filename_remote,
               TFTPc_MODE      mode,
               RTOS_ERR        *p_err)
{
  TFTPc_CONN_OBJ conn_obj;
  NET_PORT_NBR   server_port;
#ifdef NET_IPv6_MODULE_EN
  NET_IP_ADDR_FAMILY ip_family = NET_IP_ADDR_FAMILY_IPv6;
#else
  NET_IP_ADDR_FAMILY ip_family = NET_IP_ADDR_FAMILY_IPv4;
#endif
  CPU_BOOLEAN is_hostname;
  CPU_BOOLEAN retry;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_host_server != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_filename_local != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_filename_remote != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("TFTPc_Get: Request for %s\n\r", (s)p_filename_remote));

  Mem_Clr(&conn_obj, sizeof(TFTPc_CONN_OBJ));

  if (p_cfg == DEF_NULL) {
    conn_obj.CfgPtr = &TFTPc_CfgDflt;
  } else {
    conn_obj.CfgPtr = p_cfg;
  }

  server_port = conn_obj.CfgPtr->ServerPortNbr;

  //                                                               Open file
  conn_obj.FileHandle = TFTPc_FileOpenMode(p_filename_local, TFTPc_FILE_OPEN_WR);
  if (conn_obj.FileHandle == DEF_NULL) {
    TFTPc_Terminate(&conn_obj);
    goto exit;
  }

  retry = DEF_YES;
  while (retry == DEF_YES) {
    is_hostname = TFTPc_SockInit(&conn_obj,
                                 p_host_server,              // Init sock.
                                 server_port,
                                 &ip_family,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      if ((ip_family == NET_IP_ADDR_FAMILY_IPv6)
          && (is_hostname == DEF_YES)                ) {
        retry = DEF_YES;
        ip_family = NET_IP_ADDR_FAMILY_IPv4;
      } else {
        TFTPc_Terminate(&conn_obj);
        retry = DEF_NO;
        goto exit;
      }
    } else {
      retry = DEF_NO;
    }

    if (retry == DEF_NO) {
      //                                                           Tx rd req.
      TFTPc_TxReq(&conn_obj, TFTP_OPCODE_RRQ, p_filename_remote, mode, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        if ((ip_family == NET_IP_ADDR_FAMILY_IPv6)
            && (is_hostname == DEF_YES)                ) {
          retry = DEF_YES;
          ip_family = NET_IP_ADDR_FAMILY_IPv4;
        } else {
          TFTPc_Terminate(&conn_obj);
          retry = DEF_NO;
          goto exit;
        }
      } else {
        retry = DEF_NO;
      }

      if (retry == DEF_NO) {
        //                                                         Process req.
        conn_obj.RxBlkNbrNext = 1u;
        conn_obj.State = TFTPc_STATE_DATA_GET;
        conn_obj.AddrFamily = ip_family;

        TFTPc_Processing(&conn_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
    }
  }

exit:
  return;
}

/****************************************************************************************************//**
 *                                               TFTPc_Put()
 *
 * @brief    Puts a file on the TFTP server.
 *
 * @param    p_host_server       Server host name.
 *
 * @param    p_cfg               Pointer to TFTP client configuration to use.
 *                               DEF_NULL, if the default configuration must be used.
 *
 * @param    p_filename_local    Pointer to name of the file to be read by the client.
 *
 * @param    p_filename_remote   Pointer to name of the file to be written to the server.
 *
 * @param    mode                TFTP transfer mode :
 *                                   - TFTPc_MODE_NETASCII     ASCII  mode.
 *                                   - TFTPc_MODE_OCTET        Binary mode.
 *
 * @param    p_err               Pointer to the variable that will receive one of the following error
 *                               code(s) from this function:
 *                                   - RTOS_ERR_NONE
 *                                   - RTOS_ERR_TX
 *                                   - RTOS_ERR_WOULD_BLOCK
 *                                   - RTOS_ERR_TIMEOUT
 *******************************************************************************************************/
void TFTPc_Put(CPU_CHAR        *p_host_server,
               const TFTPc_CFG *p_cfg,
               CPU_CHAR        *p_filename_local,
               CPU_CHAR        *p_filename_remote,
               TFTPc_MODE      mode,
               RTOS_ERR        *p_err)
{
  TFTPc_CONN_OBJ conn_obj;
  NET_PORT_NBR   server_port;
#ifdef NET_IPv6_MODULE_EN
  NET_IP_ADDR_FAMILY ip_family = NET_IP_ADDR_FAMILY_IPv6;
#else
  NET_IP_ADDR_FAMILY ip_family = NET_IP_ADDR_FAMILY_IPv4;
#endif
  CPU_BOOLEAN is_hostname;
  CPU_BOOLEAN retry;

  RTOS_ASSERT_DBG_ERR_PTR_VALIDATE(p_err,; );
  RTOS_ASSERT_DBG_ERR_SET((p_host_server != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_filename_local != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ASSERT_DBG_ERR_SET((p_filename_remote != DEF_NULL), *p_err, RTOS_ERR_NULL_PTR,; );
  RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);

  LOG_VRB(("TFTPc_Put: Request for %s\n\r", (s)p_filename_local));

  Mem_Clr(&conn_obj, sizeof(TFTPc_CONN_OBJ));

  if (p_cfg == DEF_NULL) {
    conn_obj.CfgPtr = &TFTPc_CfgDflt;
  } else {
    conn_obj.CfgPtr = p_cfg;
  }

  server_port = conn_obj.CfgPtr->ServerPortNbr;

  //                                                               Open file.
  conn_obj.FileHandle = TFTPc_FileOpenMode(p_filename_local, TFTPc_FILE_OPEN_RD);
  if (conn_obj.FileHandle == DEF_NULL) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_FAIL);
    goto exit;
  }

  retry = DEF_YES;
  while (retry == DEF_YES) {
    is_hostname = TFTPc_SockInit(&conn_obj,                     // Init sock.
                                 p_host_server,
                                 server_port,
                                 &ip_family,
                                 p_err);
    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      if ((ip_family == NET_IP_ADDR_FAMILY_IPv6)
          && (is_hostname == DEF_YES)                ) {
        retry = DEF_YES;
        ip_family = NET_IP_ADDR_FAMILY_IPv4;
      } else {
        TFTPc_Terminate(&conn_obj);
        retry = DEF_NO;

        goto exit;
      }
    } else {
      retry = DEF_NO;
    }

    if (retry == DEF_NO) {
      //                                                           Tx rd to server.
      TFTPc_TxReq(&conn_obj, TFTP_OPCODE_WRQ, p_filename_remote, mode, p_err);
      if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
        if ((ip_family == NET_IP_ADDR_FAMILY_IPv6)
            && (is_hostname == DEF_YES)                ) {
          retry = DEF_YES;
          ip_family = NET_IP_ADDR_FAMILY_IPv4;
        } else {
          TFTPc_Terminate(&conn_obj);
          retry = DEF_NO;
          goto exit;
        }
      } else {
        retry = DEF_NO;
      }

      if (retry == DEF_NO) {
        //                                                         Process req.
        conn_obj.TxPktBlkNbr = 0;
        conn_obj.State = TFTPc_STATE_DATA_PUT;
        conn_obj.AddrFamily = ip_family;

        TFTPc_Processing(&conn_obj, p_err);
        if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
          goto exit;
        }
      }
    }
  }

exit:
  return;
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               TFTPc_SockInit()
 *
 * @brief    Initialize the communication socket.
 *
 * @param    p_conn              Pointer to TFTP connection object.
 *
 * @param    p_server_hostname   Pointer to hostname or IP address string of the TFTP server.
 *
 * @param    server_port         Port number of the TFTP server.
 *
 * @param    p_ip_family         Pointer to pass IP family for DNS address retrieval & to return
 *                               IP family to use.
 *                               NET_IP_ADDR_FAMILY_IPv4
 *                               NET_IP_ADDR_FAMILY_IPv6
 *
 * @param    p_err               Pointer to variable that will receive the return error code from this function.
 *
 * @return   DEF_YES, if string passed was hostname.
 *           DEF_NO, otherwise.
 *******************************************************************************************************/
static CPU_BOOLEAN TFTPc_SockInit(TFTPc_CONN_OBJ     *p_conn,
                                  CPU_CHAR           *p_server_hostname,
                                  NET_PORT_NBR       server_port,
                                  NET_IP_ADDR_FAMILY *p_ip_family,
                                  RTOS_ERR           *p_err)
{
  CPU_BOOLEAN        is_hostname;
  NET_IP_ADDR_FAMILY ip_family_rtn;

  ip_family_rtn = NetApp_ClientDatagramOpenByHostname(&p_conn->SockID,
                                                      p_server_hostname,
                                                      server_port,
                                                      *p_ip_family,
                                                      &p_conn->SockAddr,
                                                      &is_hostname,
                                                      p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  *p_ip_family = ip_family_rtn;

  (void)NetSock_CfgBlock(p_conn->SockID, NET_SOCK_BLOCK_SEL_BLOCK, p_err);

exit:
  return (is_hostname);
}

/****************************************************************************************************//**
 *                                           TFTPc_Processing()
 *
 * @brief    Process data transfer.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TFTPc_Processing(TFTPc_CONN_OBJ *p_conn,
                             RTOS_ERR       *p_err)
{
  const TFTPc_CFG   *p_cfg = p_conn->CfgPtr;
  NET_SOCK_RTN_CODE rx_pkt_len;
  NET_SOCK_ADDR_LEN sock_addr_size;
  CPU_INT32U        timeout;
  RTOS_ERR          local_err;

  //                                                               Set rx sock timeout.
  timeout = p_cfg->RxInactivityTimeout_ms;
  NetSock_CfgTimeoutRxQ_Set(p_conn->SockID,
                            timeout,
                            &local_err);

  while (p_conn->State != TFTPc_STATE_TRANSFER_COMPLETE) {
    rx_pkt_len = TFTPc_RxPkt(p_conn,
                             &p_conn->RxPktBuf[0],
                             sizeof(p_conn->RxPktBuf),
                             p_err);
    switch (RTOS_ERR_CODE_GET(*p_err)) {
      case RTOS_ERR_NONE:
        p_conn->RxPktLen = rx_pkt_len;
        p_conn->RxPktOpcode = NET_UTIL_VAL_GET_NET_16(&p_conn->RxPktBuf[TFTP_PKT_OFFSET_OPCODE]);

        switch (p_conn->State) {
          case TFTPc_STATE_DATA_GET:
            TFTPc_StateDataGet(p_conn, p_err);
            break;

          case TFTPc_STATE_DATA_PUT:
          case TFTPc_STATE_DATA_PUT_WAIT_LAST_ACK:
            TFTPc_StateDataPut(p_conn, p_err);
            break;

          default:
            RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
            break;
        }

        break;

      case RTOS_ERR_WOULD_BLOCK:
      case RTOS_ERR_TIMEOUT:
        if (p_conn->TxPktLen > 0) {                               // If pkt tx'd ...
                                                                  // ... and max retry NOT reached, ...
          if (p_conn->TxPktRetry < TFTPc_MAX_NBR_TX_RETRY) {
            //                                                     ... re-tx last tx'd pkt.
            sock_addr_size = sizeof(NET_SOCK_ADDR);
            (void)NetSock_TxDataTo(p_conn->SockID,
                                   &p_conn->TxPktBuf[0],
                                   p_conn->TxPktLen,
                                   NET_SOCK_FLAG_NONE,
                                   &p_conn->SockAddr,
                                   sock_addr_size,
                                   p_err);

            p_conn->TxPktRetry++;
          }
        }
        break;

      case RTOS_ERR_RX:
      default:
        break;
    }

    if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
      LOG_VRB(("TFTPc_Processing: Error, session terminated\n\r"));
      p_conn->State = TFTPc_STATE_TRANSFER_COMPLETE;
    }
  }

  TFTPc_Terminate(p_conn);
}

/****************************************************************************************************//**
 *                                           TFTPc_StateDataGet()
 *
 * @brief    Process received packets for a read request.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 *                   @note         (1) If the data block received is not the expected one, nothing is written in the file,
 *                   and the function silently returns.
 *******************************************************************************************************/
static void TFTPc_StateDataGet(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err)
{
  CPU_INT16U rx_blk_nbr;
  CPU_INT16U wr_data_len;
  RTOS_ERR   err;

  switch (p_conn->RxPktOpcode) {
    case TFTP_OPCODE_DATA:
      LOG_VRB(("TFTPc_StateDataGet: Opcode DATA rx'd\n\r"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case TFTP_OPCODE_ERR:
      LOG_VRB(("TFTPc_StateDataGet: Opcode ERROR rx'd\n\r"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;

    case TFTP_OPCODE_ACK:
    case TFTP_OPCODE_WRQ:
    case TFTP_OPCODE_RRQ:
    default:
      LOG_VRB(("TFTPc_StateDataGet: Invalid opcode rx'd\n\r"));
      TFTPc_TxErr(p_conn, TFTP_ERR_CODE_ILLEGAL_OP, DEF_NULL, &err);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  rx_blk_nbr = TFTPc_GetRxBlkNbr(p_conn);                       // Get rx'd pkt's blk nbr.

  if (rx_blk_nbr == p_conn->RxBlkNbrNext) {                     // If data blk nbr expected, (see Note #1) ...
    wr_data_len = TFTPc_DataWr(p_conn, p_err);                  // ... wr data to file                ...

    if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
      TFTPc_TxAck(p_conn, rx_blk_nbr, &err);                    // ... and tx ack.
      p_conn->TxPktRetry = 0;

      if (wr_data_len < TFTPc_DATA_BLOCK_SIZE) {                // If rx'd data len < TFTP blk size, ...
        p_conn->State = TFTPc_STATE_TRANSFER_COMPLETE;          // ... last blk rx'd.
      } else {
        p_conn->RxBlkNbrNext++;
      }
    } else {                                                    // Err wr'ing data to file.
      TFTPc_TxErr(p_conn, TFTP_ERR_CODE_NOT_DEF, TFTPc_ERR_MSG_WR_ERR, &err);
    }
  }
}

/****************************************************************************************************//**
 *                                           TFTPc_StateDataPut()
 *
 * @brief    Process received packet for a write request.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 *                   @note         (1) If the acknowledge block received is not the expected one, nothing is done, and the
 *                   function silently returns.  This is done in order to prevent the 'Sorcerer's Apprentice'
 *                   bug.  Only a receive timeout is supposed to trigger a block retransmission.
 *******************************************************************************************************/
static void TFTPc_StateDataPut(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err)
{
  CPU_INT16U rx_blk_nbr;
  CPU_INT16U rd_data_len;
  RTOS_ERR   err;

  switch (p_conn->RxPktOpcode) {
    case TFTP_OPCODE_ACK:
      LOG_VRB(("TFTPc_StateDataPut: Opcode ACK rx'd\n\r"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
      break;

    case TFTP_OPCODE_ERR:
      LOG_VRB(("TFTPc_StateDataPut: Opcode ERROR rx'd\n\r"));
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;

    case TFTP_OPCODE_DATA:
    case TFTP_OPCODE_WRQ:
    case TFTP_OPCODE_RRQ:
    default:
      LOG_VRB(("TFTPc_StateDataPut: Invalid opcode rx'd\n\r"));
      TFTPc_TxErr(p_conn, TFTP_ERR_CODE_ILLEGAL_OP, DEF_NULL, &err);
      RTOS_ERR_SET(*p_err, RTOS_ERR_RX);
      break;
  }

  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    return;
  }

  rx_blk_nbr = TFTPc_GetRxBlkNbr(p_conn);                       // Get rx'd pkt's blk nbr.

  if (rx_blk_nbr == p_conn->TxPktBlkNbr) {                      // If ACK blk nbr matches data sent (see Note #1) ...
    switch (p_conn->State) {
      case TFTPc_STATE_DATA_PUT:                                // ... and more data to read, ...
        rd_data_len = TFTPc_DataRd(p_conn, p_err);              // ... rd next blk from file                      ...
        if (RTOS_ERR_CODE_GET(*p_err) == RTOS_ERR_NONE) {
          p_conn->TxPktBlkNbr++;                                // ... and tx data.
          TFTPc_TxData(p_conn, p_conn->TxPktBlkNbr, rd_data_len, p_err);
          p_conn->TxPktRetry = 0;

          if (rd_data_len < TFTPc_DATA_BLOCK_SIZE) {
            p_conn->State = TFTPc_STATE_DATA_PUT_WAIT_LAST_ACK;
          }
        } else {                                                // Err rd'ing data to file.
          TFTPc_TxErr(p_conn, TFTP_ERR_CODE_NOT_DEF, TFTPc_ERR_MSG_RD_ERR, &err);
        }
        break;

      case TFTPc_STATE_DATA_PUT_WAIT_LAST_ACK:                  // If waiting for last ack, ...
        p_conn->State = TFTPc_STATE_TRANSFER_COMPLETE;            // ... transfer completed.
        break;

      default:
        RTOS_ERR_SET(*p_err, RTOS_ERR_INVALID_STATE);
        break;
    }
  }
}

/****************************************************************************************************//**
 *                                           TFTPc_GetRxBlkNbr()
 *
 * @brief    Extract the block number from the received TFTP packet.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @return   Received block number.
 *******************************************************************************************************/
static CPU_INT16U TFTPc_GetRxBlkNbr(TFTPc_CONN_OBJ *p_conn)
{
  CPU_INT16U blk_nbr;

  blk_nbr = NET_UTIL_VAL_GET_NET_16(&p_conn->RxPktBuf[TFTP_PKT_OFFSET_BLK_NBR]);

  return (blk_nbr);
}

/****************************************************************************************************//**
 *                                           TFTPc_FileOpenMode()
 *
 * @brief    Open the specified file for read or write.
 *
 * @param    filename        Name of the file to open.
 *
 * @param    file_access     File access :
 *                           TFTPc_FILE_OPEN_RD      Open for reading.
 *                           TFTPc_FILE_OPEN_WR      Open for writing.
 *
 * @return   Pointer to a file handle for the opened file, if NO error.
 *           Pointer to NULL, otherwise.
 *******************************************************************************************************/
static void *TFTPc_FileOpenMode(CPU_CHAR          *p_filename,
                                TFTPc_FILE_ACCESS file_access)
{
  void *p_file = DEF_NULL;

  switch (file_access) {
    case TFTPc_FILE_OPEN_RD:
      p_file = TFTPc_FS_API_Ptr->Open(p_filename,
                                      NET_FS_FILE_MODE_OPEN,
                                      NET_FS_FILE_ACCESS_RD);
      break;

    case TFTPc_FILE_OPEN_WR:
      p_file = TFTPc_FS_API_Ptr->Open(p_filename,
                                      NET_FS_FILE_MODE_CREATE,
                                      NET_FS_FILE_ACCESS_WR);
      break;

    default:
      break;
  }

  return (p_file);
}

/****************************************************************************************************//**
 *                                               TFTPc_DataWr()
 *
 * @brief    Write data to the file system.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of octets written to file.
 *******************************************************************************************************/
static CPU_INT16U TFTPc_DataWr(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err)
{
  CPU_SIZE_T rx_data_len;
  CPU_SIZE_T wr_data_len;

  rx_data_len = p_conn->RxPktLen - TFTP_PKT_SIZE_OPCODE - TFTP_PKT_SIZE_BLK_NBR;
  wr_data_len = 0;

  if (rx_data_len > 0) {
    (void)TFTPc_FS_API_Ptr->Wr(p_conn->FileHandle,
                               &p_conn->RxPktBuf[TFTP_PKT_OFFSET_DATA],
                               rx_data_len,
                               &wr_data_len);
  }

  if (wr_data_len != rx_data_len) {
    RTOS_ERR_SET(*p_err, RTOS_ERR_IO);
  } else {
    RTOS_ERR_SET(*p_err, RTOS_ERR_NONE);
  }

  return ((CPU_INT16U)wr_data_len);
}

/****************************************************************************************************//**
 *                                               TFTPc_DataRd()
 *
 * @brief    Read data from the file system.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *
 * @param    p_err   Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of octets read from file.
 *******************************************************************************************************/
static CPU_INT16U TFTPc_DataRd(TFTPc_CONN_OBJ *p_conn,
                               RTOS_ERR       *p_err)
{
  CPU_SIZE_T  rd_data_len;
  CPU_BOOLEAN err;

  //                                                               Rd data from file.
  err = TFTPc_FS_API_Ptr->Rd(p_conn->FileHandle,
                             &p_conn->TxPktBuf[TFTP_PKT_OFFSET_DATA],
                             TFTPc_DATA_BLOCK_SIZE,
                             &rd_data_len);

  if (rd_data_len == 0) {                                       // If NO data rd                   ...
    if (err == DEF_FAIL) {                                      // ... and err occurred (NOT EOF), ...
      RTOS_ERR_SET(*p_err, RTOS_ERR_IO);                        // ... rtn err.
    }
  }

  return ((CPU_INT16U)rd_data_len);
}

/****************************************************************************************************//**
 *                                               TFTPc_RxPkt()
 *
 * @brief    Receive TFTP packet.
 *
 * @param    p_conn      Pointer to TFTP connection object..
 *
 * @param    p_pkt       Pointer to packet to receive.
 *
 * @param    pkt_len     Length of  packet to receive (in octets).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 * @return   Number of positive data octets received, if NO errors.
 *
 *           NET_SOCK_BSD_RTN_CODE_CONN_CLOSED,       if socket connection closed.
 *
 *           NET_SOCK_BSD_ERR_RX,                     otherwise.
 *
 * @note         (1) #### When returning from the receive function, we should make sure the received
 *               packet comes from the host we are connected to.
 *
 * @note         (2) #### Transitory errors should probably trigger another attempt to
 *               transmit the packet, instead of returning an error right away.
 *******************************************************************************************************/
static NET_SOCK_RTN_CODE TFTPc_RxPkt(TFTPc_CONN_OBJ *p_conn,
                                     void           *p_pkt,
                                     CPU_INT16U     pkt_len,
                                     RTOS_ERR       *p_err)
{
#ifdef  NET_IPv4_MODULE_EN
  NET_SOCK_ADDR_IPv4 *p_addrv4;
  NET_SOCK_ADDR_IPv4 *p_serverv4;
#endif
#ifdef  NET_IPv6_MODULE_EN
  NET_SOCK_ADDR_IPv6 *p_addrv6;
  NET_SOCK_ADDR_IPv6 *p_serverv6;
#endif
  NET_SOCK_RTN_CODE rtn_code;
  NET_SOCK_ADDR     server_sock_addr_ip;
  NET_SOCK_ADDR_LEN server_sock_addr_ip_len;

  //                                                               --------------- RX PKT THROUGH SOCK ----------------
  //                                                               See Note #1.
  server_sock_addr_ip_len = sizeof(server_sock_addr_ip);
  rtn_code = NetSock_RxDataFrom(p_conn->SockID,
                                p_pkt,
                                pkt_len,
                                NET_SOCK_FLAG_NONE,
                                &server_sock_addr_ip,
                                &server_sock_addr_ip_len,
                                DEF_NULL,
                                0,
                                DEF_NULL,
                                p_err);
  if (RTOS_ERR_CODE_GET(*p_err) != RTOS_ERR_NONE) {
    goto exit;
  }

  if (p_conn->TID_Set != DEF_YES) {                             // If terminal ID NOT set, ...
                                                                // ... change server port to last rx'd one.
    switch (p_conn->AddrFamily) {
#ifdef  NET_IPv4_MODULE_EN
      case NET_IP_ADDR_FAMILY_IPv4:
        p_addrv4 = (NET_SOCK_ADDR_IPv4 *)&p_conn->SockAddr;
        p_serverv4 = (NET_SOCK_ADDR_IPv4 *)&server_sock_addr_ip;
        p_addrv4->Port = p_serverv4->Port;
        break;
#endif
#ifdef  NET_IPv6_MODULE_EN
      case NET_IP_ADDR_FAMILY_IPv6:
        p_addrv6 = (NET_SOCK_ADDR_IPv6 *)&p_conn->SockAddr;
        p_serverv6 = (NET_SOCK_ADDR_IPv6 *)&server_sock_addr_ip;
        p_addrv6->Port = p_serverv6->Port;
        break;
#endif
      default:
        goto exit;
    }

    p_conn->TID_Set = DEF_YES;
  }

exit:
  return (rtn_code);
}

/****************************************************************************************************//**
 *                                               TFTPc_TxReq()
 *
 * @brief    Transmit TFTP request packet.
 *
 * @param    p_conn      Pointer to TFTP connection object.
 *
 * @param    req_opcode  Opcode for this request :
 *                       TFTP_OPCODE_RRQ         Read  request.
 *                       TFTP_OPCODE_WRQ         Write request.
 *
 * @param    p_filename  Name of the file to transfer.
 *
 * @param    mode        TFTP mode :
 *                       TFTPc_MODE_NETASCII     ASCII  mode.
 *                       TFTPc_MODE_OCTET        Binary mode.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *
 *                       @note         (1) RFC #1350, section 1 'Purpose' states that "the mail mode is obsolete and should not
 *                       be implemented or used".
 *******************************************************************************************************/
static void TFTPc_TxReq(TFTPc_CONN_OBJ *p_conn,
                        CPU_INT16U     req_opcode,
                        CPU_CHAR       *p_filename,
                        TFTPc_MODE     mode,
                        RTOS_ERR       *p_err)
{
  CPU_CHAR          *pmode_str;
  CPU_INT16U        filename_len;
  CPU_INT16U        mode_len;
  CPU_INT16U        wr_pkt_ix;
  NET_SOCK_ADDR_LEN sock_addr_size;

  switch (mode) {
    case TFTPc_MODE_NETASCII:
      pmode_str = TFTP_MODE_NETASCII_STR;
      break;

    case TFTPc_MODE_OCTET:
      pmode_str = TFTP_MODE_BINARY_STR;
      break;

    case TFTPc_MODE_MAIL:                                       // See Note #1.
    default:
      RTOS_ERR_SET(*p_err, RTOS_ERR_NOT_SUPPORTED);
      return;
  }

  //                                                               -------------------- CREATE PKT --------------------
  //                                                               Wr opcode.
  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_OPCODE],
                          req_opcode);

  //                                                               Copy filename.
  Str_Copy((CPU_CHAR *)&p_conn->TxPktBuf[TFTP_PKT_OFFSET_FILENAME],
           (CPU_CHAR *) p_filename);

  //                                                               Wr mode.
  filename_len = Str_Len(p_filename);
  wr_pkt_ix = TFTP_PKT_OFFSET_FILENAME
              + filename_len
              + TFTP_PKT_SIZE_NULL;

  Str_Copy((CPU_CHAR *)&p_conn->TxPktBuf[wr_pkt_ix],
           (CPU_CHAR *) pmode_str);

  mode_len = Str_Len(pmode_str);

  //                                                               Get total pkt size.
  p_conn->TxPktLen = wr_pkt_ix + mode_len + TFTP_PKT_SIZE_NULL;

  //                                                               --------------------- TX PKT ----------------------
  sock_addr_size = sizeof(NET_SOCK_ADDR);
  (void)NetSock_TxDataTo(p_conn->SockID,
                         &p_conn->TxPktBuf[0],
                         p_conn->TxPktLen,
                         NET_SOCK_FLAG_NONE,
                         &p_conn->SockAddr,
                         sock_addr_size,
                         p_err);
}

/****************************************************************************************************//**
 *                                               TFTPc_TxData()
 *
 * @brief    Transmit TFTP data packet.
 *
 * @param    p_conn      Pointer to TFTP connection object.
 *
 * @param    blk_nbr     Block number for data packet.
 *
 * @param    data_len    Length of data portion of packet (in octets).
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TFTPc_TxData(TFTPc_CONN_OBJ *p_conn,
                         TFTPc_BLK_NBR  blk_nbr,
                         CPU_INT16U     data_len,
                         RTOS_ERR       *p_err)
{
  NET_SOCK_ADDR_LEN sock_addr_size;

  //                                                               -------------------- CREATE PKT --------------------
  //                                                               Wr opcode.
  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_OPCODE],
                          TFTP_OPCODE_DATA);

  //                                                               Wr blk nbr.
  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_BLK_NBR],
                          blk_nbr);

  //                                                               Get total pkt size.
  p_conn->TxPktLen = TFTP_PKT_SIZE_OPCODE
                     + TFTP_PKT_SIZE_BLK_NBR
                     + data_len;

  //                                                               --------------------- TX PKT ----------------------
  sock_addr_size = sizeof(NET_SOCK_ADDR);
  (void)NetSock_TxDataTo(p_conn->SockID,
                         &p_conn->TxPktBuf[0],
                         p_conn->TxPktLen,
                         NET_SOCK_FLAG_NONE,
                         &p_conn->SockAddr,
                         sock_addr_size,
                         p_err);
}

/****************************************************************************************************//**
 *                                               TFTPc_TxAck()
 *
 * @brief    Transmit TFTP acknowledge packet.
 *
 * @param    p_conn      Pointer to TFTP connection object.
 *
 * @param    blk_nbr     Block number to acknowledge.
 *
 * @param    p_err       Pointer to variable that will receive the return error code from this function.
 *******************************************************************************************************/
static void TFTPc_TxAck(TFTPc_CONN_OBJ *p_conn,
                        TFTPc_BLK_NBR  blk_nbr,
                        RTOS_ERR       *p_err)
{
  NET_SOCK_ADDR_LEN sock_addr_size;

  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_OPCODE],
                          TFTP_OPCODE_ACK);

  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_BLK_NBR],
                          blk_nbr);

  p_conn->TxPktLen = TFTP_PKT_SIZE_OPCODE + TFTP_PKT_SIZE_BLK_NBR;

  //                                                               --------------------- TX PKT ----------------------
  sock_addr_size = sizeof(NET_SOCK_ADDR);
  (void)NetSock_TxDataTo(p_conn->SockID,
                         &p_conn->TxPktBuf[0],
                         p_conn->TxPktLen,
                         NET_SOCK_FLAG_NONE,
                         &p_conn->SockAddr,
                         sock_addr_size,
                         p_err);
}

/****************************************************************************************************//**
 *                                               TFTPc_TxErr()
 *
 * @brief    Transmit TFTP error packet.
 *
 * @param    p_conn      Pointer to TFTP connection object.
 *
 * @param    err_code    Code indicating the nature of the error.
 *
 * @param    p_err_msg   String associated with error (terminated by NULL character).
 *
 * @param    p_err       _msg   String associated with error (terminated by NULL character).
 *******************************************************************************************************/
static void TFTPc_TxErr(TFTPc_CONN_OBJ *p_conn,
                        CPU_INT16U     err_code,
                        CPU_CHAR       *p_err_msg,
                        RTOS_ERR       *p_err)
{
  CPU_INT16U        err_msg_len;
  NET_SOCK_ADDR_LEN sock_addr_size;

  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_OPCODE],
                          TFTP_OPCODE_ERR);

  NET_UTIL_VAL_SET_NET_16(&p_conn->TxPktBuf[TFTP_PKT_OFFSET_ERR_CODE],
                          err_code);

  //                                                               Copy err msg into tx pkt.
  if (p_err_msg != DEF_NULL) {
    Str_Copy((CPU_CHAR *)&p_conn->TxPktBuf[TFTP_PKT_OFFSET_ERR_MSG],
             (CPU_CHAR *) p_err_msg);

    err_msg_len = Str_Len(p_err_msg);
  } else {
    p_conn->TxPktBuf[TFTP_PKT_OFFSET_ERR_MSG] = (CPU_CHAR)0;
    err_msg_len = 0;
  }

  p_conn->TxPktLen = TFTP_PKT_SIZE_OPCODE
                     + TFTP_PKT_SIZE_ERR_CODE
                     + err_msg_len
                     + TFTP_PKT_SIZE_NULL;

  //                                                               --------------------- TX PKT ----------------------
  sock_addr_size = sizeof(NET_SOCK_ADDR);
  (void)NetSock_TxDataTo(p_conn->SockID,
                         &p_conn->TxPktBuf[0],
                         p_conn->TxPktLen,
                         NET_SOCK_FLAG_NONE,
                         &p_conn->SockAddr,
                         sock_addr_size,
                         p_err);
}

/****************************************************************************************************//**
 *                                               TFTPc_Terminate()
 *
 * @brief    (1) Terminate the current file transfer process.
 *
 *           - (a) Set TFTP client state to 'COMPLETE'
 *           - (b) Close opened file.
 *
 * @param    p_conn  Pointer to TFTP connection object.
 *******************************************************************************************************/
static void TFTPc_Terminate(TFTPc_CONN_OBJ *p_conn)
{
  RTOS_ERR local_err;

  if (p_conn->SockID != NET_SOCK_ID_NONE) {                       // Close sock.
    NetSock_Close(p_conn->SockID, &local_err);
    p_conn->SockID = NET_SOCK_ID_NONE;
  }

  if (p_conn->FileHandle != DEF_NULL) {                           // Close file.
    TFTPc_FS_API_Ptr->Close(p_conn->FileHandle);
    p_conn->FileHandle = DEF_NULL;
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_TFTP_CLIENT_AVAIL
