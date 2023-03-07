/*******************************************************************************
* @file  rsi_throughput_app.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Throughput Measurement
 * @section Description :
 * This application will demonstrate the throughput measurement, measuring the
 * throughput from Client to Server and vice versa.
 =================================================================================*/

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include "rsi_driver.h"

// include file to refer data types
#include "rsi_data_types.h"

// COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

// WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

// WLAN include file for configuration
#include "rsi_wlan_config.h"

// socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
// Error include files
#include "rsi_error.h"

// OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifndef LINUX_PLATFORM
#include "rsi_hal.h"
#endif

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_chip.h"
#endif

// Access point SSID to connect
#define SSID "SILABS_AP"

//Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

// Security type
#define SECURITY_TYPE RSI_WPA2

// Password
#define PSK "1234567890"

// DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//#define RSI_HIGH_PERFORMANCE_SOCKET         1
// If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)

// IP address of the module
// E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.10.101" //0x6500A8C0

// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF

#endif

//Module port number
#define PORT_NUM 5001

// Server port number
#define SERVER_PORT 5001

// Server IP address.
#define SERVER_IP_ADDRESS "192.168.10.1"

// Memory length for driver
#define GLOBAL_BUFF_LEN 15000

// Wlan task priority
#define RSI_APPLICATION_TASK_PRIORITY 1

// Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

// Wlan task stack size
#define RSI_APPLICATION_TASK_STACK_SIZE (512 * 2)

// Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2)

#ifdef RSI_M4_INTERFACE
#define ICACHE2_ADDR_TRANSLATE_1_REG  *(volatile uint32_t *)(0x20280000 + 0x24)
#define MISC_CFG_SRAM_REDUNDANCY_CTRL *(volatile uint32_t *)(0x46008000 + 0x18)
#define MISC_CONFIG_MISC_CTRL1        *(volatile uint32_t *)(0x46008000 + 0x44)
#define MISC_QUASI_SYNC_MODE          *(volatile uint32_t *)(0x46008000 + 0x84)

#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define PS4_SOC_FREQ          119000000 /*<! PLL out clock 100MHz            */
void switch_m4_frequency(void);
#endif

// Type of throughput
#define UDP_TX 0
#define UDP_RX 1
#define TCP_TX 2
#define TCP_RX 3
#define SSL_TX 4
#define SSL_RX 5

#define SOCKET_ASYNC_FEATURE 1
// Power measurement type
#define THROUGHPUT_TYPE UDP_TX
//#define THROUGHPUT_TYPE  UDP_RX
//#define THROUGHPUT_TYPE  TCP_RX
//#define THROUGHPUT_TYPE TCP_TX
//#define THROUGHPUT_TYPE  SSL_TX
//#define THROUGHPUT_TYPE  SSL_RX

// Memory length for send buffer
#define TCP_BUFF_SIZE     1460
#define UDP_BUFF_SIZE     1470
#define SSL_BUFF_SIZE     1370
#define DEFAULT_BUFF_SIZE 1460
#if ((THROUGHPUT_TYPE == UDP_RX) || (THROUGHPUT_TYPE == UDP_TX))
#define BUF_SIZE UDP_BUFF_SIZE
#elif ((THROUGHPUT_TYPE == TCP_RX) || (THROUGHPUT_TYPE == TCP_TX))
#define BUF_SIZE TCP_BUFF_SIZE
#elif ((THROUGHPUT_TYPE == SSL_RX) || (THROUGHPUT_TYPE == SSL_TX))
#define BUF_SIZE SSL_BUFF_SIZE
#endif

// throughput interval configurations
#define MAX_TX_PKTS         10000 // Applies in SSL TX and UDP_RX, calculate throughput after transmitting MAX_TX_PKTS
#define THROUGHPUT_AVG_TIME 30000 // throughput average time in ms
#define CONTINUOUS_THROUGHPUT \
  0 /*! 1- measure throughput for every throughout interval configured,
     0 - measure once for throughout interval configured */

#if ((THROUGHPUT_TYPE == SSL_TX) || (THROUGHPUT_TYPE == SSL_RX))
#define SSL 1
// Load certificate to device flash :
// Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

