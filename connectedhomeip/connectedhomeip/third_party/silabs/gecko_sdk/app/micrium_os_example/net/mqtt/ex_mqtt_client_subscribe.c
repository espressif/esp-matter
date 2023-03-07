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
#define  EX_MQTTc_CLIENT_ID_NAME            "micrium-example-sub"
#endif

#define  EX_MQTTc_MSG_QTY                         2u

#define  EX_MQTTc_MSG_LEN_MAX                   128u
#define  EX_MQTTc_PUBLISH_RX_MSG_LEN_MAX        512u

//                                                                 Domain to which to subscribe.
#define  EX_MQTTc_DOMAIN_SUBSCRIBE                "domain/subscribe_topic"
#define  EX_MQTTc_DOMAIN_SUBSCRIBE_QoS            2u

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MQTTc_CONN Ex_MQTTc_Conn;

static MQTTc_MSG  Ex_MQTTc_Msg;
static CPU_INT08U Ex_MQTTc_MsgBuf[EX_MQTTc_MSG_LEN_MAX];

static MQTTc_MSG Ex_MQTTc_MsgPublishRx;
static MQTTc_MSG Ex_MQTTc_MsgPublishRxBuf[EX_MQTTc_PUBLISH_RX_MSG_LEN_MAX];

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

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_MQTTc_Subscribe()
 *
 * @brief  Open a connection and subscribe to a topic.
 *
 * @return  DEF_OK,   if NO error(s),
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
void Ex_MQTT_Client_Subscribe(void)
{
  RTOS_ERR err;

  MQTTc_MsgClr(&Ex_MQTTc_Msg, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_Msg, MQTTc_PARAM_TYPE_MSG_BUF_PTR, (void *)&Ex_MQTTc_MsgBuf[0u], &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_Msg, MQTTc_PARAM_TYPE_MSG_BUF_LEN, (void *)EX_MQTTc_MSG_LEN_MAX, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgClr(&Ex_MQTTc_MsgPublishRx, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_MsgPublishRx, MQTTc_PARAM_TYPE_MSG_BUF_PTR, (void *)&Ex_MQTTc_MsgPublishRxBuf[0u], &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_MsgSetParam(&Ex_MQTTc_MsgPublishRx, MQTTc_PARAM_TYPE_MSG_BUF_LEN, (void *)EX_MQTTc_PUBLISH_RX_MSG_LEN_MAX, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_ConnClr(&Ex_MQTTc_Conn, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  //                                                               Err handling should be done in your application.
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
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_SUBSCRIBE_CMPL, (void *) Ex_MQTTc_OnSubscribeCmplCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR, (void *)&Ex_MQTTc_MsgPublishRx, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_PUBLISH_RX, (void *) Ex_MQTTc_OnPublishRxCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_TIMEOUT_MS, (void *) 30000u, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_ConnOpen(&Ex_MQTTc_Conn, MQTTc_FLAGS_NONE, &err);       // Open conn to MQTT server with parameters set in Conn.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  MQTTc_Connect(&Ex_MQTTc_Conn, &Ex_MQTTc_Msg, &err);           // Send CONNECT msg to MQTT server.
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Initialization and CONNECT to server successful.\r\n");
}

/****************************************************************************************************//**
 *                                     Ex_MQTTc_OnCmplCallbackFnct()
 *
 * @breif  Generic callback function for MQTTc module.
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
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_arg);

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

  EX_TRACE("ConnectCmpl callback called without err, sending SUBSCRIBE message.\n\r");

  MQTTc_Subscribe(p_conn,
                  p_msg,
                  EX_MQTTc_DOMAIN_SUBSCRIBE,
                  EX_MQTTc_DOMAIN_SUBSCRIBE_QoS,
                  &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
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
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_msg);
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("SubscribeCmpl callback called. Can now receive PUBLISHed messages from server.\n\r");
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
 * @note  (1) This example assumes that the message received in the publish is a string ending
 *            with a null character, else the TRACE will caused problem.
 *******************************************************************************************************/
static void Ex_MQTTc_OnPublishRxCallbackFnct(MQTTc_CONN       *p_conn,
                                             const CPU_CHAR   *topic_name_str,
                                             CPU_INT32U       topic_len,
                                             const CPU_INT08U *p_message,
                                             CPU_INT32U       message_len,
                                             void             *p_arg,
                                             RTOS_ERR         err)
{
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(message_len);
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("Received PUBLISH message from server. Topic is %.*s.", topic_len, topic_name_str);
  EX_TRACE(" Message is %s.\n\r", (CPU_CHAR *)p_message);       // See Note #1.
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
