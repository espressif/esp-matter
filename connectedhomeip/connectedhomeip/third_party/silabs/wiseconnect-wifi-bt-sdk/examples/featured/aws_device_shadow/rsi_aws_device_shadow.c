/*******************************************************************************
* @file  rsi_aws_device_shadow.c
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

// Include files
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#include "aws_iot_config.h"
#include "aws_iot_log.h"
#include "aws_iot_version.h"
#include "aws_iot_mqtt_client_interface.h"
#include "aws_iot_shadow_interface.h"
#include "rsi_data_types.h"
#include "rsi_common_apis.h"
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"
#include "rsi_wlan_config.h"
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

// Certificate includes
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

#define MQTT_TOPIC_LENGTH            255
#define RSI_MQTT_TOPIC               "$aws/things/Test_IoT/shadow/update"
#define ROOM_TEMPERATURE_UPPER_LIMIT 32.0f
#define ROOM_TEMPERATURE_LOWER_LIMIT 25.0f
#define STARTING_ROOM_TEMPERATURE    ROOM_TEMPERATURE_LOWER_LIMIT
#define JSON_DOC_BUFFER_LENGTH       200
#define SSID                         "SILABS_AP"
#define PSK                          "1234567890"
#define SECURITY_TYPE                RSI_WPA2
#define GLOBAL_BUFF_LEN              15000

// Application task priority and stack size
#define RSI_APPLICATION_TASK_PRIORITY   1
#define RSI_APPLICATION_TASK_STACK_SIZE (512 * 4)

// Wireless driver priority and stack size
#define RSI_DRIVER_TASK_PRIORITY   2
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2)

#define RSI_APP_BUF_SIZE 1600

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

#ifdef RSI_WITH_OS
rsi_task_handle_t application_handle = NULL;
rsi_task_handle_t driver_task_handle = NULL;
rsi_semaphore_handle_t rsi_shadow_sem;
#endif

// Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE                   = 0,
  RSI_WLAN_SCAN_STATE                      = 1,
  RSI_WLAN_UNCONNECTED_STATE               = 2,
  RSI_WLAN_CONNECTED_STATE                 = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE             = 4,
  RSI_WLAN_SHADOW_INIT_STATE               = 5,
  RSI_WLAN_SHADOW_CONNECT_STATE            = 6,
  RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE = 7,
  RSI_WLAN_SHADOW_REGISTER_DELTA           = 8,
  RSI_WLAN_SHADOW_YIELD_STATE              = 9,
  RSI_WLAN_SHADOW_IDLE_STATE               = 10
} rsi_wlan_app_state_t;

// WLAN application context
typedef struct rsi_wlan_app_context_s {

  // WLAN application state
  rsi_wlan_app_state_t state;

  // Length of buffer to copy
  uint32_t length;

  // application buffer
  uint8_t buffer[RSI_APP_BUF_SIZE];

  // Used to check application buffer availability
  uint8_t buf_in_use;

  // Application events bit map
  uint32_t event_map;

} rsi_wlan_app_context_t;

rsi_wlan_app_context_t rsi_wlan_app_context;
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint8_t mac_addr[6] = { 0 };
char json_document_buffer[JSON_DOC_BUFFER_LENGTH];
static char host_address[MQTT_TOPIC_LENGTH] = AWS_IOT_MQTT_HOST;
static uint32_t port                        = AWS_IOT_MQTT_PORT;

void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  // Remote socket has been terminated
}

void rsi_wlan_app_callbacks_init(void)
{
  // Initialize remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);
}

static void simulate_room_temperature(float *p_room_temperature, bool *p_window_open)
{
  static float delta_change;

  if (*p_room_temperature >= ROOM_TEMPERATURE_UPPER_LIMIT) {
    delta_change = -0.5f;
  } else if (*p_room_temperature <= ROOM_TEMPERATURE_LOWER_LIMIT) {
    delta_change = +0.5f;
  }

  *p_room_temperature += delta_change;

  *p_window_open = false;
  if (*p_room_temperature >= (ROOM_TEMPERATURE_UPPER_LIMIT + ROOM_TEMPERATURE_LOWER_LIMIT) / 2) {
    *p_window_open = true;
  }
}

static void shadow_update_status_callback(const char *p_thing_name,
                                          ShadowActions_t action,
                                          Shadow_Ack_Status_t status,
                                          const char *p_received_json_document,
                                          void *p_context_data)
{
  IOT_UNUSED(p_thing_name);
  IOT_UNUSED(action);
  IOT_UNUSED(p_received_json_document);
  IOT_UNUSED(p_context_data);

  if (SHADOW_ACK_TIMEOUT == status) {
    LOG_PRINT("Update Timeout--");
  } else if (SHADOW_ACK_REJECTED == status) {
    LOG_PRINT("Update RejectedXX");
  } else if (SHADOW_ACK_ACCEPTED == status) {
    LOG_PRINT("Update Accepted !!");
  }
}

static void window_actuate_callback(const char *p_json_string, uint32_t json_string_data_len, jsonStruct_t *p_context)
{
  IOT_UNUSED(p_json_string);
  IOT_UNUSED(json_string_data_len);

  if (p_context != NULL) {
    LOG_PRINT("Delta - Window state changed to %d", *(bool *)(p_context->pData));
  }
}

int32_t application(void)
{
  int32_t status         = RSI_SUCCESS;
  uint8_t ip_rsp[18]     = { 0 };
  IoT_Error_t aws_result = FAILURE;

  ShadowInitParameters_t sp           = ShadowInitParametersDefault;
  ShadowConnectParameters_t scp       = ShadowConnectParametersDefault;
  float temperature                   = 0.0;
  size_t size_of_json_document_buffer = sizeof(json_document_buffer) / sizeof(json_document_buffer[0]);

  bool window_open = false;
  jsonStruct_t temperature_handler;
  jsonStruct_t window_actuator;

  // Initialize the MQTT client
  AWS_IoT_Client mqtt_client = { 0 };

  window_actuator.cb         = window_actuate_callback;
  window_actuator.dataLength = sizeof(bool);
  window_actuator.pData      = &window_open;
  window_actuator.pKey       = "Window Open";
  window_actuator.type       = SHADOW_JSON_BOOL;

  temperature_handler.cb         = NULL;
  temperature_handler.dataLength = sizeof(float);
  temperature_handler.pData      = &temperature;
  temperature_handler.pKey       = "Temperature";
  temperature_handler.type       = SHADOW_JSON_FLOAT;

  sp.pHost               = host_address;
  sp.port                = port;
  sp.pClientCRT          = (char *)aws_client_certificate;
  sp.pClientKey          = (char *)aws_client_private_key;
  sp.pRootCA             = (char *)aws_starfield_ca;
  sp.enableAutoReconnect = NULL;
  sp.disconnectHandler   = NULL;

  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  // Initialize RS911x
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

#ifdef RSI_WITH_OS
  // Create Semaphore rsi_shadow_sem
  rsi_semaphore_create(&rsi_shadow_sem, 0);

  // Wireless driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  // Load Security Certificates
  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, aws_starfield_ca, (sizeof(aws_starfield_ca) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet TLS CA Certificate Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT, aws_client_certificate, (sizeof(aws_client_certificate) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet TLS Client Certificate Failed, Error Code : 0x%lX \r\n", status);
    return status;
  }

  status =
    rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, aws_client_private_key, (sizeof(aws_client_private_key) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet TLS Client Private Key Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  rsi_wlan_app_context.state = RSI_WLAN_INITIAL_STATE;

#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_shadow_sem);
#endif

  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_shadow_sem, 0);
#endif
    switch (rsi_wlan_app_context.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init(); // Register callback to initialize WLAN
        rsi_wlan_app_context.state = RSI_WLAN_SCAN_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SCAN_STATE: {
        // Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Scan Failed...\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SCAN_STATE;
        } else {
          rsi_wlan_app_context.state = RSI_WLAN_UNCONNECTED_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_UNCONNECTED_STATE: {
        // Connect to an Access point
        LOG_PRINT("\r\nWi-Fi Connecting ...");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" FAILED!\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          LOG_PRINT(" Success!\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_CONNECTED_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_CONNECTED_STATE: {
        // Configure IP Address
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, ip_rsp, 18, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("IP config fail...\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_CONNECTED_STATE;
        } else {
          rsi_wlan_app_context.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        }
#if ENABLE_POWER_SAVE
        //! Apply power save profile   PSP_MODE, PSP_TYPE
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_context.state = RSI_WLAN_SHADOW_INIT_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SHADOW_INIT_STATE: {
        LOG_PRINT("\r\nShadow Initialization Start\r\n");
        aws_result = aws_iot_shadow_init(&mqtt_client, &sp);
        if (aws_result == SUCCESS) {
          LOG_PRINT("\r\nShadow Initialization Done\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_CONNECT_STATE;
        } else {
          LOG_PRINT("\r\nShadow Initialization Error\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_INIT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SHADOW_CONNECT_STATE: {
        scp.pMyThingName    = AWS_IOT_MY_THING_NAME;
        scp.pMqttClientId   = AWS_IOT_MQTT_CLIENT_ID;
        scp.mqttClientIdLen = (uint16_t)strlen(AWS_IOT_MQTT_CLIENT_ID);

        LOG_PRINT("\r\nShadow Connection Start\r\n");
        aws_result = aws_iot_shadow_connect(&mqtt_client, &scp);
        if (aws_result == SUCCESS) {
          LOG_PRINT("\r\nShadow Connection Done\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE;
        } else {
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_CONNECT_STATE;
          LOG_PRINT("\r\nShadow Connection Error\r\n");
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE: {
        // Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
        //   #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
        //   #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
        LOG_PRINT("\r\nSet Auto Reconnection Start\r\n");
        aws_result = aws_iot_shadow_set_autoreconnect_status(&mqtt_client, true);
        if (aws_result == SUCCESS) {
          LOG_PRINT("\r\nSet Auto Reconnection Done\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_REGISTER_DELTA;
        } else {
          LOG_PRINT("\r\nUnable to set Auto Reconnect\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SHADOW_REGISTER_DELTA: {
        aws_result = aws_iot_shadow_register_delta(&mqtt_client, &window_actuator);
        if (aws_result == SUCCESS) {
          LOG_PRINT("\r\nShadow Register Delta Done\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_YIELD_STATE;
        } else {
          LOG_PRINT("\r\nShadow Register Delta Error\r\n");
          rsi_wlan_app_context.state = RSI_WLAN_SHADOW_REGISTER_DELTA;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      case RSI_WLAN_SHADOW_YIELD_STATE: {
        temperature = STARTING_ROOM_TEMPERATURE;

        // Loop and publish a change in temperature
        while ((NETWORK_ATTEMPTING_RECONNECT == aws_result) || (NETWORK_RECONNECTED == aws_result)
               || (SUCCESS == aws_result)) {
          aws_result = aws_iot_shadow_yield(&mqtt_client, 2000);
          if (NETWORK_ATTEMPTING_RECONNECT == aws_result) {
            continue;
          }

          simulate_room_temperature(&temperature, &window_open);
          aws_result = aws_iot_shadow_init_json_document(json_document_buffer, size_of_json_document_buffer);
          if (aws_result == SUCCESS) {
            aws_result = aws_iot_shadow_add_reported(json_document_buffer,
                                                     size_of_json_document_buffer,
                                                     2,
                                                     &temperature_handler,
                                                     &window_actuator);
            if (aws_result == SUCCESS) {
              aws_result = aws_iot_finalize_json_document(json_document_buffer, size_of_json_document_buffer);
              if (aws_result == SUCCESS) {
                LOG_PRINT("\r\nUpdate Shadow: %s\r\n", json_document_buffer);
                aws_result = aws_iot_shadow_update(&mqtt_client,
                                                   AWS_IOT_MY_THING_NAME,
                                                   json_document_buffer,
                                                   shadow_update_status_callback,
                                                   NULL,
                                                   40,
                                                   true);
              }
            }
          }

          rsi_delay_ms(1);
          if (aws_result != SUCCESS) {
            LOG_PRINT("\r\n Error occurred in the loop \r\n");
            rsi_wlan_app_context.state = RSI_WLAN_SHADOW_YIELD_STATE;
          }
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;

      default:
        break;
    }
  }
}

int main()
{
#ifdef RSI_WITH_OS
  // Create application task
  rsi_task_create((rsi_task_function_t)(int32_t)application,
                  (uint8_t *)"application_task",
                  RSI_APPLICATION_TASK_STACK_SIZE,
                  NULL,
                  RSI_APPLICATION_TASK_PRIORITY,
                  &application_handle);

  // Start the scheduler
  rsi_start_os_scheduler();
#else
  application();

  while (1) {
    rsi_wireless_driver_task();
  }
#endif
}
