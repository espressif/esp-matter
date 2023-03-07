/***************************************************************************//**
 * @file
 * @brief Network - HTTP Server
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

#ifndef  _HTTP_SERVER_PRIV_H_
#define  _HTTP_SERVER_PRIV_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                           INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  "../http_dict_priv.h"

#include  <net/include/http.h>
#include  <net/include/http_server.h>
#include  <net/include/net_type.h>
#include  <net/include/net_cfg_net.h>

#include  <cpu/include/cpu.h>
#include  <common/source/kal/kal_priv.h>

#include  <http_server_cfg.h>
#include  <net_cfg.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                       PRE CONFIGURATION ERROR
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  HTTPs_CFG_CTR_STAT_EN
    #error  "HTTPs_CFG_CTR_STAT_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_CTR_STAT_EN != DEF_ENABLED) \
  && (HTTPs_CFG_CTR_STAT_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_CTR_STAT_EN illegally #define'd in 'http-s_cfg.h'. MUST be  DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_CTR_ERR_EN
    #error  "HTTPs_CFG_CTR_ERR_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_CTR_ERR_EN != DEF_ENABLED) \
  && (HTTPs_CFG_CTR_ERR_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_CTR_ERR_EN illegally #define'd in 'http-s_cfg.h'. MUST be  DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_PERSISTENT_CONN_EN
    #error  "HTTPs_CFG_PERSISTENT_CONN_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_PERSISTENT_CONN_EN != DEF_ENABLED) \
  && (HTTPs_CFG_PERSISTENT_CONN_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_PERSISTENT_CONN_EN illegally #define'd in 'http-s_cfg.h'. MUST be  DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_TOKEN_PARSE_EN
    #error  "HTTPs_CFG_TOKEN_PARSE_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_TOKEN_PARSE_EN != DEF_ENABLED) \
  && (HTTPs_CFG_TOKEN_PARSE_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_TOKEN_PARSE_EN illegally #define'd in 'http-s_cfg.h'. MUST be  DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_ABSOLUTE_URI_EN
    #error  "HTTPs_CFG_ABSOLUTE_URI_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_ABSOLUTE_URI_EN != DEF_ENABLED) \
  && (HTTPs_CFG_ABSOLUTE_URI_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_ABSOLUTE_URI_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_QUERY_STR_EN
    #error  "HTTPs_CFG_QUERY_STR_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_QUERY_STR_EN != DEF_ENABLED) \
  && (HTTPs_CFG_QUERY_STR_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_QUERY_STR_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_FORM_EN
    #error  "HTTPs_CFG_FORM_EN not #define'd in 'http-s_cfg.h'"
#elif ((HTTPs_CFG_FORM_EN != DEF_ENABLED) \
  && (HTTPs_CFG_FORM_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_FORM_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"

#elif (HTTPs_CFG_FORM_EN == DEF_ENABLED)
    #ifndef  HTTPs_CFG_FORM_MULTIPART_EN
        #error  "HTTPs_CFG_FORM_MULTIPART_EN not #define'd in 'http-s_cfg.h'"
    #elif  ((HTTPs_CFG_FORM_MULTIPART_EN != DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN != DEF_DISABLED))
        #error  "HTTPs_CFG_FORM_MULTIPART_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
    #endif
#endif

#ifndef HTTPs_CFG_HDR_RX_EN
    #error  "HTTPs_CFG_RX_HDR_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_HDR_RX_EN != DEF_ENABLED) \
  && (HTTPs_CFG_HDR_RX_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_HDR_RX_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef HTTPs_CFG_HDR_TX_EN
    #error  "HTTPs_CFG_TX_HDR_EN not #define'd in 'http-s_cfg.h'"
#elif  ((HTTPs_CFG_HDR_TX_EN != DEF_ENABLED) \
  && (HTTPs_CFG_HDR_TX_EN != DEF_DISABLED))
    #error  "HTTPs_CFG_HDR_TX_EN illegally #define'd in 'http-s_cfg.h'. MUST be DEF_DISABLED or DEF_ENABLED"
#endif

#ifndef  HTTPs_CFG_HTML_DFLT_ERR_PAGE
    #error  "HTTPs_CFG_HTML_DFLT_ERR_PAGE not #define'd in 'http-s_cfg.h'. MUST be defined as a string"
#endif

/********************************************************************************************************
 *                                       NETWORK CONFIGURATION ERRORS
 *******************************************************************************************************/

#if     (NET_SOCK_CFG_SEL_EN != DEF_ENABLED)
#error  "NET_SOCK_CFG_SEL_EN: illegally #define'd in 'net_cfg.h'. MUST be DEF_ENABLED"
#endif

