/***************************************************************************//**
 * @file
 * @brief Network - Http Client Module
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
 * @defgroup NET_HTTPc HTTP Client API
 * @ingroup  NET
 * @brief    HTTP Client API
 *
 * @addtogroup NET_HTTPc
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _HTTP_CLIENT_H_
#define  _HTTP_CLIENT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <http_client_cfg.h>

#include  <net/include/http.h>
#include  <net/include/net_sock.h>
#include  <net/include/net_type.h>
#include  <net/include/net_app.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_mem.h>
#include  <common/include/rtos_types.h>
#include  <common/include/rtos_err.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                                DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                            MODULE DEFINES
 *******************************************************************************************************/

#if (HTTPc_CFG_MODE_ASYNC_TASK_EN == DEF_ENABLED)

    #define  HTTPc_TASK_MODULE_EN

    #if  (HTTPc_CFG_MODE_BLOCK_EN == DEF_ENABLED)
        #define  HTTPc_SIGNAL_TASK_MODULE_EN
    #endif

    #if  (HTTPc_CFG_WEBSOCKET_EN == DEF_ENABLED)
        #define  HTTPc_WEBSOCK_MODULE_EN
    #endif

#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                              DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                 ADVANCE TASK CONFIGURATION DATA TYPE
 * @brief Task configuration structure
 *******************************************************************************************************/

typedef  struct  httpc_qty_cfg {
  CPU_SIZE_T MsgQ_Size;                                         ///< Message Queue Size.
} HTTPc_QTY_CFG;

/********************************************************************************************************
 *                                  CONNECTION CONFIGURATION DATA TYPE
 * @brief Connection configuration structure
 *******************************************************************************************************/

typedef  struct  httpc_conn_cfg {
  CPU_INT16U ConnConnectTimeout_ms;                             ///< Connection Connect Timeout.
  CPU_INT16U ConnInactivityTimeout_s;                           ///< Connection Inactivity Timeout.
} HTTPc_CONN_CFG;

/********************************************************************************************************
 *                                     INIT CONFIGURATION DATA TYPE
 * @brief Init configuration structure
 *******************************************************************************************************/

typedef  struct  httpc_init_cfg {
  HTTPc_CONN_CFG ConnCfg;

  HTTPc_QTY_CFG  QtyCfg;
  CPU_INT08U     TaskDly_ms;                                    ///< Task Delay in milliseconds.

#ifdef  HTTPc_TASK_MODULE_EN
  CPU_STK_SIZE   StkSizeElements;                               ///< Size of the stk, in CPU_STK elements.
  void           *StkPtr;                                       ///< Ptr to base of the stk.
#endif

  MEM_SEG        *MemSegPtr;                                    ///< Ptr to clk mem seg.
} HTTPc_INIT_CFG;

/********************************************************************************************************
 *                                      HTTP CLIENT OBJECTS DATA TYPE
 *******************************************************************************************************/

typedef  struct  httpc_conn_obj HTTPc_CONN_OBJ;

typedef  struct  httpc_req_obj HTTPc_REQ_OBJ;

typedef  struct  httpc_resp HTTPc_RESP_OBJ;

typedef  struct  httpc_websock_obj HTTPc_WEBSOCK_OBJ;

typedef  struct  httpc_websock_msg_obj HTTPc_WEBSOCK_MSG_OBJ;

typedef  struct  httpc_conn HTTPc_CONN;

typedef  struct  httpc_req HTTPc_REQ;

typedef  struct  httpc_resp HTTPc_RESP;

typedef  struct  httpc_websock HTTPc_WEBSOCK;

typedef  struct  httpc_websock_req HTTPc_WEBSOCK_REQ;

typedef  struct  httpc_websock_msg HTTPc_WEBSOCK_MSG;

/********************************************************************************************************
 *                                     API PARAMETERS DATA TYPE
 * @brief API Paramaters enumeration
 *******************************************************************************************************/

typedef  enum  httpc_param_type {
  HTTPc_PARAM_TYPE_SERVER_PORT,
  HTTPc_PARAM_TYPE_PERSISTENT,
  HTTPc_PARAM_TYPE_CONNECT_TIMEOUT,
  HTTPc_PARAM_TYPE_INACTIVITY_TIMEOUT,
  HTTPc_PARAM_TYPE_SECURE_CFG_PTR,
  HTTPc_PARAM_TYPE_USER_DATA,

  HTTPc_PARAM_TYPE_REQ_QUERY_STR_TBL,
  HTTPc_PARAM_TYPE_REQ_QUERY_STR_HOOK,
  HTTPc_PARAM_TYPE_REQ_HDR_TBL,
  HTTPc_PARAM_TYPE_REQ_UPGRADE_WEBSOCKET,
  HTTPc_PARAM_TYPE_REQ_HDR_HOOK,
  HTTPc_PARAM_TYPE_REQ_FORM_TBL,
  HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_TYPE,
  HTTPc_PARAM_TYPE_REQ_BODY_CONTENT_LEN,
  HTTPc_PARAM_TYPE_REQ_BODY_CHUNK,
  HTTPc_PARAM_TYPE_REQ_BODY_HOOK,
  HTTPc_PARAM_TYPE_REQ_USER_DATA,

  HTTPc_PARAM_TYPE_RESP_HDR_HOOK,
  HTTPc_PARAM_TYPE_RESP_BODY_HOOK,

  HTTPc_PARAM_TYPE_TRANS_COMPLETE_CALLBACK,
  HTTPc_PARAM_TYPE_TRANS_ERR_CALLBACK,

  HTTPc_PARAM_TYPE_CONN_CONNECT_CALLBACK,
  HTTPc_PARAM_TYPE_CONN_CLOSE_CALLBACK,

  HTTPc_PARAM_TYPE_WEBSOCK_ON_OPEN,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_CLOSE,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_ERR,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_PONG,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_INIT,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_DATA,
  HTTPc_PARAM_TYPE_WEBSOCK_ON_MSG_RX_COMPLETE,
  HTTPc_PARAM_TYPE_WEBSOCK_USER_DATA,

  HTTPc_PARAM_TYPE_WEBSOCK_MSG_USER_DATA,
  HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_INIT,
  HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_DATA,
  HTTPc_PARAM_TYPE_WEBSOCK_MSG_ON_TX_COMPLETE,
} HTTPc_PARAM_TYPE;

