/********************************************************************/
/** \file rsi_sock_test.c
 *
 * \brief Test code to run HTTP/HTTPS download test using Silabs
 *        implementation of BSD socket API
 *
 *   \par MODULE NAME:
 *       redpine_sock_test.c - Silabs sockets test
 *
 *   \par DESCRIPTION:
 *       This module exercises Silabs BSD socket API implementation
 *       by connecting to an HTTP/S server and downloading a test file
 *       with a known pattern. A few runtime knobs are provided via
 *       a few variables which control certain aspects of the test.
 *
 * Copyright 2019 by Garmin Ltd. or its subsidiaries.
 *
 * NOTICE:
 *   This code is intended for Silicon Laboratories internal use only. The code is
 *   considered Confidential Information under our NDA, and is not
 *   considered to be Feedback. This code may not be put into a
 *   public SDK.
 *
 *********************************************************************/
#include <rsi_common_app.h>
//#if COEX_MAX_APP
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "rsi_socket.h"
#include "rsi_wlan_config.h"
#include "rsi_wlan_common_config.h"
#include "rsi_wlan_non_rom.h"
#include "rsi_os.h"
#include "rsi_common_config.h"
#include "rsi_driver.h"
#include <rsi_sock_test.h>
#if WINDOW_UPDATE_FEATURE
#include "FreeRTOS.h"
#include "task.h"
#endif
/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define RSI_CHUNK_SIZE 1024

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/

redpine_socktest_ctx_t redpine_socktest_ctx[SOCKTEST_INSTANCES_MAX];
#if WINDOW_UPDATE_FEATURE
socket_window_memory_ctx_t rsi_socket_window_mem_ctx;
extern rsi_max_available_rx_window_t *max_available_rx_window;
void switch_proto_async(uint16_t mode, uint8_t *bt_disabled_status);
extern uint8_t window_update;
#endif
// 192.168.165.78=NathanLaptop, 192.168.165.28=AlexLaptop
#if RX_DATA
#if HTTPS_DOWNLOAD
const char http_request_str_first[] = "GET /" DOWNLOAD_FILENAME " HTTPS/1.1\r\n"
                                      "Host: " SERVER_IP_ADDRESS "\r\n"
                                      "User-Agent: sock_test\r\n"
                                      "Accept: text/plain\r\n"
                                      "Accept-Language: en-US,en;q=0.5\r\n"
                                      "Accept-Encoding: identity\r\n";
#else
const char http_request_str_first[] = "GET " DOWNLOAD_FILENAME " HTTP/1.1\r\n"
                                      "Host: " SERVER_IP_ADDRESS "\r\n"
                                      "User-Agent: sock_test\r\n"
                                      "Accept: text/plain\r\n"
                                      "Accept-Language: en-US,en;q=0.5\r\n"
                                      "Accept-Encoding: identity\r\n";
#endif
#endif
const char http_request_str_connection_close[] = "Connection: close\r\n";
const char http_request_str_end[]              = "\r\n";
const char tls_sni_name[]                      = "www.garmin.com";

int expected_bytes_to_receive = BYTES_TO_RECEIVE;
int expected_bytes_to_send    = BYTES_TO_TRANSMIT;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_task_handle_t http_socket_task_handle[SOCKTEST_INSTANCES_MAX];
extern uint32_t rsi_convert_4R_to_BIG_Endian_uint32(uint32_t *pw);
extern rsi_semaphore_handle_t wlan_app_sem;
#if SOCKET_ASYNC_FEATURE
extern rsi_semaphore_handle_t sock_wait_sem;
#endif
#if (WLAN_SYNC_REQ && !WLAN_TRANSIENT_CASE)
extern bool rsi_bt_running;
#endif
extern bool rsi_ble_running, rsi_ant_running;

