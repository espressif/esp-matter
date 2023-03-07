/*******************************************************************************
* @file  rsi_azure_telemetry_c2d.c
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
 * Includes
 */
#include <stdlib.h>

#include "iothub.h"
#include "iothub_device_client_ll.h"
#include "iothub_client_options.h"
#include "iothub_message.h"
#include "azure_c_shared_utility/threadapi.h"
#include "azure_c_shared_utility/crt_abstractions.h"
#include "azure_c_shared_utility/shared_util_options.h"
#include "rsi_data_types.h"
#include "rsi_common_apis.h"
#include "rsi_wlan_apis.h"
#include "rsi_socket.h"
#include "rsi_bootup_config.h"
#include "rsi_error.h"
#include "rsi_os.h"
#include "rsi_wlan.h"
#include "rsi_nwk.h"
#include "rsi_utils.h"
#include "rtc.h"
#include "rsi_wlan_config.h"
#include "azure_iot_config.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#define SAMPLE_MQTT
#ifdef SAMPLE_MQTT
#include "iothubtransportmqtt.h"
#endif
#ifdef SAMPLE_MQTT_OVER_WEBSOCKETS
#include "iothubtransportmqtt_websockets.h"
#endif
#ifdef SAMPLE_AMQP
#include "iothubtransportamqp.h"
#endif
#ifdef SAMPLE_AMQP_OVER_WEBSOCKETS
#include "iothubtransportamqp_websockets.h"
#endif
#ifdef SAMPLE_HTTP
#include "iothubtransporthttp.h"
#endif

// Certificate includes
#include "azure_client_certificate.pem.crt.h"
#include "azure_client_private_key.pem.key.h"
#include "azure_baltimore_ca.pem.h"

#define GLOBAL_BUFF_LEN 8000
#define SSID            "SILABS_AP"
#define SECURITY_TYPE   RSI_WPA2
#define PSK             "1234567890"
#define DHCP_MODE       1

//NTP Related parameters
#define NTP_TIMESTAMP_DELTA 2208988800
#define LI(packet)          (uint8_t)((packet.li_vn_mode & 0xC0) >> 6) // (li   & 11 000 000) >> 6  //Leap Indicator
#define VN(packet)          (uint8_t)((packet.li_vn_mode & 0x38) >> 3) // (vn   & 00 111 000) >> 3	 //Version number
#define MODE(packet)        (uint8_t)((packet.li_vn_mode & 0x07) >> 0) // (mode & 00 000 111) >> 0 // Mode
#define rsi_ntohl(a) \
  ((((a)&0xff000000) >> 24) | (((a)&0x00ff0000) >> 8) | (((a)&0x0000ff00) << 8) | (((a)&0x000000ff) << 24))
char *host_name = "in.pool.ntp.org";

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

#ifdef RSI_WITH_OS
// Application task priority and stack size
#define RSI_APPLICATION_TASK_PRIORITY   1
#define RSI_APPLICATION_TASK_STACK_SIZE (512 * 4)
// Wireless driver priority and stack size
#define RSI_DRIVER_TASK_PRIORITY   2
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2)
rsi_task_handle_t application_handle = NULL;
rsi_task_handle_t driver_task_handle = NULL;
rsi_semaphore_handle_t rsi_azure_sem;
#endif

typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE             = 0,
  RSI_WLAN_SCAN_STATE                = 1,
  RSI_WLAN_UNCONNECTED_STATE         = 2,
  RSI_WLAN_CONNECTED_STATE           = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE       = 4,
  RSI_WLAN_IOTHUB_LL_INIT_STATE      = 5,
  RSI_WLAN_IOTHUB_LL_SETOPTION_STATE = 6,
  RSI_WLAN_IOTHUB_LL_DOWORK_STATE    = 7,
  RSI_WLAN_IOTHUB_LL_DEINIT_STATE    = 8,
  RSI_WLAN_EXIT_STATE                = 9,
  RSI_WLAN_MAX_STATE                 = 10
} rsi_wlan_app_state_t;

//! wlan application control block
typedef struct rsi_wlan_app_cb_s {
  //! wlan application state
  rsi_wlan_app_state_t state;

} rsi_wlan_app_cb_t;

//! application control block
rsi_wlan_app_cb_t rsi_wlan_app_cb;

//Structure that defines the 48 byte NTP packet protocol.
typedef struct {
  uint8_t li_vn_mode;      // Eight bits. li, vn, and mode.
                           // li.   Two bits.   Leap indicator.
                           // vn.   Three bits. Version number of the protocol.
                           // mode. Three bits. Client will pick mode 3 for client.
  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.
  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.
  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.
  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.
  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.
  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.
} ntp_packet;              // Total: 384 bits or 48 bytes.

