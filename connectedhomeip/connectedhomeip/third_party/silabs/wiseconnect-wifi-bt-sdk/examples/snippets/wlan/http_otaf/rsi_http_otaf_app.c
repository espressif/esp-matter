/*******************************************************************************
* @file  rsi_http_ota_app.c
* @brief Application file for http/s OTA firmware update
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
#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_chip.h"
#endif

#include "rsi_wlan_config.h"
//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan.h"

//! HTTP include file to refer HTTP APIs
#include "rsi_http_client.h"

#include "rsi_utils.h"
//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_nwk.h"
//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"
#include "rsi_bootup_config.h"
#include "rsi_http_otaf_app.h"

//! Enable IPv6 set this bit in FLAGS, Default is IPv4
#define HTTPV6 BIT(0)

//! Set HTTPS_SUPPORT to use HTTPS feature
#define HTTPS_SUPPORT BIT(1)

//! Set HTTP_POST_DATA to use HTTP POST LARGE DATA feature
#define HTTP_POST_DATA BIT(5)

//! Set HTTP_V_1_1 to use HTTP version 1.1
#define HTTP_V_1_1 BIT(6)

//! Enable user defined http content type in FLAGS
#define HTTP_USER_DEFINED_CONTENT_TYPE BIT(7)

//! configuration Parameters

//! Access point SSID to connect
#define SSID "SILABS_AP"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "1234567890"

//!SSL TLS versions

/*
RSI_SSL_V_1_0                       BIT(2)

RSI_SSL_V_1_1                       BIT(4)

RSI_SSL_V_1_2                       BIT(3)

 */

#define SSL_TLS_VERSION RSI_SSL_V_1_1

#ifdef AWS_ENABLE
//NOTE : FW package mentioned below is wiseonnect FW (version : 2.4.0.20)
//! Flags
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS HTTPS_SUPPORT
//! include the certificate
#include "http_baltimore_ca.pem.h"
//! Server port number
#define HTTP_PORT 443
//! Server URL
#define HTTP_URL "RS9116W.2.4.0.24.rps"
//! Server Hostname
char *hostname = "awsfw.s3.ap-south-1.amazonaws.com";
//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL
//! set Username
#define USERNAME ""
//! set Password
#define PASSWORD      ""
#define DNS_REQ_COUNT 5

#elif AZURE_ENABLE
//NOTE : FW package mentioned below is wiseonnect FW (version : 2.4.0.20)
//! Flags
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS HTTPS_SUPPORT
//! include the certificate
#include "http_baltimore_ca.pem.h"
//! Server port number
#define HTTP_PORT            443
//! Server URL
#define HTTP_URL             "rps/firmware.rps"
//! Server Hostname
char *hostname = "rs9116updates.blob.core.windows.net";
//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL
//! set Username
#define USERNAME             ""
//! set Password
#define PASSWORD             ""
#define DNS_REQ_COUNT        5

#else
#define FLAGS 0
//! include the certificate
#include "cacert.pem"
//! Server port number
#define HTTP_PORT              80

//! HTTP Server IP address.
#define HTTP_SERVER_IP_ADDRESS "192.168.1.2"

//! HTTP resource name
#define HTTP_URL               "Firmware.rps" //Add firwmare file name to update

//! set HTTP hostname
#define HTTP_HOSTNAME          "192.168.1.2"

//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER   NULL

//! set HTTP hostname
#define USERNAME               "admin"

//! set HTTP hostname
#define PASSWORD               "admin"
#endif

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.10.101" //0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF

#endif

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

//! Application buffer length
#define APP_BUFF_LEN 2000

//! Max HTTP PUT buffer length
#define MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH 900

//! HTTP OTAF success status
#define RSI_HTTP_OTAF_UPGRADE_SUCCESS 1

//! HTTP OTAF Failed status
#define RSI_HTTP_OTAF_UPGRADE_FAILED 2