#endif
#define RSI_SSL_BIT_ENABLE 0

// Throughput parameters
uint64_t num_bits = 0;
uint32_t xfer_time;
uint32_t t_start = 0;
uint32_t t_end;
float throughput;

volatile uint8_t data_recvd = 0;
volatile uint64_t num_bytes = 0;

// Memory to initialize driv
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint32_t total_tx_bytes = 0;
uint32_t total_rx_bytes;
uint32_t secs;
uint8_t ip_buff[20];
uint32_t pkt_rx_cnt = 0;
#ifdef LINUX_PLATFORM
static uint32_t last_tx_print_time = 0;
static uint32_t last_rx_print_time;
#endif

#if SSL
// Include SSL CA certificate
#include "cacert.pem"
#endif

void compute_throughput(void);
extern int32_t rsi_wlan_buffer_config(void);
extern void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern uint64_t ip_to_reverse_hex(char *ip);

// Call back for Socket Async
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);

  num_bytes += length;
//! Measure throughput for every interval
#if CONTINUOUS_THROUGHPUT
  //! compute throughput for every interval of THROUGHPUT_AVG_TIME
  if ((rsi_hal_gettickcount() - t_start) >= THROUGHPUT_AVG_TIME) {
    t_end = rsi_hal_gettickcount();
    compute_throughput();
    num_bytes = 0;
    t_start   = rsi_hal_gettickcount();
  }
#else
#if (THROUGHPUT_TYPE == UDP_RX)
  pkt_rx_cnt++;
  if (pkt_rx_cnt == MAX_TX_PKTS) {
    t_end      = rsi_hal_gettickcount();
    data_recvd = 1;
  }
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
  num_bits   = num_bytes * 8;                               // number of bits
  xfer_time  = ((t_end - t_start) / 1000);                  // data transfer time
  throughput = (((float)(num_bits) / xfer_time) / 1000000); //Throughput calculation
#ifdef RSI_DEBUG_PRINTS
  LOG_PRINT("throughput in mbps=%f \r\n", (throughput));
  LOG_PRINT("Time taken in sec: %lu \r\n", xfer_time);
#endif
}