/********************************************************************************************************
 *                                     TABLE PARAMETER DATA TYPE
 * @brief Table parameter structure
 *******************************************************************************************************/

typedef  struct  httpc_param_tbl {
  void       *TblPtr;
  CPU_INT16U EntryNbr;
} HTTPc_PARAM_TBL;

/********************************************************************************************************
 *                                   HTTP CLIENT FLAGS DATA TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U HTTPc_FLAGS;

#define  HTTPc_FLAG_NONE                                    DEF_BIT_NONE

//                                                                             CONNECTION FLAGS
#if 0
#define  HTTPc_FLAG_CONN_CONNECT                            DEF_BIT_00      // bits 0-3 internal usage.
#define  HTTPc_FLAG_CONN_IN_USE                             DEF_BIT_01
#define  HTTPc_FLAG_CONN_TO_CLOSE                           DEF_BIT_02
#endif
#define  HTTPc_FLAG_CONN_PERSISTENT                         DEF_BIT_04      // bits 4-7 configurable.
#define  HTTPc_FLAG_CONN_NO_BLOCK                           DEF_BIT_05
#define  HTTPc_FLAG_CONN_WEBSOCKET                          DEF_BIT_06

//                                                                             REQUEST CFG FLAGS
#define  HTTPc_FLAG_REQ_NO_BLOCK                            DEF_BIT_00
#define  HTTPc_FLAG_REQ_BODY_PRESENT                        DEF_BIT_01
#define  HTTPc_FLAG_REQ_FORM_PRESENT                        DEF_BIT_02
#define  HTTPc_FLAG_REQ_BODY_CHUNK_TRANSFER                 DEF_BIT_03
#define  HTTPc_FLAG_REQ_IN_USE                              DEF_BIT_04
#define  HTTPc_FLAG_REQ_UPGRADE_WEBSOCKET                   DEF_BIT_05

//                                                                             REQUEST CFG HDR FLAGS
#define  HTTPc_FLAG_REQ_HDR_HOST_ADD                        DEF_BIT_00
#define  HTTPc_FLAG_REQ_HDR_CONTENT_TYPE_ADD                DEF_BIT_01
#define  HTTPc_FLAG_REQ_HDR_TRANSFER_ENCODE_ADD             DEF_BIT_02
#define  HTTPc_FLAG_REQ_HDR_CONTENT_LENGTH_ADD              DEF_BIT_03
#define  HTTPc_FLAG_REQ_HDR_CONNECTION_ADD                  DEF_BIT_04
#define  HTTPc_FLAG_REQ_HDR_UPGRADE_ADD                     DEF_BIT_05
#define  HTTPc_FLAG_REQ_HDR_WEBSOCKET_ADD                   DEF_BIT_06

/********************************************************************************************************
 *                                  CONNECTION CLOSE STATUS DATA TYPE
 * @brief Connection close status structure
 *******************************************************************************************************/

typedef enum httpc_conn_close_status {
  HTTPc_CONN_CLOSE_STATUS_NONE,
  HTTPc_CONN_CLOSE_STATUS_ERR_INTERNAL,
  HTTPc_CONN_CLOSE_STATUS_SERVER,
  HTTPc_CONN_CLOSE_STATUS_NO_PERSISTENT,
  HTTPc_CONN_CLOSE_STATUS_APP
} HTTPc_CONN_CLOSE_STATUS;

/********************************************************************************************************
 *                                    HTTP CLIENT HDR FIELD DATA TYPE
 * @brief HTTP client HDR field structure
 *******************************************************************************************************/

typedef  struct  httpc_hdr {
  HTTP_HDR_FIELD HdrField;
  CPU_CHAR       *ValPtr;
  CPU_INT16U     ValLen;
} HTTPc_HDR;

/********************************************************************************************************
 *                                      HTTP CLIENT FORM DATA TYPE
 * @brief HTTP client form field structure
 *******************************************************************************************************/

typedef  enum  httpc_form_field_type {
  HTTPc_FORM_FIELD_TYPE_KEY_VAL,
  HTTPc_FORM_FIELD_TYPE_KEY_VAL_EXT,
  HTTPc_FORM_FIELD_TYPE_FILE
} HTTPc_FORM_FIELD_TYPE;

