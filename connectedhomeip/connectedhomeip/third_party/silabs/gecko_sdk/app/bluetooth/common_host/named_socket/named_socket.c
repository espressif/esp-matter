/***************************************************************************//**
 * @file
 * @brief Named socket handler
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

#include <stdio.h>
#include <stdlib.h>
#include "app_log.h"
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/param.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>
#include "named_socket.h"

#if __APPLE__ != 1 && __linux != 1 && __CYGWIN__ != 1
#error "**** Unsupported OS! This UART driver works on OSX and Linux only! ****"
#endif // __APPLE__ != 1 && __linux != 1 && __CYGWIN__ != 1

static int enc_client_socket = -1;
static int unenc_client_socket = -1;
static bool encrypted = false;

static uint8_t buf[MAX_PACKET_SIZE];
static uint8_t *buf_pointer;
static uint8_t unhandled_data_size;

static struct pollfd pollStruct;

static int read_domain_socket(int fd, int revents);

int connect_domain_socket_server(void *handle, char *fname_server, char *fname_client,
                                 int encrypted)
{
  int len, rc;
  struct sockaddr_un server_sockaddr;
  struct sockaddr_un client_sockaddr;
  static int *client_socket;

  if (encrypted) {
    client_socket = &enc_client_socket;
  } else {
    client_socket = &unenc_client_socket;
  }

  //create socket
  memset(&server_sockaddr, 0, sizeof(struct sockaddr_un));
  memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));

  // Create UNIX domain stream socket for the Client
  *client_socket = socket(AF_UNIX, SOCK_STREAM, 0);
  if (*client_socket == -1) {
    perror("Socket Error");
    return -1;
  }

  client_sockaddr.sun_family = AF_UNIX;
  strcpy(client_sockaddr.sun_path, fname_client);
  len = sizeof(client_sockaddr);

  // Add filepath to UNIX sockaddr structure, unlink (delete
  // any existing socket with same path) and bind to socket
  unlink(fname_client);
  rc = bind(*client_socket, (struct sockaddr *) &client_sockaddr, len);
  if (rc == -1) {
    perror("BIND ERROR");
    close(*client_socket);
    return -1;
  }

  // Create UNIX domain stream socket for the Server
  server_sockaddr.sun_family = AF_UNIX;
  strcpy(server_sockaddr.sun_path, fname_server);
  rc = connect(*client_socket, (struct sockaddr *) &server_sockaddr, len);
  if (rc == -1) {
    perror("CONNECT ERROR");
    close(*client_socket);
    return -1;
  }

  /* Update poll structure with client domain socket file descriptor */
  pollStruct.fd = *client_socket;
  pollStruct.events = POLLIN;
  pollStruct.revents = 0;

  *(int32_t *)handle = *client_socket;
  return 0;
}

void af_socket_tx(uint32_t msg_len, uint8_t *msg_data)
{
  if (encrypted) {
    send(enc_client_socket, msg_data, msg_len, 0);
  } else {
    send(unenc_client_socket, msg_data, msg_len, 0);
  }
}

int32_t af_socket_rx(uint32_t msg_len, uint8_t *msg_data)
{
  poll_update(50);

  if (unhandled_data_size > 0) {
    if (unhandled_data_size >= msg_len) {
      unhandled_data_size -= msg_len;
      for (int i = 0; i < msg_len; i++) {
        *(msg_data + i) = *buf_pointer;
        buf_pointer++;
      }
      return msg_len;
    } else {
      unhandled_data_size = 0;
      return -1;
    }
  }
  return -1;
}

int32_t af_socket_rx_peek()
{
  poll_update(50);

  return unhandled_data_size > 0;
}

void turn_encryption_on(void)
{
  encrypted = true;
}

void turn_encryption_off(void)
{
  encrypted = false;
}

void poll_update(int timeout)
{
  poll(&pollStruct, 1, timeout == 0 ? -1 : timeout);
  if (pollStruct.revents != 0) {
    read_domain_socket(pollStruct.fd, pollStruct.revents);
  }
}

//this callback is called when file descriptor has an event
static int read_domain_socket(int fd, int revents)
{
  if ((!unhandled_data_size) && ((revents & POLLIN)
                                 && ((fd == enc_client_socket) || (fd == unenc_client_socket)))) {
    //receive data after the packet
    buf_pointer = buf;
    int len = recv(fd, buf, sizeof(buf), 0);

    if (len == 0) {
      //socket closed
      app_log("Host unencrypted disconnected");
      close(fd);
      if (fd == enc_client_socket) {
        enc_client_socket = -1;
      } else {
        unenc_client_socket = -1;
      }
      unhandled_data_size = 0;
      return -1;
    } else if (len < 0) {
      unhandled_data_size = 0;
      return 0;
    } else {
      unhandled_data_size = len;
      return len;
    }
  }
  return 0;
}
