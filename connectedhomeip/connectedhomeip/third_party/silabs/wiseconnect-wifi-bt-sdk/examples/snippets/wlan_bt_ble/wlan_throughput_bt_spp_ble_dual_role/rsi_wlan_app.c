/*******************************************************************************
* @file  rsi_wlan_app.c
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
 * @file    rsi_wlan_app.c
 * @version 0.1
 * @date    01 Feb 2021
 *
 *
 *  @brief : This file manages the WLAN connection, IP configurations and creation of wlan subtasks
 *
 *  @section Description  This file contains code to handle Module WLAN connections and Throughput/HTTP download task creations
 *
 *
 */
/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

#include "rsi_common_config.h"
#if RSI_ENABLE_WLAN_TEST
#include "stdlib.h"
#include "rsi_driver.h"
#include "rsi_utils.h"
#include <rsi_wlan_non_rom.h>
//#include <rsi_bt_common_apis.h>
#include "rsi_wlan_config.h"
#if SSL
#include "servercert.pem" //! Include SSL CA certificate
#endif

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
rsi_mutex_handle_t thrput_compute_mutex;
rsi_wlan_app_cb_t rsi_wlan_app_cb; //! application control block
uint64_t num_bits   = 0;
uint64_t total_bits = 0;
uint32_t xfer_time;
uint32_t total_time = 0;
uint64_t xfer_time_usec;
uint32_t t_start = 0;
uint32_t t_end;
volatile uint8_t data_recvd = 0;
volatile uint64_t num_bytes = 0;
uint32_t pkt_rx_cnt         = 0;
uint32_t total_tx_bytes     = 0;
uint32_t total_rx_bytes;
uint32_t secs;
rsi_semaphore_handle_t wlan_throughput_sync_sem[2];
wlan_throughput_config_t wlan_thrput_conf[2] = { 0 };
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_semaphore_handle_t ble_main_task_sem, ble_slave_conn_sem, bt_app_sem, bt_inquiry_sem, ble_scan_sem;
#if WLAN_SYNC_REQ
extern rsi_semaphore_handle_t sync_coex_ble_sem, sync_coex_bt_sem;
#endif
rsi_semaphore_handle_t wlan_pkt_trnsfer_compl_sem;      //! semaphore to indicate wlan data transfer complete
rsi_semaphore_handle_t wlan_thrghput_measurement_compl; //! semaphore to indicate wlan throughput measurement complete
extern bool rsi_ble_running, rsi_bt_running, wlan_radio_initialized, powersave_cmd_given;
extern rsi_mutex_handle_t power_cmd_mutex;
#if (RSI_ENABLE_BT_TEST && WLAN_THROUGHPUT_TEST)
extern rsi_semaphore_handle_t bt_wlan_throughput_sync_sem;
#endif
#if (RSI_ENABLE_BLE_TEST && WLAN_THROUGHPUT_TEST)
extern rsi_semaphore_handle_t ble_wlan_throughput_sync_sem;
#endif

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/
extern uint64_t ip_to_reverse_hex(char *ip);

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_stations_connect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_stations_disconnect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time);
void compute_throughput(void);

/*************************************************************************/
//!  CALLBACK FUNCTIONS
/*************************************************************************/

/*====================================================*/
/**
 * @fn          void rsi_wlan_app_callbacks_init(void)
 * @brief       To initialize WLAN application callback
 * @param[in]   void
 * @return      void
 * @section description
 * This callback is used to initialize WLAN
 * ==================================================*/
void rsi_wlan_app_callbacks_init(void)
{

  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, rsi_join_fail_handler);     //! Initialize join fail call back
  rsi_wlan_register_callbacks(RSI_IP_FAIL_CB, rsi_ip_renewal_fail_handler); //! Initialize IP renewal fail call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB,
                              rsi_remote_socket_terminate_handler); //! Initialize remote terminate call back
  rsi_wlan_register_callbacks(RSI_IP_CHANGE_NOTIFY_CB,
                              rsi_ip_change_notify_handler); //! Initialize IP change notify call back
  rsi_wlan_register_callbacks(RSI_STATIONS_CONNECT_NOTIFY_CB,
                              rsi_stations_connect_notify_handler); //! Initialize IP change notify call back
  rsi_wlan_register_callbacks(RSI_STATIONS_DISCONNECT_NOTIFY_CB,
                              rsi_stations_disconnect_notify_handler); //! Initialize IP change notify call back
}

