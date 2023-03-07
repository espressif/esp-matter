/*******************************************************************************
* @file  rsi_socket_select.c
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

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"

#include <string.h>

//! OS include file to refer OS specific functionality
#include "rsi_os.h"

#include "rsi_driver.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#define ZERO 0

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

//!TCP Max retries
#define RSI_MAX_TCP_RETRIES 10

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
#define RECV_BUFFER_SIZE 1000

//! Device port number
#define DEVICE_PORT 5001

//! Server port number
#define SERVER_PORT 5001
//! Parameter to run forever loop
#define RSI_FOREVER 1

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

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
#define MAX_SOCK 1

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

int32_t new_socket[MAX_SOCK];
int32_t server_socket[MAX_SOCK];
struct rsi_sockaddr_in client_addr[MAX_SOCK];
int32_t client_socket;
struct rsi_sockaddr_in server_addr[MAX_SOCK];
int32_t status       = RSI_SUCCESS;
int32_t packet_count = 0, recv_size = 0, addr_size;
uint8_t max_tcp_retry = RSI_MAX_TCP_RETRIES;
#if !(DHCP_MODE)
uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
uint32_t network_mask = ip_to_reverse_hex(NETMASK);
uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#endif
uint8_t count = 0;
int8_t recv_buffer[RECV_BUFFER_SIZE];
rsi_fd_set read_fds;
rsi_fd_set write_fds;
rsi_fd_set except_fds;
//struct rsi_timeval time;
//! Enumeration for states in applcation
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE       = 0,
  RSI_WLAN_UNCONNECTED_STATE   = 1,
  RSI_WLAN_CONNECTED_STATE     = 2,
  RSI_WLAN_IPCONFIG_DONE_STATE = 3,
  RSI_WAIT_FOR_SELECT_CONFIRM  = 4,
  RSI_WLAN_DATA_RECEIVE_STATE  = 5
} rsi_wlan_app_state_t;
//! wlan application control block
typedef struct rsi_wlan_app_cb_s {
  //! wlan application state
  rsi_wlan_app_state_t state;

} rsi_wlan_app_cb_t;

//! application control block
rsi_wlan_app_cb_t rsi_wlan_app_cb;

extern int rsi_getsockopt(int32_t sockID,
                          int level,
                          int option_name,
                          const void *option_value,
                          rsi_socklen_t option_len);

uint32_t socket_select_response = 0;
void async_socket_select(rsi_fd_set *fd_read, rsi_fd_set *fd_write, rsi_fd_set *fd_except, int32_t status)
{
  UNUSED_PARAMETER(fd_except); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(fd_write);  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(status);    //This statement is added only to resolve compilation warning, value is unchanged
  memset(&read_fds, 0, sizeof(rsi_fd_set));
  memcpy(&read_fds, fd_read, sizeof(rsi_fd_set));
  socket_select_response = 1;
}

int32_t rsi_socket_select()
{
  uint8_t ip_buff[20];
  int32_t status = RSI_SUCCESS;
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

  //! WiSeConnect initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }

  //! Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  while (RSI_FOREVER) {
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        //! update wlan application state
        rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
      }
      //no break
      //fall through
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
          break;
        } else {
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        }
      }
      //no break
      //fall through
      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, RSI_DHCP, 0, 0, 0, ip_buff, sizeof(ip_buff), 0);
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
          break;
        } else {
          LOG_PRINT("\r\nIP Config Success\r\n");
          LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
          break;
        }
      }
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
#if ENABLE_POWER_SAVE
        //! Apply power save profile
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
        for (count = 0; count < MAX_SOCK; count++) {
          //! Create socket
          server_socket[count] = rsi_socket(AF_INET, SOCK_STREAM, 0);
          if (server_socket[count] < 0) {
            status = rsi_wlan_get_status();
            LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
            break;
          }

          //! Set server structure
          memset(&server_addr[count], 0, sizeof(server_addr[count]));

          //! Set family type
          server_addr[count].sin_family = AF_INET;

          //! Set local port number
          server_addr[count].sin_port = htons(DEVICE_PORT + count);

          //! Bind socket
          status =
            rsi_bind(server_socket[count], (struct rsi_sockaddr *)&server_addr[count], sizeof(server_addr[count]));
          if (status != RSI_SUCCESS) {
            status = rsi_wlan_get_status();
            rsi_shutdown(server_socket[count], 0);
            LOG_PRINT("\r\nBind Failed, Error Code : 0x%lX\r\n", status);
            break;
          }

          //! Socket listen
          status = rsi_listen(server_socket[count], 1);
          if (status != RSI_SUCCESS) {
            status = rsi_wlan_get_status();
            rsi_shutdown(server_socket[count], 0);
            LOG_PRINT("\r\nListen Failed, Error Code : 0x%lX\r\n", status);
            break;
          }

          addr_size = sizeof(server_socket[count]);
          //! Socket accept
        }
        for (count = 0; count < MAX_SOCK; count++) {
          new_socket[count] = rsi_accept(server_socket[count], (struct rsi_sockaddr *)&client_addr[count], &addr_size);
          if (new_socket[count] < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(server_socket[count], 0);
            LOG_PRINT("\r\nSocket Accept Failed, Error Code : 0x%lX\r\n", status);
            break;
          }
        }
        rsi_wlan_app_cb.state = RSI_WAIT_FOR_SELECT_CONFIRM;
        break;
      }
      case RSI_WAIT_FOR_SELECT_CONFIRM: {
        memset(&read_fds, 0, sizeof(rsi_fd_set));
        for (count = 0; count < MAX_SOCK; count++) {
          if (!(rsi_getsockopt(new_socket[count], SOL_SOCKET, SO_CHECK_CONNECTED_STATE, NULL, (rsi_socklen_t)NULL))) {
            RSI_FD_SET(new_socket[count], &read_fds);
          }
        }
        status = rsi_select(new_socket[MAX_SOCK - 1] + 1, &read_fds, NULL, NULL, NULL, async_socket_select);
        if (status < 0) {
          break;
        } else {
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_DATA_RECEIVE_STATE;
          break;
        }
      }
      case RSI_WLAN_DATA_RECEIVE_STATE: {
        if (socket_select_response) {
          recv_size = RECV_BUFFER_SIZE;
          {
            for (count = 0; count < MAX_SOCK; count++) {
              if (RSI_FD_ISSET(new_socket[count], &read_fds)) {
                //! Receive data on socket
                status = rsi_recvfrom(new_socket[count],
                                      recv_buffer,
                                      recv_size,
                                      0,
                                      (struct rsi_sockaddr *)&client_addr,
                                      &addr_size);
                if (status < 0) {
                  status = rsi_wlan_get_status();
                  if (status == RSI_RX_BUFFER_CHECK) {
                    break;
                  }
                }
              }
              //! subtract received bytes
            }
            socket_select_response = 0;
            rsi_wlan_app_cb.state  = RSI_WAIT_FOR_SELECT_CONFIRM;
          }
        }
      }
      default:
        break;
    }
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
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_socket_select,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  while (RSI_FOREVER) {
    //! WLAN application tasks
    rsi_socket_select();

    //! wireless driver tasks
    rsi_wireless_driver_task();
  }
#endif
#ifdef RSI_WITH_OS
  return status;
#endif
}