static bool g_continueRunning                    = true;
static size_t g_message_count_send_confirmations = 0;
static size_t g_message_recv_count               = 0;
bool network_connect                             = false; //Variable to check if the connect to network is successful
uint8_t nw_retry_count                           = 0;
uint8_t ntp_retry_count                          = 0;
uint8_t global_buf[GLOBAL_BUFF_LEN];

//fetching Current Date and time from the server //
uint32_t rsi_fetch_set_current_time(bool *set_status)
{
  rsi_rsp_dns_query_t dns_query_rsp  = { 0 };
  int32_t client_socket              = 0;
  struct rsi_sockaddr_in server_addr = { 0 };
  int32_t status                     = RSI_SUCCESS;
  int32_t set_retval                 = RSI_SUCCESS;
  uint8_t count                      = DNS_REQ_COUNT;
  uint32_t server_address            = 0;
  ntp_packet packet                  = { 0 };
  uint32_t epoch_time                = 0;
  struct rsi_timeval timeout         = { 0 };

  // Set the rsi_recv timeout
  memset(&timeout, 0, sizeof(timeout));
  timeout.tv_usec = TIMEOUT_VAL_NTP_MS * 1000;
  timeout.tv_sec  = 0;

  memset(&packet, 0, sizeof(ntp_packet));

  // Perform DNS to resolve the ntp server domain name
  do {
    status = rsi_dns_req(RSI_IP_VERSION_4, (uint8_t *)host_name, NULL, NULL, &dns_query_rsp, sizeof(dns_query_rsp));
    if (status == RSI_SUCCESS) {
      break;
    }
    count--;
  } while (count != 0);

  if (status != RSI_SUCCESS) {
    // Return if DNS resolution fails
    return status;
  }

  server_address = rsi_bytes4R_to_uint32(dns_query_rsp.ip_address[0].ipv4_address);
  char *addr     = (char *)&server_address;
  client_socket  = rsi_socket(AF_INET, SOCK_DGRAM, 0); // Create a UDP socket.
  if (client_socket == RSI_SOCK_ERROR) {
    status = rsi_wlan_get_status();
    LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%X\r\n", (unsigned int)status);
    return status;
  } else {
    LOG_PRINT("\r\nSocket Create Success\r\n");
  }
  // Set server structure
  memset(&server_addr, 0, sizeof(server_addr));
  // Set server address family
  server_addr.sin_family = AF_INET;
  // Set server port number, using htons function to use proper byte order
  server_addr.sin_port = htons(NTP_SERVER_PORT);
  // Set IP address to localhost
  server_addr.sin_addr.s_addr = rsi_bytes4R_to_uint32((uint8_t *)addr);

  status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
  if (status != RSI_SUCCESS) {
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket, 0);
    return status;
  } else {
    LOG_PRINT("\rconnect success\n");
  }

  LOG_PRINT("\r\nSend NTP time Req\r\n");
  // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
  *((char *)&packet + 0) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

  //! Send data on socket
  status = rsi_send(client_socket, (int8_t *)&packet, (sizeof(ntp_packet)), 0);
  if (status < 0) {
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket, 0);
    LOG_PRINT("\r\nFailed to send data to UDP Server, Error Code : 0x%X\r\n", (unsigned int)status);
    return status;
  }

  status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
  if (status != RSI_SUCCESS) {
    status = rsi_wlan_socket_get_status(client_socket);
    rsi_shutdown(client_socket, 0);
    return status;
  }

  status = rsi_recv(client_socket, (char *)&packet, (sizeof(ntp_packet)), 0);
  if (status < 0) {
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket, 0);
    return status;
  } else {
    LOG_PRINT("Received Time from NTP server\r\n");
  }

  // Convert the port number integer to network big-endian style and save it to the server address structure.
  packet.txTm_s = rsi_ntohl(packet.txTm_s); // Time-stamp seconds.
  packet.txTm_f = rsi_ntohl(packet.txTm_f);
  epoch_time    = ((packet.txTm_s) - NTP_TIMESTAMP_DELTA);

  // Set the time fetched from the network to RTP Set time
  set_retval = set_time(epoch_time);
  if (set_retval == RSI_FAILURE) {
    LOG_PRINT("Set time failure observed, Exit the application");
    *set_status = false;
  }

  // Close the socket connection
  rsi_shutdown(client_socket, 0);

  *set_status = true;
  return RSI_SUCCESS;
}

