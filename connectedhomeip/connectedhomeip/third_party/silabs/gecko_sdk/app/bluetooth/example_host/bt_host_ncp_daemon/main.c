/***************************************************************************//**
 * @file
 * @brief NCP Daemon Host application for PC.
 *
 * Bridges UART data to an encrypted and an unencrypted socket.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <time.h>
#include "app_log.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "uart.h"
#include "sl_poll.h"
#include "sl_ncp_daemon.h"
#include "config.h"
#include "ncp_sec_host.h"

#if __APPLE__ != 1 && __linux != 1 && __CYGWIN__ != 1
#error "**** Unsupported OS! This UART driver works on OSX and Linux only! ****"
#endif /* __APPLE__ != 1 && __linux != 1 && __CYGWIN__ != 1 */

int enc_socket = -1;
int enc_server_socket = -1;
int unenc_socket = -1;
int unenc_server_socket = -1;
int serial_handle = -1;
sl_bgapi_context_t uart_bgapi;
/** The serial port to use for BGAPI communication. */
static char* uart_port = NULL;
/** The baud rate to use. */
static uint32_t baud_rate = 0;

int32_t handle = -1;
void * handle_ptr;

/** Usage string */
#define USAGE "Usage: %s [serial port] [baud rate] [encrypted_domain_socket] [unencrypted_domain_socket]" APP_LOG_NL APP_LOG_NL

/**************************************************************************//**
 * Uart Init.
 *****************************************************************************/
int uart_init(int argc, char* argv[])
{
  /**
   * Handle the command-line arguments.
   */

  baud_rate = atoi(argv[2]);
  uart_port = argv[1];

  if (!uart_port || !baud_rate ) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  handle_ptr = &handle;

  /**
   * Initialise the serial port.
   */
  serial_handle = uartOpen(handle_ptr, (int8_t*)uart_port, baud_rate, 1, -1);
  if (serial_handle < 0) {
    app_log_critical("Uart open failed" APP_LOG_NL);
    exit(EXIT_FAILURE);
  }
  sl_poll_add(serial_handle);

  return serial_handle;
}

/**************************************************************************//**
 * Creating a connectable domain (named) socket server.
 *****************************************************************************/
int create_domain_socket_server(char *fname)
{
  int len, rc;
  int sock;
  struct sockaddr_un server_sockaddr;
  //create socket
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  sock = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock == -1) {
    app_log_critical("Socket Error: %d" APP_LOG_NL, errno);
    exit(EXIT_FAILURE);
  }
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path, fname);
  len = sizeof(server_sockaddr);

  //remove existing file
  unlink(fname);
  rc = bind(sock, (struct sockaddr *) &server_sockaddr, len);
  if (rc == -1) {
    app_log_error("BIND ERROR: %d" APP_LOG_NL, errno);
    close(sock);
    return -1;
  }

  //Listen
  rc = listen(sock, 10);
  if (rc == -1) {
    app_log_error("LISTEN ERROR: %d" APP_LOG_NL, errno);
    close(sock);
    return -1;
  }
  sl_poll_add(sock);
  return sock;
}
// accept incoming socket, close old socket if exist
int accept_domain_socket(int server, int oldsock)
{
  int n;
  struct sockaddr_un client_sockaddr;
  socklen_t socklen = sizeof(client_sockaddr);
  memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
  // new connection, close previout socket if exist
  if (oldsock >= 0) {
    close(oldsock);
    sl_poll_remove(oldsock);
  }
  // accept connection request
  n = accept(server, (struct sockaddr *) &client_sockaddr, &socklen);
  if (n < 0) {
    return -1;
  }

  return n;
}

/**************************************************************************//**
 * Main program.
 *****************************************************************************/
int main(int argc, char* argv[])
{
  int serialh;
  if (argc < 5) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  if (sl_poll_init() < 0) {
    app_log_critical("Socket initialization error" APP_LOG_NL);
    exit(EXIT_FAILURE);
  }
  if ((serialh = uart_init(argc, argv)) < 0) {
    app_log_critical("HW init failure" APP_LOG_NL);
    exit(EXIT_FAILURE);
  }
  if (security_init() < 0) {
    app_log_critical("Security init failure" APP_LOG_NL);
    exit(EXIT_FAILURE);
  }

  enc_server_socket = create_domain_socket_server(argv[3]);
  unenc_server_socket = create_domain_socket_server(argv[4]);
  sl_bgapi_context_init(&uart_bgapi, serialh);
  sl_bgapi_set_ncp_target(&uart_bgapi);

  app_log_info("Connected to UART, ready for socket connection." APP_LOG_NL);

  while (1) {
    sl_poll_wait(0);

    sl_poll_update();
  }
  return 0;
}

