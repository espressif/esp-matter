/***************************************************************************//**
 * @file
 * @brief MQTTc Application
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

/********************************************************************************************************
 ********************************************************************************************************
 *                                     DEPENDENCIES & AVAIL CHECK(S)
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <rtos_description.h>

#if (defined(RTOS_MODULE_NET_MQTT_CLIENT_AVAIL))

/********************************************************************************************************
 ********************************************************************************************************
 *                                            INCLUDE FILES
 ********************************************************************************************************
 *******************************************************************************************************/

#include  <ex_description.h>

#include  <cpu/include/cpu.h>
#include  <common/include/lib_def.h>
#include  <common/include/lib_str.h>
#include  <common/include/rtos_utils.h>

#include  <net/include/mqtt_client.h>

/********************************************************************************************************
 ********************************************************************************************************
 *                                            LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

//                                                                 Broker's host name or IP address.
#ifndef  EX_MQTTc_BROKER_NAME
    #define  EX_MQTTc_BROKER_NAME           "mqtt.micrium.com"
#endif

#ifndef  EX_MQTTc_USERNAME                                      // Username from MQTT server portal, if any.
    #define  EX_MQTTc_USERNAME              "micrium"
#endif

#ifndef  EX_MQTTc_PASSWORD                                      // Password or MD5 hash of your password.
    #define  EX_MQTTc_PASSWORD              "micrium"
#endif

#ifndef  EX_MQTTc_CLIENT_ID_NAME
#define  EX_MQTTc_CLIENT_ID_NAME            "micrium-example-echo"
#endif

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

#define  EX_MQTTc_MSG_QTY                         3u
#define  EX_MQTTc_MSG_LEN_MAX                  1024u

#define  EX_MQTTc_DOMAIN_PUBLISH_STATUS           "domain/status"
#define  EX_MQTTc_DOMAIN_PUBLISH_STATUS_QoS       2u

#define  EX_MQTTc_DOMAIN_PUBLISH_ECHO             "domain/echo"
#define  EX_MQTTc_DOMAIN_PUBLISH_ECHO_QoS         0u

#define  EX_MQTTc_DOMAIN_SUBSCRIBE_LISTEN         "domain/listen"
#define  EX_MQTTc_DOMAIN_SUBSCRIBE_LISTEN_QoS     1u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MQTTc_CONN Ex_MQTTc_Conn;
static MQTTc_MSG  Ex_MQTTc_StatusMsg;
static MQTTc_MSG  Ex_MQTTc_EchoMsg;
static MQTTc_MSG  Ex_MQTTc_ListenRxMsg;
static CPU_INT08U Ex_MQTTc_BufTbl[EX_MQTTc_MSG_QTY][EX_MQTTc_MSG_LEN_MAX];

static CPU_BOOLEAN Ex_MQTTc_StatusMsgIsAvail;
static CPU_BOOLEAN Ex_MQTTc_EchoMsgIsAvail;

/********************************************************************************************************
 ********************************************************************************************************
 *                                      LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static void Ex_MQTTc_OnCmplCallbackFnct(MQTTc_CONN *p_conn,
                                        MQTTc_MSG  *p_msg,
                                        void       *p_arg,
                                        RTOS_ERR   err);

static void Ex_MQTTc_OnConnectCmplCallbackFnct(MQTTc_CONN *p_conn,
                                               MQTTc_MSG  *p_msg,
                                               void       *p_arg,
                                               RTOS_ERR   err);

static void Ex_MQTTc_OnPublishCmplCallbackFnct(MQTTc_CONN *p_conn,
                                               MQTTc_MSG  *p_msg,
                                               void       *p_arg,
                                               RTOS_ERR   err);

static void Ex_MQTTc_OnSubscribeCmplCallbackFnct(MQTTc_CONN *p_conn,
                                                 MQTTc_MSG  *p_msg,
                                                 void       *p_arg,
                                                 RTOS_ERR   err);

static void Ex_MQTTc_OnPublishRxCallbackFnct(MQTTc_CONN       *p_conn,
                                             const CPU_CHAR   *topic_name_str,
                                             CPU_INT32U       topic_len,
                                             const CPU_INT08U *p_message,
                                             CPU_INT32U       message_len,
                                             void             *p_arg,
                                             RTOS_ERR         err);

static void Ex_MQTTc_OnErrCallbackFnct(MQTTc_CONN *p_conn,
                                       void       *p_arg,
                                       RTOS_ERR   err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                            Ex_MQTTc_Echo()
 *
 * @brief  Open a connection and publish message from the callback and receive message using subscribe.
 *
 * @return  DEF_OK,   if NO error(s),
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
void Ex_MQTT_Client_Echo(void)
{
  RTOS_ERR err;

  Ex_MQTTc_StatusMsgIsAvail = DEF_YES;
  Ex_MQTTc_EchoMsgIsAvail = DEF_YES;

  MQTTc_MsgClr(&Ex_MQTTc_StatusMsg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_StatusMsg, MQTTc_PARAM_TYPE_MSG_BUF_PTR, (void *)&Ex_MQTTc_BufTbl[0u], &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_StatusMsg, MQTTc_PARAM_TYPE_MSG_BUF_LEN, (void *) EX_MQTTc_MSG_LEN_MAX, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgClr(&Ex_MQTTc_EchoMsg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_EchoMsg, MQTTc_PARAM_TYPE_MSG_BUF_PTR, (void *)&Ex_MQTTc_BufTbl[1u], &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_EchoMsg, MQTTc_PARAM_TYPE_MSG_BUF_LEN, (void *) EX_MQTTc_MSG_LEN_MAX, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgClr(&Ex_MQTTc_ListenRxMsg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_ListenRxMsg, MQTTc_PARAM_TYPE_MSG_BUF_PTR, (void *)&Ex_MQTTc_BufTbl[2u], &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_ListenRxMsg, MQTTc_PARAM_TYPE_MSG_BUF_LEN, (void *) EX_MQTTc_MSG_LEN_MAX, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_ConnClr(&Ex_MQTTc_Conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_BROKER_NAME, (void *) EX_MQTTc_BROKER_NAME, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CLIENT_ID_STR, (void *) EX_MQTTc_CLIENT_ID_NAME, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_USERNAME_STR, (void *) EX_MQTTc_USERNAME, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_PASSWORD_STR, (void *) EX_MQTTc_PASSWORD, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_KEEP_ALIVE_TMR_SEC, (void *) 1000u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_COMPL, (void *) Ex_MQTTc_OnCmplCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_CONNECT_CMPL, (void *) Ex_MQTTc_OnConnectCmplCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_CMPL, (void *) Ex_MQTTc_OnPublishCmplCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_SUBSCRIBE_CMPL, (void *) Ex_MQTTc_OnSubscribeCmplCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_RX, (void *) Ex_MQTTc_OnPublishRxCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_ERR_CALLBACK, (void *) Ex_MQTTc_OnErrCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR, (void *)&Ex_MQTTc_ListenRxMsg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_TIMEOUT_MS, (void *) 30000u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Done setting params.\r\n");

  MQTTc_ConnOpen(&Ex_MQTTc_Conn, MQTTc_FLAGS_NONE, &err);       // Open conn to MQTT server with parameters set in Conn.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Done opening conn.\r\n");
  Ex_MQTTc_StatusMsgIsAvail = DEF_NO;

  MQTTc_Connect(&Ex_MQTTc_Conn, &Ex_MQTTc_StatusMsg, &err);     // Send CONNECT msg to MQTT server.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Done calling MQTTc_Connect().\r\n");
}

/****************************************************************************************************//**
 *                                     Ex_MQTTc_OnCmplCallbackFnct()
 *
 * @brief  Generic callback function for MQTTc module.
 *
 * @param  p_conn          Pointer to MQTTc Connection object for which operation has completed.
 *
 * @param  p_msg           Pointer to MQTTc Message object used for operation.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @param  err             Error code from processing message.
 *******************************************************************************************************/
