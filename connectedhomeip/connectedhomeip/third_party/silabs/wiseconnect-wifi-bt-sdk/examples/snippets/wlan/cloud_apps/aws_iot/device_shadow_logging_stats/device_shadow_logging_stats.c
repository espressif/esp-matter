/*******************************************************************************
 * @file  rsi_shadow_logging_stats_sample.c
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
 * @file shadow_sample.c
 * @brief A simple connected window example demonstrating the use of Thing Shadow
 */

/**
 * Include files
 * */
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

//! certificate includes
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"
#include "aws_starfield_ca.pem.h"

#define RSI_MQTT_TOPIC                   "$aws/things/Test_IoT/shadow/update"
#define HOST_ADDRESS_SIZE                255
#define ROOMTEMPERATURE_UPPERLIMIT       32.0f
#define ROOMTEMPERATURE_LOWERLIMIT       25.0f
#define STARTING_ROOMTEMPERATURE         ROOMTEMPERATURE_LOWERLIMIT
#define MAX_LENGTH_OF_UPDATE_JSON_BUFFER 200
#define DHCP_MODE                        1
#define SSID                             "SILABS_AP"
#define SECURITY_TYPE                    RSI_WPA2
#define PSK                              "1234567890"
#define GLOBAL_BUFF_LEN                  15000
#define PSP_MODE                         RSI_SLEEP_MODE_2
#define PSP_TYPE                         RSI_FAST_PSP
#define PKT_SEND_INTERVAL                55000
#define DNS_FAILED_COUNT                 5
#define RSI_APP_BUF_SIZE                 1600

#ifdef RSI_WITH_OS
#define RSI_MQTT_CLIENT_TASK_STACK_SIZE (512 * 4) //! Common task stack size
#define RSI_DRIVER_TASK_STACK_SIZE      (512 * 2) //! Driver task stack size
#define RSI_MQTT_CLIENT_TASK_PRIORITY   1         //! Common Initializations task priority
#define RSI_DRIVER_TASK_PRIORITY        2         //! Wireless driver task priority

rsi_task_handle_t common_init_task_handle = NULL;
rsi_task_handle_t wlan_task_handle        = NULL;
rsi_task_handle_t driver_task_handle      = NULL;
rsi_semaphore_handle_t rsi_shadow_sem;
#endif

//! Enumeration for states in application
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

static char HostAddress[HOST_ADDRESS_SIZE] = AWS_IOT_MQTT_HOST;
static uint32_t port                       = AWS_IOT_MQTT_PORT;
uint32_t wifi_connections, wifi_disconnections, rejoin_failures, dns_success, dns_query_fail;
uint32_t raising_time, falling_time, initial_time;
uint8_t global_buf[GLOBAL_BUFF_LEN];
char JsonDocumentBuffer[MAX_LENGTH_OF_UPDATE_JSON_BUFFER];
uint8_t mac_addr[6] = { 0 };
rsi_wlan_app_cb_t rsi_wlan_app_cb; //! application control block

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
extern int32_t rsi_wlan_filter_broadcast(uint16_t beacon_drop_threshold,
                                         uint8_t filter_bcast_in_tim,
                                         uint8_t filter_bcast_tim_till_next_cmd);
