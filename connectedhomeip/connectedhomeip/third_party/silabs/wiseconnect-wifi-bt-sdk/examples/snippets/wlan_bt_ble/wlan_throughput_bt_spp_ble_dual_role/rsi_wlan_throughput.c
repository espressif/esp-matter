/*******************************************************************************
* @file  rsi_wlan_throughput.c
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
 * @file    rsi_wlan_throughput.c
 * @version 0.1
 * @date    01 Feb 2021
 *
 *
 *  @brief : This file manages the WLAN connections to remote server, SSL/TCP/UDP data transfers and throughput measurements
 *
 *  @section Description  This file contains code to handle connections between server and client,
 *  					   handles SSL/TCP/UDP data transfers and throughput measurements
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
#include "rsi_wlan_config.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
struct rsi_sockaddr_in server_addr, client_addr; //! server and client IP addresses
int32_t client_socket;                           //! client socket id
//! Throughput parameters

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_mutex_handle_t thrput_compute_mutex;
extern rsi_semaphore_handle_t wlan_pkt_trnsfer_compl_sem;
extern rsi_semaphore_handle_t wlan_thrghput_measurement_compl;
extern uint32_t t_end, t_start;
extern volatile uint8_t data_recvd;

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/
extern void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length);
extern void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time);
extern void compute_throughput(void);
extern uint64_t ip_to_reverse_hex(char *ip);
/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/

/*************************************************************************/
//!  CALLBACK FUNCTIONS
/*************************************************************************/