#if     (NET_TCP_CFG_EN != DEF_ENABLED)
#error  "NET_TCP_CFG_EN: illegally #define'd in 'net_cfg.h'. MUST be DEF_ENABLED"
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                                   DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       GENERAL HTTPs DEFINES
 *******************************************************************************************************/

#define  HTTPs_BUF_LEN_MIN                                  256u

#define  HTTPs_PATH_SEP_CHAR_DFLT                           '/'

/********************************************************************************************************
 *                                       HTTP SERVER FLAGS DEFINES
 *******************************************************************************************************/

#define  HTTPs_FLAG_NONE                            DEF_BIT_NONE

#define  HTTPs_FLAG_INIT                           (DEF_BIT_NONE | HTTPs_FLAG_RESP_CHUNKED)

//                                                                 SOCKET FLAGS
#define  HTTPs_FLAG_SOCK_RDY_RD                     DEF_BIT_00
#define  HTTPs_FLAG_SOCK_RDY_WR                     DEF_BIT_01
#define  HTTPs_FLAG_SOCK_RDY_ERR                    DEF_BIT_02

//                                                                 CONN & TRANSACTION FLAGS
#define  HTTPs_FLAG_CONN_PERSISTENT                 DEF_BIT_00  // Flag indicating if Connection is persistent.
#define  HTTPs_FLAG_REQ_FLUSH                       DEF_BIT_01
#define  HTTPs_FLAG_RESP_BODY_PRESENT               DEF_BIT_02
#define  HTTPs_FLAG_RESP_LOCATION                   DEF_BIT_03  // Flag indicating Location hdr requirement in resp.
#define  HTTPs_FLAG_RESP_CHUNKED                    DEF_BIT_04
#define  HTTPs_FLAG_RESP_CHUNKED_HOOK               DEF_BIT_05

/********************************************************************************************************
 *                                               FORM DEFINES
 *******************************************************************************************************/

#define  HTTPs_FORM_BOUNDARY_STR_LEN_MAX                     72u

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                       HTTP INTERNAL TOKEN DATA TYPE
 *******************************************************************************************************/

#define  HTTPs_STR_TOKEN_INTERNAL_STATUS_CODE                "STATUS_CODE"
#define  HTTPs_STR_TOKEN_INTERNAL_REASON_PHRASE              "REASON_PHRASE"

typedef  enum  https_token_int {
  HTTPs_TOKEN_INTERNAL_STATUS_CODE,
  HTTPs_TOKEN_INTERNAL_REASON_PHRASE
} HTTPs_TOKEN_INTERNAL;

/********************************************************************************************************
 *                                       HTTP SERVER TASK DATA TYPE
 *******************************************************************************************************/

struct  https_os_task_obj {
  KAL_TASK_HANDLE TaskHandle;
  KAL_SEM_HANDLE  SemStopReq;
  KAL_SEM_HANDLE  SemStopCompleted;
  KAL_LOCK_HANDLE LockObj;
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                           GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

extern CPU_INT16U HTTPs_InstanceRunningNbr;
extern CPU_INT16U HTTPs_InstanceInitializedNbr;

extern const HTTP_DICT HTTPs_DictionaryTokenInternal[];
extern CPU_SIZE_T      HTTPs_DictionarySizeTokenInternal;

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MACRO'S
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               HOOK MACRO
 *******************************************************************************************************/

#define  HTTPs_HOOK_DEFINED(hookCfg, hookName)              ((hookCfg != DEF_NULL) && (hookCfg->hookName != DEF_NULL))

/********************************************************************************************************
 *                                           HTTPs COUNTER MACRO'S
 *
 * Description : Functionality to set and increment statistic and error counter
 *
 * Argument(s) : Various HTTP server counter variable(s) & values.
 *
 * Return(s)   : none.
 *
 * Caller(s)   : various.
 *
 *               These macro's are INTERNAL HTTP server suite macro's & SHOULD NOT be called by
 *               application function(s).
 *
 * Note(s)     : none.
 *******************************************************************************************************/

#if (HTTPs_CFG_CTR_STAT_EN == DEF_ENABLED)
    #define  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance)   { \
    p_ctr_stats = &p_instance->StatsCtr;                      \
}

    #define  HTTPs_STATS_INC(p_ctr)                         { \
    p_ctr++;                                                  \
}

    #define  HTTPs_STATS_OCTET_INC(p_ctr, octet)            { \
    p_ctr += octet;                                           \
}

#else
//                                                                 Prevent 'variable unused' compiler warning.
    #define  HTTPs_SET_PTR_STATS(p_ctr_stats, p_instance)   { \
    PP_UNUSED_PARAM(p_ctr_stats);                             \
}

    #define  HTTPs_STATS_INC(p_ctr)

    #define  HTTPs_STATS_OCTET_INC(p_ctr, octet)
#endif

