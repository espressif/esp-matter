/*
 * Copyright 2010-2015 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *  http://aws.amazon.com/apache2.0
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file subscribe_publish_sample.c
 * @brief simple MQTT publish and subscribe on the same topic
 *
 * This example takes the parameters from the aws_iot_config.h file and establishes a connection to the AWS IoT MQTT Platform.
 * It subscribes and publishes to the same topic - "sdkTest/sub"
 *
 * If all the certs are correct, you should see the messages received by the application in a loop.
 *
 * The application takes in the certificate path, host name , port and the number of times the publish should happen.
 *
 */

// Include files
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_error.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "rsi_data_types.h"
#include "rsi_common_apis.h"
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"
#include "rsi_socket.h"
#include "rsi_bootup_config.h"
#include "rsi_error.h"
#include "rsi_driver.h"
#include "rsi_os.h"
#include "rsi_wlan.h"
#include "rsi_nwk.h"
#include "rsi_utils.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_chip.h"
#include "rsi_board.h"
#endif

// certificate includes
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

#define RSI_MQTT_TOPIC     "$aws/things/Test_IoT/shadow/update"
#define DISCONNECTION_TIME 100
#define GPIO_PIN           0
#define DHCP_MODE          1
#define SSID               "SILABS_AP"
#define SECURITY_TYPE      RSI_WPA2
#define PSK                "1234567890"
#define GLOBAL_BUFF_LEN    15000

#ifdef RSI_WITH_OS
//RTOS DEFINES
#define RSI_MQTT_CLIENT_TASK_STACK_SIZE (512 * 2) //! Common task stack size
#define RSI_DRIVER_TASK_STACK_SIZE      (512 * 2) //! Driver task stack size
#define RSI_MQTT_CLIENT_TASK_PRIORITY   1         //! Common Initializations task priority
#define RSI_DRIVER_TASK_PRIORITY        2         //! Wireless driver task priority

rsi_task_handle_t common_init_task_handle = NULL;
rsi_task_handle_t wlan_task_handle        = NULL;
rsi_task_handle_t driver_task_handle      = NULL;
rsi_semaphore_handle_t rsi_mqtt_sem;
#endif

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

uint8_t publish_message_mq[] = "{\"state\":{\"desired\":{\"toggle\":1}}}";
int8_t username_mq[]         = "username";
int8_t password_mq[]         = "password";
uint8_t global_buf[GLOBAL_BUFF_LEN];

void rsi_remote_socket_terminate_handler1(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_CONST_PARAMETER(
    length); //This statement is added only to resolve compilation warning, value is unchanged terminated
}
void rsi_wlan_app_callbacks_init(void)
{
  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler1);
}
//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE                 = 0,
  RSI_WLAN_SCAN_STATE                    = 1,
  RSI_WLAN_UNCONNECTED_STATE             = 2,
  RSI_WLAN_CONNECTED_STATE               = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE           = 4,
  RSI_WLAN_DISCONNECT                    = 5,
  RSI_WLAN_MQTT_INIT_STATE               = 6,
  RSI_WLAN_MQTT_CONNECT_STATE            = 7,
  RSI_WLAN_MQTT_AUTO_RECONNECT_SET_STATE = 8,
  RSI_WLAN_MQTT_SUBSCRIBE_STATE          = 9,
  RSI_WLAN_MQTT_PUBLISH_STATE            = 10,
  RSI_WLAN_MQTT_RECEIVE_STATE            = 11,
  RSI_WLAN_MQTT_DISCONNECT               = 12,
} rsi_wlan_app_state_t;

#define RSI_APP_BUF_SIZE 1600
//! wlan application control block
typedef struct rsi_wlan_app_cb_s {
  //! wlan application state
  rsi_wlan_app_state_t state;

  //! length of buffer to copy
  uint32_t length;

  //! application buffer
  uint8_t buffer[RSI_APP_BUF_SIZE];

  //! to check application buffer availability
  uint8_t buf_in_use;

  //! application events bit map
  uint32_t event_map;

} rsi_wlan_app_cb_t;

rsi_wlan_app_cb_t rsi_wlan_app_cb; //! application control block

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */

static void iot_subscribe_callback_handler(AWS_IoT_Client *pClient,
                                           char *topicName,
                                           uint16_t topicNameLen,
                                           IoT_Publish_Message_Params *params,
                                           void *pData)
{
  UNUSED_PARAMETER(
    pClient); //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_PARAMETER(
    topicName); //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_PARAMETER(
    topicNameLen);          //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_PARAMETER(params); //This statement is added only to resolve compilation warning, value is unchanged terminated
  UNUSED_PARAMETER(pData);  //This statement is added only to resolve compilation warning, value is unchanged terminated
}