int32_t wlan_throughput_task(void *paramaters)
{
  int32_t status = RSI_SUCCESS;
  //! buffer to receive data over TCP/UDP client socket
  int8_t *send_buf;
  uint8_t socket_init_done = 0;
  uint32_t total_bytes_tx = 0, tt_start = 0, tt_end = 0, pkt_cnt = 0, i;
  struct rsi_sockaddr_in server_addr, client_addr;
  int32_t server_socket, new_socket;
  int32_t addr_size;
  uint32_t buff_size = 0;
#if !SOCKET_ASYNC_FEATURE
  int8_t *recv_buffer;
  //! buffer to send data over TCP/UDP client socket
  uint32_t recv_size      = 0;
  uint32_t total_bytes_rx = 0;
#endif
  uint8_t high_performance_socket            = RSI_HIGH_PERFORMANCE_SOCKET;
  uint8_t ssl_bit_map                        = RSI_SSL_BIT_ENABLE;
  wlan_throughput_config_t *wlan_thrput_conf = NULL;
  wlan_thrput_conf                           = (wlan_throughput_config_t *)paramaters;

  //! assign the pkt size based on protocol
  if (wlan_thrput_conf->throughput_type == UDP_TX || wlan_thrput_conf->throughput_type == UDP_RX) {
    buff_size = UDP_BUFF_SIZE;
  } else if (wlan_thrput_conf->throughput_type == TCP_TX || wlan_thrput_conf->throughput_type == TCP_RX) {
    buff_size = TCP_BUFF_SIZE;
  } else if (wlan_thrput_conf->throughput_type == SSL_TX || wlan_thrput_conf->throughput_type == SSL_RX) {
    buff_size = SSL_BUFF_SIZE;
  } else {
    buff_size = DEFAULT_BUFF_SIZE;
  }

  while (1) {
    switch (wlan_thrput_conf->throughput_type) {
      case UDP_TX: {
        LOG_PRINT("\r\nconfiguring socket for UDP tx, conn%d", wlan_thrput_conf->thread_id);
        //! Create socket
        client_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
        if (client_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Create Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        //! Set server structure
        memset(&server_addr, 0, sizeof(server_addr));

        //! Set server address family
        server_addr.sin_family = AF_INET;

        //! Set server port number, using htons function to use proper byte order
        server_addr.sin_port = htons(SERVER_PORT);

        //! Set IP address to localhost
        server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);

        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

        LOG_PRINT("\r\nUDP TX started, conn%d\r\n", wlan_thrput_conf->thread_id);

        //! allocate memory for tx buffer
        send_buf = (int8_t *)rsi_malloc(buff_size * sizeof(int8_t));

        for (i = 0; i < buff_size; i++) {
          send_buf[i] = i;
        }

        //! take the current time
        tt_start = rsi_hal_gettickcount();

        while (1) {
          //! Send data on socket
          status =
            rsi_sendto(client_socket, send_buf, buff_size, 0, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
          if (status < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(client_socket, 0);
            rsi_free(send_buf);
            LOG_PRINT("\r\nFailed to send data to UDP Server, Error Code : 0x%lX, conn%d\r\n",
                      status,
                      wlan_thrput_conf->thread_id);
            LOG_PRINT("\r\n client socket is shutdown, conn%d\r\n", wlan_thrput_conf->thread_id);
            //rsi_wlan_app_cb.state = RSI_WLAN_IDLE_STATE;
            //break;
            return status;
          }
          pkt_cnt++;

          //! Measure throughput if avg time is reached
          if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
            // store the time after sending data
            tt_end         = rsi_hal_gettickcount();
            total_bytes_tx = pkt_cnt * buff_size;
            //! Measure throughput for every interval of THROUGHPUT_AVG_TIME
#if CONTINUOUS_THROUGHPUT
            //! Measure throughput
            rsi_mutex_lock(&thrput_compute_mutex);
            measure_throughput(total_bytes_tx, tt_start, tt_end);
            rsi_mutex_unlock(&thrput_compute_mutex);
            //! reset to initial value
            total_bytes_tx = 0;
            pkt_cnt        = 0;
            tt_start       = rsi_hal_gettickcount();
#else
            rsi_free(send_buf);
            LOG_PRINT("\r\nUDP TX completed, conn%d\r\n", wlan_thrput_conf->thread_id);
            rsi_mutex_lock(&thrput_compute_mutex);
            //! Measure throughput
            measure_throughput(total_bytes_tx, tt_start, tt_end);
            rsi_mutex_unlock(&thrput_compute_mutex);
            //! wlan measurement is completed, release the sem
            rsi_semaphore_post(&wlan_thrghput_measurement_compl);
            //! delete the task
            rsi_task_destroy(NULL);
#endif
          }
        }
      }
      case UDP_RX: {
        LOG_PRINT("\r\nconfiguring socket for UDP rx, conn%d", wlan_thrput_conf->thread_id);
        //! Create socket
#if SOCKET_ASYNC_FEATURE
        server_socket = rsi_socket_async(AF_INET, SOCK_DGRAM, 0, socket_async_recive);
#else
        server_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
#endif
        if (server_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Create Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }
        //! Set server structure
        memset(&server_addr, 0, sizeof(server_addr));

        //! Set family type
        server_addr.sin_family = AF_INET;

        //! Set local port number
        server_addr.sin_port = htons(DEVICE_PORT);

        //! Bind socket
        status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nBind Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }
        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

        addr_size = sizeof(server_addr);
#if SOCKET_ASYNC_FEATURE
        LOG_PRINT("\r\n UDP RX started, conn%d\r\n", wlan_thrput_conf->thread_id);
        t_start = rsi_hal_gettickcount(); //! capture time-stamp before data transfer starts
        while (!data_recvd) {
          rsi_semaphore_wait(&wlan_pkt_trnsfer_compl_sem, 0);
        }
        LOG_PRINT("\r\nUDP RXcompleted, conn%d\r\n", wlan_thrput_conf->thread_id);
        rsi_mutex_lock(&thrput_compute_mutex);
        compute_throughput();
        rsi_mutex_unlock(&thrput_compute_mutex);
        //! wlan measurement is completed, release the sem
        rsi_semaphore_post(&wlan_thrghput_measurement_compl);
        //! delete the task
        rsi_task_destroy(NULL);
#else
        //! allocate memory for rx buffer
        recv_buffer = (int8_t *)rsi_malloc(buff_size * sizeof(int8_t));
        while (1) {
          recv_size = buff_size;

          //! take the current time
          tt_start = rsi_hal_gettickcount();

          do {
            //! Receive data on socket
            status =
              rsi_recvfrom(server_socket, recv_buffer, recv_size, 0, (struct rsi_sockaddr *)&client_addr, &addr_size);
            if (status < 0) {
              status = rsi_wlan_socket_get_status(server_socket);
              if (status == RSI_RX_BUFFER_CHECK) {
                continue;
              }
              rsi_shutdown(server_socket, 0);
              rsi_free(recv_buffer);
              LOG_PRINT("\r\nFailed to receive data from UDP client, Error Code : 0x%X\r\n", status);
              return status;
            }

            //! subtract received bytes
            recv_size -= status;
            total_bytes_rx += status;
            //! store the time after sending data
            tt_end = rsi_hal_gettickcount();
            //! Measure throughput if avg time is reached
            if (tt_end - tt_start >= THROUGHPUT_AVG_TIME) {
              //! Measure throughput
              measure_throughput(total_bytes_rx, tt_start, tt_end);
              //! reset to initial value
              rsi_free(recv_buffer);
              total_bytes_rx = 0;
              tt_start = rsi_hal_gettickcount();
            }
          } while (recv_size > 0);
        }
#endif
        break;
      }
      case TCP_TX: {
        if (socket_init_done == 0) {
          LOG_PRINT("\r\nconfiguring socket for TCP tx, conn%d", wlan_thrput_conf->thread_id);
          //! Create socket
          client_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
          if (client_socket < 0) {
            status = rsi_wlan_get_status();
            LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d \r\n", status, wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSocket Create Success - conn%d \r\n", wlan_thrput_conf->thread_id);
          }

          //! Set server structure
          memset(&server_addr, 0, sizeof(server_addr));

          //! Set server address family
          server_addr.sin_family = AF_INET;

          //! Set server port number, using htons function to use proper byte order
          server_addr.sin_port = htons(SERVER_PORT);

          //! Set IP address to localhost
          server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
          //server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)wlan_thrput_conf->server_addr);
          socket_init_done = 1;
        }
        //! Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          //rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX, conn%d \r\n",
                    status,
                    wlan_thrput_conf->thread_id);
          //return status;
          continue;
        } else {
          LOG_PRINT("\r\nConnect to Server Socket Success, conn%d \r\n", wlan_thrput_conf->thread_id);
        }

        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