/*====================================================*/
/**
 * @fn         void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
 * @brief      Function to create Async socket
 * @param[in]  uint32_t sock_no, uint8_t *buffer, uint32_t length
 * @return     void
 * @section description
 * Callback for Socket Async Receive
 * ====================================================*/
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);  //This statement is added only to resolve compilation warning, value is unchanged
  num_bytes += length;
#if WLAN_THROUGHPUT_TEST
//! Measure throughput for every interval of 'THROUGHPUT_AVG_TIME'
#if CONTINUOUS_THROUGHPUT
  if ((rsi_hal_gettickcount() - t_start) >= THROUGHPUT_AVG_TIME) {
    t_end = rsi_hal_gettickcount();
    compute_throughput();
    num_bytes = 0;
    t_start   = rsi_hal_gettickcount();
  }
#else
#if ((THROUGHPUT_TYPE == UDP_RX) /*|| (THROUGHPUT_TYPE == UDP_BIDIRECTIONAL)*/)
  pkt_rx_cnt++;
  if (pkt_rx_cnt == MAX_TX_PKTS) {
    t_end      = rsi_hal_gettickcount();
    data_recvd = 1;
    rsi_semaphore_post(&wlan_pkt_trnsfer_compl_sem);
  }
#endif
#endif
#endif
}

/*====================================================*/
/**
 * @fn         void Throughput(void)
 * @brief      Function to calculate throughput
 * @param[in]  void
 * @return     void
 * @section description
 *====================================================*/
void compute_throughput(void)
{
  float throughput;
  num_bits   = num_bytes * 8;                   //! number of bits
  xfer_time  = t_end - t_start;                 //! data transfer time
  throughput = ((float)(num_bits) / xfer_time); //!Throughput calculation

  throughput /= 1000;
  LOG_PRINT("\r\nThroughput in mbps : %2.2f\r\n", throughput);

  LOG_PRINT("Time taken in sec: %lu \r\n", (xfer_time / 1000));
}

/*====================================================*/
/**
 * @fn         void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time)
 * @brief      Function to calculate throughput
 * @param[in]  void
 * @return     void
 * @section description
 *====================================================*/
void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time)
{
  float through_put;
  through_put = ((float)(total_bytes * 8) / ((end_time - start_time)));
  through_put /= 1000;
  LOG_PRINT("\r\nThroughput in mbps is : %3.2f\r\n", through_put);
  LOG_PRINT("Time taken in sec: %lu \r\n", (uint32_t)((end_time - start_time) / 1000));
}

#if SSL
/*====================================================*/
/**
 * @fn         int32_t rsi_app_load_ssl_cert()
 * @brief      Function to load SSL certificate
 * @param[in]  void
 * @return     void
 *====================================================*/
int32_t rsi_app_load_ssl_cert()
{
  int32_t status = RSI_SUCCESS;
  status         = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, NULL, 0); //! erase existing certificate
  if (status != RSI_SUCCESS) {
    LOG_PRINT("CA cert erase failed \r\n");
    return status;
  }

  status =
    rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, servercert, (sizeof(servercert) - 1)); //! Load SSL CA certificate
  if (status != RSI_SUCCESS) {
    LOG_PRINT("CA cert load failed \r\n");
    return status;
  }
  return status;
}
#endif

/*====================================================*/
/**
 * @fn         int32_t  rsi_wlan_app_task(void)
 * @brief      Function to work with application task
 * @param[in]  void
 * @return     void
 *=====================================================*/