#if (HTTPs_CFG_CTR_ERR_EN == DEF_ENABLED)
    #define  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance)      { \
    p_ctr_err = &p_instance->ErrsCtr;                         \
}

    #define  HTTPs_ERR_INC(p_ctr)                           { \
    p_ctr++;                                                  \
}

#else
//                                                                 Prevent 'variable unused' compiler warning.
    #define  HTTPs_SET_PTR_ERRS(p_ctr_err, p_instance)      { \
    PP_UNUSED_PARAM(p_ctr_err);                               \
}

    #define  HTTPs_ERR_INC(p_ctr)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                           FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/********************************************************************************************************
 *                                               MEMORY MODULE
 *******************************************************************************************************/

//                                                                 Instance functionalities.
void HTTPsMem_InstanceInit(MEM_SEG  *p_mem_seg,
                           RTOS_ERR *p_err);

HTTPs_OS_TASK_OBJ *HTTPsMem_InstanceTaskInit(RTOS_ERR *p_err);

HTTPs_INSTANCE *HTTPsMem_InstanceGet(RTOS_ERR *p_err);

void HTTPsMem_InstanceRelease(HTTPs_INSTANCE *p_instance);

#if (HTTPs_CFG_FS_PRESENT_EN == DEF_ENABLED)
void HTTPsMem_InstanceWorkingDirInit(HTTPs_INSTANCE *p_instance,
                                     RTOS_ERR       *p_err);
#endif

//                                                                 Conn functionalities.
void HTTPsMem_ConnPoolInit(HTTPs_INSTANCE *p_instance,
                           RTOS_ERR       *p_err);

HTTPs_CONN *HTTPsMem_ConnGet(HTTPs_INSTANCE *p_instance,
                             NET_SOCK_ID    sock_id,
                             NET_SOCK_ADDR  client_addr);

void HTTPsMem_ConnRelease(HTTPs_INSTANCE *p_instance,
                          HTTPs_CONN     *p_conn);

void HTTPsMem_ConnClr(HTTPs_INSTANCE *p_instance,
                      HTTPs_CONN     *p_conn);

#if (HTTPs_CFG_TOKEN_PARSE_EN == DEF_ENABLED)
CPU_BOOLEAN HTTPsMem_TokenGet(HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN     *p_conn,
                              RTOS_ERR       *p_err);

void HTTPsMem_TokenRelease(HTTPs_INSTANCE *p_instance,
                           HTTPs_CONN     *p_conn);
#endif

#if (HTTPs_CFG_QUERY_STR_EN == DEF_ENABLED)
HTTPs_KEY_VAL *HTTPsMem_QueryStrKeyValBlkGet(HTTPs_INSTANCE *p_instance,
                                             HTTPs_CONN     *p_conn,
                                             RTOS_ERR       *p_err);

void HTTPsMem_QueryStrKeyValBlkReleaseAll(HTTPs_INSTANCE *p_instance,
                                          HTTPs_CONN     *p_conn);
#endif

#if (HTTPs_CFG_FORM_EN == DEF_ENABLED)
HTTPs_KEY_VAL *HTTPsMem_FormKeyValBlkGet(HTTPs_INSTANCE *p_instance,
                                         HTTPs_CONN     *p_conn,
                                         RTOS_ERR       *p_err);

void HTTPsMem_FormKeyValBlkReleaseAll(HTTPs_INSTANCE *p_instance,
                                      HTTPs_CONN     *p_conn);
#endif

#if (HTTPs_CFG_HDR_RX_EN == DEF_ENABLED)
HTTPs_HDR_BLK *HTTPsMem_ReqHdrGet(HTTPs_INSTANCE     *p_instance,
                                  HTTPs_CONN         *p_conn,
                                  HTTP_HDR_FIELD     hdr_fied,
                                  HTTPs_HDR_VAL_TYPE val_type,
                                  RTOS_ERR           *p_err);

void HTTPsMem_ReqHdrRelease(HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN     *p_conn);
#endif

#if (HTTPs_CFG_HDR_TX_EN == DEF_ENABLED)
HTTPs_HDR_BLK *HTTPsMem_RespHdrGet(HTTPs_INSTANCE     *p_instance,
                                   HTTPs_CONN         *p_conn,
                                   HTTP_HDR_FIELD     hdr_fied,
                                   HTTPs_HDR_VAL_TYPE val_type,
                                   RTOS_ERR           *p_err);

void HTTPsMem_RespHdrRelease(HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN     *p_conn);
#endif

/********************************************************************************************************
 *                                           CONNECTION MODULE
 *******************************************************************************************************/

void HTTPsConn_Process(HTTPs_INSTANCE *p_instance);

/********************************************************************************************************
 *                                           REQUEST MODULE
 *******************************************************************************************************/