extern volatile uint64_t num_bytes;
extern uint32_t t_start;
#if WINDOW_UPDATE_FEATURE
extern bool rsi_window_update_sem_waiting;
extern uint8_t no_of_iterations;
extern rsi_mutex_handle_t window_update_mutex;
extern rsi_semaphore_handle_t window_reset_notify_sem;
extern redpine_socktest_ctx_t *ctx;
#endif
#if WLAN_SYNC_REQ
extern rsi_semaphore_handle_t sync_coex_ble_sem, sync_coex_ant_sem, sync_coex_bt_sem;
#endif
#if (RX_DATA && HTTPS_DOWNLOAD)
extern rsi_semaphore_handle_t cert_sem, conn_sem;
extern rsi_task_handle_t cert_bypass_task_handle[SOCKTEST_INSTANCES_MAX];
extern cert_bypass_struct_t rsi_cert_bypass[SOCKTEST_INSTANCES_MAX];
#endif
/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/
extern void rsi_task_destroy(rsi_task_handle_t *task_handle);
#if SOCKET_ASYNC_FEATURE
extern void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length);
#endif

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
void init_redpine_sock_test(void);
void perform_redpine_sock_test(void *instance_ctx);

/*=======================================================================*/
//   ! VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

static void break_if(int should_break)
{
  if (should_break) {
    should_break = FALSE;
  }
}