//! Firmware file name length
#define RSI_FIRMWARE_NAME_SIZE 20

//! HTTP OTAF semaphore wait timeout
#define HTTP_OTAF_SEM_WAIT_TIMEOUT 0

#define RSI_APP_BUF_SIZE 1600
//! http receive status
int32_t http_recv_status;

//! Application Buffer
uint8_t app_buf[APP_BUFF_LEN];

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

//! Join_fail indication
uint8_t rejoin_fail = 0;

//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE                 = 0,
  RSI_WLAN_SCAN_STATE                    = 1,
  RSI_WLAN_UNCONNECTED_STATE             = 2,
  RSI_WLAN_CONNECTED_STATE               = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE           = 4,
  RSI_WLAN_HTTP_OTA_UPDATE_STATE         = 5,
  RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE = 6,
  RSI_WLAN_HTTP_OTA_UPDATE_DONE          = 7
} rsi_wlan_app_state_t;

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

#ifdef RSI_WITH_OS
rsi_task_handle_t wlan_task_handle   = NULL;
rsi_task_handle_t driver_task_handle = NULL;
rsi_semaphore_handle_t rsi_http_otaf_sem;
rsi_semaphore_handle_t rsi_http_response_sem;
#endif

void join_fail_notify_handler(uint16_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(length);  //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\n join fail notify received \n");
#ifdef RSI_WITH_OS
  if (rsi_wlan_app_cb.state == RSI_WLAN_HTTP_OTA_UPDATE_STATE) {
    rsi_semaphore_post(&rsi_http_response_sem);
  }
#endif
  rejoin_fail = 1;
  /* No explict response will be received from FW for OTA upgarde status, so change the http_recv_status to FAILED here */
  http_recv_status = RSI_HTTP_OTAF_UPGRADE_FAILED;
}

void rsi_wlan_app_callbacks_init(void)
{
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, join_fail_notify_handler);
}
//! prototypes
void http_otaf_response_handler(uint16_t status, const uint8_t *buffer);
int32_t rsi_http_response_status(int32_t *rsp_var);

//! http client Application
int32_t rsi_http_otaf_app()
{
  uint8_t ip_buff[20];
  int32_t status = RSI_SUCCESS;
  uint8_t resp_buf[RSI_FIRMWARE_NAME_SIZE];
  //! memset the buffer
  memset(resp_buf, 0, sizeof(resp_buf));

#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  uint8_t flags = FLAGS;

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed\r\n");
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#ifdef RSI_WITH_OS
  //! Task created for Driver task
  status = rsi_task_create(rsi_wireless_driver_task,
                           "driver_task",
                           RSI_DRIVER_TASK_STACK_SIZE,
                           NULL,
                           RSI_DRIVER_TASK_PRIORITY,
                           &driver_task_handle);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDriver Task Creation Failed\r\n");
    return status;
  }
#endif
  //! WC initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed\r\n");
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }
#ifdef RSI_WITH_OS
  rsi_semaphore_create(&rsi_http_response_sem, 0);
#endif

  status = rsi_wlan_get(RSI_FW_VERSION, resp_buf, RSI_FIRMWARE_NAME_SIZE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nFirmware Version Request Failed\r\n");
    return status;
  }
  LOG_PRINT("\nFirmware Version is :%s\r\n", resp_buf);

#if LOAD_CERTIFICATE
  if (flags & HTTPS_SUPPORT) {
    //Reset Loaded certificates
    status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, NULL, 0);
    //Load certificate
#if defined(AWS_ENABLE) || defined(AZURE_ENABLE)
    status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, http_baltimore_ca, (sizeof(http_baltimore_ca) - 1));
#else
    status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, cacert, (sizeof(cacert) - 1));
#endif

    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nLoad Certificate Failed\r\n");
      return status;
    } else {
      LOG_PRINT("\r\nLoad Certificate Success\r\n");
    }
  }
