/*******************************************************************************
* @file  rsi_wlan_tcp_logging_stats.c
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
#include "rsi_utils.h"
#include "rsi_driver.h"
//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "1234567890"

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

//! Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.10.100"

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 5000

#define PKT_SEND_INTERVAL 55000
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_FAST_PSP

//! Runs scheduler for some delay and do not send any command
#define RSI_DELAY 6000000

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
#define RSI_APP_BUF_SIZE           1600
//! Device port number
#define DEVICE_PORT 5001
#define BUFF_SIZE   256
//! GPIO event number used in the driver
#define RSI_GPIO_EVENT 0
#define RSI_SEND_EVENT 1
//!TCP Max retries
#define RSI_MAX_TCP_RETRIES 10
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint32_t wifi_connections, wifi_disconnections, rejoin_failures;
uint32_t raising_time, falling_time, initial_time;
uint32_t buffer[BUFF_SIZE];
int32_t client_socket;
struct rsi_sockaddr_in server_addr;
int32_t status       = RSI_SUCCESS;
int32_t packet_count = 0;
struct rsi_sockaddr_in client_addr;
//! Event map for gpio application
uint32_t rsi_gpio_app_event_map;
uint64_t ip_to_reverse_hex(char *ip);
//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE       = 0,
  RSI_POWER_STATE_ACTIVE       = 2,
  RSI_WLAN_SCAN_STATE          = 3,
  RSI_WLAN_UNCONNECTED_STATE   = 4,
  RSI_WLAN_CONNECTED_STATE     = 5,
  RSI_WLAN_IPCONFIG_DONE_STATE = 6,
  RSI_POWER_SAVE_STATE         = 7,
  RSI_SOCKET_CONNECTED         = 8
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
rsi_wlan_app_cb_t rsi_wlan_app_cb;
void rsi_tx_send_handler(void);
int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
extern int32_t rsi_wlan_filter_broadcast(uint16_t beacon_drop_threshold,
                                         uint8_t filter_bcast_in_tim,
                                         uint8_t filter_bcast_tim_till_next_cmd);

void rsi_wlan_async_module_state(uint16_t status, uint8_t *payload, const uint32_t payload_length);
void join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_wlan_async_module_state(uint16_t status, uint8_t *payload, const uint32_t payload_length)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
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
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("Rejoin failure %ld\n", ++rejoin_failures);
  rsi_wlan_app_cb.state = RSI_POWER_STATE_ACTIVE;
}
void rsi_remote_socket_terminate_handler1(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  //! Remote socket has been terminated
  rsi_wlan_app_cb.state = RSI_POWER_SAVE_STATE;
}
void rsi_wlan_app_callbacks_init(void)
{
  rsi_wlan_register_callbacks(RSI_WLAN_ASYNC_STATS, rsi_wlan_async_module_state);
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, join_fail_handler);
  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler1);
}

/*==============================================*/
/**
 * @fn         rsi_wakeup_gpio_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_wakeup_gpio_app_get_event(void)
{
  uint32_t ix;
  for (ix = 0; ix < 32; ix++) {
    if (ix <= 31) {
      if (rsi_gpio_app_event_map & (1 << ix)) {
        return ix;
      }
    }
  }
  return (RSI_FAILURE);
}
/*==============================================*/
/**
 * @fn         rsi_wakeup_gpio_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_wakeup_gpio_app_set_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_gpio_app_event_map |= BIT(event_num);
  }
}

/*==============================================*/
/**
 * @fn         rsi_wakeup_gpio_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_wakeup_gpio_app_clear_event(uint32_t event_num)
{
  if (event_num <= 31) {
    rsi_gpio_app_event_map &= ~BIT(event_num);
  }
}
/*==============================================*/
/**
 * @fn         rsi_wakeup_gpio_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during gpio initialization.
 */
static void rsi_wakeup_gpio_app_init_events()
{
  rsi_gpio_app_event_map = 0;
}

/*==========================================*/
void rsi_wakeup_indication_handler(void)
{
  uint32_t j = 0;
  for (j = 0; j < BUFF_SIZE; j++) {
    if (j % 2 == 0) {
      LOG_PRINT("\n WT %ld", buffer[j]);
    } else if (j % 2 == 1) {
      LOG_PRINT("\n ST %ld", buffer[j]);
    }
  }
  rsi_wakeup_gpio_app_clear_event(RSI_GPIO_EVENT);
}

/*==============================================*/
/**
 * @fn         rsi_wakeup_gpio_event_scheduler
 * @brief      event scheduler for wakeup app
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function invokes the handlers for wakeup gpio application events
 */
