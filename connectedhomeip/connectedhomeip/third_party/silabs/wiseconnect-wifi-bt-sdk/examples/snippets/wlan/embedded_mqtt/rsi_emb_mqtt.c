/*******************************************************************************
* @file  rsi_emb_mqtt.c
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

/**
 * Include files
 * */
//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"

#include "rsi_mqtt_client.h"

#include "rsi_wlan.h"
#include "rsi_utils.h"
#include "rsi_nwk.h"
#include "rsi_emb_mqtt_client.h"
#include "rsi_driver.h"
//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "1234567890"

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.10.101" //0x6500A8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF

#endif

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.10.100"

//! Server port number
#define SERVER_PORT 1234

//! Client port number
#define CLIENT_PORT 5001

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 1

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

//! Keep alive period
#define RSI_KEEP_ALIVE_PERIOD 0

//! QOS of the message
#define QOS 0

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

//! MQTT Related Macros and declarations
#define RSI_MQTT_TOPIC "SILABS_TEST"

//! Message to publish
uint8_t publish_message[] = "THIS IS MQTT CLIENT DEMO FROM SILABS";

//! MQTT CLient ID
int8_t clientID[] = "MQTTCLIENT";

//! user name for login credentials
int8_t username[] = "username";

//! password for login credentials
int8_t password[] = "password";
uint64_t ip_to_reverse_hex(char *ip);
volatile int halt = 0;

void rsi_emb_mqtt_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
}
void rsi_emb_mqtt_pub_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
}
void rsi_emb_mqtt_ka_timeout_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
}

int32_t rsi_mqtt_client_app()
{

  int32_t status = RSI_SUCCESS;
  uint8_t ip_buff[20];

#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  uint32_t server_address = ip_to_reverse_hex(SERVER_IP_ADDRESS);

  rsi_mqtt_pubmsg_t publish_msg;

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }
  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#ifdef RSI_WITH_OS

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nScan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nScan Success\r\n");
  }

  //! Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to Access point Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nConnect to Access point Success\r\n");
  }

  //! Configure IP
#if DHCP_MODE
  status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_buff, sizeof(ip_buff), 0);
#else
  status            = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                RSI_STATIC,
                                (uint8_t *)&ip_addr,
                                (uint8_t *)&network_mask,
                                (uint8_t *)&gateway,
                                NULL,
                                0,
                                0);
#endif
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nIP Config Success\r\n");
  LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);

#if ENABLE_POWER_SAVE
  //! Apply power save profile
  status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  //! Initialze remote terminate call back
  rsi_emb_mqtt_register_call_back(RSI_WLAN_NWK_EMB_MQTT_REMOTE_TERMINATE_CB,
                                  rsi_emb_mqtt_remote_socket_terminate_handler);
  rsi_emb_mqtt_register_call_back(RSI_WLAN_NWK_EMB_MQTT_PUB_MSG_CB, rsi_emb_mqtt_pub_handler);
  rsi_emb_mqtt_register_call_back(RSI_WLAN_NWK_EMB_MQTT_KEEPALIVE_TIMEOUT_CB, rsi_emb_mqtt_ka_timeout_handler);

  //! MQTT client initialisation
  status = rsi_emb_mqtt_client_init((int8_t *)&server_address,
                                    SERVER_PORT,
                                    CLIENT_PORT,
                                    RSI_EMB_MQTT_CLEAN_SESSION,
                                    RSI_KEEP_ALIVE_PERIOD,
                                    (int8_t *)clientID,
                                    (int8_t *)username,
                                    (int8_t *)password);

  if (status != RSI_SUCCESS) {
    return status;
  }
  //! Connect to the MQTT brohker/server
  status = rsi_emb_mqtt_connect((RSI_EMB_MQTT_USER_FLAG | RSI_EMB_MQTT_PWD_FLAG), NULL, 0, NULL);
  if (status != RSI_SUCCESS) {
    return status;
  }
  //! Subscribe to the topic given
  status = rsi_emb_mqtt_subscribe(QOS, (int8_t *)RSI_MQTT_TOPIC);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //!The DUP flag MUST be set to 1 by the Client or Server when it attempts to re-deliver a PUBLISH Packet
  //!The DUP flag MUST be set to 0 for all QoS 0 messages
  publish_msg.dup = 0;

  //! This field indicates the level of assurance for delivery of an Application Message. The QoS levels are
  //! 0  - At most once delivery
  //! 1  - At least once delivery
  //! 2  - Exactly once delivery
  publish_msg.qos = QOS;

  //! If the RETAIN flag is set to 1, in a PUBLISH Packet sent by a Client to a Server, the Server MUST store
  //! the Application Message and its QoS, so that it can be delivered to future subscribers whose
  //! subscriptions match its topic name
  publish_msg.retained = 0;

  //! Attach paylaod
  publish_msg.payload = publish_message;

  //! Fill paylaod length
  publish_msg.payloadlen = sizeof(publish_message);

  //! Publish message on the topic
  status = rsi_emb_mqtt_publish((int8_t *)RSI_MQTT_TOPIC, &publish_msg);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! UnSubscribe to the topic given
  status = rsi_emb_mqtt_unsubscribe((int8_t *)RSI_MQTT_TOPIC);
  if (status != RSI_SUCCESS) {
    //! Error in receiving
    return status;
  }
  //! Disconnect to the MQTT broker
  status = rsi_emb_mqtt_disconnect();
  if (status != RSI_SUCCESS) {
    //! Error in receiving
    return status;
  }
  status = rsi_emb_mqtt_destroy();
  if (status != RSI_SUCCESS) {
    //! Error in receiving
    return status;
  }
  return 0;
}

void main_loop(void)
{
  while (1) {
    ////////////////////////
    //! Application code ///
    ////////////////////////

    //! event loop
    rsi_wireless_driver_task();
  }
}

int main()
{
  int32_t status = RSI_SUCCESS;
#ifdef RSI_WITH_OS
  rsi_task_handle_t wlan_task_handle = NULL;
#endif

  //! Unmask interrupts

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_mqtt_client_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call MQTT client application
  status = rsi_mqtt_client_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}