void init_redpine_sock_test(void)
{
  memset(redpine_socktest_ctx, 0, sizeof(redpine_socktest_ctx));
}
#if WINDOW_UPDATE_FEATURE
void rsi_window_reset_notify_app_task(void)
{

  if (!rsi_window_update_sem_waiting) {
    LOG_PRINT("Waiting TCP connection to unblock\n");
    rsi_semaphore_wait(&window_reset_notify_sem, 0);
  }

  while (1) {
    int32_t status = RSI_SUCCESS;
    if (no_of_iterations != NO_OF_ITERATIONS) {
      status = rsi_switch_proto(1, NULL);
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\n failed to enable bt protocol \r\n");
      } else {
        LOG_PRINT("\r\n Enabled bt \r\n");
      }
      vTaskDelay(3000);
    }
    if (no_of_iterations != NO_OF_ITERATIONS) {

      status = rsi_switch_proto(0, switch_proto_async);
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\n failed to disable bt protocol \r\n");
      } else {
        LOG_PRINT("\r\n Disabled bt \r\n");
      }
      vTaskDelay(3000);

    } else {
      rsi_task_destroy(NULL);
    }
  }
}
#endif
#if (RX_DATA && HTTPS_DOWNLOAD)
uint8_t cert_buffer[8192];
void certificate_response_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  rsi_cert_recv_t *cert_recev     = (rsi_cert_recv_t *)buffer;
  uint32_t sockID                 = 0;
  sockID                          = rsi_get_socket_descriptor(cert_recev->src_port,
                                     cert_recev->dst_port,
                                     cert_recev->ip_address.ipv6_address,
                                     rsi_bytes2R_to_uint16(cert_recev->ip_version),
                                     rsi_bytes2R_to_uint16(cert_recev->sock_desc));
  rsi_cert_bypass[sockID].sock_id = rsi_bytes2R_to_uint16(cert_recev->sock_desc);
  //! User needs to copy the certificate into the certificate buffer
  if (status != RSI_SUCCESS) {
    LOG_PRINT("error status 0x%x", status);
    while (1)
      ;
  } else {
    if (cert_recev->more_chunks == 0) {
      rsi_cert_bypass[sockID].cert_valid = 1;
      //! Complete certificate received
      rsi_semaphore_post(&cert_sem);
    }
  }

  return;
}
void rsi_app_task_send_certificates()
{

  int32_t status = RSI_SUCCESS;
  int i          = 0;
  while (1) {
    ////////////////////////////////////////
    //! User certificate validation code ///
    ////////////////////////////////////////
    /* User need to take care of validating the received certificate
					 and then only should send the certificate valid reponse to the module */
    rsi_semaphore_wait(&cert_sem, 0);
    for (i = 0; i < SOCKTEST_INSTANCES_MAX; i++) {
      if (rsi_cert_bypass[i].cert_valid) {
        rsi_cert_bypass[i].cert_valid = 0;
        //! Send certificate valid response
        status = rsi_certificate_valid(1, rsi_cert_bypass[i].sock_id);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("SSL certificate valid send failed\n");
          //return status;
        }
        break;
      } else {
        continue;
      }
    }
    rsi_task_destroy(NULL);
  }
}
#endif
void perform_redpine_sock_test(void *instance_ctx)
{

  uint32_t k = 0;
  //This statement is added to resolve the warning: [-unused-but-set-variable],so declared the variable in a macro in which the variable is used.
#if VERIFY_RX_DATA
  uint32_t data_start_offs;
#endif
  int num_fds_ready;
  rsi_fd_set readfds;
  //This statement is added to resolve the warning: [-unused-but-set-variable],so declared the variable in a macro in which the variable is used.
#if SELECT_ON_WRITEFDS
  rsi_fd_set writefds;
#endif
  //uint32_t t_start=0;
  struct rsi_timeval timeout  = { 0 };
  redpine_socktest_ctx_t *ctx = instance_ctx;
  //This statement is added to resolve the warning: [-unused-but-set-variable],so declared the variable in a macro in which the variable is used.
#if WINDOW_UPDATE_FEATURE
  uint32_t window_update_value = 0;
#endif

  LOG_PRINT("Thread %d started \r\n", ctx->threadid);

  rsi_semaphore_wait(&ctx->http_soc_wait_sem, 0);

  /* Clear most of the context, except for the error tracking members */
  memset(ctx, 0, (uint32_t)(void *)&ctx->num_successful_test - (uint32_t)(void *)ctx);
  ctx->sock_fd = -1;

  /* RS9116 uses bit 1 of the protocol to signal TLS socket */
#if SOCK_NON_BLOCK
  ctx->sock_fd = rsi_socket(PF_INET, SOCK_STREAM | O_NONBLOCK, 0);
#elif (HIGH_PERFORMANCE_ENABLE && SOCKET_ASYNC_FEATURE)
  ctx->sock_fd = rsi_socket_async(AF_INET, SOCK_STREAM, 0, socket_async_recive);
#else
  ctx->sock_fd     = rsi_socket(PF_INET, SOCK_STREAM, 0);
#endif
  if (-1 == ctx->sock_fd) {
    /* Failed to create socket */
    ctx->errortrack[0]++;
    return;
  }
#if WLAN_THROUGHPUT_ENABLE
  ctx->high_performance_socket = 1;
#endif
#if RX_DATA
  if ((ctx->threadid < TCP_RX_HIGH_PERFORMANCE_SOCKETS) && (ctx->high_performance_socket == 1)) {
    ctx->res = rsi_setsockopt(ctx->sock_fd, SOL_SOCKET, SO_HIGH_PERFORMANCE_SOCKET, NULL, 0);
    if (ctx->res != 0) {
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[16]++;
      break_if((int)1);
    }
  }
#endif
#if TEST_SOCKET_SHUTDOWN

  ctx->res = rsi_shutdown(ctx->sock_fd, 0);
  if (ctx->res != 0) {
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[15]++;
    break_if((int)1);
  }
  //TSK_suspend( 1000 );
  rsi_os_task_delay(1000);
  return;
#endif

  memset(&ctx->sa, 0x00, sizeof(ctx->sa));
  ctx->sa.sin_family = AF_INET;

  /* IP addr for server */
  ctx->sa.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);

#if (RX_DATA && HTTPS_DOWNLOAD)
  /* Talk to port 443 required */
  ctx->sa.sin_port = htons(443);
#elif (RX_DATA && (!HTTPS_DOWNLOAD))
  /* Talk to port 80 required */
  ctx->sa.sin_port = htons(80);
