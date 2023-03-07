/***************************************************************************//**
 * @file
 * @brief Network - Mqtt Client
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
 * @defgroup NET_MQTTc MQTT Client API
 * @ingroup  NET
 * @brief    MQTT Client API
 *
 * @addtogroup NET_MQTTc
 * @{
 *******************************************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                               MODULE
 ********************************************************************************************************
 *******************************************************************************************************/

#ifndef  _MQTT_CLIENT_H_
#define  _MQTT_CLIENT_H_

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <common/include/lib_mem.h>
#include  <common/include/rtos_opt_def.h>
#include  <common/include/rtos_path.h>
#include  <common/include/rtos_types.h>

#include  <rtos_cfg.h>
#include  <rtos_description.h>

#include  <net/include/net_app.h>
#include  <net/include/net_sock.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                               DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  MQTTc_FLAGS_NONE                       DEF_BIT_NONE    // Reserved for future usage.

/********************************************************************************************************
 *                                    DEFAULT CONFIGURATION VALUES
 *******************************************************************************************************/

#define  MQTT_CLIENT_CFG_INACTIVITY_TIMEOUT_SEC_DFLT            1800u
#define  MQTT_CLIENT_CFG_MSG_NBR_MAX_DFLT                       2u
#define  MQTT_CLIENT_CFG_TASK_DLY_MS_DFLT                       0u

#define  MQTT_CLIENT_TASK_CFG_STK_SIZE_ELEMENTS_DFLT            512u
#define  MQTT_CLIENT_TASK_CFG_STK_PTR_DFLT                      DEF_NULL

#define  MQTT_CLIENT_MEM_SEG_PTR_DFLT                           DEF_NULL

/********************************************************************************************************
 ********************************************************************************************************
 *                                             DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

typedef  struct  mqttc_conn MQTTc_CONN;                         ///< Forward declaration of MQTTc_CONN.
typedef  struct  mqttc_msg MQTTc_MSG;                           ///< Forward declaration of MQTTc_MSG.

/********************************************************************************************************
 *                                 QUANTITY CONFIGURATION DATA TYPE
 * @brief MQTT quantity configuration
 *******************************************************************************************************/

typedef  struct  mqttc_qty_cfg {
  CPU_SIZE_T MaxMsgNbr;                                         ///< Message number max.
} MQTTc_QTY_CFG;

/********************************************************************************************************
 *                                           MQTTC INIT CFG
 * @brief MQTT intializtion configuration
 *******************************************************************************************************/

typedef  struct  mqttc_init_cfg {
  CPU_INT16U    InactivityDfltTimeout_s;
  CPU_INT16U    TaskDly_ms;

  MQTTc_QTY_CFG QtyCfg;

  CPU_STK_SIZE  StkSizeElements;                                ///< Size of the stk, in CPU_STK elements.
  void          *StkPtr;                                        ///< Ptr to base of the stk.

  MEM_SEG       *MemSegPtr;                                     ///< Ptr to network core mem seg.
} MQTTc_INIT_CFG;

/********************************************************************************************************
 *                                           MQTTc PARAM TYPE
 * @brief MQTT parameter
 *******************************************************************************************************/

typedef  enum  mqttc_param_type {
  MQTTc_PARAM_TYPE_BROKER_IP_ADDR,                              ///< Conn's broker's IP addr.
  MQTTc_PARAM_TYPE_BROKER_NAME,                                 ///< Conn's broker's name.
  MQTTc_PARAM_TYPE_BROKER_PORT_NBR,                             ///< Conn's broker's port nbr.
  MQTTc_PARAM_TYPE_INACTIVITY_TIMEOUT_S,                        ///< Conn's inactivity timeout, in seconds.
  MQTTc_PARAM_TYPE_CLIENT_ID_STR,                               ///< Conn's client ID str.
  MQTTc_PARAM_TYPE_USERNAME_STR,                                ///< Conn's client username str.
  MQTTc_PARAM_TYPE_PASSWORD_STR,                                ///< Conn's client password str.
  MQTTc_PARAM_TYPE_KEEP_ALIVE_TMR_SEC,                          ///< Conn's keep alive tmr, in seconds.
  MQTTc_PARAM_TYPE_WILL_CFG_PTR,                                ///< Conn's will cfg ptr, if any.
  MQTTc_PARAM_TYPE_SECURE_CFG_PTR,                              ///< Conn's ptr to secure cfg struct.

  MQTTc_PARAM_TYPE_CALLBACK_ON_COMPL,                           ///< Conn's generic on     cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_CONNECT_CMPL,                    ///< Conn's on connect     cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_CMPL,                    ///< Conn's on publish     cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_SUBSCRIBE_CMPL,                  ///< Conn's on subscribe   cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_UNSUBSCRIBE_CMPL,                ///< Conn's on unsubscribe cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_PINGREQ_CMPL,                    ///< Conn's on pingreq     cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_DISCONNECT_CMPL,                 ///< Conn's on disconnect  cmpl callback.
  MQTTc_PARAM_TYPE_CALLBACK_ON_ERR_CALLBACK,                    ///< Conn's on err              callback.

  MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_RX,                      ///< Conn's on publish rx'd callback.

  MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR,                            ///< Conn's ptr on arg passed to callback.

  MQTTc_PARAM_TYPE_TIMEOUT_MS,                                  ///< Conn's 'Open' timeout, in milliseconds.

  MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR,                          ///< Conn's ptr on msg that is used to rx publish msg.

  MQTTc_PARAM_TYPE_MSG_BUF_PTR,                                 ///< Msg's buf ptr.
  MQTTc_PARAM_TYPE_MSG_BUF_LEN                                  ///< Msg's buf len.
} MQTTc_PARAM_TYPE;