static void send_confirm_callback(IOTHUB_CLIENT_CONFIRMATION_RESULT result, void *userContextCallback)
{
  (void)userContextCallback;
  //! When a message is sent this callback will get envoked
  g_message_count_send_confirmations++;
  LOG_PRINT("Confirmation callback received for message %lu with result %s\r\n",
            (unsigned long)g_message_count_send_confirmations,
            MU_ENUM_TO_STRING(IOTHUB_CLIENT_CONFIRMATION_RESULT, result));
}

static void connection_status_callback(IOTHUB_CLIENT_CONNECTION_STATUS result,
                                       IOTHUB_CLIENT_CONNECTION_STATUS_REASON reason,
                                       void *user_context)
{
  (void)reason;
  (void)user_context;
  //! This sample DOES NOT take into consideration network outages.
  if (result == IOTHUB_CLIENT_CONNECTION_AUTHENTICATED) {
    network_connect = true; // Set to true as network connectivity is successful
    LOG_PRINT("The device client is connected to iothub\r\n");
  } else { //RSC-10025 : Handling of Error reasons. Retry for reason other than MQTT connection failure.
    switch (reason) {
      case IOTHUB_CLIENT_CONNECTION_EXPIRED_SAS_TOKEN:
        LOG_PRINT(
          "Connection failed with reason CONNECTION_EXPIRED_SAS_TOKEN. Do not Retry. Exit the application.\r\n");
        g_continueRunning = false;
        break;
      case IOTHUB_CLIENT_CONNECTION_DEVICE_DISABLED:
        LOG_PRINT("Connection failed with reason CONNECTION_DEVICE_DISABLED. Do not Retry. Exit the application.\r\n");
        g_continueRunning = false;
        break;
      case IOTHUB_CLIENT_CONNECTION_BAD_CREDENTIAL:
        LOG_PRINT("Connection failed with reason CONNECTION_BAD_CREDENTIAL. Do not Retry. Exit the application.\r\n");
        g_continueRunning = false;
        break;
      case IOTHUB_CLIENT_CONNECTION_RETRY_EXPIRED:
        LOG_PRINT("Connection failed with reason CONNECTION_RETRY_EXPIRED. Retry the connection. \r\n");
        nw_retry_count++;
        break;
      case IOTHUB_CLIENT_CONNECTION_NO_NETWORK:
        LOG_PRINT("Connection failed with reason CONNECTION_NO_NETWORK. Retry the connection. \r\n");
        nw_retry_count++;
        break;
      case IOTHUB_CLIENT_CONNECTION_COMMUNICATION_ERROR:
        LOG_PRINT("Connection failed with reason CONNECTION_COMMUNICATION_ERROR. Retry the connection. \r\n");
        nw_retry_count++;
        break;
      default:
        // In the deafult case do not retry the connection. Exit the application.
        // For other reasons if explicit handling is needed separate case is to be written.
        LOG_PRINT("CLient Connection Error.:w! Do not retry. Exit the application. \r\n");
        g_continueRunning = false;
        break;
    } // End of else

    LOG_PRINT("network connect retry count = %d\r\n", nw_retry_count);
  }
}

//! Remote socket terminate call back handler
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  //! Remote socket has been terminated
}

void rsi_wlan_app_callbacks_init(void)
{
  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);
}
static IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void *user_context)
{
  (void)user_context;
  const char *messageId;
  const char *correlationId;

  //! Message properties
  if ((messageId = IoTHubMessage_GetMessageId(message)) == NULL) {
    messageId = "<unavailable>";
  }

  if ((correlationId = IoTHubMessage_GetCorrelationId(message)) == NULL) {
    correlationId = "<unavailable>";
  }

  IOTHUBMESSAGE_CONTENT_TYPE content_type = IoTHubMessage_GetContentType(message);
  if (content_type == IOTHUBMESSAGE_BYTEARRAY) {
    const unsigned char *buff_msg;
    size_t buff_len;

    if (IoTHubMessage_GetByteArray(message, &buff_msg, &buff_len) != IOTHUB_MESSAGE_OK) {
      LOG_PRINT("Failure retrieving byte array message\r\n");
    } else {
      LOG_PRINT("Received Binary message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%.*s>>> & Size=%d\r\n",
                messageId,
                correlationId,
                (int)buff_len,
                buff_msg,
                (int)buff_len);
    }
  } else {
    const char *string_msg = IoTHubMessage_GetString(message);
    if (string_msg == NULL) {
      LOG_PRINT("Failure retrieving byte array message\r\n");
    } else {
      LOG_PRINT("Received String Message\r\nMessage ID: %s\r\n Correlation ID: %s\r\n Data: <<<%s>>>\r\n",
                messageId,
                correlationId,
                string_msg);
    }
  }
  const char *property_value = "property_value";
  const char *property_key   = IoTHubMessage_GetProperty(message, property_value);
  if (property_key != NULL) {
    LOG_PRINT("\r\nMessage Properties:\r\n");
    LOG_PRINT("\tKey: %s Value: %s\r\n", property_value, property_key);
  }
  g_message_recv_count++;

  return IOTHUBMESSAGE_ACCEPTED;
}

