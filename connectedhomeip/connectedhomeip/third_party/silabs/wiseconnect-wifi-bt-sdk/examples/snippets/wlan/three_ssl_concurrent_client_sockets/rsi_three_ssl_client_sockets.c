/*******************************************************************************
* @file  rsi_three_ssl_client_sockets.c
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
 * @file    rsi_three_ssl_client_sockets.c
 * @version 1.0
 * @date    15 Apr 2021
 *
 *
 *
 *  @brief : This file contains example application for three TCP client socket connections over SSL with the certificates loaded in to FLASH
 *
 *  @section Description  This file contains example application for three TCP client socket connections over SSL with the SSL certificates loaded in to FLASH
 *
 *
 */

/**
 * Include files
 * */

//! Driver include file to refer driver APIs
#include "rsi_driver.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//! Certificates to be loaded
#include "cacert.pem"
#include "aws_starfield_ca.pem.h"
#include "aws_client_certificate.pem.crt.h"
#include "aws_client_private_key.pem.key.h"

//! Access point SSID to connect
#define SSID "SILABS_AP"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

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
#define DEVICE_IP "192.168.10.101" //0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF
#endif

//! Server IP address.
#define SERVER_ADDR "192.168.10.100"

//! AWS Domain name
#define AWS_DOMAIN_NAME "a25jwtlmds8eip-ats.iot.us-east-2.amazonaws.com"

//! SSL bitmap 1- Enable 0- Disable
#define RSI_SSL_BIT_ENABLE 1

//! Device port number for 1st connection
#define DEVICE_PORT1 5001

//! Device port number for 2nd connection
#define DEVICE_PORT2 5003

//! Device port number for 3rd connection
#define DEVICE_PORT3 5005

//! Server port number for 1st connection
#define SERVER_PORT1 5001

//! Server port number for 2nd connection
#define SERVER_PORT2 5003

//! Server port number for 3rd connection
#define SERVER_PORT3 8883

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