int32_t rsi_wlan_app_task(void)
{
  int32_t status = RSI_SUCCESS;
  uint8_t ip[20] = { 0 };
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  uint32_t socket_instances = 0;
  uint32_t socket_type[TOTAL_PROTOCOLS_CNT];

  while (1) {
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init();                      //! register callback to initialize WLAN
        rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update WLAN application state to unconnected state

#if (WLAN_THROUGHPUT_TEST && TX_RX_RATIO_ENABLE)
        status = rsi_wlan_buffer_config();
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif

#if ENABLE_POWER_SAVE
        rsi_mutex_lock(&power_cmd_mutex);
        if (!powersave_cmd_given) {
          status = rsi_initiate_power_save();
          if (status != RSI_SUCCESS) {
            LOG_PRINT("failed to keep module in power save \r\n");
            return status;
          }
          powersave_cmd_given = true;
        }
        rsi_mutex_unlock(&power_cmd_mutex);
        LOG_PRINT("Module is in deepsleep \r\n");
#endif
      }
        //no break
        //fall through
      case RSI_WLAN_UNCONNECTED_STATE: {
        LOG_PRINT("WLAN scan started \r\n");
        status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("scan failed %lx\r\n", status);
          break;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_DONE_STATE; //! update WLAN application state to connected state
#if ENABLE_POWER_SAVE
          LOG_PRINT("Module is in standby \r\n");
#endif
          LOG_PRINT("scan done state \r\n");
        }

        //! while running BLE/BT throughput test, go to idle state if WLAN_SCAN_ONLY is configured
#if (BT_THROUGHPUT_TEST || BLE_THROUGHPUT_TEST)
#if WLAN_SCAN_ONLY
        rsi_wlan_app_cb.state = RSI_WLAN_IDLE_STATE;
#if WLAN_SYNC_REQ
        if (other_protocol_activity_enabled == false) {
          //! unblock other protocol activities
          if (rsi_bt_running) {
            rsi_semaphore_post(&sync_coex_bt_sem);
          }
          if (rsi_ble_running) {
            rsi_semaphore_post(&sync_coex_ble_sem);
          }
          other_protocol_activity_enabled = true;
        }
#endif
        break;
#endif
#endif
      }
      //no break
      //fall through
      case RSI_WLAN_SCAN_DONE_STATE: {
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("WLAN connection failed %lx\r\n", status);
          break;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to connected state
          LOG_PRINT("WLAN connected state \r\n");
        }
#if (RX_DATA && ENABLE_POWER_SAVE)
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in initiating power save\r\n");
          return status;
        }
        status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, RSI_UAPSD);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in initiating power save\r\n");
          return status;
        }
        LOG_PRINT("Kept uapsd powersave \r\n");
#endif
      }
      //no break
      //fall through
      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip, sizeof(ip), 0);
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
          LOG_PRINT("IP Config failed %lx\r\n", status);
          break;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
          LOG_PRINT("WLAN ipconfig done state \r\n");
          LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip[6], ip[7], ip[8], ip[9]);
        }
      }
        //no break
        //fall through
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
#if WLAN_SYNC_REQ
        //! start wifi tx/rx after bt and ble start-up activities
#if RSI_ENABLE_BLE_TEST
        rsi_semaphore_post(&sync_coex_ble_sem); // BLE will proceed with Adv.
        rsi_delay_ms(100);
#endif
#if RSI_ENABLE_BT_TEST
        rsi_semaphore_post(&sync_coex_bt_sem); // start BT task init
        rsi_delay_ms(100);
#endif
#endif
        status = rsi_semaphore_create(&wlan_pkt_trnsfer_compl_sem, 0);
        if (status != RSI_ERROR_NONE) {
          LOG_PRINT("\r\n semaphore creation failed, wlan_pkt_trnsfer_compl_sem \r\n");
          while (1)
            ;
        }
        status = rsi_semaphore_create(&wlan_thrghput_measurement_compl, 0);
        if (status != RSI_ERROR_NONE) {
          LOG_PRINT("\r\n semaphore creation failed, wlan_thrghput_measurement_compl \r\n");
          while (1)
            ;
        }

        //! create mutex
        status = rsi_mutex_create(&thrput_compute_mutex);
        if (status != RSI_ERROR_NONE) {
          LOG_PRINT("failed to create mutex object, error = %ld \r\n", status);
          while (1)
            ;
        }

        //! get socket instances
        for (uint8_t i = 0; i < TOTAL_PROTOCOLS_CNT; i++) {
          uint8_t temp = 0;
          temp         = (1 << i) & THROUGHPUT_TYPE;
          if (temp != 0) {
            socket_type[socket_instances] = temp;
            socket_instances++;
          }
        }
#if (RSI_ENABLE_BT_TEST && WLAN_THROUGHPUT_TEST)
        //LOG_PRINT("\r\n WLAN task waiting for BT activity to complete..............");
        rsi_semaphore_wait(&bt_wlan_throughput_sync_sem, 0);