#else
  ctx->sa.sin_port = htons(SERVER_PORT);
#endif

#if (RX_DATA && HTTPS_DOWNLOAD)
  rsi_semaphore_wait(&conn_sem, 0);
  ctx->res = rsi_setsockopt(ctx->sock_fd, SOL_SOCKET, SO_SSL_ENABLE, NULL, 0);
  if (0 != ctx->res) {
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[1]++;
    break_if((int)1);
    goto ERROR_EXIT_send_socket_data;
  }
  ctx->res = rsi_setsockopt(ctx->sock_fd, SOL_SOCKET, SO_RCVBUF, cert_buffer, sizeof(cert_buffer));
  if (0 != ctx->res) {
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[1]++;
    break_if((int)1);
    goto ERROR_EXIT_send_socket_data;
  }
#if TLS_SNI_FEATURE

  ctx->res = rsi_setsockopt(ctx->sock_fd, SOL_SOCKET, SO_TLS_SNI, tls_sni_name, (sizeof(tls_sni_name) - 1));
  if (0 != ctx->res) {
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[1]++;
    break_if((int)1);
    goto ERROR_EXIT_send_socket_data;
  }
#endif
#endif
  ctx->res = rsi_connect(ctx->sock_fd, (struct rsi_sockaddr *)&ctx->sa, sizeof(struct rsi_sockaddr_in));
  if (0 != ctx->res) {
    ctx->res = rsi_wlan_socket_get_status(ctx->sock_fd);
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[2]++;
    break_if((int)1);
    goto ERROR_EXIT_send_socket_data;
  }
#if (RX_DATA && HTTPS_DOWNLOAD)
  rsi_semaphore_post(&conn_sem);
#endif
#if WINDOW_UPDATE_FEATURE
  window_update_value = 8096;
  window_update_value = (window_update_value > rsi_socket_window_mem_ctx.Max_Window_threshold)
                          ? rsi_socket_window_mem_ctx.Max_Window_threshold
                          : window_update_value;
  if (rsi_socket_window_mem_ctx.Avaiable_TCP_Window
      >= (window_update_value - rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd])) {
    ctx->res = rsi_tcp_window_update(ctx->sock_fd, window_update_value);
    if (ctx->res > 0) {
      LOG_PRINT("Updated window size: %d\r\n", ctx->res);
      rsi_socket_window_mem_ctx.Avaiable_TCP_Window -=
        (ctx->res - rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd]);
      rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd] = ctx->res;
    } else {
      ctx->res = rsi_get_error(ctx->sock_fd);
      LOG_PRINT("Window update error: %x\r\n", ctx->res);
    }
  }
  if (rsi_window_update_sem_waiting == false) {
    rsi_window_update_sem_waiting = true;
    rsi_semaphore_post(&window_reset_notify_sem);
  }
#endif
#if SOCK_NON_BLOCK
SET_WRITEFDS:
  RSI_FD_ZERO(&writefds);
  RSI_FD_SET(ctx->sock_fd, &writefds);
  ctx->even_odd++;
  timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
  num_fds_ready  = rsi_select(ctx->sock_fd + 1, NULL, &writefds, NULL, &timeout, NULL);
  ctx->selwrfd_ret_cnt++;
  /* Check for error */
  if (num_fds_ready <= 0) {
    ctx->errortrack[3]++;
    num_fds_ready = 0;
    break_if((int)1);
    goto SET_WRITEFDS;
  }
  ctx->selwrfd_ready_cnt++;

  if (RSI_FD_ISSET(ctx->sock_fd, &writefds)) {
    ctx->res = rsi_getsockopt(ctx->sock_fd, SOL_SOCKET, SO_CHECK_CONNECTED_STATE, NULL, 0);
    if (0 != ctx->res) {
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[1]++;
      break_if((int)1);
      goto ERROR_EXIT_send_socket_data;
    }
  }