static void Ex_MQTTc_OnCmplCallbackFnct(MQTTc_CONN *p_conn,
                                        MQTTc_MSG  *p_msg,
                                        void       *p_arg,
                                        RTOS_ERR   err)
{
  (void)&p_conn;
  (void)&p_arg;

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  switch (p_msg->Type) {
    case MQTTc_MSG_TYPE_CONNECT:                                // Gen callback called for event type: Connect Cmpl.
      EX_TRACE("Gen callback called for event type: Connect Cmpl.\n\r");
      break;

    case MQTTc_MSG_TYPE_PUBLISH:                                // Gen callback called for event type: Publish Cmpl.
      EX_TRACE("Gen callback called for event type: Publish Cmpl.\n\r");
      break;

    case MQTTc_MSG_TYPE_SUBSCRIBE:                              // Gen callback called for event type: Subscribe Cmpl.
      EX_TRACE("Gen callback called for event type: Subscribe Cmpl.\n\r");
      break;

    case MQTTc_MSG_TYPE_UNSUBSCRIBE:                            // Gen callback called for event type: Unsubscribe Cmpl.
      EX_TRACE("Gen callback called for event type: Unsubscribe Cmpl.\n\r");
      break;

    case MQTTc_MSG_TYPE_PINGREQ:                                // Gen callback called for event type: PingReq Cmpl.
      EX_TRACE("Gen callback called for event type: PingReq Cmpl.\n\r");
      break;

    case MQTTc_MSG_TYPE_DISCONNECT:                             // Gen callback called for event type: Disconnect Cmpl.
      EX_TRACE("Gen callback called for event type: Disconnect Cmpl.\n\r");
      break;

    default:
      EX_TRACE("Gen callback called for event type: default. !!! ERROR !!! %i\n\r", p_msg->Type);
      break;
  }
}