static void disconnectCallbackHandler(AWS_IoT_Client *pClient, void *data)
{

  IoT_Error_t rc = FAILURE;
  LOG_PRINT("MQTT Disconnect");
  if (NULL == pClient) {
    return;
  }
  IOT_UNUSED(data);
  if (aws_iot_is_autoreconnect_enabled(pClient)) {
    LOG_PRINT("Auto Reconnect is enabled, Reconnecting attempt will start now\n");
  } else {
    LOG_PRINT("Auto Reconnect not enabled. Starting manual reconnect...\n");
    rc = aws_iot_mqtt_attempt_reconnect(pClient);
    if (NETWORK_RECONNECTED == rc) {
      LOG_PRINT("Manual Reconnect Successful");
    } else {
      LOG_PRINT("Manual Reconnect \n");
    }
  }
}

int32_t rsi_mqtt_client_app()
{
  int32_t status = RSI_SUCCESS;
#ifdef MQTT_YIELD_EN
  uint32_t i;
#endif
#if !(DHCP_MODE)
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

  uint8_t ip_rsp[18]     = { 0 };
  uint8_t fw_version[10] = { 0 };

  IoT_Error_t rc = FAILURE;

  AWS_IoT_Client client                   = { 0 };
  IoT_Client_Init_Params mqttInitParams   = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

  IoT_Publish_Message_Params paramsQOS0;
  IoT_Publish_Message_Params paramsQOS1;

  mqttInitParams.enableAutoReconnect       = false;
  mqttInitParams.pHostURL                  = AWS_IOT_MQTT_HOST;
  mqttInitParams.port                      = AWS_IOT_MQTT_PORT;
  mqttInitParams.pRootCALocation           = (char *)aws_starfield_ca;
  mqttInitParams.pDeviceCertLocation       = (char *)aws_client_certificate;
  mqttInitParams.pDevicePrivateKeyLocation = (char *)aws_client_private_key;
  mqttInitParams.mqttCommandTimeout_ms     = 20000;
  mqttInitParams.tlsHandshakeTimeout_ms    = 5000;
  mqttInitParams.isSSLHostnameVerify       = true;
  mqttInitParams.disconnectHandler         = disconnectCallbackHandler;
  mqttInitParams.disconnectHandlerData     = NULL;

  connectParams.keepAliveIntervalInSec = 600;
  connectParams.isCleanSession         = true;
  connectParams.MQTTVersion            = MQTT_3_1_1;
  connectParams.pClientID              = AWS_IOT_MQTT_CLIENT_ID;
  connectParams.clientIDLen            = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);
  connectParams.isWillMsgPresent       = false;

  connectParams.pUsername   = (char *)username_mq;
  connectParams.usernameLen = strlen((char *)username_mq);
  connectParams.pPassword   = (char *)password_mq;
  connectParams.passwordLen = strlen((char *)password_mq);

  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! Silicon labs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%X\r\n", (unsigned int)status);
    return status;
  }
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

#ifdef RSI_WITH_OS
  //! Create Semaphores
  rsi_semaphore_create(&rsi_mqtt_sem, 0);
  //! Driver task
  rsi_task_create((void *)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_wlan_get(RSI_FW_VERSION, fw_version, 10);
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, aws_starfield_ca, (sizeof(aws_starfield_ca) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT, aws_client_certificate, (sizeof(aws_client_certificate) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }

  status =
    rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, aws_client_private_key, (sizeof(aws_client_private_key) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }
  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_mqtt_sem);
#endif
  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_mqtt_sem, 0);
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init();               //! register callback to initialize WLAN
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE; //! update WLAN application state to unconnected state
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_SCAN_STATE: {
        //! Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Scan Failed...\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update WLAN application state to unconnected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wi-Fi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Wi-Fi Connection failed\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to unconnected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_rsp, 18, 0);
#else
        status = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                      RSI_STATIC,
                                      (uint8_t *)&ip_addr,
                                      (uint8_t *)&network_mask,
                                      (uint8_t *)&gateway,
                                      NULL,
                                      0,
                                      0);
