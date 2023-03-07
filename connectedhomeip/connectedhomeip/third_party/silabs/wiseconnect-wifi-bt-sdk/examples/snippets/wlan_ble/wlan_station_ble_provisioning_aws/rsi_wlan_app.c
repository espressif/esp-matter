/*******************************************************************************
 * @file  rsi_wlan_app.c
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Wlan Station BLE
 * Provisioning
 * @section Description :
 * This application explains how to get the WLAN connection functionality using
 * BLE provisioning.
 * SiLabs Module starts advertising and with BLE Provisioning the Access Point
 * details are fetched.
 * SiLabs device is configured as a WiFi station and connects to an Access Point.
 =================================================================================*/

/**
 * Include files
 * */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

#include "rsi_driver.h"
//! Caws iot includes
#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_error.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"

//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

//! WLAN include file for configuration
#include <rsi_wlan_config.h>

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"

#include "rsi_wlan_non_rom.h"

#include "rsi_bt_common_apis.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_chip.h"
#endif

#ifdef EFR32MG21A020F1024IM32
#include "em_cmu.h"
#include "em_gpio.h"
#include "tempdrv.h"
#endif

//! certificate includes
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

#define RSI_APP_BUF_SIZE 1600

rsi_semaphore_handle_t rsi_mqtt_sem;

/* Mqtt topics */
//! Subscribe Topic
#define MQTT_TOPIC1 "$aws/things/SmartLockWebApp-v01/shadow/update/device_status"
//! Publish Topic
#define MQTT_TOPIC2 "$aws/things/SmartLockWebApp-v01/shadow/update/delta/temperature_reading"

#define HOST_ADDRESS_SIZE 255

unsigned char *buff = NULL;
uint8_t yield;
/**
 * @brief Default MQTT HOST URL is pulled from the aws_iot_config.h
 */
static char HostAddress[HOST_ADDRESS_SIZE] = AWS_IOT_MQTT_HOST;

/**
 * @brief Default MQTT port is pulled from the aws_iot_config.h
 */
static uint32_t port = AWS_IOT_MQTT_PORT;

//! Message to publish
uint8_t publish_message_mq[] = "{\"state\":{\"desired\":{\"toggle\":1}}}";

AWS_IoT_Client client = { 0 };
IoT_Publish_Message_Params paramsQOS0;

//! user name for login credentials
int8_t username_mq[] = "username";

//! password for login credentials
int8_t password_mq[] = "password";

uint8_t power_save_given;
#define GPIO_PIN 0

#define DHCP_MODE       1
#define SSID            "SILABS_AP"
#define SECURITY_TYPE   RSI_WPA2
#define PSK             "123456789"
#define AWS_DOMAIN_NAME "a25jwtlmds8eip-ats.iot.us-east-2.amazonaws.com"

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Memory length for send buffer
#define BUF_SIZE 1400

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 1

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

/*
 *********************************************************************************************************
 *                                         LOCAL GLOBAL VARIABLES
 *********************************************************************************************************
 */

rsi_rsp_scan_t scan_result;
uint16_t scanbuf_size;
uint8_t connected = 0, timeout = 0;
uint8_t disconnected = 0, disassosiated = 0, wlan_connected;
uint8_t disconnect_flag = 0;
uint8_t a               = 0;
uint8_t rsp_buf[18];

/*
 *********************************************************************************************************
 *                                               DATA TYPES
 *********************************************************************************************************
 */

typedef enum rsi_app_cmd_e {
  RSI_DATA                = 0,
  RSI_WLAN_SCAN_RESP      = 1,
  RSI_WLAN_CONN_STATUS    = 2,
  RSI_WLAN_DISCONN_STATUS = 3,
  RSI_WLAN_DISCONN_NOTIFY = 4,
  RSI_WLAN_TIMEOUT_NOTIFY = 5

} rsi_app_cmd_t;

extern uint8_t coex_ssid[50], pwd[34], sec_type;
uint8_t retry = 1;

uint8_t conn_status;

#ifdef BLE_CONFIGURATOR
extern uint8_t magic_word;
#endif

extern rsi_wlan_app_cb_t rsi_wlan_app_cb;

//extern volatile uint32_t _dwTickCount;

uint32_t received_length = 0;
uint8_t temp_enable;

void rsi_wlan_mqtt_task(void);
extern void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);

//! Call back for Socket Async
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);  //This statement is added only to resolve compilation warning, value is unchanged
  received_length += length;
}

//! rejoin failure call back handler in station mode
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  //! update wlan application state
  disconnected          = 1;
  connected             = 0;
  rsi_wlan_app_cb.state = RSI_WLAN_DISCONNECTED_STATE;
}

