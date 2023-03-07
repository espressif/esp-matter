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
#include <string.h>
#include "sl_wisun_app_core_util.h"
#include "socket_hnd.h"
#include "socket.h"
#include "app.h"
#include "sl_wisun_meter.h"

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
  const char *collector_ip = NULL;
  uint8_t received_token[SL_WISUN_METER_COLLECTOR_TOKEN_MAX_SIZE] = { 0 };
  int32_t r = SOCKET_INVALID_ID;
  socklen_t len = 0;
  struct sockaddr_in6 meter_addr = { 0 };
  struct sockaddr_in6 collector_addr = { 0 };
  int32_t sockd_meter = SOCKET_INVALID_ID; // servert socket of meter
  uint16_t token_len = 0;

  (void) args;

  // connect to the wisun network
  app_wisun_connect_and_wait();
  printf("[Port: %u]\n", SL_WISUN_METER_PORT);

  // init reference token
  sl_wisun_mc_init_token(SL_WISUN_METER_COLLECTOR_TOKEN);
  token_len = sl_wisun_mc_get_token_size();

  // creating socket
  sockd_meter = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_UDP);
  assert_res(sockd_meter, "Wi-SUN Meter socket()");

  // fill the server address structure
  meter_addr.sin6_family = AF_WISUN;
  meter_addr.sin6_addr = in6addr_any;
  meter_addr.sin6_port = htons(SL_WISUN_METER_PORT);

  // bind address to the socket
  r = bind(sockd_meter, (const struct sockaddr *) &meter_addr, sizeof(struct sockaddr_in6));
  assert_res(r, "Wi-SUN Meter bind()");

  len = sizeof(collector_addr);
  // receiver loop
  while (1) {
    // receive token
    r = recvfrom(sockd_meter, received_token, token_len, 0,
                 (struct sockaddr *) &collector_addr, &len);

    if (r > 0) {
      // not valid address
      if (!memcmp(&collector_addr.sin6_addr, &in6addr_any, sizeof(in6addr_any))) {
        printf("[Not valid address received]\n");
        msleep(1);
        continue;
      }

      // compare token
      if (!sl_wisun_mc_compare_token(received_token, r)) {
        msleep(1);
        continue;
      }

      // if token matched, send measurement packet
      collector_ip = app_wisun_trace_util_get_ip_address_str(&collector_addr.sin6_addr);
      if (!sl_wisun_meter_meas_params_and_send(sockd_meter, &collector_addr)) {
        printf("[%s: Measurement has NOT been sent]\n", collector_ip);
      } else {
        printf("[%s: Measurement packet has been sent (%d bytes)]\n",
               collector_ip, sizeof(sl_wisun_meter_packet_t));
      }
    }

    // dispatch thread
    msleep(1);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