void HTTPsReq_Handler(HTTPs_INSTANCE *p_instance,
                      HTTPs_CONN     *p_conn);

void HTTPsReq_Body(HTTPs_INSTANCE *p_instance,
                   HTTPs_CONN     *p_conn);

CPU_BOOLEAN HTTPsReq_RdySignal(HTTPs_INSTANCE *p_instance,
                               HTTPs_CONN     *p_conn);

/********************************************************************************************************
 *                                           RESPONSE MODULE
 *******************************************************************************************************/

CPU_BOOLEAN HTTPsResp_Prepare(HTTPs_INSTANCE *p_instance,
                              HTTPs_CONN     *p_conn);

CPU_BOOLEAN HTTPsResp_Handle(HTTPs_INSTANCE *p_instance,
                             HTTPs_CONN     *p_conn);

void HTTPsResp_DataComplete(HTTPs_INSTANCE *p_instance,
                            HTTPs_CONN     *p_conn);

/********************************************************************************************************
 *                                               SOCKET MODULE
 *******************************************************************************************************/

NET_SOCK_ID HTTPsSock_ListenInit(const HTTPs_CFG          *p_cfg,
                                 NET_SOCK_PROTOCOL_FAMILY family,
                                 RTOS_ERR                 *p_err);

void HTTPsSock_ListenClose(HTTPs_INSTANCE *p_instance,
                           NET_SOCK_ID    sock_listen_id);

NET_SOCK_QTY HTTPsSock_ConnSel(HTTPs_INSTANCE *p_instance,
                               CPU_BOOLEAN    accept);

CPU_BOOLEAN HTTPsSock_ConnDataRx(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn);

CPU_BOOLEAN HTTPsSock_ConnDataTx(HTTPs_INSTANCE *p_instance,
                                 HTTPs_CONN     *p_conn);

void HTTPsSock_ConnClose(HTTPs_INSTANCE *p_instance,
                         HTTPs_CONN     *p_conn);

/********************************************************************************************************
 *                                               STRING MODULE
 *******************************************************************************************************/

#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
CPU_BOOLEAN HTTPs_StrPathFormat(CPU_CHAR   *p_filename,
                                CPU_CHAR   *p_folder,
                                CPU_CHAR   *p_path_dst,
                                CPU_SIZE_T path_len_max,
                                CPU_CHAR   path_sep);
#endif

#if (HTTPs_CFG_ABSOLUTE_URI_EN == DEF_ENABLED)
CPU_CHAR *HTTPs_StrPathGet(CPU_CHAR    *p_path,
                           CPU_INT16U  path_len_max,
                           CPU_CHAR    *p_host,
                           CPU_INT16U  host_len_max,
                           CPU_BOOLEAN *p_resp_location);
#endif

#if ((HTTPs_CFG_FORM_EN == DEF_ENABLED) \
  && (HTTPs_CFG_FORM_MULTIPART_EN == DEF_ENABLED))
CPU_CHAR *HTTPs_StrMemSrch(const CPU_CHAR *p_data,
                           CPU_INT32U     data_len,
                           const CPU_CHAR *p_str,
                           CPU_INT32U     str_len);
#endif

/********************************************************************************************************
 *                                               TASK MODULE
 *******************************************************************************************************/

KAL_LOCK_HANDLE HTTPsTask_LockCreate(RTOS_ERR *p_err);

void HTTPsTask_LockAcquire(KAL_LOCK_HANDLE os_lock_obj);

void HTTPsTask_LockRelease(KAL_LOCK_HANDLE os_lock_obj);

void HTTPsTask_InstanceObjInit(HTTPs_INSTANCE *p_instance,
                               RTOS_ERR       *p_err);

void HTTPsTask_InstanceTaskCreate(HTTPs_INSTANCE *p_instance,
                                  RTOS_ERR       *p_err);

void HTTPsTask_InstanceTaskPrioSet(HTTPs_INSTANCE *p_instance,
                                   KAL_TASK_PRIO  prio,
                                   RTOS_ERR       *p_err);

void HTTPsTask_InstanceTaskDel(HTTPs_INSTANCE *p_instance);

void HTTPsTask_InstanceStopReqSignal(HTTPs_INSTANCE *p_instance);

CPU_BOOLEAN HTTPsTask_InstanceStopReqPending(HTTPs_INSTANCE *p_instance);

void HTTPsTask_InstanceStopCompletedSignal(HTTPs_INSTANCE *p_instance);

void HTTPsTask_InstanceStopCompletedPending(HTTPs_INSTANCE *p_instance,
                                            RTOS_ERR       *p_err);

void HTTPsTask_TimeDly_ms(CPU_INT32U time_dly_ms);

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _HTTP_SERVER_PRIV_H_