int rsi_azure_client_app(void)
{
  int32_t status = RSI_SUCCESS;
#if !(DHCP_MODE)
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#endif
  uint8_t ip_rsp[18]     = { 0 };
  uint8_t fw_version[10] = { 0 };
  IOTHUB_CLIENT_TRANSPORT_PROVIDER protocol;
  IOTHUB_MESSAGE_HANDLE message_handle;
  size_t messages_sent      = 0;
  const char *telemetry_msg = "test_message";
  bool set_status           = true;

  //! Select the Protocol to use with the connection
#ifdef SAMPLE_MQTT
  protocol = MQTT_Protocol;
#endif
#ifdef SAMPLE_MQTT_OVER_WEBSOCKETS
  protocol = MQTT_WebSocket_Protocol;
#endif
#ifdef SAMPLE_AMQP
  protocol = AMQP_Protocol;
#endif
#ifdef SAMPLE_AMQP_OVER_WEBSOCKETS
  protocol = AMQP_Protocol_over_WebSocketsTls;
#endif
#ifdef SAMPLE_HTTP
  protocol = HTTP_Protocol;
#endif

  IOTHUB_DEVICE_CLIENT_LL_HANDLE device_ll_handle = NULL;

#ifdef RSI_M4_INTERFACE
  SystemCoreClockUpdate();
#endif

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
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

#ifdef RSI_WITH_OS
  //Create Semaphores
  rsi_semaphore_create(&rsi_azure_sem, 0);
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

  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, azure_baltimore_ca, (sizeof(azure_baltimore_ca) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }
  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT, azure_client_certificate, (sizeof(azure_client_certificate) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_wlan_set_certificate(RSI_SSL_CLIENT_PRIVATE_KEY,
                                    azure_client_private_key,
                                    (sizeof(azure_client_private_key) - 1));
  if (status != RSI_SUCCESS) {
    return status;
  }

  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_azure_sem);
#endif
  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_azure_sem, 0);
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        //! register callback to initialize WLAN
        rsi_wlan_app_callbacks_init();
        //! update WLAN application state to unconnected state
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_SCAN_STATE: {
        //! Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Scan Failed...\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
        } else {
          //! update WLAN application state to unconnected state
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wifi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Wifi Connection failed\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          //! update WLAN application state to unconnected state
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_CONNECTED_STATE: {
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, ip_rsp, 18, 0);
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
          LOG_PRINT("Ip config DONE...\n");
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        }
#if ENABLE_POWER_SAVE
        //! Apply power save profile
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("power save mode is failed...\n");
          return status;
        }
#endif

#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        //set current Date and Time
        status = rsi_fetch_set_current_time(&set_status);
        if (status != RSI_SUCCESS) {
          ntp_retry_count++;
          LOG_PRINT("NTP fetch current time failed. Retry count = %d\r\n", ntp_retry_count);
          if (ntp_retry_count <= MAX_NTP_RETRY_COUNT) {
            // retry to connect to NTP server and get the time till max retry count is reached
            rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
          } else {
            // Max retry has been reached to get the current time from NTP server, exit the application
            LOG_PRINT("MAX_NTP_RETRY_COUNT reached. Exit application\r\n");
            rsi_wlan_app_cb.state = RSI_WLAN_EXIT_STATE;
          }
        } else {
          if (set_status == true) {
            rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_INIT_STATE;
          } else {
            LOG_PRINT("Set Time has failed : Exit the application\n");
            rsi_wlan_app_cb.state = RSI_WLAN_EXIT_STATE;
          }
        }

#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_IOTHUB_LL_INIT_STATE: {
        //! Used to initialize IoTHub SDK subsystem
        LOG_PRINT("IOT HuB Init...\n");
        (void)IoTHub_Init();
        LOG_PRINT("Creating IoTHub Device handle\r\n");
        device_ll_handle = IoTHubDeviceClient_LL_CreateFromConnectionString(CONNECTION_STRING, protocol);
        if (device_ll_handle == NULL) {
          LOG_PRINT("Failure creating IotHub device. Hint: Check your connection string.\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DEINIT_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_SETOPTION_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_IOTHUB_LL_SETOPTION_STATE: {
        //! Set any option that are neccessary.
        //! For available options please see the iothub_sdk_options.md documentation
#ifndef SAMPLE_HTTP
        //! Can not set this options in HTTP
        bool traceOn = true;
        IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_LOG_TRACE, &traceOn);
#endif

#ifdef SET_TRUSTED_CERT_IN_SAMPLES
        //! Setting the Trusted Certificate. This is only necessary on systems without
        //! built in certificate stores.
        IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_TRUSTED_CERT, certificates);
