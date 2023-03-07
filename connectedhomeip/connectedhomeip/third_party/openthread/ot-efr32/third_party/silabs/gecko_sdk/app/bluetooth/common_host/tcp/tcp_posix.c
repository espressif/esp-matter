/***************************************************************************//**
 * @file
 * @brief TCP communication through socket implementation on POSIX platform
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <netinet/ip.h>
#include <pthread.h>
#include "tcp.h"

// -----------------------------------------------------------------------------
// Public Function Definitions

int32_t tcp_open(void *handle, char *ip, char *port)
{
  struct addrinfo *addr = NULL, hints = { 0 };
  int socket_handle;
  int ret;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  ret = getaddrinfo(ip, port, &hints, &addr);
  if (ret != 0) {
    perror("Get address info failed");
    return ret;
  }

  // Create a SOCKET for connecting to server
  socket_handle = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (socket_handle < 0) {
    perror("Error while creating TCP socket");
    freeaddrinfo(addr);
    return -1;
  }

  // Connect to server.
  ret = connect(socket_handle, addr->ai_addr, (int)addr->ai_addrlen);
  if (ret < 0) {
    perror("Error while connecting to server");
  }

  freeaddrinfo(addr);

  *(int32_t *)handle = socket_handle;
  return ret;
}

int32_t tcp_tx(void *handle, uint32_t data_length, uint8_t *data)
{
  int32_t ret = -1;

  if (*(int32_t *)handle < 0) {
    return ret;
  }

  ret = write(*(int32_t *)handle, (void *) data, (size_t) data_length);

  return ret;
}

int32_t tcp_rx(void *handle, uint32_t data_length, uint8_t *data)
{
  // The amount of bytes still needed to be read.
  uint32_t data_to_read = data_length;
  // The amount of bytes read.
  uint32_t data_read;

  if (*(int32_t *)handle < 0) {
    return -1;
  }

  while (data_to_read) {
    // recv will block until all the requested data is available.
    // To make it non-blocking, the sl_bt_ncp_host has to be re-designed.
    int32_t size = read(*(int32_t *)handle, (void *) data, (size_t) data_to_read);

    if (size < 0) {
      perror("TCP receive failed");
      return -1;
    } else {
      data_read = (uint32_t)size;
      if (!data_read) {
        continue;
      }
    }
    data_to_read -= data_read;
    data += data_read;
  }

  return (int32_t)data_length;
}

int32_t tcp_rx_peek(void *handle)
{
  int32_t count;
  int32_t ret = -1;

  if (*(int32_t *)handle < 0) {
    return ret;
  }

  ret = ioctl(*(int32_t *)handle, FIONREAD, &count);

  if (ret < 0) {
    perror("Peek failed");
    return ret;
  }

  return count;
}

int32_t tcp_close(void *handle)
{
  int32_t ret;
  // cleanup
  ret = close(*(int32_t *)handle);
  if (ret < 0) {
    perror("Close failed");
  }

  *(int32_t *)handle = -1;

  return ret;
}