#endif
#if (WLAN_SYNC_REQ && !WLAN_TRANSIENT_CASE)
  //! unblock other protocol activities
  if (rsi_bt_running) {
    rsi_semaphore_post(&sync_coex_bt_sem);
  }
  if (rsi_ble_running) {
    rsi_semaphore_post(&sync_coex_ble_sem);
  }
  if (rsi_ant_running) {
    rsi_semaphore_post(&sync_coex_ant_sem);
  }
#endif
  t_start = rsi_hal_gettickcount();

  /***************************************************
TODO:
Basic wait loop to wait for the socket to connect to the server.
This loop requires that the server is available and we can connect to it.
Proper handling should be made using the rsi_wlan_rsp_socket_create callback notification.
	 ***************************************************/

#if (!(THROUGHPUT_EN && HIGH_PERFORMANCE_ENABLE))
  while (rsi_socket_pool[ctx->sock_fd].sock_state == 1) {
    //TSK_suspend(10);
    rsi_os_task_delay(10);
  };

#if TX_DATA

  ctx->bytes_sent = 0;
  while (ctx->bytes_sent != expected_bytes_to_send) {

    j = 0;
    for (k = 0; k < (RSI_CHUNK_SIZE) / 4; k++) {
      memcpy(ctx->http_buffer + j, &i, 4);
      j += 4;
      i++;
    }

#if SELECT_ON_WRITEFDS

    RSI_FD_ZERO(&writefds);
    RSI_FD_SET(ctx->sock_fd, &writefds);
    ctx->even_odd++;
    timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
    num_fds_ready  = rsi_select(ctx->sock_fd + 1, NULL, &writefds, NULL, &timeout, NULL);
    ctx->selwrfd_ret_cnt++;
    /* Check for error */
    if (num_fds_ready < 0) {
      ctx->errortrack[3]++;
      num_fds_ready = 0;
      break_if((int)1);
      break;
    }
    if (0 == RSI_FD_ISSET(ctx->sock_fd, &writefds)) {
      /* Not really ready, must be timeout 0 seconds */
      ctx->selwrfd_notready_cnt++;
      continue;
    }
    ctx->selwrfd_ready_cnt++;
    ;
#endif

    ctx->res = rsi_send(ctx->sock_fd, (int8_t *)ctx->http_buffer, RSI_CHUNK_SIZE, 0);
    /* Check for error */

    if (ctx->res < 0) {
      ctx->res = rsi_wlan_socket_get_status(ctx->sock_fd);
      LOG_PRINT("error %x \r\n", ctx->res);
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[4]++;
      break_if((int)1);
      goto ERROR_EXIT_send_socket_data;
    }
    ctx->bytes_sent += RSI_CHUNK_SIZE;
  }
  break_if((int)1);
  goto ERROR_EXIT_send_socket_data;
#endif
#if RX_DATA
  /* Send first set of headers to server */
  ctx->bytes_sent = 0;
  while (ctx->bytes_sent != strlen(http_request_str_first)) {
#if SELECT_ON_WRITEFDS

    RSI_FD_ZERO(&writefds);
    RSI_FD_SET(ctx->sock_fd, &writefds);
    ctx->even_odd++;
    timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
    num_fds_ready  = rsi_select(ctx->sock_fd + 1, NULL, &writefds, NULL, &timeout, NULL);
    ctx->selwrfd_ret_cnt++;
    /* Check for error */
    if (num_fds_ready < 0) {
      ctx->errortrack[3]++;
      num_fds_ready = 0;
      break_if((int)1);
      break;
    }
    if (0 == RSI_FD_ISSET(ctx->sock_fd, &writefds)) {
      /* Not really ready, must be timeout 0 seconds */
      ctx->selwrfd_notready_cnt++;
      continue;
    }
    ctx->selwrfd_ready_cnt++;
    ;
#endif

    ctx->res = rsi_send(ctx->sock_fd,
                        (int8_t *)(http_request_str_first + ctx->bytes_sent),
                        strlen(http_request_str_first) - ctx->bytes_sent,
                        0);
    /* Check for error */
    if (ctx->res < 0) {
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[4]++;
      break_if((int)1);
      goto ERROR_EXIT_send_socket_data;
    }
    ctx->bytes_sent += ctx->res;
  }

  /* Send connection close headers to server */