/********************************************************************************************************
 *                                            MQTTc MSG TYPE
 *******************************************************************************************************/

typedef  enum  mqttc_msg_type {
  MQTTc_MSG_TYPE_NONE,
  MQTTc_MSG_TYPE_CONNECT,
  MQTTc_MSG_TYPE_CONNACK,
  MQTTc_MSG_TYPE_PUBLISH,
  MQTTc_MSG_TYPE_PUBACK,
  MQTTc_MSG_TYPE_PUBREC,
  MQTTc_MSG_TYPE_PUBREL,
  MQTTc_MSG_TYPE_PUBCOMP,
  MQTTc_MSG_TYPE_SUBSCRIBE,
  MQTTc_MSG_TYPE_SUBACK,
  MQTTc_MSG_TYPE_UNSUBSCRIBE,
  MQTTc_MSG_TYPE_UNSUBACK,
  MQTTc_MSG_TYPE_PINGREQ,
  MQTTc_MSG_TYPE_PINGRESP,
  MQTTc_MSG_TYPE_DISCONNECT,

  MQTTc_MSG_TYPE_REQ_CLOSE
} MQTTc_MSG_TYPE;

/********************************************************************************************************
 *                                            MQTTc MSG STATE
 * @brief MQTT message state
 *******************************************************************************************************/

typedef  enum  mqttc_msg_state {
  MQTTc_MSG_STATE_NONE,                                         ///< Msg is in 'Idle'/'No' state.
  MQTTc_MSG_STATE_CMPL,                                         ///< Msg has cmpl'd.

  MQTTc_MSG_STATE_MUST_TX,                                      ///< Msg must be tx'd.
  MQTTc_MSG_STATE_WAIT_TX_CMPL,                                 ///< Msg is waiting for tx to cmpl.

  MQTTc_MSG_STATE_WAIT_RX                                       ///< Msg is waiting to rx.
} MQTTc_MSG_STATE;

/********************************************************************************************************
 *                                         MQTTc CALLBACK TYPES
 *******************************************************************************************************/

//                                                                 Type of callback exec'd when user-req'd oper cmpl.
typedef  void (*MQTTc_CMPL_CALLBACK)           (MQTTc_CONN *p_conn,
                                                MQTTc_MSG  *p_msg,
                                                void       *p_arg,
                                                RTOS_ERR   err);

//                                                                 Type of callback exec'd when err occurs/conn closes.
typedef  void (*MQTTc_ERR_CALLBACK)            (MQTTc_CONN *p_conn,
                                                void       *p_arg,
                                                RTOS_ERR   err);

//                                                                 Type of callback exec'd when a publish is rx'd.
typedef  void (*MQTTc_PUBLISH_RX_CALLBACK)     (MQTTc_CONN       *p_conn,
                                                const CPU_CHAR   *topic_name_str,
                                                CPU_INT32U       topic_len,
                                                const CPU_INT08U *p_message,
                                                CPU_INT32U       message_len,
                                                void             *p_arg,
                                                RTOS_ERR         err);

/********************************************************************************************************
 *                                            MQTTc FLAG TYPE
 *******************************************************************************************************/

typedef  CPU_INT08U MQTTc_FLAGS;                                // No flags implemented, reserved for future usage.

/********************************************************************************************************
 *                                          MQTTc WILL CFG TYPE
 * @brief MQTT WILL configuration
 *******************************************************************************************************/

typedef  struct  mqttc_will_cfg {
  CPU_CHAR    *WillTopic;                                       ///< Will's topic.
  CPU_CHAR    *WillMessage;                                     ///< Will's msg.
  CPU_BOOLEAN WillRetain;                                       ///< Flag indicating if will must be retained.
  CPU_INT08U  WillQoS;                                          ///< Will's QoS level.
} MQTTc_WILL_CFG;