#ifdef LINUX_PLATFORM
void measure_throughput(uint32_t pkt_length, uint32_t tx_rx)
{
  static uint32_t current_time;
  static uint32_t last_print_time;
  uint32_t total_bytes;
  float through_put;
#ifdef LINUX_PLATFORM
  struct rsi_timeval tv1;
  gettimeofday(&tv1, NULL);
  current_time = tv1.tv_sec * 1000000 + tv1.tv_usec;
#else
  current_time = rsi_hal_gettickcount();
#endif
  if (tx_rx == 0) {
    total_tx_bytes += pkt_length;
    total_bytes     = total_tx_bytes;
    last_print_time = last_tx_print_time;
  } else {
    total_rx_bytes += pkt_length;
    total_bytes     = total_rx_bytes;
    last_print_time = last_rx_print_time;
  }
#ifdef LINUX_PLATFORM
  if ((current_time - last_print_time) >= 1000000) //for 1 sec
#else
  if ((current_time - last_print_time) >= 30000) //for 30 sec
#endif
  {
#ifdef LINUX_PLATFORM
    through_put = ((float)(total_bytes) / ((current_time - last_print_time))) * 8;
#else
    through_put = ((float)(total_bytes) / ((current_time - last_print_time))) * 8;
    through_put /= 1000;
#endif
    if (tx_rx == 0) {
#ifdef LINUX_PLATFORM
      printf("\nSecs: %d     Bytes Transmitted %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
#endif
      last_tx_print_time = current_time;
      total_tx_bytes     = 0;
    } else {
#ifdef LINUX_PLATFORM
      printf("\nSecs: %d     Bytes Received %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
#endif
      last_rx_print_time = current_time;
      total_rx_bytes     = 0;
    }
  }
  return;
}
#else

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
#endif

/*====================================================*/
/**
 * @fn         int32_t application(void)
 * @brief      This function explains how to scan, connect and tx/rx wlan packets from remote device
 * @param[in]  void
 * @return     int32_t
 * @section description
 *====================================================*/
int32_t application()
{
  int32_t client_socket;
  int32_t server_socket, new_socket;
  struct rsi_sockaddr_in server_addr, client_addr;
  int32_t status = RSI_SUCCESS;
#if !SOCKET_ASYNC_FEATURE
  uint32_t recv_size = 0;
  // buffer to receive data over TCP/UDP client socket
  int8_t recv_buffer[BUF_SIZE];
  uint32_t total_bytes_rx = 0;
#endif
  int32_t addr_size;
  uint8_t high_performance_socket = RSI_HIGH_PERFORMANCE_SOCKET;
  uint8_t ssl_bit_map             = RSI_SSL_BIT_ENABLE;
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  int8_t send_buf[BUF_SIZE];
  uint16_t i              = 0;
  uint32_t total_bytes_tx = 0, tt_start = 0, tt_end = 0, pkt_cnt = 0;
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

  // Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  // Silicon labs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

#ifdef RSI_M4_INTERFACE
  switch_m4_frequency();
  SysTick_Config(SystemCoreClock / 1000);
#endif

#ifdef RSI_WITH_OS
  // Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  // WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWireless Initialization Success\r\n");

  // Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nSend Feature Frame Success\r\n");

  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB,
                              rsi_remote_socket_terminate_handler); // Initialize remote terminate call back

#if LOAD_CERTIFICATE
#if SSL
  // Load SSL CA certificate
  status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoad SSL CA Certificate Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nLoad SSL CA Certificate Success\r\n");
#endif
#endif

#if TX_RX_RATIO_ENABLE
  status = rsi_wlan_buffer_config();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
  // Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Scan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWLAN AP Scan Success\r\n");

  // Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWLAN AP Connect Success\r\n");

  // Configure IP
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
    return status;
  }
  LOG_PRINT("\r\nIP Config Success\r\n");
  LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);

  for (i = 0; i < BUF_SIZE; i++) {
    send_buf[i] = i;
  }

  switch (THROUGHPUT_TYPE) {
    case UDP_TX: {
      LOG_PRINT("\r\nUDP_TX TEST\r\n");
      // Create socket
      client_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
      if (client_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set server address family
      server_addr.sin_family = AF_INET;

      // Set server port number, using htons function to use proper byte order
      server_addr.sin_port = htons(SERVER_PORT);

      // Set IP address to localhost
      server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);

      LOG_PRINT("\r\nUDP TX start\r\n");
      // take the current time
      tt_start = rsi_hal_gettickcount();
      while (1) {
        // Send data on socket
        status =
          rsi_sendto(client_socket, send_buf, BUF_SIZE, 0, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to send data to UDP Server, Error Code : 0x%lX\r\n", status);
          return status;
        }

#ifdef LINUX_PLATFORM
        // Measure throughput per second, 0 - Tx and 1 - Rx
        measure_throughput(BUF_SIZE, 0);
#else
        pkt_cnt++;
        // Measure throughput if avg time is reached
        if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
          // store the time after sending data
          tt_end = rsi_hal_gettickcount();
          total_bytes_tx = pkt_cnt * BUF_SIZE;
          // Measure throughput for every interval of THROUGHPUT_AVG_TIME
#if CONTINUOUS_THROUGHPUT
          // Measure throughput
          measure_throughput(total_bytes_tx, tt_start, tt_end);
          // reset to initial value
          total_bytes_tx = 0;
          pkt_cnt = 0;
          tt_start = rsi_hal_gettickcount();
#else
          LOG_PRINT("\r\nUDP TX completed\r\n");
          // Measure throughput
          measure_throughput(total_bytes_tx, tt_start, tt_end);
          break;
#endif
        }
#endif
      }
    } break;

    case UDP_RX: {
      LOG_PRINT("\r\nUDP_RX TEST\r\n");
      // Create socket
#if SOCKET_ASYNC_FEATURE
      server_socket = rsi_socket_async(AF_INET, SOCK_DGRAM, 0, socket_async_recive);
#else
      server_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
#endif
      if (server_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set family type
      server_addr.sin_family = AF_INET;

      // Set local port number
      server_addr.sin_port = htons(SERVER_PORT);

      // Bind socket
      status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(server_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nBind Success\r\n");

      addr_size = sizeof(server_addr);

      LOG_PRINT("\r\nUDP RX start\r\n");
#if SOCKET_ASYNC_FEATURE
      t_start = rsi_hal_gettickcount(); // capture time-stamp before data transfer starts
      while (!data_recvd) {
#ifndef RSI_WITH_OS
        rsi_wireless_driver_task();
#endif
      }
      LOG_PRINT("\r\nUDP RX completed \r\n");
#if !CONTINUOUS_THROUGHPUT
      compute_throughput(); //! measure throughput if continuous throughput is not enabled
#endif
#else
      // take the current time
      tt_start = rsi_hal_gettickcount();
      while (1) {
        recv_size = BUF_SIZE;

        do {
          // Receive data on socket
          status =
            rsi_recvfrom(server_socket, recv_buffer, recv_size, 0, (struct rsi_sockaddr *)&client_addr, &addr_size);
          if (status < 0) {
            status = rsi_wlan_socket_get_status(server_socket);
            if (status == RSI_RX_BUFFER_CHECK) {
              continue;
            }
            rsi_shutdown(server_socket, 0);
            LOG_PRINT("\r\nFailed to receive data from UDP client, Error Code : 0x%lX\r\n", status);
            return status;
          }

          // subtract received bytes
          recv_size -= status;

          // Measure throughput per second, 0 - Tx and 1 - Rx
#ifdef LINUX_PLATFORM
          measure_throughput(status, 1);
#else
          total_bytes_rx += status;
          // store the time after sending data
          tt_end = rsi_hal_gettickcount();
          // Measure throughput if avg time is reached
          if (tt_end - tt_start >= THROUGHPUT_AVG_TIME) {
            LOG_PRINT("\r\nUDP RX completed \r\n");
            // Measure throughput
            measure_throughput(total_bytes_rx, tt_start, tt_end);
            break;
          }
#endif
        } while (recv_size > 0);
        if (tt_end - tt_start >= THROUGHPUT_AVG_TIME) {
          break;
        }
      }
#endif
      break;
    }
    case TCP_TX: {
      LOG_PRINT("\r\nTCP_TX TEST\r\n");
      // Create socket
      client_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
      if (client_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      // Memset client structrue
      memset(&client_addr, 0, sizeof(client_addr));

      // Set family type
      client_addr.sin_family = AF_INET;

      // Set local port number
      client_addr.sin_port = htons(PORT_NUM);

      // Bind socket
      status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nBind Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set server address family
      server_addr.sin_family = AF_INET;

      // Set server port number, using htons function to use proper byte order
      server_addr.sin_port = htons(SERVER_PORT);

      // Set IP address to localhost
      server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
      ;

      // Connect to server socket
      status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nConnect to Server Socket Success\r\n");

      LOG_PRINT("\r\nTCP TX Start\r\n");

      // take the current time
      tt_start = rsi_hal_gettickcount();
      while (1) {
        // Send data on socket
        status = rsi_send(client_socket, send_buf, BUF_SIZE, 0);
        if (status < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to Send data to TCP Server, Error Code : 0x%lX\r\n", status);
          return status;
        }

#ifdef LINUX_PLATFORM
        // Measure throughput per second, 0 - Tx and 1 - Rx
        measure_throughput(BUF_SIZE, 0);
#else
        pkt_cnt++;
        // Measure throughput if avg time is reached
        if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
          // store the time after sending data
          tt_end = rsi_hal_gettickcount();
          total_bytes_tx = pkt_cnt * BUF_SIZE;
#if CONTINUOUS_THROUGHPUT
          // Measure throughput
          measure_throughput(total_bytes_tx, tt_start, tt_end);
          // reset to initial value
          total_bytes_tx = 0;
          pkt_cnt = 0;
          tt_start = rsi_hal_gettickcount();
#else
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nTCP TX completed \r\n");
          // Measure throughput
          measure_throughput(total_bytes_tx, tt_start, tt_end);
          break;
#endif
        }
#endif
      }
    } break;

    case TCP_RX: {
      LOG_PRINT("\r\nTCP RX Test\r\n");
#if HIGH_PERFORMANCE_ENABLE

      status = rsi_socket_config();
      if (status < 0) {
        status = rsi_wlan_get_status();
        return status;
      }

#endif

      // Create socket
#if SOCKET_ASYNC_FEATURE
      server_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 0, socket_async_recive);
#else
      server_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
#endif
      if (server_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      status = rsi_setsockopt(server_socket,
                              SOL_SOCKET,
                              SO_HIGH_PERFORMANCE_SOCKET,
                              &high_performance_socket,
                              sizeof(high_performance_socket));
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSet Socket Option Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set family type
      server_addr.sin_family = AF_INET;

      // Set local port number
      server_addr.sin_port = htons(SERVER_PORT);

      // Bind socket
      status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(server_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nBind Success\r\n");

      // Socket listen
      status = rsi_listen(server_socket, 1);
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(server_socket, 0);
        LOG_PRINT("\r\nListen Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nListen Success\r\n");

      addr_size = sizeof(server_socket);

      // Socket accept
      new_socket = rsi_accept(server_socket, (struct rsi_sockaddr *)&client_addr, &addr_size);
      if (new_socket < 0) {
        status = rsi_wlan_get_status();
        rsi_shutdown(server_socket, 0);
        LOG_PRINT("\r\nSocket Accept Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Accept Success\r\n");

#if SOCKET_ASYNC_FEATURE
      LOG_PRINT("\r\nTCP RX started \r\n");
      t_start = rsi_hal_gettickcount(); // capture time-stamp before data transfer is starts
      while (!data_recvd) {
#ifndef RSI_WITH_OS
        rsi_wireless_driver_task();
#endif
      }
      rsi_shutdown(server_socket, 0);
      LOG_PRINT("\r\nTCP RX completed \r\n");
#if !CONTINUOUS_THROUGHPUT
      compute_throughput();
#endif
#else
      // take the current time
      t_start = rsi_hal_gettickcount();
      while (1) {
        recv_size = BUF_SIZE;

        do {
          // Receive data on socket
          status = rsi_recvfrom(new_socket, recv_buffer, recv_size, 0, (struct rsi_sockaddr *)&client_addr, &addr_size);
          if (status < 0) {
            status = rsi_wlan_socket_get_status(new_socket);
            if (status == RSI_RX_BUFFER_CHECK) {
              continue;
            }
            // data received completed from remote device
            if (data_recvd) {
              rsi_shutdown(server_socket, 0);
              break;
            } else {
              LOG_PRINT("\r\nTCP Recv Failed, Error code : 0x%lX\r\n", status);
              rsi_shutdown(server_socket, 0);
              return status;
            }
          }

          // subtract received bytes
          recv_size -= status;
          total_bytes_rx += status;

#ifdef LINUX_PLATFORM
          // Measure throughput per second, 0 - Tx and 1 - Rx
          measure_throughput(status, 1);
#endif
        } while (recv_size > 0);
        if (data_recvd) {
          break;
        }
      }
      LOG_PRINT("\r\nTCP RX completed \r\n");
      // Measure throughput
      measure_throughput(total_bytes_rx, t_start, t_end);
#endif
      break;
    }
    case SSL_TX: {
      // Create socket
      client_socket = rsi_socket(AF_INET, SOCK_STREAM, 1);
      if (client_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSet Socket Option Success\r\n");

      // Memset client structrue
      memset(&client_addr, 0, sizeof(client_addr));

      // Set family type
      client_addr.sin_family = AF_INET;

      // Set local port number
      client_addr.sin_port = htons(PORT_NUM);

      // Bind socket
      status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nBind Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set server address family
      server_addr.sin_family = AF_INET;

      // Set server port number, using htons function to use proper byte order
      server_addr.sin_port = htons(SERVER_PORT);

      // Set IP address to localhost
      server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
      ;

      // Connect to server socket
      status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nFailed to Connect to SSL Server, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nConnect to SSL Server Success\r\n");

      LOG_PRINT("\r\nSSL TX Start \r\n");
      // take the current time
      t_start = rsi_hal_gettickcount();
      while (1) {
        // Send data on socket
        status = rsi_send(client_socket, send_buf, BUF_SIZE, 0);
        if (status < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to Send data to SSL Server, Error Code : 0x%lX\r\n", status);
          return status;
        }

#ifdef LINUX_PLATFORM
        // Measure throughput per second, 0 - Tx and 1 - Rx
        measure_throughput(BUF_SIZE, 0);
#else
        pkt_cnt++;
#if CONTINUOUS_THROUGHPUT
        if ((rsi_hal_gettickcount() - t_start) >= THROUGHPUT_AVG_TIME) {
          // take the current time
          t_end = rsi_hal_gettickcount();
          total_bytes_tx = pkt_cnt * BUF_SIZE;
          measure_throughput(total_bytes_tx, t_start, t_end);
          pkt_cnt = 0;
          t_start = rsi_hal_gettickcount();
        }
#else
        // measure throughput if pkt_cnt reaches max transmit packets
        if (pkt_cnt == MAX_TX_PKTS) {
          // take the current time
          t_end = rsi_hal_gettickcount();
          total_bytes_tx = pkt_cnt * BUF_SIZE;
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("SSL client closed\n");
          LOG_PRINT("\r\nSSL TX completed \r\n");
          measure_throughput(total_bytes_tx, t_start, t_end);
          total_bytes_tx = 0;
          pkt_cnt = 0;
          break;
        }
#endif
#endif
      }
    } break;
    case SSL_RX: {
      LOG_PRINT("\r\n SSL RX Test\r\n");
#if HIGH_PERFORMANCE_ENABLE
      status = rsi_socket_config();
      if (status < 0) {
        status = rsi_wlan_get_status();
        return status;
      }
#endif
      // Create socket
      client_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 1, socket_async_recive);
      if (client_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSet Socket Option Success\r\n");

      status = rsi_setsockopt(client_socket,
                              SOL_SOCKET,
                              SO_HIGH_PERFORMANCE_SOCKET,
                              &high_performance_socket,
                              sizeof(high_performance_socket));
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nSet Socket Option Success\r\n");

      // Memset client structrue
      memset(&client_addr, 0, sizeof(client_addr));

      // Set family type
      client_addr.sin_family = AF_INET;

      // Set local port number
      client_addr.sin_port = htons(PORT_NUM);

      // Bind socket
      status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nBind Success\r\n");

      // Set server structure
      memset(&server_addr, 0, sizeof(server_addr));

      // Set server address family
      server_addr.sin_family = AF_INET;

      // Set server port number, using htons function to use proper byte order
      server_addr.sin_port = htons(SERVER_PORT);

      // Set IP address to localhost
      server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
      ;

      // Connect to server socket
      status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_wlan_get_status();
        rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nFailed to Connect to Server, Error code : 0x%lX\r\n", status);
        return status;
      }
      LOG_PRINT("\r\nConnect to Server Success\r\n");

      LOG_PRINT("\r\nSSL RX Start\r\n");
      t_start = rsi_hal_gettickcount(); // capture time-stamp before data transfer is starts
      while (1) {
        if (data_recvd) {
#if !CONTINUOUS_THROUGHPUT
          compute_throughput();
#endif
          break;
        } else {
#ifndef RSI_WITH_OS
          rsi_wireless_driver_task();
#endif
        }
      }
    } break;

    default:
      break;
  }

  return 0;
}

#ifdef RSI_M4_INTERFACE
void switch_m4_frequency(void)
{
  /*Switch M4 SOC clock to Reference clock*/
  /*Default keep M4 in reference clock*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
  /*Configure the PLL frequency*/
  RSI_CLK_SetSocPllFreq(M4CLK, PS4_SOC_FREQ, SOC_PLL_REF_FREQUENCY);
  /*Switch M4 clock to PLL clock for speed operations*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
}
#endif

/*====================================================*/
/**
 * @fn         void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to terminate stations remote socket
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(buffer);
  UNUSED_CONST_PARAMETER(length);
  data_recvd = 1; // Set data receive flag

  t_end = rsi_hal_gettickcount(); // capture time-stamp after data transfer is completed
}

// main function definition
int main(void)
{
#ifdef RSI_WITH_OS
  rsi_task_handle_t application_handle = NULL;

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
#endif
}