/********************************************************************************************************
 *                                      HTTP CLIENT FORM TABLE DATA TYPE
 * @brief HTTP client form table field structure
 *******************************************************************************************************/
typedef  struct  httpc_form_tbl_field {
  HTTPc_FORM_FIELD_TYPE Type;
  void                  *FieldObjPtr;
} HTTPc_FORM_TBL_FIELD;

/********************************************************************************************************
 *                                        KEY-VALUE PAIR DATA TYPE
 * @brief HTTP client key-value pair structure
 *******************************************************************************************************/

typedef  struct  httpc_key_val {
  CPU_CHAR   *KeyPtr;
  CPU_INT16U KeyLen;
  CPU_CHAR   *ValPtr;
  CPU_INT16U ValLen;
} HTTPc_KEY_VAL;

/********************************************************************************************************
 *                                      BIG KEY-VALUE PAIR DATA TYPE
 *******************************************************************************************************/

typedef  struct  httpc_key_val_ext HTTPc_KEY_VAL_EXT;

typedef  CPU_BOOLEAN (*HTTPc_KEY_VAL_EXT_HOOK) (HTTPc_CONN_OBJ    *p_conn,
                                                HTTPc_REQ_OBJ     *p_req,
                                                HTTPc_KEY_VAL_EXT *p_key_val_obj,
                                                CPU_CHAR          *p_buf,
                                                CPU_INT16U        buf_len,
                                                CPU_INT16U        *p_len_wr);

struct  httpc_key_val_ext {
  CPU_CHAR               *KeyPtr;
  CPU_INT16U             KeyLen;
  HTTPc_KEY_VAL_EXT_HOOK OnValTx;
  CPU_INT32U             ValLen;
};

/********************************************************************************************************
 *                                      HTTP MULTIPART FORM DATA TYPE
 *******************************************************************************************************/

typedef  struct  http_multipart_file HTTPc_MULTIPART_FILE;

typedef  CPU_BOOLEAN (*HTTPc_MULTIPART_FILE_HOOK) (HTTPc_CONN_OBJ       *p_conn,
                                                   HTTPc_REQ_OBJ        *p_req,
                                                   HTTPc_MULTIPART_FILE *p_file_obj,
                                                   CPU_CHAR             *p_buf,
                                                   CPU_INT16U           buf_len,
                                                   CPU_INT16U           *p_len_wr);

struct  http_multipart_file {
  CPU_CHAR                  *NamePtr;
  CPU_INT16U                NameLen;
  CPU_CHAR                  *FileNamePtr;
  CPU_INT16U                FileNameLen;
  CPU_INT32U                FileLen;
  HTTP_CONTENT_TYPE         ContentType;
  HTTPc_MULTIPART_FILE_HOOK OnFileTx;
};

/********************************************************************************************************
 *                                  HOOK & CALLBACK FUNCTIONS DATA TYPE
 *******************************************************************************************************/

typedef  void (*HTTPc_CONNECT_CALLBACK)      (HTTPc_CONN_OBJ *p_conn,
                                              CPU_BOOLEAN    open_status);

typedef  void (*HTTPc_CONN_CLOSE_CALLBACK)   (HTTPc_CONN_OBJ          *p_conn,
                                              HTTPc_CONN_CLOSE_STATUS close_status,
                                              RTOS_ERR                err);

typedef  void (*HTTPc_CONN_ERR_CALLBACK)     (HTTPc_CONN_OBJ *p_conn,
                                              RTOS_ERR       err);

typedef  CPU_BOOLEAN (*HTTPc_REQ_QUERY_STR_HOOK)    (HTTPc_CONN_OBJ *p_conn,
                                                     HTTPc_REQ_OBJ  *p_req,
                                                     HTTPc_KEY_VAL  **p_key_val);

typedef  CPU_BOOLEAN (*HTTPc_REQ_HDR_HOOK)          (HTTPc_CONN_OBJ *p_conn,
                                                     HTTPc_REQ_OBJ  *p_req,
                                                     HTTPc_HDR      **p_hdr);

typedef  CPU_BOOLEAN (*HTTPc_REQ_BODY_HOOK)         (HTTPc_CONN_OBJ *p_conn,
                                                     HTTPc_REQ_OBJ  *p_req,
                                                     void           **p_data,
                                                     CPU_CHAR       *p_buf,
                                                     CPU_INT16U     buf_len,
                                                     CPU_INT16U     *p_data_len);

typedef  void (*HTTPc_RESP_HDR_HOOK)         (HTTPc_CONN_OBJ *p_conn,
                                              HTTPc_REQ_OBJ  *p_req,
                                              HTTP_HDR_FIELD hdr_field,
                                              CPU_CHAR       *p_hdr_val,
                                              CPU_INT16U     val_len);

typedef  CPU_INT32U (*HTTPc_RESP_BODY_HOOK)        (HTTPc_CONN_OBJ    *p_conn,
                                                    HTTPc_REQ_OBJ     *p_req,
                                                    HTTP_CONTENT_TYPE content_type,
                                                    void              *p_data,
                                                    CPU_INT16U        data_len,
                                                    CPU_BOOLEAN       last_chunk);

