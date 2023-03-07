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
  printf("[Port: %u]\n", SL_WISUN_UDP_SERVER_PORT);

  char buff[SL_WISUN_UDP_SERVER_BUFF_SIZE];
  int32_t r;
  socklen_t len;

  struct sockaddr_in6 srv_addr_udp, clnt_addr_udp;
  int32_t sockd_udp_srv;

  // creating socket
  sockd_udp_srv = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_UDP);
  assert_res(sockd_udp_srv, "UDP server socket()");

  // fill the server address structure
  srv_addr_udp.sin6_family = AF_WISUN;
  srv_addr_udp.sin6_addr = in6addr_any;
  srv_addr_udp.sin6_port = htons(SL_WISUN_UDP_SERVER_PORT);

  // bind address to the socket
  r = bind(sockd_udp_srv, (const struct sockaddr *) &srv_addr_udp, sizeof(struct sockaddr_in6));
  assert_res(r, "UDP server bind()");

  len = sizeof(clnt_addr_udp);

  // receiver loop
  while (1) {
    r = recvfrom(sockd_udp_srv, buff, SL_WISUN_UDP_SERVER_BUFF_SIZE - 1, 0, (struct sockaddr *) &clnt_addr_udp, &len);
    if (r > 0) {
      buff[r] = 0;
      // print the received message
      printf("[%s] %s\n", app_wisun_trace_util_get_ip_address_str((void *) &clnt_addr_udp.sin6_addr), buff);

      // send back to the client
      if (sendto(sockd_udp_srv, buff, r, 0, (const struct sockaddr *) &clnt_addr_udp, len) == -1) {
        printf("Error sendto()\n");
      }
    }
    // dispatch thread
    msleep(1);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