#define MAX_SSL_CERTS_TO_FLASH 3

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif
uint64_t ip_to_reverse_hex(char *ip);
int32_t rsi_three_ssl_client_sockets_app()
{
  uint8_t ip_buff[20];
  int32_t client_socket1;
  int32_t client_socket2;
  int32_t client_socket3;
  struct rsi_sockaddr_in client_addr1, server_addr1;
  struct rsi_sockaddr_in client_addr2, server_addr2;
  struct rsi_sockaddr_in client_addr3, server_addr3;
  int32_t status            = RSI_SUCCESS;
  uint8_t ssl_bit_map       = RSI_SSL_BIT_ENABLE;
  uint32_t ssl_cert_bit_map = RSI_CERT_INDEX_0;
  uint8_t i                 = 0;
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  uint32_t server_address = 0;
  rsi_rsp_dns_query_t dns_query_rsp;

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
  //! OS case
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

  for (i = 0; i < MAX_SSL_CERTS_TO_FLASH; i++) {
    //! Clearing SSL CA certificate loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Clearing SSL Client certificate loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Clearing SSL private key loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT_PRIVATE_KEY, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }
  }

  //! Loading SSL CA certificate in to FLASH with index 0
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_0, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 0 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 0 Success\r\n");
  }

  //! Loading SSL CA certificate in to FLASH with index 1
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_1, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 1 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 1 Success\r\n");
  }

  //! Loading SSL CA certificate in to FLASH with index 2
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE,
                                          RSI_CERT_INDEX_2,
                                          aws_starfield_ca,
                                          (sizeof(aws_starfield_ca) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 2 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 2 Success\r\n");
  }

  //! Loading SSL Client certificate in to FLASH with index 2
  status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT,
                                          RSI_CERT_INDEX_2,
                                          aws_client_certificate,
                                          (sizeof(aws_client_certificate) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL Client certificate in to FLASH with index 2 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL Client certificate in to FLASH with index 2 Success\r\n");
  }

  //! Loading SSL private key in to FLASh with index 2
  status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT_PRIVATE_KEY,
                                          RSI_CERT_INDEX_2,
                                          aws_client_private_key,
                                          (sizeof(aws_client_private_key) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL private key in to FLASh with index 2 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL private key in to FLASh with index 2 Success\r\n");
  }

  //! Scan Access points
  status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
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

  do {
    //! Getting IP address of the AWS server using DNS request
    status =
      rsi_dns_req(RSI_IP_VERSION_4, (uint8_t *)AWS_DOMAIN_NAME, NULL, NULL, &dns_query_rsp, sizeof(dns_query_rsp));

  } while (status != RSI_SUCCESS);

  server_address = rsi_bytes4R_to_uint32(dns_query_rsp.ip_address[0].ipv4_address);

  //! 1st Socket ceration
  client_socket1 = rsi_socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket1 < 0) {
    LOG_PRINT("\r\n1st Socket Create Failed\r\n");
    status = rsi_wlan_get_status();
    return status;
  } else {
    LOG_PRINT("\r\n1st Socket Create Success\r\n");
  }
  ssl_cert_bit_map = RSI_CERT_INDEX_0;
  //! Setting SSL socket option
  status = rsi_setsockopt(client_socket1, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }
  status = rsi_setsockopt(client_socket1, SOL_SOCKET, SO_CERT_INDEX, &ssl_cert_bit_map, sizeof(ssl_cert_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! Memset client structure
  memset(&client_addr1, 0, sizeof(client_addr1));

  //! Set family type
  client_addr1.sin_family = AF_INET;

  //! Set local port number
  client_addr1.sin_port = htons(DEVICE_PORT1);

  //! Bind socket
  status = rsi_bind(client_socket1, (struct rsi_sockaddr *)&client_addr1, sizeof(client_addr1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nBind Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket1, 0);
    return status;
  }
  LOG_PRINT("\r\nBind Success\r\n");
  //! Set server structure
  memset(&server_addr1, 0, sizeof(server_addr1));

  //! Set server address family
  server_addr1.sin_family = AF_INET;

  //! Set server port number, using htons function to use proper byte order
  server_addr1.sin_port = htons(SERVER_PORT1);

  //! Set IP address to localhost
  server_addr1.sin_addr.s_addr = ip_to_reverse_hex(SERVER_ADDR);

  //! Connect to server socket
  status = rsi_connect(client_socket1, (struct rsi_sockaddr *)&server_addr1, sizeof(server_addr1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket1, 0);
    return status;
  }
  LOG_PRINT("\r\nConnect to Server Socket Success \r\n");

  //! 2nd Socket ceration
  client_socket2 = rsi_socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket2 < 0) {
    LOG_PRINT("\r\n2nd Socket Create Failed\r\n");
    status = rsi_wlan_get_status();
    return status;
  } else {
    LOG_PRINT("\r\n2nd Socket Create Success\r\n");
  }
  ssl_cert_bit_map = RSI_CERT_INDEX_1;
  //! Setting SSL socket option
  status = rsi_setsockopt(client_socket2, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }
  status = rsi_setsockopt(client_socket2, SOL_SOCKET, SO_CERT_INDEX, &ssl_cert_bit_map, sizeof(ssl_cert_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! Memset client structure
  memset(&client_addr2, 0, sizeof(client_addr2));

  //! Set family type
  client_addr2.sin_family = AF_INET;

  //! Set local port number
  client_addr2.sin_port = htons(DEVICE_PORT2);

  //! Bind socket
  status = rsi_bind(client_socket2, (struct rsi_sockaddr *)&client_addr2, sizeof(client_addr2));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nBind Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket2, 0);
    return status;
  }
  LOG_PRINT("\r\nBind Success\r\n");

  //! Set server structure
  memset(&server_addr2, 0, sizeof(server_addr2));

  //! Set server address family
  server_addr2.sin_family = AF_INET;

  //! Set server port number, using htons function to use proper byte order
  server_addr2.sin_port = htons(SERVER_PORT2);

  //! Set IP address to localhost
  server_addr2.sin_addr.s_addr = ip_to_reverse_hex(SERVER_ADDR);

  //! Connect to server socket
  status = rsi_connect(client_socket2, (struct rsi_sockaddr *)&server_addr2, sizeof(server_addr2));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket2, 0);
    return status;
  }
  LOG_PRINT("\r\nConnect to Server Socket Success \r\n");

  //! 3rd Socket ceration
  client_socket3 = rsi_socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket3 < 0) {
    LOG_PRINT("\r\n3rd Socket Create Failed\r\n");
    status = rsi_wlan_get_status();
    return status;
  } else {
    LOG_PRINT("\r\n3rd Socket Create Success\r\n");
  }
  ssl_cert_bit_map = RSI_CERT_INDEX_2;
  //! Setting SSL socket option
  status = rsi_setsockopt(client_socket3, SOL_SOCKET, SO_CERT_INDEX, &ssl_cert_bit_map, sizeof(ssl_cert_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }
  status = rsi_setsockopt(client_socket3, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! Memset client structure
  memset(&client_addr3, 0, sizeof(client_addr3));

  //! Set family type
  client_addr3.sin_family = AF_INET;

  //! Set local port number
  client_addr3.sin_port = htons(DEVICE_PORT3);

  //! Bind socket
  status = rsi_bind(client_socket3, (struct rsi_sockaddr *)&client_addr3, sizeof(client_addr3));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nBind Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket3, 0);
    return status;
  }
  LOG_PRINT("\r\nBind Success\r\n");
  //! Set server structure
  memset(&server_addr3, 0, sizeof(server_addr3));

  //! Set server address family
  server_addr3.sin_family = AF_INET;

  //! Set server port number, using htons function to use proper byte order
  server_addr3.sin_port = htons(SERVER_PORT3);

  //! Set IP address to localhost
  server_addr3.sin_addr.s_addr = server_address;

  //! Connect to server socket
  status = rsi_connect(client_socket3, (struct rsi_sockaddr *)&server_addr3, sizeof(server_addr3));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket3, 0);
    return status;
  }
  LOG_PRINT("\r\nConnect to Server Socket Success \r\n");

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

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_three_ssl_client_sockets_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call SSL client with certificates loaded in to RAM application
  status = rsi_three_ssl_client_sockets_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}