/**************************************************************************//**
 * This callback is called when received bgapi packet needs to be forwarded to
 * application.
 *****************************************************************************/
void sl_bgapi_process_packet_cb(sl_bgapi_context_t *ctx, char *buf, int len,
                                int encrypted)
{
  if (encrypted) {//if packet is encrypted, forward to encrypted socket
    if (enc_socket >= 0) {
      send(enc_socket, buf, len, 0);
    }
  } else {//unencrypted data, send to both sockets
    if (unenc_socket >= 0) {
      send(unenc_socket, buf, len, 0);
    }
    if (enc_socket >= 0) {
      send(enc_socket, buf, len, 0);
    }
  }
}

/**************************************************************************//**
 * This callback is called when security state changes.
 *****************************************************************************/
void security_state_change_cb(security_state_t state)
{
  switch (state) {
    case SECURITY_STATE_ENCRYPTED:
      //move encrypted socket to be polled if not already
      if (!sl_poll_is_polled(enc_socket)) {//
        app_log_info("Link encrypted" APP_LOG_NL);
        sl_poll_add(enc_socket);
      }
      break;
    case SECURITY_STATE_UNENCRYPTED:
      //security state is unencrypted
      if (enc_socket >= 0) {
        // move encrypted socket to unpolled state
        if (sl_poll_is_polled(enc_socket)) {//
          app_log_info("Link not encrypted" APP_LOG_NL);
          sl_poll_remove(enc_socket);
        }
        //start security handshake
        security_start();
      }

      break;
    default:
      break;
  }
}

/**************************************************************************//**
 * This callback is called when file descriptor has an event.
 *****************************************************************************/
int sl_poll_cb(int fd, int revents)
{
  char buf[MAX_PACKET_SIZE];

  // encrypted server socket event
  if ((revents & POLLIN) && (fd == enc_server_socket)) {
    enc_socket = accept_domain_socket(enc_server_socket, enc_socket);
    app_log_info("Host connected to encrypted socket" APP_LOG_NL);
    if (get_security_state() == SECURITY_STATE_ENCRYPTED) {
      //already encrypted, add to poll list
      if (sl_poll_add(enc_socket) < 0) {
        //failed to add to poll list
        close(enc_socket);
      }
    }
    security_start();
  }
  // unencrypted server socket event
  if ((revents & POLLIN) && (fd == unenc_server_socket)) {
    unenc_socket = accept_domain_socket(unenc_server_socket, unenc_socket);
    app_log_info("Host connected to unencrypted socket" APP_LOG_NL);
    // and add it to poll list
    if (sl_poll_add(unenc_socket) < 0) {
      //failed to add to poll list
      close(unenc_socket);
    }
  }
  // Serial port event
  if ((revents & POLLIN) && (fd == serial_handle)) {
    int len = read(serial_handle, buf, sizeof(buf));
    app_log_info("%x< ", len);
    sl_bgapi_recv_data(&uart_bgapi, buf, len);
  }
  // Host encrypted socket event
  if ((revents & POLLIN) && (fd == enc_socket)) {
    //receive data after the packet
    int len = recv(fd, buf, sizeof(buf), 0);
    app_log_info("%x> ", len);

    if (len == 0) {
      //socket closed
      app_log_info("Host encrypted disconnected" APP_LOG_NL);
      close(fd);
      sl_poll_remove(fd);
      enc_socket = -1;
      return -1;
    } else if (len < 0) {
      return 0;
    } else {
      //ENCRYPT
      char tbuf[MAX_PACKET_SIZE];
      unsigned tlen = len;

      security_encrypt_packet(buf, tbuf, &tlen);

      //
      write(serial_handle, tbuf, (size_t)tlen);
    }
  }
  // Host unencrypted socket event
  if ((revents & POLLIN) && (fd == unenc_socket)) {
    //receive data after the packet
    int len = recv(fd, buf, sizeof(buf), 0);
    app_log_info("%x> ", len);

    if (len == 0) {
      //socket closed
      app_log_info("Host unencrypted disconnected" APP_LOG_NL);
      close(fd);
      sl_poll_remove(fd);
      unenc_socket = -1;
      return -1;
    } else if (len < 0) {
      return 0;
    } else {
      write(serial_handle, buf, (size_t)len);
    }
  }
  return 0;
}
