/***************************************************************************//**
 * @file
 * @brief TCP communication through socket implementation on Windows platform
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

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "tcp.h"

#define WINERRORLOG do {                                      \
    wchar_t *s = NULL;                                        \
    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER             \
                   | FORMAT_MESSAGE_FROM_SYSTEM               \
                   | FORMAT_MESSAGE_IGNORE_INSERTS,           \
                   NULL, WSAGetLastError(),                   \
                   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
                   (LPWSTR)&s, 0, NULL);                      \
    fprintf(stderr, "\nError @ %s::%d\nReason: %S\n",         \
            __FILE__, __LINE__, s);                           \
    LocalFree(s);                                             \
} while (0)

// -----------------------------------------------------------------------------
// Public Function Definitions

int32_t tcp_open(void *handle, char *ip, char *port)
{
  struct addrinfo *addr = NULL, hints;
  WSADATA wsa_data;
  SOCKET socket_handle;
  int ret;

  // Initialize Winsock
  ret = WSAStartup(MAKEWORD(2, 2), &wsa_data);
  if (ret != 0) {
    fprintf(stderr, "WSAStartup failed: %d\n", ret);
    WINERRORLOG;
    return -1;
  }

  ZeroMemory(&hints, sizeof(hints) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_protocol = IPPROTO_TCP;

  // Resolve the server address and port
  ret = getaddrinfo(ip, port, &hints, &addr);
  if (ret != 0) {
    fprintf(stderr, "Get address info failed: %d\n", ret);
    WINERRORLOG;
    WSACleanup();
    return -1;
  }

  // Create a SOCKET for connecting to server
  socket_handle = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  if (socket_handle == INVALID_SOCKET) {
    fprintf(stderr, "Error %d while creating TCP socket\n", WSAGetLastError());
    WINERRORLOG;
    freeaddrinfo(addr);
    WSACleanup();
    return -1;
  }

  // Connect to server.
  ret = connect(socket_handle, addr->ai_addr, (int)addr->ai_addrlen);
  if (ret == SOCKET_ERROR) {
    fprintf(stderr, "Error %d while connecting to server\n", WSAGetLastError());
    WINERRORLOG;
    return -1;
  }

  freeaddrinfo(addr);

  *(SOCKET *)handle = socket_handle;
  return 0;
}

int32_t tcp_tx(void *handle, uint32_t data_length, uint8_t *data)
{
  int ret;

  if (*(SOCKET *)handle == INVALID_SOCKET) {
    return -1;
  }

  ret = send(*(SOCKET *)handle, (const char *) data, (int) data_length, 0);

  if (ret == SOCKET_ERROR) {
    WINERRORLOG;
    return -1;
  }

  return ret;
}

int32_t tcp_rx(void *handle, uint32_t data_length, uint8_t *data)
{
  // Variable for storing function return values.
  int ret;
  // The amount of bytes still needed to be read.
  uint32_t data_to_read = data_length;
  // The amount of bytes read.
  uint32_t data_read;

  if (*(SOCKET *)handle == INVALID_SOCKET) {
    return -1;
  }

  while (data_to_read) {
    // recv will block until all the requested data is available.
    // To make it non-blocking, the sl_bt_ncp_host has to be re-designed.
    ret = recv(*(SOCKET *)handle, (char *) data, (int) data_to_read, 0);

    if (ret == SOCKET_ERROR) {
      fprintf(stderr, "TCP receive failed: %d\n", WSAGetLastError());
      WINERRORLOG;
      return -1;
    } else {
      data_read = (uint32_t)ret;
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
  u_long count = 0;
  int ret;

  if (*(SOCKET *)handle == INVALID_SOCKET) {
    return -1;
  }

  ret = ioctlsocket(*(SOCKET *)handle, FIONREAD, &count);
  if (ret == SOCKET_ERROR) {
    fprintf(stderr, "Peek failed: %d\n", WSAGetLastError());
    WINERRORLOG;
    return -1;
  }

  return count;
}

int32_t tcp_close(void *handle)
{
  int ret;

  // cleanup
  ret = closesocket(*(SOCKET *)handle);
  if (ret == SOCKET_ERROR) {
    fprintf(stderr, "Close failed: %d\n", WSAGetLastError());
    WINERRORLOG;
    return -1;
  }

  *(SOCKET*)handle = INVALID_SOCKET;
  WSACleanup();

  return 0;
}