void rsi_wlan_app_call_backs_init(void)
{
  //! Initialize join fail call back
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, rsi_join_fail_handler);
}

/**
 * @brief This parameter will avoid infinite loop of publish and exit the program after certain number of publishes
 */

static void iot_subscribe_callback_handler(AWS_IoT_Client *pClient,
                                           char *topicName,
                                           uint16_t topicNameLen,
                                           IoT_Publish_Message_Params *params,
                                           void *pData)
{
  UNUSED_PARAMETER(pClient);      //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(topicName);    //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(topicNameLen); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(pData);        //This statement is added only to resolve compilation warning, value is unchanged
  char dt[1000] = { 0 };
  uint32_t len;
  len = params->payloadLen;

  strncpy(dt, params->payload, len);
  LOG_PRINT("\r\n data received = %s\r\n", dt);
  rsi_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
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

int32_t rsi_wlan_mqtt_certs_init(void)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, NULL, 0);
  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT, NULL, 0);
  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, NULL, 0);

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
#ifndef BLE_CONFIGURATOR
  rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
#endif

  rsi_wlan_app_call_backs_init();

  return status;
}

void rsi_wlan_app_task()
{
  int32_t status = RSI_SUCCESS;
#ifdef RSI_WITH_OS
  while (1) {
#endif

    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        //rsi_delay_ms(6);
        //! update wlan application state

#if (BLE_CONFIGURATOR || BLE_SCANNER)

        //  rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        if (magic_word) {
          rsi_wlan_app_cb.state = RSI_WLAN_FLASH_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        }

#elif (STATIC_AP_OPEN || STATIC_AP_WPA2_SECURE || STATIC_AP_WPA_SECURE || STATIC_AP_MIXED)
      rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
#endif
      } break;

      case RSI_WLAN_UNCONNECTED_STATE: {
        //Any additional code if required
      } break;

      case RSI_WLAN_SCAN_STATE: {
#ifdef BLE_CONFIGURATOR
        scanbuf_size = sizeof(scan_result);
        memset(&scan_result, 0, sizeof(scan_result));

        status = rsi_wlan_scan(0, 0, &scan_result, scanbuf_size);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Scan Failed, Error Code : 0x%lX\r\n", status);
          break;
        } else {
          //! update wlan application state

#if WYZBEE_CONFIGURATOR
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
          rsi_wlan_app_send_to_ble(RSI_WLAN_SCAN_RESP, (uint8_t *)&scan_result, scanbuf_size);

#elif (STATIC_AP_OPEN || STATIC_AP_WPA2_SECURE || STATIC_AP_WPA_SECURE || STATIC_AP_MIXED)
          rsi_wlan_app_cb.state = RSI_WLAN_JOIN_STATE;
#endif
        }
#else
      status                = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nScan Failed...\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
      } else {
        LOG_PRINT("\r\n Scan Success\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_JOIN_STATE; // update WLAN application state to unconnected state
      }
#endif
      } break;

      case RSI_WLAN_JOIN_STATE: {
#ifdef BLE_CONFIGURATOR
        if (sec_type == 0 || sec_type == '0') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_OPEN, NULL);
        } else if (sec_type == 1 || sec_type == '1') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA, pwd);
        } else if (sec_type == 2 || sec_type == '2') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA2, pwd);
        } else if (sec_type == 6 || sec_type == '6') {
          status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA_WPA2_MIXED, pwd);
        }
#else
      status                  = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
#endif

#if STATIC_AP_OPEN
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_OPEN, NULL);
#endif

#if STATIC_AP_WPA2_SECURE
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA2, static_ap_pwd);
#endif

#if STATIC_AP_WPA_SECURE
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA, static_ap_pwd);
#endif

#if STATIC_AP_MIXED
        status = rsi_wlan_connect((int8_t *)static_ap_ssid, RSI_WPA_WPA2_MIXED, static_ap_pwd);
#endif

        if (status != RSI_SUCCESS) {
#if BLE_CONFIGURATOR
          a++;
          if (a == 5) {
            a       = 0;
            timeout = 1;
            rsi_wlan_app_send_to_ble(RSI_WLAN_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
            rsi_wlan_app_cb.state = RSI_WLAN_ERROR_STATE;
          }
#endif
          LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
          break;
        } else {
          a = 0;
          LOG_PRINT("\r\nAP Joined Successfully\r\n");
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        }
      } break;

      case RSI_WLAN_FLASH_STATE: {
        if (retry) {
#if WYZBEE_CONFIGURATOR
          if (sec_type == 0 || sec_type == '0') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_OPEN, NULL);
          } else if (sec_type == 1 || sec_type == '1') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA, pwd);
          } else if (sec_type == 2 || sec_type == '2') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA2, pwd);
          } else if (sec_type == 6 || sec_type == '6') {
            status = rsi_wlan_connect((int8_t *)coex_ssid, RSI_WPA_WPA2_MIXED, pwd);
          }
