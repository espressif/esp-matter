/*******************************************************************************
* @file  rsi_eap_connectivity.c
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

//! Driver include file to refer driver APIs
#include "rsi_driver.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

//! Error include files
#include "rsi_error.h"
#include "rsi_utils.h"
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
//! include the certificate
#include "wifiuser.pem"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE RSI_WPA2_EAP

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

//! User ID which is configured in the user configuration file of the radius sever
#define USER_IDENTITY "\"user1\""

//! Password which is configured in the user configuration file of the Radius Server for that User Identity.
#define PASSWORD "\"test123\""

//! Device port number
#define DEVICE_PORT 5001

//! Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define REMOTE_IP "192.168.10.100"

//! Number of packet to send or receive
#define NUMBER_OF_PING_PKTS 1000

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

#define PING_SIZE 100

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint8_t ping_rsp_received;
uint64_t ip_to_reverse_hex(char *ip);
void rsi_ping_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length);
#ifndef RSI_WITH_OS
static void main_loop(void);
#endif
uint8_t ip_buff[20];
int32_t ping_count = 0;

int32_t rsi_eap_connectivity()
{

  int32_t status = RSI_SUCCESS;

  uint32_t remote_ip_addr = ip_to_reverse_hex(REMOTE_IP);
  uint16_t size           = PING_SIZE;

#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  rsi_eap_credentials_t credentials;
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
    LOG_PRINT("\ndevice init Failed,Error Code is:0x%04lx\r\n", status);
    return status;
  }
  LOG_PRINT("\ndevice init success\r\n");
#ifdef RSI_WITH_OS
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! configuring user name
  rsi_strcpy(credentials.username, USER_IDENTITY);

  //! configuring password
  rsi_strcpy(credentials.password, PASSWORD);

  //! WC initialization
  status = rsi_wireless_init(2, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\nrsi_wireless_init Failed,Error Code is:0x%04lx\r\n", status);
    return status;
  }
  LOG_PRINT("\n Wireless Init is Successful\r\n");

#if LOAD_CERTIFICATE
  //! Load certificates
  status = rsi_wlan_set_certificate(RSI_EAP_CLIENT, wifiuser, (sizeof(wifiuser) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n Set Certificate failed,Error Code is:0x%04lx\r\n", status);
    return status;
  }
#endif

  //! Connect to an Acces point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, (uint8_t *)&credentials);
  if (status != RSI_SUCCESS) {
    status = rsi_wlan_get_status();
    LOG_PRINT("\n Join Failed,Error Code is:0x%04lx\r\n", status);
    return status;
  }
  LOG_PRINT("\n Join Successful \r\n");

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
    status = rsi_wlan_get_status();
    LOG_PRINT("\n ipconfig Failed,Error Code is:0x%04lx\r\n", status);
    return status;
  }

  LOG_PRINT("\n IP address is:%d.%d.%d.%d\r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);

  //! PING Request
  while (ping_count < NUMBER_OF_PING_PKTS) {

    status = rsi_wlan_ping_async(0, (uint8_t *)&remote_ip_addr, size, rsi_ping_response_handler);
    if (status != RSI_SUCCESS) {
      return status;
    }

    ping_rsp_received = 0;
    ping_count++;
    rsi_delay_ms(1000);

    do {
      rsi_wireless_driver_task();
    } while (!ping_rsp_received);
  }

  return 0;
}
#ifndef RSI_WITH_OS
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
#endif
int main()
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS
  rsi_task_handle_t wlan_task_handle = NULL;
#endif

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_eap_connectivity,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call EAP application
  status = rsi_eap_connectivity();

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! ping response notify call back handler
void rsi_ping_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length)
{
  UNUSED_CONST_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  if (status == RSI_SUCCESS) {
    ping_rsp_received = 1;
  }
}