typedef  void (*HTTPc_COMPLETE_CALLBACK)     (HTTPc_CONN_OBJ *p_conn,
                                              HTTPc_REQ_OBJ  *p_req,
                                              HTTPc_RESP_OBJ *p_resp,
                                              CPU_BOOLEAN    status);

typedef  void (*HTTPc_TRANS_ERR_CALLBACK)    (HTTPc_CONN_OBJ *p_conn,
                                              HTTPc_REQ_OBJ  *p_req,
                                              RTOS_ERR       err);

/********************************************************************************************************
 *                                      HTTPc RESPONSE DATA TYPE
 * @brief HTTP client response structure
 *******************************************************************************************************/

struct httpc_resp {
  HTTP_PROTOCOL_VER ProtocolVer;                                ///< HTTP version received in response message.
  HTTP_STATUS_CODE  StatusCode;                                 ///< Status code received in response.
  const CPU_CHAR    *ReasonPhrasePtr;                           ///< Pointer to received reason phrase.
  HTTP_CONTENT_TYPE ContentType;                                ///< Content type received in response.
  CPU_INT32U        ContentLen;                                 ///< Content length received in response if any.
};

/********************************************************************************************************
 *                                         WEBSOCKET DATA TYPE
 *******************************************************************************************************/

#ifdef  HTTPc_WEBSOCK_MODULE_EN
//                                                                 ----------- WEBSOCK MESSAGE TYPE OPCODE ------------
typedef  CPU_INT08U HTTPc_WEBSOCK_OPCODE;

#define  HTTPc_WEBSOCK_OPCODE_CONTINUATION_FRAME         0x0
#define  HTTPc_WEBSOCK_OPCODE_TXT_FRAME                  0x1
#define  HTTPc_WEBSOCK_OPCODE_BIN_FRAME                  0x2
#define  HTTPc_WEBSOCK_OPCODE_CLOSE                      0x8
#define  HTTPc_WEBSOCK_OPCODE_PING                       0x9
#define  HTTPc_WEBSOCK_OPCODE_PONG                       0xA
#define  HTTPc_WEBSOCK_OPCODE_NONE                       0xFF   // Not defined by the RFC 6455, but for internal use.

//                                                                 ---------------- WEBSOCK CLOSE CODE ----------------
typedef  CPU_INT16U HTTPc_WEBSOCK_CLOSE_CODE;

#define  HTTPc_WEBSOCK_CLOSE_CODE_NONE                   0u     // Not defined by the RFC 6455, but for internal use.

#define  HTTPc_WEBSOCK_CLOSE_CODE_NORMAL                 1000u
#define  HTTPc_WEBSOCK_CLOSE_CODE_GOING_AWAY             1001u
#define  HTTPc_WEBSOCK_CLOSE_CODE_PROTOCOL_ERR           1002u
#define  HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_NOT_ALLOWED  1003u
#define  HTTPc_WEBSOCK_CLOSE_CODE_DATA_TYPE_ERR          1007u
#define  HTTPc_WEBSOCK_CLOSE_CODE_POLICY_VIOLATION       1008u
#define  HTTPc_WEBSOCK_CLOSE_CODE_MSG_TOO_BIG            1009u
#define  HTTPc_WEBSOCK_CLOSE_CODE_INVALID_EXT            1010u
#define  HTTPc_WEBSOCK_CLOSE_CODE_UNEXPECTED_CONDITION   1011u

typedef enum {
  HTTPc_WEBSOCK_MSG_TYPE_TXT_FRAME = HTTPc_WEBSOCK_OPCODE_TXT_FRAME,
  HTTPc_WEBSOCK_MSG_TYPE_BIN_FRAME = HTTPc_WEBSOCK_OPCODE_BIN_FRAME,
  HTTPc_WEBSOCK_MSG_TYPE_CLOSE = HTTPc_WEBSOCK_OPCODE_CLOSE,
  HTTPc_WEBSOCK_MSG_TYPE_PING = HTTPc_WEBSOCK_OPCODE_PING,
  HTTPc_WEBSOCK_MSG_TYPE_PONG = HTTPc_WEBSOCK_OPCODE_PONG,
} HTTPc_WEBSOCK_MSG_TYPE;

#endif

/********************************************************************************************************
 *                                    CONNECTION STATES DATA TYPE
 * @brief HTTP client connection state enumeration
 *******************************************************************************************************/