void rsi_wlan_async_module_state(uint16_t status, uint8_t *payload, const uint32_t payload_length);
void join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_wlan_async_module_state(uint16_t status, uint8_t *payload, const uint32_t payload_length)
{
  int i = 0, j = 0;
  char *unknown       = "unknown";
  char *higher_nibble = unknown;
  char *lower_nibble  = unknown;
  char *reason_code   = unknown;
  uint8_t bssid_string[18];
  if (payload_length != sizeof(rsi_state_notification_t))
    return;
  rsi_state_notification_t *state = (rsi_state_notification_t *)payload;
  for (i = 0; i < WLAN_MODULE_STATES; i++) {
    //higher nibble information
    if (STATE[i].bit == (state->StateCode & 0xF0)) {
      higher_nibble = STATE[i].string;
    }
    //lower nibble information
    if (STATE[i].bit == (state->StateCode & 0x0F)) {
      lower_nibble = STATE[i].string;
    }
  }
  for (j = 0; j < WLAN_REASON_CODES; j++) {
    //!reason code info
    if (REASONCODE[j].bit == (state->reason_code & 0xFF)) {
      reason_code = REASONCODE[j].string;
    }
  }
  if (higher_nibble == unknown && lower_nibble == unknown && reason_code == unknown)
    return;
  rsi_6byte_dev_address_to_ascii((uint8_t *)bssid_string, state->rsi_bssid);

  if (*higher_nibble == 'A') {
    wifi_connections++;
    LOG_PRINT("Wi-Fi connections %ld\n", wifi_connections);
  }
  if (*higher_nibble == 'U') {
    wifi_disconnections++;
    LOG_PRINT("Wi-Fi disconnections %ld\n", wifi_disconnections);
  }
  if (state->rsi_channel == 0 && state->rsi_rssi == 100) {

  } else if (state->rsi_channel == 0 && state->rsi_rssi != 100) {

  } else if (state->rsi_channel != 0 && state->rsi_rssi == 100) {

  } else {
  }
}
void join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  LOG_PRINT("Rejoin failure %ld\n", ++rejoin_failures);
  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
}
void rsi_remote_socket_terminate_handler1(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  //! Remote socket has been terminated
  rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
}
void rsi_wlan_app_callbacks_init(void)
{
  rsi_wlan_register_callbacks(RSI_WLAN_ASYNC_STATS, rsi_wlan_async_module_state);
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, join_fail_handler);
  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler1);
}

static void simulateRoomTemperature(float *pRoomTemperature)
{
  static float deltaChange;

  if (*pRoomTemperature >= ROOMTEMPERATURE_UPPERLIMIT) {
    deltaChange = -0.5f;
  } else if (*pRoomTemperature <= ROOMTEMPERATURE_LOWERLIMIT) {
    deltaChange = 0.5f;
  }

  *pRoomTemperature += deltaChange;
}

static void ShadowUpdateStatusCallback(const char *pThingName,
                                       ShadowActions_t action,
                                       Shadow_Ack_Status_t status,
                                       const char *pReceivedJsonDocument,
                                       void *pContextData)
{
  IOT_UNUSED(pThingName);
  IOT_UNUSED(action);
  IOT_UNUSED(pReceivedJsonDocument);
  IOT_UNUSED(pContextData);

  if (SHADOW_ACK_TIMEOUT == status) {
    LOG_PRINT("Update Timeout--\n");
  } else if (SHADOW_ACK_REJECTED == status) {
    LOG_PRINT("Update RejectedXX\n");
  } else if (SHADOW_ACK_ACCEPTED == status) {
    LOG_PRINT("Update Accepted !!\n");
  }
}

static void windowActuate_Callback(const char *pJsonString, uint32_t JsonStringDataLen, jsonStruct_t *pContext)
{
  IOT_UNUSED(pJsonString);
  IOT_UNUSED(JsonStringDataLen);

  if (pContext != NULL) {
    LOG_PRINT("Delta - Window state changed to %d", *(bool *)(pContext->pData));
  }
}