#if USE_CONNECTION_CLOSE

  ctx->bytes_sent = 0;
  while (ctx->bytes_sent != strlen(http_request_str_connection_close)) {
#if SELECT_ON_WRITEFDS

    RSI_FD_ZERO(&writefds);
    RSI_FD_SET(ctx->sock_fd, &writefds);
    ctx->even_odd++;
    timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
    num_fds_ready  = rsi_select(ctx->sock_fd + 1, NULL, &writefds, NULL, &timeout, NULL);
    ctx->selwrfd_ret_cnt++;
    /* Check for error */
    if (num_fds_ready < 0) {
      ctx->errortrack[5]++;
      num_fds_ready = 0;
      break_if((int)1);
      break;
    }
    if (0 == RSI_FD_ISSET(ctx->sock_fd, &writefds)) {
      /* Not really ready, must be timeout 0 seconds */
      ctx->selwrfd_notready_cnt++;
      continue;
    }
    ctx->selwrfd_ready_cnt++;
    ;
#endif

    ctx->res = rsi_send(ctx->sock_fd,
                        (int8_t *)(http_request_str_connection_close + ctx->bytes_sent),
                        strlen(http_request_str_connection_close) - ctx->bytes_sent,
                        0);
    /* Check for error */
    if (ctx->res < 0) {
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[6]++;
      break_if((int)1);
      goto ERROR_EXIT_send_socket_data;
    }
    ctx->bytes_sent += ctx->res;
  }
#endif

  /* Send last set of headers to server */
#if !HTTPS_DOWNLOAD
  ctx->bytes_sent = 0;
  while (ctx->bytes_sent != strlen(http_request_str_end)) {
#if SELECT_ON_WRITEFDS

    RSI_FD_ZERO(&writefds);
    RSI_FD_SET(ctx->sock_fd, &writefds);
    ctx->even_odd++;
    timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
    num_fds_ready  = rsi_select(ctx->sock_fd + 1, NULL, &writefds, NULL, &timeout, NULL);
    ctx->selwrfd_ret_cnt++;
    /* Check for error */
    if (num_fds_ready < 0) {
      ctx->errortrack[7]++;
      num_fds_ready = 0;
      break_if((int)1);
      break;
    }
    if (0 == RSI_FD_ISSET(ctx->sock_fd, &writefds)) {
      /* Not really ready, must be timeout 0 seconds */
      ctx->selwrfd_notready_cnt++;
      continue;
    }
    ctx->selwrfd_ready_cnt++;
    ;
#endif

    ctx->res = rsi_send(ctx->sock_fd,
                        (int8_t *)(http_request_str_end + ctx->bytes_sent),
                        strlen(http_request_str_end) - ctx->bytes_sent,
                        0);
    /* Check for error */
    if (ctx->res < 0) {
      ctx->res = rsi_get_error(ctx->sock_fd);
      ctx->errortrack[8]++;
      break_if((int)1);
      goto ERROR_EXIT_send_socket_data;
    }
    ctx->bytes_sent += ctx->res;
  }
#endif
  /* Receive data from server */
  ctx->headers_received = 0;
  ctx->bytes_received   = 0;
#if (HTTPS_DOWNLOAD && RX_DATA)
  LOG_PRINT("HTTPS download started \r\n");
#elif ((!HTTPS_DOWNLOAD) && RX_DATA)
  LOG_PRINT("HTTP download started \r\n");