typedef enum httpc_conn_state {
  HTTPc_CONN_STATE_NONE = 0u,                                           // 0b000xxxxx
  HTTPc_CONN_STATE_UNKNOWN,
  HTTPc_CONN_STATE_PARAM_VALIDATE,
  HTTPc_CONN_STATE_CONNECT,
  HTTPc_CONN_STATE_COMPLETED,
  HTTPc_CONN_STATE_CLOSE,
  HTTPc_CONN_STATE_ERR,
  HTTPc_CONN_STATE_WEBSOCK_INIT,
  HTTPc_CONN_STATE_WEBSOCK_CLOSE,
  HTTPc_CONN_STATE_WEBSOCK_ERR,

  HTTPc_CONN_STATE_REQ_LINE_METHOD = 32u,                               // 0b001xxxxx
  HTTPc_CONN_STATE_REQ_LINE_URI,
  HTTPc_CONN_STATE_REQ_LINE_QUERY_STR,
  HTTPc_CONN_STATE_REQ_LINE_PROTO_VER,
  HTTPc_CONN_STATE_REQ_HDR_HOST,
  HTTPc_CONN_STATE_REQ_HDR_CONTENT_TYPE,
  HTTPc_CONN_STATE_REQ_HDR_CONTENT_LEN,
  HTTPc_CONN_STATE_REQ_HDR_TRANSFER_ENCODE,
  HTTPc_CONN_STATE_REQ_HDR_CONN,
  HTTPc_CONN_STATE_REQ_HDR_UPGRADE,
  HTTPc_CONN_STATE_REQ_HDR_WEBSOCKET,
  HTTPc_CONN_STATE_REQ_HDR_EXT,
  HTTPc_CONN_STATE_REQ_HDR_LAST,
  HTTPc_CONN_STATE_REQ_BODY,
  HTTPc_CONN_STATE_REQ_BODY_DATA,
  HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_SIZE,
  HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_DATA,
  HTTPc_CONN_STATE_REQ_BODY_DATA_CHUNK_END,
  HTTPc_CONN_STATE_REQ_BODY_FORM_APP,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_DISPO,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_HDR_CONTENT_TYPE,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_DATA_END,
  HTTPc_CONN_STATE_REQ_BODY_FORM_MULTIPART_BOUNDARY_END,
  HTTPc_CONN_STATE_REQ_END,

  HTTPc_CONN_STATE_RESP_INIT = 64u,                                     // 0b010xxxxx
  HTTPc_CONN_STATE_RESP_STATUS_LINE,
  HTTPc_CONN_STATE_RESP_HDR,
  HTTPc_CONN_STATE_RESP_BODY,
  HTTPc_CONN_STATE_RESP_BODY_CHUNK_SIZE,
  HTTPc_CONN_STATE_RESP_BODY_CHUNK_DATA,
  HTTPc_CONN_STATE_RESP_BODY_CHUNK_CRLF,
  HTTPc_CONN_STATE_RESP_BODY_CHUNK_LAST,
  HTTPc_CONN_STATE_RESP_COMPLETED,

  HTTPc_CONN_STATE_WEBSOCK_RXTX = 96u,                                 // 0b011xxxxx
} HTTPc_CONN_STATE;

/********************************************************************************************************
 ********************************************************************************************************
 *                                                MACROS
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                         STRUCT FIELD DEFINE MACRO
 *******************************************************************************************************/

#define  FIELD_DEF(prefix, type, name, suffix)    prefix type name##suffix;

/********************************************************************************************************
 *                                          USER DATA FIELD MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_USER_DATA_EN == DEF_ENABLED)
#define  USER_DATA_FIELD_DEF(prefix, suffix)  FIELD_DEF(prefix, void, *UserDataPtr, suffix)
#else
#define  USER_DATA_FIELD_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                     REQUEST QUERY STRING FIELDS MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
#define  REQ_QUERY_STR_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_KEY_VAL, *QueryStrTbl, suffix) \
  FIELD_DEF(prefix, CPU_INT16U, QueryStrNbr, suffix)                                                      \
  FIELD_DEF(prefix, HTTPc_REQ_QUERY_STR_HOOK, OnQueryStrTx, suffix)
#else
#define  REQ_QUERY_STR_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                     REQUEST HEADER TX FIELDS MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
#define  REQ_HDR_TX_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_HDR, *HdrTbl, suffix) \
  FIELD_DEF(prefix, CPU_INT16U, HdrNbr, suffix)                                               \
  FIELD_DEF(prefix, HTTPc_REQ_HDR_HOOK, OnHdrTx, suffix)
#else
#define  REQ_HDR_TX_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                        REQUEST FORM FIELDS MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
#define  REQ_FORM_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_FORM_TBL_FIELD, *FormFieldTbl, suffix) \
  FIELD_DEF(prefix, CPU_INT16U, FormFieldNbr, suffix)
#else
#define  REQ_FORM_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                     REQUEST HDR RX HOOK FIELD MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_HDR_RX_EN == DEF_ENABLED)
#define  REQ_HDR_RX_FIELD_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_RESP_HDR_HOOK, OnHdrRx, suffix)
#else
#define  REQ_HDR_RX_FIELD_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                    REQUEST TASK CALLBACK FIELDS MACRO
 *******************************************************************************************************/

#ifdef HTTPc_TASK_MODULE_EN
#define  REQ_TASK_CALLBACK_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_COMPLETE_CALLBACK, OnTransComplete, suffix) \
  FIELD_DEF(prefix, HTTPc_TRANS_ERR_CALLBACK, OnErr, suffix)
#else
#define  REQ_TASK_CALLBACK_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                         REQUEST STRUCTURE MACRO
 *******************************************************************************************************/