/********************************************************************************************************
 *                                            MQTTc MSG TYPE
 * @brief MQTT message
 *******************************************************************************************************/

struct  mqttc_msg {
  MQTTc_CONN      *ConnPtr;                                     ///< Ptr to MQTTc_CONN associated.
  MQTTc_MSG_TYPE  Type;                                         ///< Msg's type.
  MQTTc_MSG_STATE State;                                        ///< Msg's state.
  CPU_INT08U      QoS;                                          ///< Msg's QoS.

  CPU_INT16U      MsgID;                                        ///< Msg ID used by msg.

  void            *ArgPtr;                                      ///< Ptr to rx/tx buf in case of mqttc msg, ptr to sem to post, in case of 'close' msg.
  CPU_INT32U      BufLen;                                       ///< Avail buf len for msg.
  CPU_INT32U      XferLen;                                      ///< Len of xfer.

  RTOS_ERR        Err;                                          ///< Err associated to processing of msg.

  MQTTc_MSG       *NextPtr;                                     ///< Ptr to next msg.
};

/********************************************************************************************************
 *                                            MQTTc CONN TYPE
 * @brief MQTT connection
 *******************************************************************************************************/

struct  mqttc_conn {
  NET_SOCK_ID               SockId;                             ///< Connection's socket ID.
  CPU_INT08U                SockSelFlags;                       ///< Flags to identify which oper must be checked in Sel.

  CPU_CHAR                  *BrokerNamePtr;                     ///< MQTT broker's name.
  CPU_INT16U                BrokerPortNbr;                      ///< MQTT broker's port nbr.
  CPU_INT16U                InactivityTimeout_s;                ///< Inactivity timeout, in seconds.

  CPU_CHAR                  *ClientID_Str;                      ///< Client ID str.
  CPU_CHAR                  *UsernameStr;                       ///< Username str.
  CPU_CHAR                  *PasswordStr;                       ///< Password str.

  CPU_INT16U                KeepAliveTimerSec;                  ///< Keep alive timer duration, in seconds.
  MQTTc_WILL_CFG            *WillCfgPtr;                        ///< Ptr to will cfg, if any.

  NET_APP_SOCK_SECURE_CFG   *SecureCfgPtr;                      ///< Ptr to secure will cfg, if any.

  //                                                               -------------------- CALLBACKS ---------------------
  MQTTc_CMPL_CALLBACK       OnCmpl;                             ///< Generic, on cmpl callback.
  MQTTc_CMPL_CALLBACK       OnConnectCmpl;                      ///< On connect cmpl callback.
  MQTTc_CMPL_CALLBACK       OnPublishCmpl;                      ///< On publish cmpl callback.
  MQTTc_CMPL_CALLBACK       OnSubscribeCmpl;                    ///< On subscribe cmpl callback.
  MQTTc_CMPL_CALLBACK       OnUnsubscribeCmpl;                  ///< On unsubscribe cmpl callback.
  MQTTc_CMPL_CALLBACK       OnPingReqCmpl;                      ///< On ping req cmpl callback.
  MQTTc_CMPL_CALLBACK       OnDisconnectCmpl;                   ///< On disconnect cmpl callback.
  MQTTc_ERR_CALLBACK        OnErrCallback;                      ///< On err or conn lost callback. Conn must be re-opened.
  MQTTc_PUBLISH_RX_CALLBACK OnPublishRx;                        ///< On publish rx'd cmpl callback.
  void                      *ArgPtr;                            ///< Ptr to arg that will be provided to callbacks.

  CPU_INT32U                TimeoutMs;                          ///< Timeout for 'Open' operation, in milliseconds.

  //                                                               ----------------- NEXT MSG VALUES ------------------
  CPU_INT08U                NextMsgHeader;                      ///< Header of next msg to parse.
  CPU_INT32U                NextMsgRxLen;                       ///< Rx len of next msg.
  MQTTc_MSG_TYPE            NextMsgType;                        ///< Next msg's type.
  CPU_INT32U                NextMsgLen;                         ///< Len remaining to rx for next msg.
  CPU_BOOLEAN               NextMsgLenIsCmpl;                   ///< Flag indicating if next msg's len value is rx'd.
  CPU_INT16U                NextMsgMsgID;                       ///< ID of next msg, if any.
  CPU_BOOLEAN               NextMsgMsgID_IsCmpl;                ///< Flag indicating if next msg's ID has been rx'd.
  MQTTc_MSG                 *NextMsgPtr;                        ///< Ptr to next msg, if known.

