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
#define  EX_MQTTc_CLIENT_ID_NAME            "micrium-example-publish"
#endif

#ifndef EX_TRACE
#include  <stdio.h>
#define  EX_TRACE(...)                                      printf(__VA_ARGS__)
#endif

#define  EX_MQTTc_MSG_QTY                         1u

#define  EX_MQTTc_MSG_LEN_MAX                   128u
#define  EX_MQTTc_PUBLISH_RX_MSG_LEN_MAX        512u

//                                                                 Domain to which to publish.
#define  EX_MQTTc_DOMAIN_PUBLISH                   "domain/publish_topic"

#define  EX_MQTTc_PUBLISH_TEST_MSG                 "test publish"
#define  EX_MQTTc_PUBLISH_TEST_QoS                2u

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL GLOBAL VARIABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static MQTTc_CONN Ex_MQTTc_Conn;

static MQTTc_MSG  Ex_MQTTc_Msg;
static CPU_INT08U Ex_MQTTc_MsgBuf[EX_MQTTc_MSG_LEN_MAX];
static MQTTc_MSG  Ex_MQTTc_MsgPublishRx;
static CPU_INT08U Ex_MQTTc_MsgPublishRxBuf[EX_MQTTc_PUBLISH_RX_MSG_LEN_MAX];

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

static void Ex_MQTTc_OnErrCallbackFnct(MQTTc_CONN *p_conn,
                                       void       *p_arg,
                                       RTOS_ERR   err);

/********************************************************************************************************
 ********************************************************************************************************
 *                                          GLOBAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                          Ex_MQTTc_Publish()
 *
 * @brief   Open a connection and publish message from the callback.
 *
 * @return  DEF_OK,   if NO error(s),
 *          DEF_FAIL, otherwise.
 *******************************************************************************************************/
void Ex_MQTT_Client_Publish(void)
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
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_CALLBACK_ON_ERR_CALLBACK, (void *) Ex_MQTTc_OnErrCallbackFnct, &err);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
  MQTTc_ConnSetParam(&Ex_MQTTc_Conn, MQTTc_PARAM_TYPE_PUBLISH_RX_MSG_PTR, (void *)&Ex_MQTTc_MsgPublishRx, &err);
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
  RTOS_ERR err_local;

  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("ConnectCmpl callback called. Sending PUBLISH message.\n\r");

  MQTTc_Publish(p_conn,
                p_msg,
                EX_MQTTc_DOMAIN_PUBLISH,
                EX_MQTTc_PUBLISH_TEST_QoS,
                DEF_YES,
                (CPU_INT08U *)EX_MQTTc_PUBLISH_TEST_MSG,
                (CPU_INT32U)  Str_Len(EX_MQTTc_PUBLISH_TEST_MSG),
                &err_local);
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
  RTOS_ERR err_local;

  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );

  EX_TRACE("PublishCmpl callback called. Re-sending same message.\n\r");

  MQTTc_Publish(p_conn,
                p_msg,
                EX_MQTTc_DOMAIN_PUBLISH,
                EX_MQTTc_PUBLISH_TEST_QoS,
                DEF_YES,
                (CPU_INT08U *)EX_MQTTc_PUBLISH_TEST_MSG,
                (CPU_INT32U)  Str_Len(EX_MQTTc_PUBLISH_TEST_MSG),
                &err_local);
  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
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
  PP_UNUSED_PARAM(p_conn);
  PP_UNUSED_PARAM(p_arg);

  APP_RTOS_ASSERT_CRITICAL(err.Code == RTOS_ERR_NONE,; );
}

/********************************************************************************************************
 ********************************************************************************************************
 *                                   DEPENDENCIES & AVAIL CHECK(S) END
 ********************************************************************************************************
 *******************************************************************************************************/

#endif // RTOS_MODULE_NET_MQTT_CLIENT_AVAIL