#endif
#if (RSI_ENABLE_BLE_TEST && WLAN_THROUGHPUT_TEST)
        //LOG_PRINT("\r\n WLAN task waiting for BLE activity to complete.............");
        rsi_semaphore_wait(&ble_wlan_throughput_sync_sem, 0);
#endif
        //! create tasks for each configured protocol
        for (uint8_t i = 0; i < socket_instances; i++) {
          status = rsi_semaphore_create(&wlan_thrput_conf[i].wlan_app_sem, 0);
          if (status != RSI_ERROR_NONE) {
            LOG_PRINT("\r\n semaphore creation failed wlan_throughput_sync_sem \r\n");
            return status;
          }

          status = rsi_semaphore_create(&wlan_thrput_conf[i].wlan_app_sem1, 0);
          if (status != RSI_ERROR_NONE) {
            LOG_PRINT("\r\n semaphore creation failed wlan_throughput_sync_sem \r\n");
            return status;
          }
          wlan_thrput_conf[i].thread_id       = i;
          wlan_thrput_conf[i].throughput_type = socket_type[i];

          status = rsi_task_create((rsi_task_function_t)(int32_t)wlan_throughput_task,
                                   (uint8_t *)"wlan_throughput_task1",
                                   RSI_WLAN_THRGPUT_TASK_STACK_SIZE,
                                   &wlan_thrput_conf[i],
                                   RSI_WLAN_THROUGHPUT_TASK_PRIORITY,
                                   NULL);
          if (status != RSI_ERROR_NONE) {
            LOG_PRINT("Thread creation failed %s \r\n", "wlan_throughput_task1");
            return status;
          }
          rsi_semaphore_wait(&wlan_thrput_conf[i].wlan_app_sem, 0);
        }

        //! indicate each protocol task to start the throughput measurement
        for (uint8_t i = 0; i < socket_instances; i++) {
          rsi_semaphore_post(&wlan_thrput_conf[i].wlan_app_sem1);
        }
        //! wait on sem
        rsi_semaphore_wait(&wlan_thrghput_measurement_compl, 0);
        rsi_wlan_app_cb.state = RSI_WLAN_IDLE_STATE;
      }
        //no break
        // fall through
      case RSI_WLAN_IDLE_STATE: {
        //! Task has no work to do in this state, so adding a delay of 5sec
        rsi_os_task_delay(5000);
        break;
      }
      default:
        break;
    }
  }
}

uint32_t rsi_convert_4R_to_BIG_Endian_uint32(uint32_t *pw)
{
  uint32_t val;
  uint8_t *pw1 = (uint8_t *)pw;
  val          = pw1[0];
  val <<= 8;
  val |= pw1[1] & 0x000000ff;
  val <<= 8;
  val |= pw1[2] & 0x000000ff;
  val <<= 8;
  val |= pw1[3] & 0x000000ff;
  return val;
}

/*====================================================*/
/**
 * @fn         void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler in station mode at rejoin failure
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\r\n Module Failed to rejoin the AP \r\n");
  rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update wlan application state
}

/*====================================================*/
/**
 * @fn         void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      IP renewal failure call back handler in station mode
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("In IP renewal handler \r\n");
  //! update wlan application state
  rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
}

/*====================================================*/
/**
 * @fn         void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to terminate stations remote socket
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("In IP renewal handler \r\n");
  data_recvd = 1; //! Set data receive flag
  //rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
  t_end = rsi_hal_gettickcount(); //! capture time-stamp after data transfer is completed
#if WLAN_THROUGHPUT_TEST
  rsi_semaphore_post(&wlan_pkt_trnsfer_compl_sem);
#endif
}

/*====================================================*/
/**
 * @fn         void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to notify IP change in Station mode
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("In IP renewal handler \r\n");
  LOG_PRINT("In IP change notify handler \r\n");
  //! update wlan application state
  rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
}

/*====================================================*/
/**
 * @fn         void rsi_stations_connect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to notify stations connect in AP mode
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_stations_connect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("In IP renewal handler \r\n");
  LOG_PRINT("In Station Connect \r\n");
}

/*====================================================*/
/**
 * @fn         void rsi_stations_disconnect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to notify stations disconnect in AP mode
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_stations_disconnect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("In IP renewal handler \r\n");
  LOG_PRINT("In Station Disconnect \r\n");
}
#endif
