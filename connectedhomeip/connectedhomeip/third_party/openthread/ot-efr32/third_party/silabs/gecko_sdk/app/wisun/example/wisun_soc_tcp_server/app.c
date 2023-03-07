/***************************************************************************//**
 * @file
 * @brief Application code
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
#include "app.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*App task function*/
void app_task(void *args)
{
  (void) args;

  // connect to the wisun network
  app_wisun_connect_and_wait();
  printf("[Port: %u]\n", SL_WISUN_TCP_SERVER_PORT);

  char buff[SL_WISUN_TCP_SERVER_BUFF_SIZE];
  int32_t r;
  socklen_t len;
  bool client_connected = false;

  struct sockaddr_in6 srv_addr_tcp, clnt_addr_tcp;
  int32_t sockd_tcp_srv, sockd_tcp_clnt;

  // creating socket
  sockd_tcp_srv = socket(AF_WISUN, SOCK_STREAM, IPPROTO_TCP);
  assert_res(sockd_tcp_srv, "TCP server socket()");

  // fill the server address structure
  srv_addr_tcp.sin6_family = AF_WISUN;
  srv_addr_tcp.sin6_addr = in6addr_any;
  srv_addr_tcp.sin6_port = htons(SL_WISUN_TCP_SERVER_PORT);

  // bind address to the socket
  r = bind(sockd_tcp_srv, (const struct sockaddr *) &srv_addr_tcp, sizeof(struct sockaddr_in6));
  assert_res(r, "TCP server bind()");

  // listen on socket
  r = listen(sockd_tcp_srv, 0);
  assert_res(r, "TCP server listen()");

  len = sizeof(clnt_addr_tcp);

  // waiting for connection request
  while (1) {
    printf("[Waiting for connection request]\n");
    sockd_tcp_clnt = accept(sockd_tcp_srv, (struct sockaddr *)&clnt_addr_tcp, &len);
    if (sockd_tcp_clnt != SOCKET_INVALID_ID) {
      client_connected = true;
      printf("TCP server accept() [%ld] done.\n", sockd_tcp_clnt);
    }
    while (1) {
      if (!client_connected) {
        break;
      }
      // receiver loop
      r = recv(sockd_tcp_clnt, buff, SL_WISUN_TCP_SERVER_BUFF_SIZE - 1, 0);
      switch (r) {
        case -1:
          msleep(1);
          continue;
        case 0: // scoket closed, EOF
          printf("[Socket closing: %ld]\n", sockd_tcp_clnt);
          close(sockd_tcp_clnt);
          client_connected = false;
          break;
        default: // default behaviour
          buff[r] = 0;
          // print the received message
          printf("[%s] %s\n", app_wisun_trace_util_get_ip_address_str((void *) &clnt_addr_tcp.sin6_addr), buff);
          // send back to the client
          if (send(sockd_tcp_clnt, buff, r, 0) == -1) {
            printf("Error send()\n");
          }
          break;
      }
      // dispatch thread
      msleep(1);
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