#endif
  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_http_otaf_sem, HTTP_OTAF_SEM_WAIT_TIMEOUT);
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init();
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE; //! update WLAN application state to scan state
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_SCAN_STATE: {
        //! Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN AP Scan Failed\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
        } else {
          LOG_PRINT("\r\nWLAN AP Scan Success\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update WLAN application state to unconnected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wifi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN AP Connect Failed\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to connected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_buff, sizeof(ip_buff), 0);
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
          LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nIP Config Success\r\n");
          LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } //! update WLAN application state to ipconfig done state

#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_cb.state =
          RSI_WLAN_HTTP_OTA_UPDATE_STATE; //! update WLAN application state to http otaf update state
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_HTTP_OTA_UPDATE_STATE: {

#if defined(AWS_ENABLE) || defined(AZURE_ENABLE)
        rsi_rsp_dns_query_t dns_query_rsp;
        uint32_t server_address = 0;
        uint8_t count           = DNS_REQ_COUNT;
        char server_ip[16]      = { 0 }; //for storing the server ip
        do {
          status =
            rsi_dns_req(RSI_IP_VERSION_4, (uint8_t *)hostname, NULL, NULL, &dns_query_rsp, sizeof(dns_query_rsp));
          if (status == RSI_SUCCESS) {
            break;
          }
          count--;
        } while (count != 0);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n DNS req Failed.....\r\n");
          return status;
        }

        server_address = rsi_bytes4R_to_uint32(dns_query_rsp.ip_address[0].ipv4_address);
        server_address = rsi_ntohl(server_address);
        sprintf((char *)server_ip,
                "%ld.%ld.%ld.%ld",
                (server_address & 0xff000000) >> 24,
                (server_address & 0x00ff0000) >> 16,
                (server_address & 0x0000ff00) >> 8,
                server_address & 0x000000ff);
#ifdef AWS_ENABLE
        LOG_PRINT("Resolved AWS S3 Bucket IP address = %s\n", server_ip);
#elif AZURE_ENABLE
        LOG_PRINT("Resolved AZURE Blob Storage IP address = %s\n", server_ip);
#endif

#ifdef AWS_ENABLE
        LOG_PRINT("\r\nFirmware download in progress from AWS S3 Bucket......\r\n");
#elif AZURE_ENABLE
        LOG_PRINT("\r\nFirmware download in progress from AZURE Blob Storage......\r\n");
#endif
        //! send http otaf request for the given url
        status = rsi_http_fw_update((uint8_t)flags,
                                    (uint8_t *)server_ip,
                                    (uint16_t)HTTP_PORT,
                                    (uint8_t *)HTTP_URL,
                                    (uint8_t *)hostname,
                                    (uint8_t *)HTTP_EXTENDED_HEADER,
                                    (uint8_t *)USERNAME,
                                    (uint8_t *)PASSWORD,
                                    http_otaf_response_handler);

#else
        LOG_PRINT("\r\nLocal Apache Server IP Address: %s ......\r\n", HTTP_HOSTNAME);
        LOG_PRINT("\r\nFirmware download in progress from Local Apache Server......\r\n");
        //! send http otaf request for the given url
        status = rsi_http_fw_update((uint8_t)flags,
                                    (uint8_t *)HTTP_SERVER_IP_ADDRESS,
                                    (uint16_t)HTTP_PORT,
                                    (uint8_t *)HTTP_URL,
                                    (uint8_t *)HTTP_HOSTNAME,
                                    (uint8_t *)HTTP_EXTENDED_HEADER,
                                    (uint8_t *)USERNAME,
                                    (uint8_t *)PASSWORD,
                                    http_otaf_response_handler);
#endif
        status = rsi_http_response_status(&http_recv_status);
        if (status != RSI_SUCCESS) {
          http_recv_status = 0;
          if (rejoin_fail) /* If Re-join fail is received, then retry FW upgrade */
          {
            rejoin_fail = 0;
            status      = rsi_wlan_get_status();
            LOG_PRINT("\r\n Reset the state to RSI_WLAN_INITIAL_STATE\r\n");
            rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
          } else {
            LOG_PRINT("\r\nFirmware download restart ....\r\n");
            status                = RSI_SUCCESS;
            rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_UPDATE_STATE;
          }
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE: {
#ifdef RSI_WITH_OS
        status = rsi_destroy_driver_task_and_driver_deinit(driver_task_handle);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nDriver deinit failed, Error Code : 0x%lX\r\n", status);
        } else {
          LOG_PRINT("\r\nTask destroy and driver deinit success\r\n");
        }
#endif
        status = rsi_wireless_deinit();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWireless deinit failed, Error Code : 0x%1X\r\n", status);
        }
#ifdef RSI_WITH_OS
        // Task created for Driver task
        rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                        (uint8_t *)"driver_task",
                        RSI_DRIVER_TASK_STACK_SIZE,
                        NULL,
                        RSI_DRIVER_TASK_PRIORITY,
                        &driver_task_handle);