#endif
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Ip config fail...\n");
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        }
#if ENABLE_POWER_SAVE
        //! Apply power save profile
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_INIT_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_INIT_STATE: {
        rc = aws_iot_mqtt_init(&client, &mqttInitParams);
        if (SUCCESS != rc) {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_INIT_STATE;
          LOG_PRINT("aws_iot_mqtt_init returned error\n");
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CONNECT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_CONNECT_STATE: {
        LOG_PRINT("AWS IOT MQTT Connecting...\n");
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if (SUCCESS != rc) {
          if (rc == NETWORK_ALREADY_CONNECTED_ERROR) {
            LOG_PRINT("Network is already connected\n");
            rsi_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
          } else {
            LOG_PRINT("\r\nError in MQTT connection, Error Code : 0x%X\r\n", rc);
            rsi_wlan_app_cb.state = RSI_WLAN_MQTT_DISCONNECT;
          }
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_AUTO_RECONNECT_SET_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_AUTO_RECONNECT_SET_STATE: {
        rc = aws_iot_mqtt_autoreconnect_set_status(&client, false);
        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            LOG_PRINT("MQTT auto reconnect error\n");
            rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          LOG_PRINT("Unable to set Auto Reconnect to true\n ");
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_AUTO_RECONNECT_SET_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_SUBSCRIBE_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_SUBSCRIBE_STATE: {
        LOG_PRINT("AWS IOT MQTT Subscribe...\n");
        rc = aws_iot_mqtt_subscribe(&client,
                                    RSI_MQTT_TOPIC,
                                    strlen(RSI_MQTT_TOPIC) /*11*/,
                                    QOS0,
                                    iot_subscribe_callback_handler,
                                    /*paramsQOS0.payload */ client.clientData.readBuf);
        if (SUCCESS != rc) {
          if (NETWORK_DISCONNECTED_ERROR == rc) {
            LOG_PRINT("\r\nSubscribe error\n");
            rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CONNECT_STATE;
          } else if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            // If the client is attempting to reconnect we will skip the rest of the loop.
            continue;
          }
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_SUBSCRIBE_STATE;
        }
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;

      case RSI_WLAN_MQTT_PUBLISH_STATE: {
        LOG_PRINT("AWS IOT MQTT Publish...\n");
        paramsQOS0.qos        = QOS0;
        paramsQOS0.payload    = (void *)publish_message_mq;
        paramsQOS0.isRetained = 0;

        paramsQOS1.qos        = QOS1;
        paramsQOS1.payload    = (void *)publish_message_mq;
        paramsQOS1.isRetained = 0;
#ifdef MQTT_YIELD_EN
        LOG_PRINT("AWS IOT MQTT Yield ...\n");
#ifdef RSI_M4_INTERFACE
        //! Configure LED
        RSI_EGPIO_SetPinMux(EGPIO1, EGPIO_PORT0, GPIO_PIN, EGPIO_PIN_MUX_MODE0);
        RSI_EGPIO_SetDir(EGPIO1, EGPIO_PORT0, GPIO_PIN, EGPIO_CONFIG_DIR_OUTPUT);
#endif
        //! Waiting for data from cloud
        LOG_PRINT("Waiting for data from cloud...\n");
        rc = aws_iot_mqtt_yield(&client, 1000);
        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
          // If the client is attempting to reconnect we will skip the rest of the loop.
          continue;
        }
        //! toggle LED based on msg received from cloud
        for (i = 0; i < AWS_IOT_MQTT_RX_BUF_LEN; i++) {
          if (client.clientData.readBuf[i] == 't' && client.clientData.readBuf[i + 1] == 'o'
              && client.clientData.readBuf[i + 2] == 'g') {
            LOG_PRINT("Toggling LED\n");
#ifdef RSI_M4_INTERFACE
            RSI_EGPIO_TogglePort(EGPIO1, EGPIO_PORT0, (0x1 << GPIO_PIN));
#endif
            break;
          }
        }
#endif
        paramsQOS0.payloadLen = strlen((char *)publish_message_mq);
        rc                    = aws_iot_mqtt_publish(&client, RSI_MQTT_TOPIC, strlen(RSI_MQTT_TOPIC), &paramsQOS0);

        paramsQOS1.payloadLen = strlen((char *)publish_message_mq);
        rc                    = aws_iot_mqtt_publish(&client, RSI_MQTT_TOPIC, strlen(RSI_MQTT_TOPIC), &paramsQOS1);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
          LOG_PRINT("QOS1 publish ack not received.\n");
        }
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_DISCONNECT: {
        rc = aws_iot_mqtt_disconnect(&client);
        if (SUCCESS != rc) {
          LOG_PRINT("MQTT disconnection error\n");
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_DISCONNECT;
        }
        rsi_wlan_app_cb.state = RSI_WLAN_DISCONNECT;
      }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
        break;
      case RSI_WLAN_DISCONNECT: {
        status = rsi_wlan_disconnect();
        if (SUCCESS != status) {
          LOG_PRINT("WLAN disconnection error\n");
          rsi_wlan_app_cb.state = RSI_WLAN_DISCONNECT;
        }
        rsi_delay_ms(DISCONNECTION_TIME);
        rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
      }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
        break;
      default:
        break;
    }
  }
}

int main()
{
#ifdef RSI_WITH_OS
  //! Common Init task
  rsi_task_create((void *)rsi_mqtt_client_app,
                  (uint8_t *)"mqtt_client_task",
                  RSI_MQTT_CLIENT_TASK_STACK_SIZE,
                  NULL,
                  RSI_MQTT_CLIENT_TASK_PRIORITY,
                  &common_init_task_handle);

  //! OS Task Start the scheduler
  rsi_start_os_scheduler();
#else
  rsi_mqtt_client_app();

  while (1) {
    rsi_wireless_driver_task();
  }
#endif
}