#define  STRUCT_REQ_INIT(prefix, suffix)                                                                                   \
  FIELD_DEF(prefix, HTTPc_FLAGS, Flags, suffix)                              /* Request set of flags.                   */ \
  FIELD_DEF(prefix, HTTPc_FLAGS, HdrFlags, suffix)                           /* Set of flags for hdr to include in Req. */ \
  FIELD_DEF(prefix, HTTP_METHOD, Method, suffix)                             /* HTTP Request Method.                    */ \
  FIELD_DEF(prefix, CPU_CHAR, *ResourcePathPtr, suffix)                      /* Pointer to URI or Resource path string. */ \
  FIELD_DEF(prefix, CPU_INT16U, ResourcePathLen, suffix)                     /* URI or Resource path string's length.   */ \
  FIELD_DEF(prefix, HTTP_CONTENT_TYPE, ContentType, suffix)                  /* Content Type of the Request body.       */ \
  FIELD_DEF(prefix, CPU_INT32U, ContentLen, suffix)                          /* Content Length of the Request body.     */ \
  FIELD_DEF(prefix, void, *DataPtr, suffix)                                  /* Pointer to data to put in Request body. */ \
  REQ_QUERY_STR_FIELDS_DEF(prefix, suffix)                                   /* Query String parameters.                */ \
  REQ_HDR_TX_FIELDS_DEF(prefix, suffix)                                      /* Request Header fields parameters.       */ \
  REQ_FORM_FIELDS_DEF(prefix, suffix)                                        /* Form parameters.                        */ \
  FIELD_DEF(prefix, HTTPc_REQ_BODY_HOOK, OnBodyTx, suffix)                   /* Body Transfer hook parameter.           */ \
  REQ_HDR_RX_FIELD_DEF(prefix, suffix)                                       /* Response Header fields parameter.       */ \
  FIELD_DEF(prefix, HTTPc_RESP_BODY_HOOK, OnBodyRx, suffix)                  /* Response body hook function.            */ \
  REQ_TASK_CALLBACK_FIELDS_DEF(prefix, suffix)                               /* Request's callback parameters.          */ \
  FIELD_DEF(prefix, HTTPc_CONN, *ConnPtr, suffix)                            /* Pointer to Connection object.           */ \
  FIELD_DEF(prefix, HTTPc_RESP, *RespPtr, suffix)                            /* Pointer to Response object.             */ \
  FIELD_DEF(prefix, HTTPc_REQ, *NextPtr, suffix)                             /* Pointer to next Request object.         */ \
  FIELD_DEF(prefix, HTTPc_WEBSOCK_REQ, *WebSockPtr, suffix)                  /* Pointer to WebSocket Request object.    */ \
  USER_DATA_FIELD_DEF(, )                                                    /* Pointer to user data.                   */ \


struct httpc_req_obj {
  STRUCT_REQ_INIT(const, _reserved)
};

struct httpc_req {
  STRUCT_REQ_INIT(, )
};

/********************************************************************************************************
 *                                        SOCKET SECURE CFG FIELD MACRO
 *******************************************************************************************************/

#ifdef NET_SECURE_MODULE_EN
#define  SOCK_SECURE_CFG_FIELD_DEF(prefix, suffix)  FIELD_DEF(prefix, NET_APP_SOCK_SECURE_CFG *, SockSecureCfgPtr, suffix)  // Connection's Socket Secure Cfg.
#else
#define  SOCK_SECURE_CFG_FIELD_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                      CONNECTION CALLBACK FIELDS MACRO
 *******************************************************************************************************/

#ifdef HTTPc_TASK_MODULE_EN
#define  TASK_CONN_CALLBACK_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, HTTPc_CONNECT_CALLBACK, OnConnect, suffix) \
  FIELD_DEF(prefix, HTTPc_CONN_CLOSE_CALLBACK, OnClose, suffix)
#else
#define  TASK_CONN_CALLBACK_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                       CONNECTION SIGNAL FIELDS MACRO
 *******************************************************************************************************/

#ifdef HTTPc_SIGNAL_TASK_MODULE_EN
#define  TASK_CONN_SIGNAL_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, void *, ConnectSignalPtr, suffix) \
  FIELD_DEF(prefix, void *, TransDoneSignalPtr, suffix)                                                   \
  FIELD_DEF(prefix, void *, CloseSignalPtr, suffix)
#else
#define  TASK_CONN_SIGNAL_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                      CONNENCTION QUERY STRING FIELDS MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_QUERY_STR_EN == DEF_ENABLED)
#define  CONN_QUERY_STR_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, CPU_INT16U, ReqQueryStrTxIx, suffix) \
  FIELD_DEF(prefix, HTTPc_KEY_VAL, *ReqQueryStrTempPtr, suffix)
#else
#define  CONN_QUERY_STR_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                       CONNECTION HEADER FIELDS MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_HDR_TX_EN == DEF_ENABLED)
#define  CONN_HDR_FIELDS_DEF(prefix, suffix)  FIELD_DEF(prefix, CPU_INT16U, ReqHdrTxIx, suffix) \
  FIELD_DEF(prefix, HTTPc_HDR, *ReqHdrTempPtr, suffix)
#else
#define  CONN_HDR_FIELDS_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                         CONECTION FORM FIELD MACRO
 *******************************************************************************************************/

#if (HTTPc_CFG_FORM_EN == DEF_ENABLED)
#define  CONN_FORM_FIELD_DEF(prefix, suffix)  FIELD_DEF(prefix, CPU_INT16U, ReqFormDataTxIx, suffix)
#else
#define  CONN_FORM_FIELD_DEF(prefix, suffix)
#endif

/********************************************************************************************************
 *                                       CONNECTION STRUCTURE MACRO
 *******************************************************************************************************/

