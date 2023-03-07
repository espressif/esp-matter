/***************************************************************************//**
 * @file
 * @brief
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <string.h>
#include <stdio.h>
#include "socket_hnd.h"
#include "socket.h"
#include "sl_wisun_udp_client.h"

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

/* create tcp client */
void sl_wisun_udp_client_create(void)
{
  int32_t sockid = SOCKET_INVALID_ID; // client socket id

  // create client socket
  sockid = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_IP);

  if (sockid == SOCKET_INVALID_ID) {
    printf("[Failed to create socket: %ld]\n", sockid);
    return;
  } else {
    printf("[Socket created: %ld]\n", sockid);
  }
}

/* close tcp client socket */
void sl_wisun_udp_client_close(const int32_t sockid)
{
  if (close(sockid) == RETVAL_ERROR) {
    printf("[Failed to close socket: %ld]\n", sockid);
  }
}

/* write to tcp client socket */
void sl_wisun_udp_client_write(const int32_t sockid, const char *remote_ip_address,
                               const uint16_t remote_port, const char *str)
{
  int32_t res;
  static wisun_addr_t server_addr;
  if (remote_ip_address == NULL) {
    printf("[Failed: IP address is NULL ptr]\n");
    return;
  }

  if (str == NULL) {
    printf("[Failed: Data to write is NULL ptr]\n");
    return;
  }

  // setting the server address
  server_addr.sin6_family = AF_WISUN;
  server_addr.sin6_port = remote_port;
  if (inet_pton(AF_WISUN, remote_ip_address,
                &server_addr.sin6_addr) == RETVAL_ERROR) {
    printf("[Invalid IP address: %s]\n", remote_ip_address);
    return;
  }

  res = sendto(sockid, str, strlen(str), 0,
               (const struct sockaddr *) &server_addr, sizeof(server_addr));
  if (res == RETVAL_ERROR) {
    printf("[Failed to send on socket: %ld]\n", sockid);
  }
}

/* read on tcp client socket */
void sl_wisun_udp_client_read(const int32_t sockid, const uint16_t size)
{
  char c;
  int32_t res;
  static wisun_addr_t server_addr;
  socklen_t len = sizeof(server_addr);

  for (uint16_t i = 0; i < size; ++i) {
    res = recvfrom(sockid, &c, 1, 0,
                   (struct sockaddr *)&server_addr, &len);
    if (res == RETVAL_ERROR || !res) {
      break;
    }
    printf("%c", c);
  }
  printf("\n");
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
