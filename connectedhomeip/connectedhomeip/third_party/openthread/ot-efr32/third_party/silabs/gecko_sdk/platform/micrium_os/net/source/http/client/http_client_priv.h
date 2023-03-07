/***************************************************************************//**
 * @file
 * @brief Network - HTTP Client
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

#ifndef  _HTTP_CLIENT_PRIV_H_
#define  _HTTP_CLIENT_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../http_priv.h"
#include  "../http_dict_priv.h"

#include  <net/include/http_client.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_type.h>

#include  <cpu/include/cpu.h>
#include  <common/include/rtos_types.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_utils.h>
#include  <common/source/kal/kal_priv.h>

#include  <http_client_cfg.h>
#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                          CONFIGURATION ERRORS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                        HTTPc CONFIGURATION ERRORS
 *******************************************************************************************************/

#ifndef  HTTPc_CFG_MODE_ASYNC_TASK_EN
   #error  "HTTPc_CFG_MODE_ASYNC_TASK_EN not #define'd in 'http-c_cfg.h'"

#elif  ((HTTPc_CFG_MODE_ASYNC_TASK_EN != DEF_ENABLED) \
  && (HTTPc_CFG_MODE_ASYNC_TASK_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_MODE_ASYNC_TASK_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_MODE_BLOCK_EN
   #error  "HTTPc_CFG_MODE_BLOCK_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_MODE_BLOCK_EN != DEF_ENABLED) \
  && (HTTPc_CFG_MODE_BLOCK_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_MODE_BLOCK_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_PERSISTENT_EN
   #error  "HTTPc_CFG_PERSISTENT_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_PERSISTENT_EN != DEF_ENABLED) \
  && (HTTPc_CFG_PERSISTENT_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_PERSISTENT_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_CHUNK_TX_EN
   #error  "HTTPc_CFG_CHUNK_TX_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_CHUNK_TX_EN != DEF_ENABLED) \
  && (HTTPc_CFG_CHUNK_TX_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_CHUNK_TX_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"

#endif

#ifndef  HTTPc_CFG_QUERY_STR_EN
   #error  "HTTPc_CFG_QUERY_STR_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_QUERY_STR_EN != DEF_ENABLED) \
  && (HTTPc_CFG_QUERY_STR_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_QUERY_STR_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_HDR_RX_EN
   #error  "HTTPc_CFG_HDR_RX_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_HDR_RX_EN != DEF_ENABLED) \
  && (HTTPc_CFG_HDR_RX_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_HDR_RX_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_HDR_TX_EN
   #error  "HTTPc_CFG_HDR_TX_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_HDR_TX_EN != DEF_ENABLED) \
  && (HTTPc_CFG_HDR_TX_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_HDR_TX_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_FORM_EN
   #error  "HTTPc_CFG_FORM_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_FORM_EN != DEF_ENABLED) \
  && (HTTPc_CFG_FORM_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_FORM_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_USER_DATA_EN
   #error  "HTTPc_CFG_USER_DATA_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_USER_DATA_EN != DEF_ENABLED) \
  && (HTTPc_CFG_USER_DATA_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_USER_DATA_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPc_CFG_WEBSOCKET_EN
   #error  "HTTPc_CFG_WEBSOCKET_EN not #define'd in 'http-c_cfg.h'"
#elif  ((HTTPc_CFG_WEBSOCKET_EN != DEF_ENABLED) \
  && (HTTPc_CFG_WEBSOCKET_EN != DEF_DISABLED))
   #error  "HTTPc_CFG_WEBSOCKET_EN illegally #define'd in 'http-c_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#if ((HTTPc_CFG_WEBSOCKET_EN == DEF_ENABLED) \
  && (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_DISABLED))
   #error  "HTTPc_CFG_WEBSOCKET_EN cannot be DEF_ENABLED if HTTPc_CFG_MODE_ASYNC_TASK_EN is DEF_DISABLED."
#endif

#if ((HTTPc_CFG_WEBSOCKET_EN == DEF_ENABLED) \
  && (HTTPc_CFG_USER_DATA_EN == DEF_DISABLED))
   #error  "HTTPc_CFG_WEBSOCKET_EN cannot be DEF_ENABLED if HTTPc_CFG_USER_DATA_EN is DEF_DISABLED."
#endif

/********************************************************************************************************
 *                                    NETWORK CONFIGURATION ERRORS
 *******************************************************************************************************/

#if     (NET_SOCK_CFG_SEL_EN != DEF_ENABLED)
#error  "NET_SOCK_CFG_SEL_EN               illegally #define'd in 'net_cfg.h'. MUST be DEF_ENABLED."
#endif

#if     (NET_TCP_CFG_EN != DEF_ENABLED)
#error  "NET_TCP_CFG_EN: illegally #define'd in 'net_cfg.h'. MUST be DEF_ENABLED"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  HTTPc_WEBSOCK_TX_MSG_LEN_NOT_DEFINED         DEF_INT_32U_MAX_VAL

/********************************************************************************************************
 *                                       REQUEST STRING DEFINES
 *******************************************************************************************************/

#define  HTTPc_STR_BOUNDARY                  "rgifovj80325n"

#define  HTTPc_STR_BOUNDARY_LEN              (sizeof(HTTPc_STR_BOUNDARY) - 1)

#define  HTTPc_STR_BOUNDARY_START            "--" HTTPc_STR_BOUNDARY

#define  HTTPc_STR_BOUNDARY_END              "--" HTTPc_STR_BOUNDARY "--"

#define  HTTPc_STR_BOUNDARY_START_LEN        (sizeof(HTTPc_STR_BOUNDARY_START) - 1)

#define  HTTPc_STR_BOUNDARY_END_LEN          (sizeof(HTTPc_STR_BOUNDARY_END) - 1)

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                   HTTP CLIENT FLAGS DATA TYPE
 *******************************************************************************************************/

//                                                                 SOCKET FLAGS
#define  HTTPc_FLAG_SOCK_RDY_RD                             DEF_BIT_00
#define  HTTPc_FLAG_SOCK_RDY_WR                             DEF_BIT_01
#define  HTTPc_FLAG_SOCK_RDY_ERR                            DEF_BIT_02

//                                                                 CONNECTION FLAGS
#define  HTTPc_FLAG_CONN_CONNECT                            DEF_BIT_00      // bits 0-3 internal usage.
#define  HTTPc_FLAG_CONN_IN_USE                             DEF_BIT_01
#define  HTTPc_FLAG_CONN_TO_CLOSE                           DEF_BIT_02
#if 0
#define  HTTPc_FLAG_CONN_PERSISTENT                         DEF_BIT_04      // bits 4-7 configurable.
#define  HTTPc_FLAG_CONN_NO_BLOCK                           DEF_BIT_05
#define  HTTPc_FLAG_CONN_WEBSOCKET                          DEF_BIT_06
#endif

//                                                                 REQUEST FLAGS
#define  HTTPc_FLAG_REQ_LINE_QUERY_STR_BEGIN                DEF_BIT_00
#define  HTTPc_FLAG_REQ_LINE_QUERY_STR_DONE                 DEF_BIT_01
#define  HTTPc_FLAG_REQ_HDR_DONE                            DEF_BIT_02
#define  HTTPc_FLAG_REQ_BODY_CHUNK_LAST                     DEF_BIT_03

//                                                                 RESPONSE FLAGS
#define  HTTPc_FLAG_RESP_RX_MORE_DATA                       DEF_BIT_00
#define  HTTPc_FLAG_RESP_BODY_CHUNK_TRANSFER                DEF_BIT_01
#define  HTTPc_FLAG_RESP_COMPLETE_OK                        DEF_BIT_02

//                                                                 WEBSOCK SEND FLAGS
#define  HTTPc_FLAG_WEBSOCK_NO_BLOCK                        HTTPc_FLAG_REQ_NO_BLOCK

/********************************************************************************************************
 *                                    CONNECTION STATES DATA TYPE
 *******************************************************************************************************/

#define  HTTPc_CONN_STATE_FAMILY_MASK                           DEF_BIT_MASK_08(7, 5)
#define  HTTPc_CONN_STATE_FLOW_FAMILY                           DEF_BIT_MASK_08(0, 5)
#define  HTTPc_CONN_STATE_REQ_FAMILY                            DEF_BIT_MASK_08(1, 5)
#define  HTTPc_CONN_STATE_RESP_FAMILY                           DEF_BIT_MASK_08(2, 5)
#define  HTTPc_CONN_STATE_WEBSOCK_FAMILY                        DEF_BIT_MASK_08(3, 5)

/********************************************************************************************************
 *                                     HTTP CLIENT INTERNAL STATUS
 *******************************************************************************************************/

typedef  enum  httpc_status {
  HTTPc_STATUS_NONE,
  HTTPc_STATUS_NO_MORE_SPACE,    // Buffer is full, need to transmit data to free space.
  HTTPc_STATUS_NEED_MORE_DATA,   // Buffer is empty, need to rx more data to continue process.
  HTTPc_STATUS_ERR               // An error occurred during processing. See error code.
} HTTPc_STATUS;

/********************************************************************************************************
 *                                   HTTP CLIENT TASK MSG DATA TYPES
 *******************************************************************************************************/

typedef enum {
  HTTPc_MSG_TYPE_CONN_OPEN,
  HTTPc_MSG_TYPE_CONN_CLOSE,
  HTTPc_MSG_TYPE_REQ,
  HTTPc_MSG_TYPE_WEBSOCK_MSG,
} HTTPc_MSG_TYPE;

typedef struct httpc_task_msg {
  HTTPc_MSG_TYPE Type;
  void           *DataPtr;
} HTTPc_TASK_MSG;

/********************************************************************************************************
 *                                        HTTP CLIENT DATA TYPE
 *******************************************************************************************************/

typedef  struct  httpc_data {
  MEM_DYN_POOL SemHandlePool;
  MEM_DYN_POOL MsgPool;

  CPU_INT16U   CfgConnConnectTimeout_ms;
  CPU_INT16U   CfgConnInactivityTimeout_s;

#ifdef  HTTPc_WEBSOCK_MODULE_EN
  MEM_DYN_POOL WebSockReqPool;
#endif

#ifdef  HTTPc_TASK_MODULE_EN
  CPU_INT08U      TaskDly_ms;
  HTTPc_CONN      *TaskConnFirstPtr;
  KAL_Q_HANDLE    TaskMsgQ_Handle;      // TODO_NET remove KAL Q
  KAL_TASK_HANDLE TaskHandle;
#endif
} HTTPc_DATA;

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            MEMORY MODULE
 *******************************************************************************************************/

HTTPc_TASK_MSG *HTTPc_Mem_TaskMsgGet(RTOS_ERR *p_err);

void HTTPc_Mem_TaskMsgRelease(HTTPc_TASK_MSG *p_msg);

#ifdef  HTTPc_WEBSOCK_MODULE_EN
HTTPc_WEBSOCK_REQ *HTTPc_Mem_WebSockReqGet(RTOS_ERR *p_err);

void HTTPc_Mem_WebSockReqRelease(HTTPc_WEBSOCK_REQ *p_ws_req);
#endif

/********************************************************************************************************
 *                                          CONNECTION MODULE
 *******************************************************************************************************/

void HTTPcConn_Process(HTTPc_CONN *p_conn);

void HTTPcConn_Connect(HTTPc_CONN *p_conn,
                       RTOS_ERR   *p_err);

void HTTPcConn_Close(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err);

void HTTPcConn_TransProcess(HTTPc_CONN *p_conn);

void HTTPcConn_TransParamReset(HTTPc_CONN *p_conn);

CPU_BOOLEAN HTTPcConn_TransComplete(HTTPc_CONN *p_conn);

void HTTPcConn_Add(HTTPc_CONN *p_conn);

void HTTPcConn_Remove(HTTPc_CONN *p_conn);

void HTTPcConn_ReqAdd(HTTPc_REQ *p_req);

void HTTPcConn_ReqRemove(HTTPc_CONN *p_conn);

/********************************************************************************************************
 *                                           REQUEST MODULE
 *******************************************************************************************************/

void HTTPcReq_Prepare(HTTPc_CONN *p_conn,
                      RTOS_ERR   *p_err);

CPU_BOOLEAN HTTPcReq(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err);

CPU_CHAR *HTTPcReq_HdrCopyToBuf(CPU_CHAR       *p_buf,                      // TODO_NET put function in HTTP common files ?
                                CPU_INT16U     buf_len,
                                CPU_SIZE_T     buf_len_rem,
                                HTTP_HDR_FIELD hdr_type,
                                const CPU_CHAR *p_val,
                                CPU_SIZE_T     val_len,
                                CPU_BOOLEAN    add_CRLF,
                                HTTPc_STATUS   *p_status,
                                RTOS_ERR       *p_err);

/********************************************************************************************************
 *                                           RESPONSE MODULE
 *******************************************************************************************************/

CPU_BOOLEAN HTTPcResp(HTTPc_CONN *p_conn,
                      RTOS_ERR   *p_err);

/********************************************************************************************************
 *                                            SOCKET MODULE
 *******************************************************************************************************/

void HTTPcSock_Connect(HTTPc_CONN *p_conn,
                       RTOS_ERR   *p_err);

CPU_BOOLEAN HTTPcSock_ConnDataRx(HTTPc_CONN *p_conn,
                                 CPU_INT32U max_len,
                                 RTOS_ERR   *p_err);

CPU_BOOLEAN HTTPcSock_ConnDataTx(HTTPc_CONN *p_conn,
                                 RTOS_ERR   *p_err);

void HTTPcSock_Close(HTTPc_CONN *p_conn,
                     RTOS_ERR   *p_err);

void HTTPcSock_Sel(HTTPc_CONN *p_conn,
                   RTOS_ERR   *p_err);

CPU_BOOLEAN HTTPcSock_IsRxClosed(HTTPc_CONN *p_conn,
                                 RTOS_ERR   *p_err);

/********************************************************************************************************
 *                                             TASK MODULE
 *******************************************************************************************************/

#ifdef   HTTPc_TASK_MODULE_EN

void HTTPcTask(void *p_data);

void HTTPcTask_SetDly(CPU_INT08U dly_ms);

void HTTPcTask_MsgQueue(HTTPc_MSG_TYPE type,
                        void           *p_data,
                        RTOS_ERR       *p_err);

void HTTPcTask_ConnAdd(HTTPc_CONN *p_conn);

void HTTPcTask_ConnRemove(HTTPc_CONN *p_conn);

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
void HTTPcTask_ConnConnectSignalCreate(HTTPc_CONN *p_conn,
                                       RTOS_ERR   *p_err);

void HTTPcTask_ConnConnectSignalDel(HTTPc_CONN *p_conn);

void HTTPcTask_ConnCloseSignalCreate(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err);

void HTTPcTask_ConnCloseSignalDel(HTTPc_CONN *p_conn);

void HTTPcTask_TransDoneSignalCreate(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err);

void HTTPcTask_TransDoneSignalDel(HTTPc_CONN *p_conn);

void HTTPcTask_ConnConnectSignal(HTTPc_CONN *p_conn);

void HTTPcTask_ConnCloseSignal(HTTPc_CONN *p_conn);

void HTTPcTask_TransDoneSignal(HTTPc_CONN *p_conn);

void HTTPcTask_ConnConnectSignalWait(HTTPc_CONN *p_conn,
                                     RTOS_ERR   *p_err);

void HTTPcTask_ConnCloseSignalWait(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err);

void HTTPcTask_TransDoneSignalWait(HTTPc_CONN *p_conn,
                                   RTOS_ERR   *p_err);
#endif

void HTTPcTask_Wake(HTTPc_CONN *p_conn,
                    RTOS_ERR   *p_err);

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                             MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_CLIENT_PRIV_H_