  MQTTc_MSG                 *PublishRxMsgPtr;                   ///< Ptr to msg that is used to rx publish from server.
  CPU_INT32U                PublishRemLen;                      ///< Len of data remaining in Publish msg after fixed hdr.

  MQTTc_MSG                 *TxMsgHeadPtr;                      ///< Ptr to head of msg needing to tx or waiting reply.
  CPU_INT32U                NextTxMsgTxLen;                     ///< Len of already xfer'd data.

  MQTTc_CONN                *NextPtr;                           ///< Ptr to next conn.
};

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
extern const MQTTc_INIT_CFG MQTTc_InitCfgDflt;
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                      PUBLIC FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED)
void MQTTc_ConfigureTaskStk(CPU_STK_SIZE stk_size_elements,
                            void         *p_stk_base);

void MQTTc_ConfigureMemSeg(MEM_SEG *p_mem_seg);

void MQTTc_ConfigureQty(MQTTc_QTY_CFG *p_qty_cfg);

#endif // RTOS_CFG_EXTERNALIZE_OPTIONAL_CFG_EN == DEF_DISABLED

void MQTTc_Init(RTOS_ERR *p_err);

void MQTTc_TaskPrioSet(RTOS_TASK_PRIO prio,
                       RTOS_ERR       *p_err);

void MQTTc_TaskDlySet(CPU_INT08U dly_ms,
                      RTOS_ERR   *p_err);

void MQTTc_InactivityTimeoutDfltSet(CPU_INT16U inactivity_timeout_s,
                                    RTOS_ERR   *p_err);

void MQTTc_ConnClr(MQTTc_CONN *p_conn,
                   RTOS_ERR   *p_err);

void MQTTc_ConnSetParam(MQTTc_CONN       *p_conn,
                        MQTTc_PARAM_TYPE type,
                        void             *p_param,
                        RTOS_ERR         *p_err);

void MQTTc_ConnOpen(MQTTc_CONN  *p_conn,
                    MQTTc_FLAGS flags,
                    RTOS_ERR    *p_err);

void MQTTc_ConnClose(MQTTc_CONN  *p_conn,
                     MQTTc_FLAGS flags,
                     RTOS_ERR    *p_err);

void MQTTc_MsgClr(MQTTc_MSG *p_msg,
                  RTOS_ERR  *p_err);

void MQTTc_MsgSetParam(MQTTc_MSG        *p_msg,
                       MQTTc_PARAM_TYPE type,
                       void             *p_param,
                       RTOS_ERR         *p_err);

void MQTTc_Connect(MQTTc_CONN *p_conn,
                   MQTTc_MSG  *p_msg,
                   RTOS_ERR   *p_err);

void MQTTc_Publish(MQTTc_CONN       *p_conn,
                   MQTTc_MSG        *p_msg,
                   const CPU_CHAR   *topic_str,
                   CPU_INT08U       qos_lvl,
                   CPU_BOOLEAN      retain_flag,
                   const CPU_INT08U *p_payload,
                   CPU_INT32U       payload_len,
                   RTOS_ERR         *p_err);

void MQTTc_Subscribe(MQTTc_CONN     *p_conn,
                     MQTTc_MSG      *p_msg,
                     const CPU_CHAR *topic_str,
                     CPU_INT08U     req_qos,
                     RTOS_ERR       *p_err);

void MQTTc_SubscribeMult(MQTTc_CONN     *p_conn,
                         MQTTc_MSG      *p_msg,
                         const CPU_CHAR **topic_str_tbl,
                         CPU_INT08U     *req_qos_tbl,
                         CPU_INT08U     topic_nbr,
                         RTOS_ERR       *p_err);

void MQTTc_Unsubscribe(MQTTc_CONN     *p_conn,
                       MQTTc_MSG      *p_msg,
                       const CPU_CHAR *topic_str,
                       RTOS_ERR       *p_err);

void MQTTc_UnsubscribeMult(MQTTc_CONN     *p_conn,
                           MQTTc_MSG      *p_msg,
                           const CPU_CHAR **topic_str_tbl,
                           CPU_INT08U     topic_nbr,
                           RTOS_ERR       *p_err);

void MQTTc_PingReq(MQTTc_CONN *p_conn,
                   MQTTc_MSG  *p_msg,
                   RTOS_ERR   *p_err);

void MQTTc_Disconnect(MQTTc_CONN *p_conn,
                      MQTTc_MSG  *p_msg,
                      RTOS_ERR   *p_err);

#ifdef __cplusplus
}
#endif

/****************************************************************************************************//**
 ********************************************************************************************************
 * @}                                          MODULE END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // _MQTT_CLIENT_H_