#endif
#if !SOCKET_ASYNC_FEATURE
  num_bytes = 0;
  while (ctx->bytes_received != expected_bytes_to_receive) {
#if WINDOW_UPDATE_FEATURE
    rsi_mutex_lock(&window_update_mutex);
    if (window_update) {
      window_update_value = 18096;
      window_update_value = (window_update_value > rsi_socket_window_mem_ctx.Max_Window_threshold)
                              ? rsi_socket_window_mem_ctx.Max_Window_threshold
                              : window_update_value;
      if (rsi_socket_window_mem_ctx.Avaiable_TCP_Window
          >= (window_update_value - rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd])) {
        ctx->res = rsi_tcp_window_update(ctx->sock_fd, window_update_value);
        if (ctx->res > 0) {
          LOG_PRINT("Updated window size %d\r\n", ctx->res);
          rsi_socket_window_mem_ctx.Avaiable_TCP_Window -=
            (ctx->res - rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd]);
          rsi_socket_window_mem_ctx.socket_window[ctx->sock_fd] = ctx->res;
        } else {
          ctx->res = rsi_get_error(ctx->sock_fd);
          LOG_PRINT("Window update error: %x\r\n", ctx->res);
        }
      }
      window_update = 0;
    }
    rsi_mutex_unlock(&window_update_mutex);
#endif
    int bytes_read;
    /* Configure fd set */
    RSI_FD_ZERO(&readfds);
    RSI_FD_SET(ctx->sock_fd, &readfds);

    /* Configure timeout */
    ctx->even_odd++;
    timeout.tv_sec = (ctx->even_odd % 2) ? 10 : 0; /* alternate between 10 and 0 seconds */
    num_fds_ready  = rsi_select(ctx->sock_fd + 1, &readfds, NULL, NULL, &timeout, NULL);
    ctx->selrdfd_ret_cnt++;

    /* Check for error */
    if (num_fds_ready < 0) {
      ctx->errortrack[9]++;
      break_if((int)1);
      num_fds_ready = 0;
      break;
    }

    /* Read data from socket */
    if (RSI_FD_ISSET(ctx->sock_fd, &readfds)) {
      ctx->selrdfd_ready_cnt++;
      /* recv() has been seen to return -1 on occasion. We don't have errno available yet */
      /* from Silabs but it has been observed that calling recv again works sometimes. */
      for (k = 0; k < 3; k++) {
        bytes_read = rsi_recv(ctx->sock_fd,
                              ctx->http_buffer + ctx->http_buffer_bytes_saved,
                              http_buffer_sz - ctx->http_buffer_bytes_saved,
                              0);
        if (bytes_read >= 0 && bytes_read <= http_buffer_sz) {
          if (k > 0) {
            ctx->errortrack[11]++;
          } /* Track recoveries after error. */
          break;
        }
        if (bytes_read < 0) {
          ctx->res = rsi_wlan_socket_get_status(ctx->sock_fd);
          //ctx->res = rsi_get_error(ctx->sock_fd);
          if (ctx->res == RSI_ERROR_ENOBUFS) {
            continue;
          }
        }
        ctx->errortrack[12]++;
        bytes_read = 0;
        //TSK_suspend( 10 );
        rsi_os_task_delay(20);
      }
      break_if(k >= 3);
      if (k >= 3) {
        break;
      }

      if (ctx->http_buffer_bytes_saved) {
        bytes_read += ctx->http_buffer_bytes_saved;
        ctx->http_buffer_bytes_saved = 0;
      }

      /* Null terminate the http_buffer */
      ctx->http_buffer[bytes_read] = '\0';
    } else {
      /* Not ready to read, go back to select */
      ctx->selrdfd_notready_cnt++;
      continue;
    }

    /* Loop through looking for data*/
    if (!ctx->headers_received) {
      /* http_buffer has already been null terminated to assist with processing */
      char *tmp_ptr = strstr(ctx->http_buffer, "\r\n\r\n");

      /* Header separator found */
      if (tmp_ptr) {
        tmp_ptr += 4;
        ctx->headers_received = 1;
        bytes_read -= (tmp_ptr - ctx->http_buffer);
        //This statement is added to resolve the warning: [-unused-but-set-variable],so declared the variable in a macro in which the variable is used.
#if VERIFY_RX_DATA
        data_start_offs = tmp_ptr - ctx->http_buffer;
#endif
      } else {

        /* Separator not found, save last 3 bytes to account for "\r\n\r" being at the end of the first msg and "\n" being at the start of the next */
        ctx->http_buffer_bytes_saved = 3;
        memmove(ctx->http_buffer,
                ctx->http_buffer + bytes_read - ctx->http_buffer_bytes_saved,
                ctx->http_buffer_bytes_saved);
      }
    } else {
      //This statement is added to resolve the warning: [-unused-but-set-variable],so declared the variable in a macro in which the variable is used.
#if VERIFY_RX_DATA
      data_start_offs = 0;
#endif
    }

    if (ctx->headers_received) {
      break_if((int)(bytes_read < 0));
      break_if((int)((ctx->bytes_received + bytes_read) > expected_bytes_to_receive));
      ctx->bytes_received += bytes_read;
#if VERIFY_RX_DATA

      /* Received data is expected to follow a pattern that we can verify here. */
      /* This is to detect any discrepancies in received data.*/
      uint32_t *pw        = (uint32_t *)&ctx->http_buffer[data_start_offs];
      uint32_t bytes_left = bytes_read;
#if BIG_ENDIAN_CONVERSION_REQUIRED
      uint32_t value = 0;
#endif

      while (bytes_left >= 4) {
#if BIG_ENDIAN_CONVERSION_REQUIRED
        //! converting the Big endian array data
        value = rsi_convert_4R_to_BIG_Endian_uint32(pw);
        if (ctx->verification_val != value) {
          ctx->errortrack[13]++;
          break_if((int)1);
          break;
        }
#else
        if (ctx->verification_val != *pw) {
          ctx->errortrack[13]++;
          break_if((int)1);
          break;
        }
#endif

        ctx->verification_val++;
        pw++;
        bytes_left -= 4;
      }
      if (bytes_left) {
        memmove(ctx->http_buffer, pw, bytes_left);
        ctx->http_buffer_bytes_saved = bytes_left;
        ctx->bytes_received -= bytes_left;
      }
#endif
      bytes_read = 0;
    }
  }
  num_bytes = ctx->bytes_received;