//! while running BLE/BT throughput test, delete the task if only wlan connection is configured
#if ((BT_THROUGHPUT_TEST || BLE_THROUGHPUT_TEST) && WLAN_CONNECT_ONLY)
        rsi_os_task_delay(1); //! added to ensure sem wait gets called before sem post calls
        //! wlan measurement is not required, so release the sem
        rsi_semaphore_post(&wlan_thrghput_measurement_compl);
        //! delete the task
        rsi_task_destroy(NULL);
#endif

        LOG_PRINT("\r\nTCP TX started, conn%d\r\n", wlan_thrput_conf->thread_id);

        //! allocate memory for tx buffer
        send_buf = (int8_t *)rsi_malloc(buff_size * sizeof(int8_t));

        for (i = 0; i < buff_size; i++) {
          send_buf[i] = i;
        }

        //! take the current time
        tt_start = rsi_hal_gettickcount();

        while (1) {
          //! Send data on socket
          status = rsi_send(client_socket, send_buf, buff_size, 0);
          if (status < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(client_socket, 0);
            rsi_free(send_buf);
            LOG_PRINT("\r\nFailed to Send data to TCP Server, Error Code : 0x%lX, conn%d \r\n",
                      status,
                      wlan_thrput_conf->thread_id);
            LOG_PRINT("\r\n client socket is shutdown, conn -%d \r\n", wlan_thrput_conf->thread_id);
            return status;
          }
          pkt_cnt++;
//! while running BLE/BT throughput test, don't measure the wlan throughput
#if ((BT_THROUGHPUT_TEST || BLE_THROUGHPUT_TEST) && WLAN_DATATRANSFER)
          continue;
#else
          //! Measure throughput if avg time is reached
          if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
            //! store the time after sending data
            tt_end = rsi_hal_gettickcount();
            total_bytes_tx = pkt_cnt * buff_size;
#if CONTINUOUS_THROUGHPUT
            rsi_mutex_lock(&thrput_compute_mutex);
            //! Measure throughput
            measure_throughput(total_bytes_tx, tt_start, tt_end);
            rsi_mutex_unlock(&thrput_compute_mutex);
            //! reset to initial value
            total_bytes_tx = 0;
            pkt_cnt = 0;
            tt_start = rsi_hal_gettickcount();
#else
            rsi_shutdown(client_socket, 0);
            rsi_free(send_buf);
            LOG_PRINT("\r\nTCP TX completed, conn%d\r\n", wlan_thrput_conf->thread_id);
            rsi_mutex_lock(&thrput_compute_mutex);
            //! Measure throughput
            measure_throughput(total_bytes_tx, tt_start, tt_end);
            rsi_mutex_unlock(&thrput_compute_mutex);
            //! wlan measurement is completed, release the sem
            rsi_semaphore_post(&wlan_thrghput_measurement_compl);
            //! delete the task
            rsi_task_destroy(NULL);
#endif
          }