#define  STRUCT_CONN_INIT(prefix, suffix)                                                                                       \
  FIELD_DEF(prefix, NET_SOCK_ID, SockID, suffix)                                 /* Connection's Socket ID.                  */ \
  FIELD_DEF(prefix, HTTPc_FLAGS, SockFlags, suffix)                              /* Connection's Socket flags.               */ \
  SOCK_SECURE_CFG_FIELD_DEF(prefix, suffix)                                      /* Connection's Socket Secure Cfg.          */ \
  FIELD_DEF(prefix, CPU_INT16U, ConnectTimeout_ms, suffix)                       /* Connection Connect Timeout.              */ \
  FIELD_DEF(prefix, CPU_INT16U, InactivityTimeout_s, suffix)                     /* Connection Inactivity Timeout.           */ \
  FIELD_DEF(prefix, NET_PORT_NBR, ServerPort, suffix)                            /* Server Port number.                      */ \
  FIELD_DEF(prefix, NET_SOCK_ADDR, ServerSockAddr, suffix)                       /* Server Socket address.                   */ \
  FIELD_DEF(prefix, CPU_CHAR, *HostNamePtr, suffix)                              /* Pointer to server hostname string.       */ \
  FIELD_DEF(prefix, CPU_INT16U, HostNameLen, suffix)                             /* Server Hostname length.                  */ \
  FIELD_DEF(prefix, HTTPc_CONN_STATE, State, suffix)                             /* Connection State.                        */ \
  FIELD_DEF(prefix, HTTPc_FLAGS, Flags, suffix)                                  /* Set of flags related to HTTP conn.       */ \
  FIELD_DEF(prefix, HTTPc_CONN_CLOSE_STATUS, CloseStatus, suffix)                /* Status of connection closed.             */ \
  FIELD_DEF(prefix, RTOS_ERR, ErrCode, suffix)                                   /* Error code of connection.                */ \
  TASK_CONN_CALLBACK_FIELDS_DEF(prefix, suffix)                                  /* Connection's Callback functions.         */ \
  TASK_CONN_SIGNAL_FIELDS_DEF(prefix, suffix)                                    /* Conneciton's signals's handle.           */ \
  FIELD_DEF(prefix, HTTPc_REQ, *ReqListHeadPtr, suffix)                          /* Head of the Request list.                */ \
  FIELD_DEF(prefix, HTTPc_REQ, *ReqListEndPtr, suffix)                           /* End of the Request list.                 */ \
  FIELD_DEF(prefix, HTTPc_FLAGS, ReqFlags, suffix)                               /* Req flags for internal process.          */ \
  CONN_QUERY_STR_FIELDS_DEF(prefix, suffix)                                      /* QueryStr param for internal process.     */ \
  CONN_HDR_FIELDS_DEF(prefix, suffix)                                            /* Header param for internal process.       */ \
  CONN_FORM_FIELD_DEF(prefix, suffix)                                            /* Form param for internal process.         */ \
  FIELD_DEF(prefix, CPU_INT16U, ReqDataOffset, suffix)                           /* Offset in Req Data Ptr to Tx.            */ \
  FIELD_DEF(prefix, HTTPc_FLAGS, RespFlags, suffix)                              /* Set of flags related to the resp.        */ \
  FIELD_DEF(prefix, void, *TxDataPtr, suffix)                                    /* Pointer to data to transmit.             */ \
  FIELD_DEF(prefix, CPU_CHAR, *BufPtr, suffix)                                   /* Pointer to conn buffer.                  */ \
  FIELD_DEF(prefix, CPU_INT16U, BufLen, suffix)                                  /* Conn buffer's length.                    */ \
  FIELD_DEF(prefix, CPU_CHAR, *RxBufPtr, suffix)                                 /* Pointer inside Buf where to Rx data.     */ \
  FIELD_DEF(prefix, CPU_INT16U, RxDataLenRem, suffix)                            /* Remaining data to process in the rx buf. */ \
  FIELD_DEF(prefix, CPU_INT32U, RxDataLen, suffix)                               /* Data length received.                    */ \
  FIELD_DEF(prefix, CPU_CHAR, *TxBufPtr, suffix)                                 /* Pointer inside Buf where to Tx data.     */ \
  FIELD_DEF(prefix, CPU_INT16U, TxDataLen, suffix)                               /* Length of data to Tx.                    */ \
  FIELD_DEF(, HTTPc_CONN, *NextPtr, suffix)                                      /* Pointer to next conn in list.            */ \
  FIELD_DEF(prefix, HTTPc_WEBSOCK, *WebSockPtr, suffix)                          /* Pointer to the Websock                   */ \
  USER_DATA_FIELD_DEF(, )                                                        /* Pointer to user data.                    */ \


struct httpc_conn_obj {
  STRUCT_CONN_INIT(const, _reserved)
};

struct httpc_conn {
  STRUCT_CONN_INIT(, )
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const HTTPc_INIT_CFG HTTPc_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                          FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void HTTPc_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base);

void HTTPc_ConfigureMemSeg(MEM_SEG *p_mem_seg);

#ifdef HTTPc_TASK_MODULE_EN
void HTTPc_ConfigureQty(HTTPc_QTY_CFG *p_qty_cfg);
#endif

void HTTPc_ConfigureConnParam(HTTPc_CONN_CFG *p_conn_cfg);
#endif

void HTTPc_Init(RTOS_ERR *p_err);