#endif
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nWLAN Connect Failed, Error Code : 0x%lX\r\n", status);
            break;
          } else {
            LOG_PRINT("\r\n Connected to AP\r\n");
            rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
          }
        }
      } break;

      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, rsp_buf, sizeof(rsp_buf), 0);
        if (status != RSI_SUCCESS) {
#if BLE_CONFIGURATOR
          a++;
          if (a == 3) {
            a       = 0;
            timeout = 1;
            status  = rsi_wlan_disconnect();
            if (status == RSI_SUCCESS) {
              connected     = 0;
              disassosiated = 1;
              rsi_wlan_app_send_to_ble(RSI_WLAN_TIMEOUT_NOTIFY, (uint8_t *)&timeout, 1);
              rsi_wlan_app_cb.state = RSI_WLAN_ERROR_STATE;
            }
          }
#endif
          LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
          break;
        } else {
          a             = 0;
          connected     = 1;
          conn_status   = 1;
          disconnected  = 0;
          disassosiated = 0;
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
          LOG_PRINT("\r\n IP Config Success \r\n");

#if BLE_CONFIGURATOR
          rsi_wlan_app_send_to_ble(RSI_WLAN_CONN_STATUS, (uint8_t *)&connected, 1);
#endif
        }
      } break;

      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        wlan_connected        = 1;
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_INIT_STATE;
#ifdef RSI_WITH_OS
        rsi_wlan_mqtt_task();
#endif
      } break;

      case RSI_WLAN_ERROR_STATE: {

      } break;

      case RSI_WLAN_DISCONNECTED_STATE: {
        retry = 1;
        rsi_wlan_app_send_to_ble(RSI_WLAN_DISCONN_STATUS, (uint8_t *)&disconnected, 1);
        rsi_wlan_app_cb.state = RSI_WLAN_FLASH_STATE;
      } break;

      case RSI_WLAN_DISCONN_NOTIFY_STATE: {
        status = rsi_wlan_disconnect();
        if (status == RSI_SUCCESS) {
#if RSI_WISE_MCU_ENABLE
          rsi_flash_erase((uint32_t)FLASH_ADDR_TO_STORE_AP_DETAILS);
#endif
          LOG_PRINT("\r\nWLAN Disconnected Successfully\r\n");
          disassosiated   = 1;
          connected       = 0;
          yield           = 0;
          disconnect_flag = 0; // reset flag to allow disconnecting again
          rsi_wlan_app_send_to_ble(RSI_WLAN_DISCONN_NOTIFY, (uint8_t *)&disassosiated, 1);
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nWLAN Disconnect Failed, Error Code : 0x%lX\r\n", status);
        }
      } break;
      default:

        break;
#ifdef RSI_WITH_OS
    }
#endif
  }
}