#endif

#if defined SAMPLE_MQTT || defined SAMPLE_MQTT_OVER_WEBSOCKETS
        //! Setting the auto URL Encoder (recommended for MQTT). Please use this option unless
        //! you are URL Encoding inputs yourself.
        //! ONLY valid for use with MQTT //OPTION_KEEP_ALIVE
        bool urlEncodeOn = true;
        (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_AUTO_URL_ENCODE_DECODE, &urlEncodeOn);
        int32_t keepalive = MQTT_KEEPALIVE_VAL;
        (void)IoTHubDeviceClient_LL_SetOption(device_ll_handle, OPTION_KEEP_ALIVE, &keepalive);
#endif

        //! Setting connection status callback to get indication of connection to iothub
        (void)IoTHubDeviceClient_LL_SetConnectionStatusCallback(device_ll_handle, connection_status_callback, NULL);
        status = IoTHubDeviceClient_LL_SetMessageCallback(device_ll_handle, receive_msg_callback, &messages_sent);
        if (status != IOTHUB_CLIENT_OK) {
          LOG_PRINT("Failure creating IotHub device. Hint: Check your connection string.\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DEINIT_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DOWORK_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_IOTHUB_LL_DOWORK_STATE: {
        if (network_connect) {
          if (messages_sent < MESSAGE_COUNT) {
            //! Construct the iothub message from a string or a byte array
            message_handle = IoTHubMessage_CreateFromString(telemetry_msg);
            //! Add custom properties to message
            (void)IoTHubMessage_SetProperty(message_handle, "property_key", "property_value");
            LOG_PRINT("Sending message %d to IoTHub\r\n", (int)(messages_sent + 1));
            IoTHubDeviceClient_LL_SendEventAsync(device_ll_handle, message_handle, send_confirm_callback, NULL);
            //! The message is copied to the sdk so the we can destroy it
            IoTHubMessage_Destroy(message_handle);
            messages_sent++;
          } else if ((g_message_count_send_confirmations >= MESSAGE_COUNT) && (g_message_recv_count >= MESSAGE_COUNT)) {
            //! After all messages are sent and received stop running
            g_continueRunning = false;
          } else {
            LOG_PRINT("Waiting for message from cloud\r\n");
          }
        } else {
          if (nw_retry_count >= MAX_NW_RETRY_COUNT) {
            LOG_PRINT("MAX_NW_RETRY_COUNT reached. State changed to RSI_WLAN_IOTHUB_LL_DEINIT_STATE\r\n");
            rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DEINIT_STATE;
#ifdef RSI_WITH_OS
            rsi_semaphore_post(&rsi_azure_sem);
#endif
            break;
          }
        }

        IoTHubDeviceClient_LL_DoWork(device_ll_handle);
        if (g_continueRunning) {
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DOWORK_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_IOTHUB_LL_DEINIT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;

      case RSI_WLAN_IOTHUB_LL_DEINIT_STATE: {
        IoTHubDeviceClient_LL_Destroy(device_ll_handle);
        IoTHub_Deinit();
        // Reset all the global variables
        messages_sent                      = 0;
        g_message_count_send_confirmations = 0;
        g_message_recv_count               = 0;
        g_continueRunning                  = true;
        LOG_PRINT("IoTHub DeInit Done. EXITING Application\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_EXIT_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_azure_sem);
#endif
      } break;
      default:
        break;
    }
    if (rsi_wlan_app_cb.state == RSI_WLAN_EXIT_STATE) {
      break;
    }
  }
  LOG_PRINT("EXITING APPLICATION");
  return 0;
}

int main()
{
#ifdef RSI_WITH_OS
  // Create application task
  rsi_task_create((void *)rsi_azure_client_app,
                  (uint8_t *)"application_task",
                  RSI_APPLICATION_TASK_STACK_SIZE,
                  NULL,
                  RSI_APPLICATION_TASK_PRIORITY,
                  &application_handle);

  // Start the scheduler
  rsi_start_os_scheduler();
#else
  rsi_azure_client_app();

  while (1) {
    rsi_wireless_driver_task();
  }
#endif
}