#endif
#endif
#endif
#if SOCKET_ASYNC_FEATURE
  rsi_semaphore_wait(&sock_wait_sem, 0);
#endif
ERROR_EXIT_send_socket_data:

  ctx->sock_shutdown_in_progress = TRUE;
  ctx->res                       = rsi_shutdown(ctx->sock_fd, 0);
  if (ctx->res != 0) {
    ctx->res = rsi_get_error(ctx->sock_fd);
    ctx->errortrack[14]++;
    break_if((int)1);
  }
  ctx->sock_shutdown_in_progress = FALSE;
#if !SOCKET_ASYNC_FEATURE
#if RX_DATA
  if (ctx->bytes_received != expected_bytes_to_receive) {
    ctx->num_failed_test++;
  } else {
    ctx->num_successful_test++;
  }

  break_if((int)(ctx->bytes_received != expected_bytes_to_receive));

#else
  if (ctx->bytes_sent != expected_bytes_to_send) {
    ctx->num_failed_test++;
  } else {
    ctx->num_successful_test++;
  }
#endif
#endif

  LOG_PRINT("Thread %d completed \r\n", ctx->threadid);
  http_socket_task_handle[ctx->threadid] = NULL;
  rsi_semaphore_destroy(&ctx->http_soc_wait_sem);
  rsi_semaphore_post(&wlan_app_sem);
  rsi_task_destroy(NULL);
}
//#endif