void rsi_wlan_mqtt_task()
{
  IoT_Error_t rc = FAILURE;
  uint32_t status;

  int8_t temp;
#ifndef EFR32MG21A020F1024IM32
  uint8_t temperature_upper_limit, temperature_lower_limit;
#endif
  char cpayload[150] = { 0 };

  IoT_Client_Init_Params mqttInitParams   = iotClientInitParamsDefault;
  IoT_Client_Connect_Params connectParams = iotClientConnectParamsDefault;

  IoT_Publish_Message_Params paramsQOS1;

  mqttInitParams.enableAutoReconnect       = false;
  mqttInitParams.pHostURL                  = HostAddress;
  mqttInitParams.port                      = port;
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
#ifdef RSI_WITH_OS
  while (1) {
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_MQTT_INIT_STATE: {

        rc = aws_iot_mqtt_init(&client, &mqttInitParams);
        if (SUCCESS != rc) {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_INIT_STATE;
          LOG_PRINT("aws_iot_mqtt_init returned error ");
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CONNECT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_CONNECT_STATE: {
        LOG_PRINT("\r\nAWS IOT MQTT Connecting...\r\n");
        rc = aws_iot_mqtt_connect(&client, &connectParams);
        if (SUCCESS != rc) {
          LOG_PRINT("\r\nError in connection\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CONNECT_STATE;
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
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_AUTO_RECONNECT_SET_STATE;
          LOG_PRINT("Unable to set Auto Reconnect to true\n ");
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_SUBSCRIBE_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_MQTT_SUBSCRIBE_STATE: {
        LOG_PRINT("\r\nAWS IOT MQTT Subscribe...\r\n");
        rc = aws_iot_mqtt_subscribe(&client,
                                    MQTT_TOPIC1,
                                    strlen(MQTT_TOPIC1) /*11*/,
                                    QOS0,
                                    iot_subscribe_callback_handler,
                                    /*paramsQOS0.payload */ client.clientData.readBuf);
        if (SUCCESS != rc) {
          LOG_PRINT("\r\nError subscribing\r\n ");
          rsi_wlan_app_cb.state = RSI_WLAN_MQTT_SUBSCRIBE_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_DATA_RECEIVE_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_DATA_RECEIVE_STATE: {

        if (yield == 0) {
          LOG_PRINT("\r\nWaiting for Data from Cloud...\r\n");
        }
        if (!power_save_given) {

#ifdef BLE_CONFIGURATOR
          //! initiating power save in BLE mode
          status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, RSI_MAX_PSP);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n Failed in initiating power save \r\n");
          }
#endif

          status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, RSI_MAX_PSP);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nPowersave Config Failed, Error Code : 0x%lX\r\n", status);
          }
          LOG_PRINT("\r\nPower save config Success\r\n");
          power_save_given = 1;
        }
        rc    = aws_iot_mqtt_yield(&client, 1000);
        yield = 1;
        if (NETWORK_ATTEMPTING_RECONNECT == rc) {
          //If the client is attempting to reconnect we will skip the rest of the function.
#ifdef RSI_WITH_OS
          continue;
#endif
        }
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_PUBLISH_STATE;
        return;
      } break;

      case RSI_WLAN_MQTT_PUBLISH_STATE: {

#ifdef EFR32MG21A020F1024IM32
        if (temp_enable == 0) {
          TEMPDRV_Init();
          TEMPDRV_Enable(1);
          temp_enable = 1;
        }
        //! Get Temperature value
        temp = TEMPDRV_GetTemp();
#else
      temperature_upper_limit = 40;
      temperature_lower_limit = 25;
      temp = (rand() % (temperature_upper_limit - temperature_lower_limit + 1)) + temperature_lower_limit;

#endif

        sprintf(cpayload, "{\"status\":  \"Current temperature in °C\"  ,\"value\": %d }", temp);
        LOG_PRINT("\r\n Current temperature in °C : %d\r\n ", temp);
        paramsQOS1.qos        = QOS1;
        paramsQOS1.payload    = (void *)cpayload;
        paramsQOS1.isRetained = 0;
        paramsQOS1.payloadLen = strlen((char *)cpayload);

        //! Publish message
        rc = aws_iot_mqtt_publish(&client, MQTT_TOPIC2, strlen(MQTT_TOPIC2), &paramsQOS1);
        if (rc == MQTT_REQUEST_TIMEOUT_ERROR) {
          LOG_PRINT("\r\nQOS1 publish ack not received.\r\n");
        } else if (rc == 0) {
          LOG_PRINT("\r\nData Published successfully to cloud...\r\n");
          yield = 0;
        } else {
          LOG_PRINT("\r\nData Published Fail :%d\r\n", rc);
        }
        if (disconnect_flag == 0) {
          rsi_wlan_app_cb.state = RSI_WLAN_DATA_RECEIVE_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_DISCONN_NOTIFY_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_mqtt_sem);
#endif
      } break;
      case RSI_WLAN_INITIAL_STATE:
      case RSI_WLAN_UNCONNECTED_STATE:
      case RSI_WLAN_CONNECTED_STATE:
      case RSI_WLAN_IPCONFIG_DONE_STATE:
      case RSI_WLAN_SOCKET_CONNECTED_STATE:
      case RSI_WLAN_SCAN_STATE:
      case RSI_WLAN_JOIN_STATE:
      case RSI_WLAN_SOCKET_RECEIVE_STATE:
      case RSI_WLAN_DISCONNECTED_STATE:
      case RSI_WLAN_DISCONN_NOTIFY_STATE:
      case RSI_WLAN_ERROR_STATE:
      case RSI_WLAN_FLASH_STATE:
      case RSI_SD_WRITE_STATE:
      case RSI_BLE_GATT_WRITE_EVENT:
      case RSI_WLAN_DEMO_COMPLETE_STATE:
        break;
    }
#ifdef RSI_WITH_OS
  }
#endif
}