/****************************************************************************************************//**
 *                                 Ex_MQTTc_OnConnectCmplCallbackFnct()
 *
 * @brief  Callback function for MQTTc module called when a CONNECT operation has completed.
 *
 * @param  p_conn          Pointer to MQTTc Connection object for which operation has completed.
 *
 * @param  p_msg           Pointer to MQTTc Message object used for operation.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @param  err             Error code from processing CONNECT message.
 *******************************************************************************************************/
static void Ex_MQTTc_OnConnectCmplCallbackFnct(MQTTc_CONN *p_conn,
                                               MQTTc_MSG  *p_msg,
                                               void       *p_arg,
                                               RTOS_ERR   err)
{
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("ConnectCmpl callback called without err, ready to send/receive messages.\n\r");

  MQTTc_Subscribe(p_conn,
                  p_msg,                                    // Re-using msg used by completed CONNECT msg.
                  EX_MQTTc_DOMAIN_SUBSCRIBE_LISTEN,
                  EX_MQTTc_DOMAIN_SUBSCRIBE_LISTEN_QoS,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                 Ex_MQTTc_OnPublishCmplCallbackFnct()
 *
 * @brief  Callback function for MQTTc module called when a PUBLISH operation has completed.
 *
 * @param  p_conn          Pointer to MQTTc Connection object for which operation has completed.
 *
 * @param  p_msg           Pointer to MQTTc Message object used for operation.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @param  err             Error code from processing PUBLISH message.
 *******************************************************************************************************/
static void Ex_MQTTc_OnPublishCmplCallbackFnct(MQTTc_CONN *p_conn,
                                               MQTTc_MSG  *p_msg,
                                               void       *p_arg,
                                               RTOS_ERR   err)
{
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  if (p_msg == &Ex_MQTTc_StatusMsg) {
    EX_TRACE("PublishCmpl callback called for status. Marking message as available.\n\r");
    Ex_MQTTc_StatusMsgIsAvail = DEF_YES;                    // Mark msg as re-available.
  } else {
    EX_TRACE("PublishCmpl callback called for status. Marking message as available.\n\r");
    Ex_MQTTc_EchoMsgIsAvail = DEF_YES;                      // Mark msg as re-available.
  }
}

/****************************************************************************************************//**
 *                                Ex_MQTTc_OnSubscribeCmplCallbackFnct()
 *
 * @brief  Callback function for MQTTc module called when a SUBSCRIBE operation has completed.
 *
 * @param  p_conn          Pointer to MQTTc Connection object for which operation has completed.
 *
 * @param  p_msg           Pointer to MQTTc Message object used for operation.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @param  err             Error code from processing SUBSCRIBE message.
 *******************************************************************************************************/
static void Ex_MQTTc_OnSubscribeCmplCallbackFnct(MQTTc_CONN *p_conn,
                                                 MQTTc_MSG  *p_msg,
                                                 void       *p_arg,
                                                 RTOS_ERR   err)
{
  CPU_CHAR   *p_data = "Now listening on specified topic.";
  CPU_INT32U data_len = Str_Len(p_data);

  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("SubscribeCmpl callback called. Publishing status.\n\r");

  MQTTc_Publish(p_conn,
                p_msg,                                      // Re-using msg used by completed SUBSCRIBE msg.
                EX_MQTTc_DOMAIN_PUBLISH_STATUS,
                EX_MQTTc_DOMAIN_PUBLISH_STATUS_QoS,
                DEF_NO,
                (CPU_INT08U *)p_data,
                data_len,
                &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/****************************************************************************************************//**
 *                                  Ex_MQTTc_OnPublishRxCallbackFnct()
 *
 * @brief  Callback function for MQTTc module called when a PUBLISH message has been received.
 *
 * @param  p_conn          Pointer to MQTTc Connection object for which operation has completed.
 *
 * @param  topic_name_str  String containing the topic of the message received. NOT NULL-terminated.
 *
 * @param  topic_len       Length of the topic.
 *
 * @param  p_message       Pointer to buffer containing the message received.
 *
 * @param  message_len     Length of the message received.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @note   (1) This example assumes that the message received in the publish is a string ending
 *             with a null character, else the TRACE will caused problem.
 *******************************************************************************************************/
static void Ex_MQTTc_OnPublishRxCallbackFnct(MQTTc_CONN       *p_conn,
                                             const CPU_CHAR   *topic_name_str,
                                             CPU_INT32U       topic_len,
                                             const CPU_INT08U *p_message,
                                             CPU_INT32U       message_len,
                                             void             *p_arg,
                                             RTOS_ERR         err)
{
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Received PUBLISH message from server. Topic is %.*s.", topic_len, topic_name_str);
  EX_TRACE(" Message is %s.\n\r", (CPU_CHAR *)p_message);       // See Note #1.

  if (Ex_MQTTc_EchoMsgIsAvail == DEF_YES) {
    Ex_MQTTc_EchoMsgIsAvail = DEF_NO;
    MQTTc_Publish(p_conn,
                  &Ex_MQTTc_EchoMsg,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS_QoS,
                  DEF_NO,
                  (CPU_INT08U *)p_message,
                  message_len,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  } else if (Ex_MQTTc_StatusMsgIsAvail == DEF_YES) {
    CPU_CHAR   *p_data = "Unable to send echo msg: msg unavailable.";
    CPU_INT32U data_len = Str_Len(p_data);

    Ex_MQTTc_StatusMsgIsAvail = DEF_NO;
    MQTTc_Publish(p_conn,
                  &Ex_MQTTc_StatusMsg,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS_QoS,
                  DEF_NO,
                  (CPU_INT08U *)p_data,
                  data_len,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  } else {
    EX_TRACE("!!! APP ERROR !!! Echo and Status messages are both unavailable. Cannot send either Echo or Status to broker.\r\n");
  }
}

/****************************************************************************************************//**
 *                                     Ex_MQTTc_OnErrCallbackFnct()
 *
 * @brief  Callback function for MQTTc module called when an error occurs.
 *
 * @param  p_conn          Pointer to MQTTc Connection object on which error occurred.
 *
 * @param  p_arg           Pointer to argument set in MQTTc Connection using the parameter type
 *                         MQTTc_PARAM_TYPE_CALLBACK_ARG_PTR.
 *
 * @param  err             Error code.
 *******************************************************************************************************/
static void Ex_MQTTc_OnErrCallbackFnct(MQTTc_CONN *p_conn,
                                       void       *p_arg,
                                       RTOS_ERR   err)
{
  CPU_CHAR   *p_data = "Err detected.";
  CPU_INT32U data_len = Str_Len(p_data);

  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_arg);

#if ((RTOS_ERR_CFG_EXT_EN == DEF_ENABLED) \
  && (RTOS_ERR_CFG_STR_EN == DEF_ENABLED))
  EX_TRACE("!!! APP ERROR !!! Err detected via OnErr callback. Err = %s.", err.CodeText);
#else
  EX_TRACE("!!! APP ERROR !!! Err detected via OnErr callback. Err = %i.", err.Code);
#endif

  if (Ex_MQTTc_StatusMsgIsAvail == DEF_YES) {
    Ex_MQTTc_StatusMsgIsAvail = DEF_NO;

    EX_TRACE("Sending status.\r\n");
    MQTTc_Publish(p_conn,
                  &Ex_MQTTc_StatusMsg,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS,
                  EX_MQTTc_DOMAIN_PUBLISH_STATUS_QoS,
                  DEF_NO,
                  (CPU_INT08U *)p_data,
                  data_len,
                  &err);
    APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  } else {
    EX_TRACE("Unable to send status, message is not available.\r\n");
  }
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