#endif

#ifdef RSI_M4_INTERFACE
        RSI_CLK_M4ssRefClkConfig(M4CLK, ULP_32MHZ_RC_CLK);
#endif
        status = rsi_wireless_init(0, 0);

        if (status == RSI_SUCCESS) {
          status = rsi_wlan_get(RSI_FW_VERSION, resp_buf, RSI_FIRMWARE_NAME_SIZE);
          LOG_PRINT("\r\nFirmware version after update: %s\r\n", resp_buf);
        } else {
          http_recv_status = status;
          LOG_PRINT("\r\n,Error Code : 0x%X\r\n", (unsigned int)status);
          LOG_PRINT("\r\nFirmware Update Failed\r\n");
        }
        rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_UPDATE_DONE;
      } break;
      default:
        break;
    }
  }
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
  //! OS case
  //! Task created for WLAN task
  status = rsi_task_create(rsi_http_otaf_app,
                           "wlan_task",
                           RSI_WLAN_TASK_STACK_SIZE,
                           NULL,
                           RSI_WLAN_TASK_PRIORITY,
                           &wlan_task_handle);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN Task Creation Failed\r\n");
    return status;
  }

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call HTTP client application
  status = rsi_http_otaf_app();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nHTTP OTAF Application Failed\r\n");
    return status;
  }

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! http otaf response notify call back handler

void http_otaf_response_handler(uint16_t status, const uint8_t *buffer)
{
  UNUSED_CONST_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t resp_buf[RSI_FIRMWARE_NAME_SIZE];

  //! memset the buffer
  memset(resp_buf, 0, sizeof(resp_buf));
  if (status == RSI_SUCCESS) {
    http_recv_status = RSI_HTTP_OTAF_UPGRADE_SUCCESS;
#ifdef AWS_ENABLE
    LOG_PRINT("\r\nFirmware download complete using AWS S3 Bucket\r\n");
#elif AZURE_ENABLE
    LOG_PRINT("\r\nFirmware download complete using AZURE Blob Storage\r\n");
#else
    LOG_PRINT("\r\nFirmware download complete using Local Apache Server\r\n");
#endif
  } else {
    LOG_PRINT("\r\nFirmware update FAILED , error code : %X\r\n", status);
    http_recv_status = RSI_HTTP_OTAF_UPGRADE_FAILED;
  }
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_http_response_sem);
#endif
}

int32_t rsi_http_response_status(int32_t *rsp_var)
{
  //! wait for the success response
#ifndef RSI_WITH_OS
  do {
    //! event loop
    rsi_wireless_driver_task();
  } while (!(*rsp_var));
#else
  rsi_semaphore_wait(&rsi_http_response_sem, 0);
#endif
  *rsp_var = 0;
  if (*rsp_var != RSI_HTTP_OTAF_UPGRADE_SUCCESS) {
    return *rsp_var;
  } else {
    return RSI_SUCCESS;
  }
}
