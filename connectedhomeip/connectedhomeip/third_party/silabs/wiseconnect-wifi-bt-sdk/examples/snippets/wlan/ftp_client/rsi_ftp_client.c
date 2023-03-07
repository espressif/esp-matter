/*******************************************************************************
* @file  rsi_ftp_client.c
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

/*================================================================================
 * @brief : This file contains example application for FTP client
 * @section Description :
 * This application demonstrates how to connect to FTP server opened on remote
 * peer using FTP client and how to read file from FTP server and how to write
 * file to the FTP server.
 =================================================================================*/

/**
 * Include files
 * */
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_nwk.h"

#include "string.h"
#include "rsi_utils.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif
//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE RSI_OPEN

//! Password
#define PSK ""

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

//! FTP Server port number
#define FTP_SERVER_PORT 21

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.10.100"

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

//! Expected data length from file read
#define FILE_CONTENT_LENGTH 2000

//! Expected list length from directory
#define DIRECTORY_CONTENT_LENGTH 100

//! FTP Server login username
#define FTP_SERVER_LOGIN_USERNAME "username"

//! FTP Server login Password
#define FTP_SERVER_LOGIN_PASSWORD "password"

//! File to read which is on FTP server
#define FTP_FILE_TO_READ "read.txt"

//! File to write on FTP server
#define FTP_FILE_TO_WRITE "write.txt"

//! FTP client mode 1- Passive mode 0- Active mode
#define FTP_CLIENT_MODE FTP_ENABLE_ACTIVE

#ifdef CHIP_9117
//! FTP data transfer modes 1- Block transfer mode 0- Stream transfer mode
#define FTP_TRANSFER_MODE FTP_STREAM_TRANSFER_MODE
#endif

//! File to rename on FTP server
#define FTP_FILE_TO_RENAME "read.txt"

//!Set the directory on FTP server
#define FTP_DIRECTORY_SET "/work/FTP"

//!Set the directory on FTP server
#define FTP_DIRECTORY_CREATE "FTP"

//!list the directories on FTP server
#define FTP_DIRECTORY_LIST "/work/FTP"
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

int32_t rsi_ftp_mode_set(uint8_t mode);

//! Data read from the file will be stored here
int8_t file_data_read[FILE_CONTENT_LENGTH];

//! Write data to the file
int8_t file_data_write[FILE_CONTENT_LENGTH] = "THIS IS FTP APPLICATION DEMO FROM SILABS";

int8_t directory_list_read[FILE_CONTENT_LENGTH];

volatile uint8_t wait_for_complete = 0;

volatile uint8_t wait_for_complete_dir_list = 0;

uint32_t file_data_offset = 0;

uint32_t file_write_data_length = 200;

uint32_t file_read_data_length = 0;

uint32_t dir_list_offset = 0;

uint32_t dir_list_length = 0;

uint64_t ip_to_reverse_hex(char *ip);

//! This call back is called when file read opertion is in progress
void rsi_file_read_cb(uint16_t status, uint8_t *data, uint16_t length, uint8_t end_of_file)
{
  int8_t *file_ptr;
  file_ptr = file_data_read;
  if (status != RSI_SUCCESS) {
    //! Error in file read operation
  }
  //! Indicates end of file
  if (end_of_file == 1) {
    wait_for_complete = 1;
    file_data_offset  = 0;
  }
  //! Copy data from file to global array
  if ((data != NULL) && length && (file_data_offset < FILE_CONTENT_LENGTH)) {
    memcpy((file_ptr + file_data_offset), data, length);

    file_data_offset += length;

    file_read_data_length += length;
  }
}

//! This call back is called when directory list opertion is in progress
void rsi_directory_list_cb(uint16_t status, uint8_t *data, uint16_t length, uint8_t end_of_list)
{
  int8_t *file_ptr;
  file_ptr = directory_list_read;
  if (status != RSI_SUCCESS) {
    //! Error in file read operation
  }
  //! Indicates end of file
  if (end_of_list == 1) {
    wait_for_complete_dir_list = 1;
  }
  //! Copy data from file to global array
  if ((data != NULL) && length && (dir_list_offset < DIRECTORY_CONTENT_LENGTH)) {
    memcpy((file_ptr + dir_list_offset), data, length);

    dir_list_offset += length;

    dir_list_length += length;
  }
}