void rsi_wakeup_gpio_event_scheduler()
{
  do {
    switch (rsi_wakeup_gpio_app_get_event()) {
      case RSI_GPIO_EVENT:
        rsi_wakeup_indication_handler();
        break;
      case RSI_SEND_EVENT:
        rsi_tx_send_handler();
      default:
        break;
    }
  } while (0);
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
void rsi_tx_send_handler()
{
  //! Send data on socket
  status = rsi_send(client_socket, (int8_t *)"Hello from TCP client!!!", (sizeof("Hello from TCP client!!!") - 1), 0);
  if (status < 0) {
    status = rsi_wlan_get_status();
    LOG_PRINT("\r\nFailed to Send data, Error Code : 0x%lX\r\n", status);
  }
  rsi_wakeup_gpio_app_clear_event(RSI_SEND_EVENT);
}
int32_t rsi_wlan_tcp_logging_stats()
{
  uint8_t ip_buff[20];
  uint8_t max_tcp_retry        = RSI_MAX_TCP_RETRIES;
  uint16_t tcp_keep_alive_time = RSI_SOCKET_KEEPALIVE_TIMEOUT;
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

  while (1) {
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        //! WC initialization
        status = rsi_wireless_init(0, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nWireless Initialization Success\r\n");

        //! initialize wakeup gpio application events
        rsi_wakeup_gpio_app_init_events();

        //! Send feature frame
        status = rsi_send_feature_frame();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\n Feature Frame Failed, Error Code :0x%lx \r\n", status);
          return status;
        }
        rsi_wlan_app_callbacks_init();                  //! register callback to initialize WLAN
        rsi_wlan_app_cb.state = RSI_POWER_STATE_ACTIVE; //! update WLAN application state to unconnected state
      } break;
      case RSI_POWER_STATE_ACTIVE: {
        //! Apply power save profile
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nPowersave Config Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("Powersave Active state\n");
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
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
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Connecting to AP\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Association failed 0x%lx\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to unconnected state
        }
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
        }
      } break;
      case RSI_WLAN_IPCONFIG_DONE_STATE: {

        //! Enable Broadcast data filter
        status = rsi_wlan_filter_broadcast(5000, 1, 1);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nBroadcast Data Filtering Failed with Error Code : 0x%lX\r\n", status);
          return status;
        }

        //! Apply power save profile with connected sleep
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nPowersave Config Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }

        rsi_wlan_app_cb.state = RSI_POWER_SAVE_STATE;
      } break;
      case RSI_POWER_SAVE_STATE: {
        //! Create socket
        client_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        //! To provide the tcp max retry count
        status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_MAXRETRY, &max_tcp_retry, sizeof(max_tcp_retry));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Options Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        status = rsi_setsockopt(client_socket,
                                SOL_SOCKET,
                                SO_TCP_KEEP_ALIVE,
                                &tcp_keep_alive_time,
                                sizeof(tcp_keep_alive_time));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Options Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        //! Memset client structrue
        memset(&client_addr, 0, sizeof(client_addr));

        //! Set family type
        client_addr.sin_family = AF_INET;

        //! Set local port number
        client_addr.sin_port = htons(DEVICE_PORT);

        //! Bind socket
        status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
          return status;
        }

        //! Set server structure
        memset(&server_addr, 0, sizeof(server_addr));

        //! Set server address family
        server_addr.sin_family = AF_INET;

        //! Set server port number, using htons function to use proper byte order
        server_addr.sin_port = htons(SERVER_PORT);

        //! Set IP address to localhost
        server_addr.sin_addr.s_addr = ip_to_reverse_hex(SERVER_IP_ADDRESS);
        LOG_PRINT("\r\nSocket Connecting\r\n");
        //! Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nSocket Connection Failed, Error Code : 0x%lX\r\n", status);
        } else {
          LOG_PRINT("\r\nSocket Connected\r\n");
          rsi_wlan_app_cb.state = RSI_SOCKET_CONNECTED;
        }
        break;
      }

      case RSI_SOCKET_CONNECTED: {
        return 0;
      }
    }
  }
}

void main_loop(void)
{
  initial_time = rsi_hal_gettickcount();
  while (1) {
    if (rsi_hal_gettickcount() - initial_time > PKT_SEND_INTERVAL) {
      rsi_wakeup_gpio_app_set_event(RSI_SEND_EVENT);
      initial_time = rsi_hal_gettickcount();
    }
    //! event loop
    rsi_wireless_driver_task();
    rsi_wakeup_gpio_event_scheduler();
    rsi_wlan_tcp_logging_stats();
  }
}

int main()
{
  int32_t status;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

  rsi_task_handle_t driver_task_handle = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  rsi_hal_log_stats_intr_config(rsi_give_wakeup_indication);
  //! Silabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create(rsi_wlan_tcp_logging_stats,
                  "wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! Task created for Driver task
  rsi_task_create(rsi_wireless_driver_task,
                  "driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call  Power save application
  status = rsi_wlan_tcp_logging_stats();

  //! Application main loop
  main_loop();
#endif
  return status;
}