void HTTPc_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err);

void HTTPc_TaskDlySet(CPU_INT08U dly_ms,
                      RTOS_ERR   *p_err);

void HTTPc_ConnClr(HTTPc_CONN_OBJ *p_conn_obj,
                   RTOS_ERR       *p_err);

void HTTPc_ConnSetParam(HTTPc_CONN_OBJ   *p_conn_obj,
                        HTTPc_PARAM_TYPE type,
                        void             *p_param,
                        RTOS_ERR         *p_err);

CPU_BOOLEAN HTTPc_ConnOpen(HTTPc_CONN_OBJ *p_conn_obj,
                           CPU_CHAR       *p_buf,
                           CPU_INT16U     buf_len,
                           CPU_CHAR       *p_hostname_str,
                           CPU_INT16U     hostname_str_len,
                           HTTPc_FLAGS    flags,
                           RTOS_ERR       *p_err);

void HTTPc_ConnClose(HTTPc_CONN_OBJ *p_conn_obj,
                     HTTPc_FLAGS    flags,
                     RTOS_ERR       *p_err);

void HTTPc_ReqClr(HTTPc_REQ_OBJ *p_req_obj,
                  RTOS_ERR      *p_err);

void HTTPc_ReqSetParam(HTTPc_REQ_OBJ    *p_req_obj,
                       HTTPc_PARAM_TYPE type,
                       void             *p_param,
                       RTOS_ERR         *p_err);

CPU_BOOLEAN HTTPc_ReqSend(HTTPc_CONN_OBJ *p_conn_obj,
                          HTTPc_REQ_OBJ  *p_req_obj,
                          HTTPc_RESP_OBJ *p_resp_obj,
                          HTTP_METHOD    method,
                          CPU_CHAR       *p_resource_path,
                          CPU_INT16U     resource_path_len,
                          HTTPc_FLAGS    flags,
                          RTOS_ERR       *p_err);

CPU_INT32U HTTPc_FormAppFmt(CPU_CHAR             *p_buf,
                            CPU_INT16U           buf_len,
                            HTTPc_FORM_TBL_FIELD *p_form_tbl,
                            CPU_INT16U           form_tbl_size,
                            RTOS_ERR             *p_err);

CPU_INT32U HTTPc_FormMultipartFmt(CPU_CHAR             *p_buf,
                                  CPU_INT16U           buf_len,
                                  HTTPc_FORM_TBL_FIELD *p_form_tbl,
                                  CPU_INT16U           form_tbl_size,
                                  RTOS_ERR             *p_err);

void HTTPc_FormAddKeyVal(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                         HTTPc_KEY_VAL        *p_key_val,
                         RTOS_ERR             *p_err);

void HTTPc_FormAddKeyValExt(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                            HTTPc_KEY_VAL_EXT    *p_key_val_ext,
                            RTOS_ERR             *p_err);

void HTTPc_FormAddFile(HTTPc_FORM_TBL_FIELD *p_form_tbl,
                       HTTPc_MULTIPART_FILE *p_file_obj,
                       RTOS_ERR             *p_err);

#ifdef  HTTPc_WEBSOCK_MODULE_EN
void HTTPc_WebSockSetParam(HTTPc_WEBSOCK_OBJ *p_ws_obj,
                           HTTPc_PARAM_TYPE  type,
                           void              *p_param,
                           RTOS_ERR          *p_err);

void HTTPc_WebSockMsgSetParam(HTTPc_WEBSOCK_MSG_OBJ *p_msg_obj,
                              HTTPc_PARAM_TYPE      type,
                              void                  *p_param,
                              RTOS_ERR              *p_err);

CPU_BOOLEAN HTTPc_WebSockUpgrade(HTTPc_CONN_OBJ    *p_conn_obj,
                                 HTTPc_REQ_OBJ     *p_req_obj,
                                 HTTPc_RESP_OBJ    *p_resp_obj,
                                 HTTPc_WEBSOCK_OBJ *p_ws_obj,
                                 CPU_CHAR          *p_resource_path,
                                 CPU_INT16U        resource_path_len,
                                 HTTPc_FLAGS       flags,
                                 RTOS_ERR          *p_err);

CPU_BOOLEAN HTTPc_WebSockSend(HTTPc_CONN_OBJ         *p_conn_obj,
                              HTTPc_WEBSOCK_MSG_OBJ  *p_msg_obj,
                              HTTPc_WEBSOCK_MSG_TYPE msg_type,
                              CPU_CHAR               *p_data,
                              CPU_INT32U             payload_len,
                              HTTPc_FLAGS            flags,
                              RTOS_ERR               *p_err);

void HTTPc_WebSockClr(HTTPc_WEBSOCK_OBJ *p_ws_obj,
                      RTOS_ERR          *p_err);

void HTTPc_WebSockMsgClr(HTTPc_WEBSOCK_MSG_OBJ *p_msg_obj,
                         RTOS_ERR              *p_err);

CPU_INT16U HTTPc_WebSockFmtCloseMsg(HTTPc_WEBSOCK_CLOSE_CODE close_code,
                                    CPU_CHAR                 *p_reason,
                                    CPU_CHAR                 *p_buf,
                                    CPU_INT16U               buf_len,
                                    RTOS_ERR                 *p_err);
#endif

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_CLIENT_H_