int32_t rsi_ftp_client()
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

  uint32_t server_ip = ip_to_reverse_hex(SERVER_IP_ADDRESS);
  int32_t retval     = 0;
  uint8_t mode_type;

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

  //! Silabs module intialisation
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

  //! Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Scan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Scan Success\r\n");
  }

  //! Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
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

  //! Connect to  FTP Server
  retval = rsi_ftp_connect(RSI_IP_VERSION_4,
                           (int8_t *)&server_ip,
                           (int8_t *)FTP_SERVER_LOGIN_USERNAME,
                           (int8_t *)FTP_SERVER_LOGIN_PASSWORD,
                           FTP_SERVER_PORT);
  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nConnect to  FTP Server Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nConnect to  FTP Server Success\r\n");
  }

  ////////////////////////////////////////////////////////////////
  ////// Note: By default FTP client will be in ACTIVE mode///////
  ////////////////////////////////////////////////////////////////

  if (FTP_CLIENT_MODE
#ifdef CHIP_9117
      || FTP_TRANSFER_MODE
#endif
  ) {
    //! Set FTP client PASSIVE mode
    mode_type = FTP_CLIENT_MODE;
#ifdef CHIP_9117
    mode_type = mode_type | (FTP_TRANSFER_MODE << 1);
#endif
    retval = rsi_ftp_mode_set(mode_type);
    if (retval != RSI_SUCCESS) {
      LOG_PRINT("\r\nSet FTP client mode Failed, Error Code : 0x%lX\r\n", retval);
      return retval;
    } else {
      LOG_PRINT("\r\nSet FTP client mode Success\r\n");
    }
  }

  //! Read the file in FTP server
  retval = rsi_ftp_directory_list_async((int8_t *)FTP_DIRECTORY_LIST, rsi_directory_list_cb);
  while (!(wait_for_complete_dir_list)) {
#ifndef RSI_WITH_OS
    //! Wait for the read to complete
    rsi_wireless_driver_task();
#endif
  }
  wait_for_complete_dir_list = 0;

  //! Directory Create
  retval = rsi_ftp_directory_create((int8_t *)FTP_DIRECTORY_CREATE);

  if (retval != RSI_SUCCESS) {
    //! Error in creating directory or
    //! Directory already exists
    LOG_PRINT("\r\nCreate Directory Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nCreate Directory Success\r\n");
  }

  //! Directory set
  retval = rsi_ftp_directory_set((int8_t *)FTP_DIRECTORY_SET);

  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet Directory Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nSet Directory Success\r\n");
  }

  //! File Write
  retval = rsi_ftp_file_write((int8_t *)FTP_FILE_TO_WRITE);
  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nFile Write Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nFile Write Success\r\n");
  }
#ifdef CHIP_9117
  if (FTP_TRANSFER_MODE) {
    retval = rsi_ftp_file_size_set(file_write_data_length);
    if (retval != RSI_SUCCESS) {
      LOG_PRINT("\r\nFile Size Set Failed, Error Code : 0x%lX\r\n", retval);
      return retval;
    } else {
      LOG_PRINT("\r\nFile Size Set Success\r\n");
    }
  }
#endif
  //! File Write Content
  retval = rsi_ftp_file_write_content(0, file_data_write, file_write_data_length, 1);

  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nFile Write Content Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nFile Write Content Success\r\n");
  }

  //! File Rename
  retval = rsi_ftp_file_rename((int8_t *)FTP_FILE_TO_WRITE, (int8_t *)FTP_FILE_TO_RENAME);

  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nFile Rename Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nFile Rename Success\r\n");
  }

  //! Read the file in FTP server
  retval = rsi_ftp_file_read_aysnc((int8_t *)FTP_FILE_TO_READ, rsi_file_read_cb);
  while (!(wait_for_complete)) {
#ifndef RSI_WITH_OS
    //! Wait for the read to complete
    rsi_wireless_driver_task();
#endif
  }
  wait_for_complete = 0;

  //! File Delete
  retval = rsi_ftp_file_delete((int8_t *)FTP_FILE_TO_READ);

  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nFile Delete Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nFile Delete Success\r\n");
  }

  //! File Directory delete
  retval = rsi_ftp_directory_delete((int8_t *)FTP_DIRECTORY_SET);

  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nFile Directory Delete Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nFile Directory Delete Success\r\n");
  }

  //! Disconnect from FTP server
  retval = rsi_ftp_disconnect();
  if (retval != RSI_SUCCESS) {
    LOG_PRINT("\r\nDisconnect from FTP server Failed, Error Code : 0x%lX\r\n", retval);
    return retval;
  } else {
    LOG_PRINT("\r\nDisconnect from FTP server Success\r\n");
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
  int32_t status;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)rsi_ftp_client,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! Call FTP client application
  status = rsi_ftp_client();

  //! Application main loop
  main_loop();
#endif
  return status;
}