/*============================*/
void rsi_give_wakeup_indication()
{
  if (rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
    raising_time = rsi_hal_gettickcount();
    if (raising_time && falling_time) {
      LOG_PRINT("S %ld\n", (raising_time - falling_time));
    }
  }
  if (!rsi_hal_get_gpio(RSI_HAL_WAKEUP_INDICATION_PIN)) {
    falling_time = rsi_hal_gettickcount();
    if (raising_time && falling_time) {
      LOG_PRINT("W %ld\n", (falling_time - raising_time));
    }
  }
}
int rsi_device_shadow_logging_stats_app(void)
{
  int32_t status = RSI_SUCCESS;
#if !(DHCP_MODE)
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  uint8_t ip_rsp[18] = { 0 };
  IoT_Error_t rc     = FAILURE;

  ShadowInitParameters_t sp       = ShadowInitParametersDefault;
  ShadowConnectParameters_t scp   = ShadowConnectParametersDefault;
  size_t sizeOfJsonDocumentBuffer = sizeof(JsonDocumentBuffer) / sizeof(JsonDocumentBuffer[0]);
  float temperature               = 0.0;

  bool windowOpen = false;
  jsonStruct_t temperatureHandler;
  jsonStruct_t windowActuator;

  // initialize the mqtt client
  AWS_IoT_Client mqttClient = { 0 };

  windowActuator.cb         = windowActuate_Callback;
  windowActuator.pData      = &windowOpen;
  windowActuator.dataLength = sizeof(bool);
  windowActuator.pKey       = "windowOpen";
  windowActuator.type       = SHADOW_JSON_BOOL;

  temperatureHandler.cb         = NULL;
  temperatureHandler.pKey       = "temperature";
  temperatureHandler.pData      = &temperature;
  temperatureHandler.dataLength = sizeof(float);
  temperatureHandler.type       = SHADOW_JSON_FLOAT;

  sp.pHost               = HostAddress;
  sp.port                = port;
  sp.pClientCRT          = (char *)aws_client_certificate;
  sp.pClientKey          = (char *)aws_client_private_key;
  sp.pRootCA             = (char *)aws_starfield_ca;
  sp.enableAutoReconnect = NULL;
  sp.disconnectHandler   = NULL;

  //! Call MQTT client application
  rsi_hal_log_stats_intr_config(rsi_give_wakeup_indication);
  //!Module initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
#ifdef RSI_WITH_OS
  //! Create Semaphores
  rsi_semaphore_create(&rsi_shadow_sem, 0);
  // Wireless driver task
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

  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSend Feature Frame Success\r\n");
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, aws_starfield_ca, (sizeof(aws_starfield_ca) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoad SSL CA Certificate Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoad SSL CA Certificate Success\r\n");
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT, aws_client_certificate, (sizeof(aws_client_certificate) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoad SSL Client Certificate Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoad SSL Client Certificate Success\r\n");
  }

  status =
    rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY, aws_client_private_key, (sizeof(aws_client_private_key) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoad SSL Client Private Certificate Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoad SSL Client Private Certificate Success\r\n");
  }
  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_shadow_sem);
#endif
  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_shadow_sem, 0);
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        //! Active power state
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Power Save Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nWLAN Power Save Active\r\n");
        }
        rsi_wlan_app_callbacks_init();               //! register callback to initialize WLAN
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE; //! update WLAN application state to unconnected state
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
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
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wi-Fi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Wifi Connection failed\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to unconnected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
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

        //! Enable Broadcast data filter
        status = rsi_wlan_filter_broadcast(5000, 1, 1);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nBroadcast Data Filtering Failed with Error Code : 0x%lX\r\n", status);
          return status;
        }
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Power Save Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nWLAN Power Save Success\r\n");
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_INIT_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_SHADOW_INIT_STATE: {
        LOG_PRINT("\r\nShadow Initialization Start\r\n");
        rc = aws_iot_shadow_init(&mqttClient, &sp);
        if (SUCCESS != rc) {
          LOG_PRINT("\r\nShadow Initialization Error\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_INIT_STATE;
        } else {
          LOG_PRINT("\r\nShadow Initialization Done\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_CONNECT_STATE;
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
        rc = aws_iot_shadow_connect(&mqttClient, &scp);
        if (SUCCESS != rc) {
          if (rc == NETWORK_ERR_NET_UNKNOWN_HOST) {
            dns_query_fail += DNS_FAILED_COUNT;
            LOG_PRINT("\r\nDNS Query falied %ld\r\n", dns_query_fail);
            status = rsi_wlan_power_save_disable_and_enable(PSP_MODE, PSP_TYPE);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\nWLAN Power Save Failed, Error Code : 0x%lX\r\n", status);
              return status;
            } else {
              LOG_PRINT("\r\nWLAN Power Save Success\r\n");
            }
          }
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_CONNECT_STATE;
          LOG_PRINT("\r\nShadow Connection Error\r\n");

        } else {
          LOG_PRINT("\r\nShadow Connection Done\r\n");
          LOG_PRINT("\r\nDNS Query success %ld\r\n", ++dns_success);
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE: {
        /*
       * Enable Auto Reconnect functionality. Minimum and Maximum time of Exponential backoff are set in aws_iot_config.h
       *  #AWS_IOT_MQTT_MIN_RECONNECT_WAIT_INTERVAL
       *  #AWS_IOT_MQTT_MAX_RECONNECT_WAIT_INTERVAL
       */
        LOG_PRINT("\r\nSet Auto Reconnection Start\r\n");
        rc = aws_iot_shadow_set_autoreconnect_status(&mqttClient, true);
        if (SUCCESS != rc) {
          LOG_PRINT("\r\nUnable to set Auto Reconnect\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_AUTO_RECONNECT_SET_STATE;
        } else {
          LOG_PRINT("\r\nSet Auto Reconnection Done\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_REGISTER_DELTA;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_SHADOW_REGISTER_DELTA: {
        rc = aws_iot_shadow_register_delta(&mqttClient, &windowActuator);
        if (SUCCESS != rc) {
          LOG_PRINT("\r\nShadow Register Delta Error \r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_REGISTER_DELTA;
        } else {
          LOG_PRINT("\r\nShadow Register Delta Done\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SHADOW_YIELD_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_shadow_sem);
#endif
      } break;
      case RSI_WLAN_SHADOW_YIELD_STATE: {
        temperature = STARTING_ROOMTEMPERATURE;
        // loop and publish a change in temperature
        while (NETWORK_ATTEMPTING_RECONNECT == rc || NETWORK_RECONNECTED == rc || SUCCESS == rc
               || NETWORK_SSL_READ_TIMEOUT_ERROR == rc) {

          simulateRoomTemperature(&temperature);
          rc = aws_iot_shadow_init_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
          if (SUCCESS == rc) {
            rc = aws_iot_shadow_add_reported(JsonDocumentBuffer,
                                             sizeOfJsonDocumentBuffer,
                                             2,
                                             &temperatureHandler,
                                             &windowActuator);
            if (SUCCESS == rc) {
              rc = aws_iot_finalize_json_document(JsonDocumentBuffer, sizeOfJsonDocumentBuffer);
              if (SUCCESS == rc) {
                LOG_PRINT("\r\nUpdate Shadow: %s\r\n", JsonDocumentBuffer);
                rc = aws_iot_shadow_update(&mqttClient,
                                           AWS_IOT_MY_THING_NAME,
                                           JsonDocumentBuffer,
                                           ShadowUpdateStatusCallback,
                                           NULL,
                                           40,
                                           true);
              }
            }
          }
          if (SUCCESS != rc) {
            LOG_PRINT("\r\n error occurred in the loop \r\n");
            break;
          }
          rc = aws_iot_shadow_yield(&mqttClient, 2000);
          if (NETWORK_ATTEMPTING_RECONNECT == rc) {
            continue;
          }
          rsi_delay_ms(PKT_SEND_INTERVAL);
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

/* Main application entry point */
int main()
{
#ifdef RSI_WITH_OS
  //! Common Init task
  rsi_task_create((rsi_task_function_t)rsi_device_shadow_logging_stats_app,
                  (uint8_t *)"device_shadow_task",
                  RSI_MQTT_CLIENT_TASK_STACK_SIZE,
                  NULL,
                  RSI_MQTT_CLIENT_TASK_PRIORITY,
                  &wlan_task_handle);

  // Start the scheduler
  rsi_start_os_scheduler();
#else
  rsi_device_shadow_logging_stats_app();

  while (1) {
    rsi_wireless_driver_task();
  }
#endif
}