#endif
        }
      }
      case TCP_RX: {
        LOG_PRINT("\r\nconfiguring socket for TCP rx, conn%d", wlan_thrput_conf->thread_id);
#if HIGH_PERFORMANCE_ENABLE

        status = rsi_socket_config();
        if (status < 0) {
          status = rsi_wlan_get_status();
          return status;
        }

#endif
        //! Create socket
#if SOCKET_ASYNC_FEATURE
        server_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 0, socket_async_recive);
#else
        server_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
#endif
        if (server_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d \r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Create Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        status = rsi_setsockopt(server_socket,
                                SOL_SOCKET,
                                SO_HIGH_PERFORMANCE_SOCKET,
                                &high_performance_socket,
                                sizeof(high_performance_socket));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX, conn%d\r\n",
                    status,
                    wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nSet Socket Option Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        //! Set server structure
        memset(&server_addr, 0, sizeof(server_addr));

        //! Set family type
        server_addr.sin_family = AF_INET;

        //! Set local port number
        server_addr.sin_port = DEVICE_PORT;

        //! Bind socket
        status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX, conn%d \r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nBind Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        //! Socket listen
        status = rsi_listen(server_socket, 1);
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nListen Failed, Error code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nListen Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

        addr_size = sizeof(server_socket);

        //! Socket accept
        new_socket = rsi_accept(server_socket, (struct rsi_sockaddr *)&client_addr, &addr_size);
        if (new_socket < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nSocket Accept Failed, Error code : 0x%lX, conn%d \r\n", status, wlan_thrput_conf->thread_id);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Accept Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }
#if SOCKET_ASYNC_FEATURE
        LOG_PRINT("\r\nTCP RX started, conn%d\r\n", wlan_thrput_conf->thread_id);
        t_start = rsi_hal_gettickcount(); //! capture time-stamp before data transfer is starts
        while (!data_recvd) {
          rsi_semaphore_wait(&wlan_pkt_trnsfer_compl_sem, 0);
        }
        rsi_shutdown(server_socket, 0);
        LOG_PRINT("\r\nTCP RX completed, conn%d\r\n", wlan_thrput_conf->thread_id);
#if !CONTINUOUS_THROUGHPUT
        rsi_mutex_lock(&thrput_compute_mutex);
        compute_throughput();
        rsi_mutex_unlock(&thrput_compute_mutex);
#endif
        //! wlan measurement is completed, release the sem
        rsi_semaphore_post(&wlan_thrghput_measurement_compl);
        //! delete the task
        rsi_task_destroy(NULL);
#else
        //! allocate memory for rx buffer
        recv_buffer = (int8_t *)rsi_malloc(buff_size * sizeof(int8_t));

        while (1) {
          recv_size = buff_size;
          //! take the current time
          tt_start = rsi_hal_gettickcount();
          do {
            //! Receive data on socket
            status =
              rsi_recvfrom(new_socket, recv_buffer, recv_size, 0, (struct rsi_sockaddr *)&client_addr, &addr_size);
            if (status < 0) {
              status = rsi_wlan_socket_get_status(new_socket);
              if (status == RSI_RX_BUFFER_CHECK) {
                continue;
              }
              LOG_PRINT("\r\nTCP Recv Failed, Error code : 0x%X\r\n", status);
              rsi_free(recv_buffer);
              rsi_shutdown(server_socket, 0);
              return status;
            }

            //! subtract received bytes
            recv_size -= status;

            total_bytes_rx += status;
            //! store the time after sending data
            tt_end = rsi_hal_gettickcount();
            //! Measure throughput if avg time is reached
            if (tt_end - tt_start >= THROUGHPUT_AVG_TIME) {
              rsi_mutex_lock(&thrput_compute_mutex);
              //! Measure throughput
              measure_throughput(total_bytes_rx, tt_start, tt_end);
              rsi_mutex_unlock(&thrput_compute_mutex);
              rsi_free(recv_buffer);
              //! reset to initial value
              total_bytes_rx = 0;
              tt_start = rsi_hal_gettickcount();
            }
          } while (recv_size > 0);
        }
#endif
      } break;
      case SSL_TX: {
        if (socket_init_done == 0) {
          LOG_PRINT("\r\nconfiguring socket for SSL tx, conn%d", wlan_thrput_conf->thread_id);
          //! Create socket
          client_socket = rsi_socket(AF_INET, SOCK_STREAM, 1);
          if (client_socket < 0) {
            status = rsi_wlan_get_status();
            LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSocket Create Success, conn%d\r\n", wlan_thrput_conf->thread_id);
          }

          status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX, conn%d\r\n",
                      status,
                      wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSet Socket Option Success, conn%d\r\n", wlan_thrput_conf->thread_id);
          }

          //! Set server structure
          memset(&server_addr, 0, sizeof(server_addr));

          //! Set server address family
          server_addr.sin_family = AF_INET;

          //! Set server port number, using htons function to use proper byte order
          server_addr.sin_port = htons(SERVER_PORT);

          //! Set IP address to localhost
          server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
          //server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)wlan_thrput_conf->server_addr);

          socket_init_done = 1;
        }

        //! Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          //status = rsi_wlan_get_status();
          //rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to Connect to SSL Server, Error code : 0x%lX,conn%d\r\n",
                    status,
                    wlan_thrput_conf->thread_id);
          //return status;
          continue;
        } else {
          LOG_PRINT("\r\nConnect to SSL Server Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }
        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

        LOG_PRINT("\r\nSSL TX started, conn%d\r\n", wlan_thrput_conf->thread_id);

        //! allocate memory for tx buffer
        send_buf = (int8_t *)rsi_malloc(buff_size * sizeof(int8_t));

        for (i = 0; i < buff_size; i++) {
          send_buf[i] = i;
        }

        tt_start = rsi_hal_gettickcount();
        while (1) {
          //! Send data on socket
          status = rsi_send(client_socket, send_buf, buff_size, 0);
          if (status < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(client_socket, 0);
            rsi_free(send_buf);
            LOG_PRINT("\r\nFailed to Send data to SSL Server, Error Code : 0x%lx\r\n, conn%d",
                      status,
                      wlan_thrput_conf->thread_id);
            LOG_PRINT("\r\n client socket is shutdown, conn%d\r\n", wlan_thrput_conf->thread_id);
            //rsi_wlan_app_cb.state = RSI_WLAN_IDLE_STATE;
            //break;
            //return status;
            while (1)
              ;
          }
          pkt_cnt++;
#if CONTINUOUS_THROUGHPUT
          if ((rsi_hal_gettickcount() - t_start) >= THROUGHPUT_AVG_TIME) {
            // take the current time
            t_end          = rsi_hal_gettickcount();
            total_bytes_tx = pkt_cnt * buff_size;
            measure_throughput(total_bytes_tx, t_start, t_end);
            pkt_cnt = 0;
            t_start = rsi_hal_gettickcount();
          }
#else
          //! measure throughput if pkt_cnt reaches max transmit packets
          if (pkt_cnt == MAX_TX_PKTS) {
            // take the current time
            t_end = rsi_hal_gettickcount();
            total_bytes_tx = pkt_cnt * buff_size;
            rsi_shutdown(client_socket, 0);
            rsi_free(send_buf);
            LOG_PRINT("SSL client closed\n");
            LOG_PRINT("\r\nSSL TX completed, conn%d\r\n", wlan_thrput_conf->thread_id);
            rsi_mutex_lock(&thrput_compute_mutex);
            measure_throughput(total_bytes_tx, tt_start, t_end);
            rsi_mutex_unlock(&thrput_compute_mutex);
            total_bytes_tx = 0;
            pkt_cnt = 0;
            //! wlan measurement is completed, release the sem
            rsi_semaphore_post(&wlan_thrghput_measurement_compl);
            //! delete the task
            rsi_task_destroy(NULL);
          }
#endif
        }
      }
      case SSL_RX: {
        if (socket_init_done == 0) {

          LOG_PRINT("\r\nconfiguring socket for SSL rx, conn%d", wlan_thrput_conf->thread_id);
#if HIGH_PERFORMANCE_ENABLE
          status = rsi_socket_config();
          if (status < 0) {
            status = rsi_wlan_get_status();
            return status;
          }
#endif
          //! Create socket
          client_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 1, socket_async_recive);
          if (client_socket < 0) {
            status = rsi_wlan_get_status();
            LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSocket Create Success, conn%d\r\n", wlan_thrput_conf->thread_id);
          }

          status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX, conn%d\r\n",
                      status,
                      wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSet Socket Option Success, conn%d\r\n", wlan_thrput_conf->thread_id);
          }

          status = rsi_setsockopt(client_socket,
                                  SOL_SOCKET,
                                  SO_HIGH_PERFORMANCE_SOCKET,
                                  &high_performance_socket,
                                  sizeof(high_performance_socket));
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX, conn%d\r\n",
                      status,
                      wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nSet Socket Option Success, conn%d\r\n", wlan_thrput_conf->thread_id);
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
            LOG_PRINT("\r\nBind Failed, Error code : 0x%lX, conn%d\r\n", status, wlan_thrput_conf->thread_id);
            return status;
          } else {
            LOG_PRINT("\r\nBind Success, conn%d\r\n", wlan_thrput_conf->thread_id);
          }

          //! Set server structure
          memset(&server_addr, 0, sizeof(server_addr));

          //! Set server address family
          server_addr.sin_family = AF_INET;

          //! Set server port number, using htons function to use proper byte order
          server_addr.sin_port = htons(SSL_RX_SERVER_PORT);

          //! Set IP address to localhost
          server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
          //server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)wlan_thrput_conf->server_addr);
          socket_init_done = 1;
        }

        //! Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to Connect to Server, Error code : 0x%lX, conn%d\r\n",
                    status,
                    wlan_thrput_conf->thread_id);
          //return status;
          continue;
        } else {
          LOG_PRINT("\r\nConnect to Server Success, conn%d\r\n", wlan_thrput_conf->thread_id);
        }

        //! Release the semaphore after socket configuration
        rsi_semaphore_post(&wlan_thrput_conf->wlan_app_sem);
        //! wait for signal from main WLAN task
        rsi_semaphore_wait(&wlan_thrput_conf->wlan_app_sem1, 0);

        LOG_PRINT("\r\nSSL RX started, conn%d\r\n", wlan_thrput_conf->thread_id);

        t_start = rsi_hal_gettickcount(); //! capture time-stamp before data transfer is starts
        while (!data_recvd) {
          rsi_semaphore_wait(&wlan_pkt_trnsfer_compl_sem, 0);
        }
        LOG_PRINT("\r\nSSL RX completed, conn%d\r\n", wlan_thrput_conf->thread_id);
#if !CONTINUOUS_THROUGHPUT
        rsi_mutex_lock(&thrput_compute_mutex);
        compute_throughput();
        rsi_mutex_unlock(&thrput_compute_mutex);
#endif
        //! wlan measurement is completed, release the sem
        rsi_semaphore_post(&wlan_thrghput_measurement_compl);
        //! delete the task
        rsi_task_destroy(NULL);
        break;
      }
    }
  }
}
#endif
