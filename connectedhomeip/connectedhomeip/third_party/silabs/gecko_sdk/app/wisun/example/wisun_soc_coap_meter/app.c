/***************************************************************************//**
 * @file
 * @brief Application code
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
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "app.h"
#include "sl_wisun_app_core_util.h"
#include "socket_hnd.h"
#include "socket.h"
#include "sl_wisun_coap_meter.h"
#include "sl_wisun_led_driver.h"
#include "sl_string.h"
#include "sl_wisun_coap_resource_handler.h"

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
static uint8_t recv_buff[SL_WISUN_COAP_METER_COLLECTOR_RECV_BUFF_SIZE] = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/*App task function*/
void app_task(void *args)
{
  (void) args;
  const char *collector_ip = NULL;
  sl_wisun_coap_packet_t *parsed_msg = NULL;
  int32_t sockd_meter = SOCKET_INVALID_ID;
  int32_t r = SOCKET_INVALID_ID;
  socklen_t len = 0;
  uint16_t meas_uri_path_len = 0;
  uint16_t led_uri_path_len = 0;
  struct sockaddr_in6 meter_addr = { 0 };
  struct sockaddr_in6 collector_addr = { 0 };
  int8_t led_id = 0;

  // init wisun coap with assert
  assert(sl_wisun_coap_init_default() != false);

  // connect to the wisun network
  app_wisun_connect_and_wait();
  printf("[Port: %u]\n", SL_WISUN_METER_PORT);

  meas_uri_path_len = (uint16_t)(sl_strnlen(SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH,
                                            SL_WISUN_COAP_METER_COLLECTOR_URI_PATH_MAX_SIZE) + 1);
  led_uri_path_len = (uint16_t)(sl_strnlen(SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH,
                                           SL_WISUN_COAP_METER_COLLECTOR_URI_PATH_MAX_SIZE) + 1);

  // creating socket
  sockd_meter = socket(AF_WISUN, SOCK_DGRAM, IPPROTO_UDP);
  assert_res(sockd_meter, "Wi-SUN Meter socket()");

  // fill the server address structure
  meter_addr.sin6_family = AF_WISUN;
  meter_addr.sin6_addr = in6addr_any;
  // Note: meter port is the same as the standard resource discovery (5683)
  meter_addr.sin6_port = htons(SL_WISUN_METER_PORT);

  // bind address to the socket
  r = bind(sockd_meter, (const struct sockaddr *) &meter_addr, sizeof(struct sockaddr_in6));
  assert_res(r, "Wi-SUN Meter bind()");

  len = sizeof(collector_addr);

  // receiver loop
  while (1) {
    r = recvfrom(sockd_meter, recv_buff, SL_WISUN_COAP_METER_COLLECTOR_RECV_BUFF_SIZE, 0,
                 (struct sockaddr *) &collector_addr, &len);
    if (r > 0) {
      parsed_msg = sl_wisun_coap_parser((uint16_t) r, recv_buff);

      if (parsed_msg == NULL) {
        printf("[CoAP Parser failure]\n");
        continue;
      }

      if (parsed_msg->msg_type == COAP_MSG_TYPE_ACKNOWLEDGEMENT) {
        printf("[CoAP ACK has been received]\n");
        // Destroy packet. Use this call only instead of 'free'!
        sl_wisun_coap_destroy_packet(parsed_msg);
        continue;
      }

      printf("[Request packet]\n");
      sl_wisun_coap_print_packet(parsed_msg, false);

      collector_ip = app_wisun_trace_util_get_ip_address_str(&collector_addr.sin6_addr);
      if (collector_ip == NULL) {
        printf("[Collector IP address not valid]\n");
        sl_wisun_coap_destroy_packet(parsed_msg);
        continue;
      }

      // Handle any incoming resource discovery request
      if (sl_wisun_coap_resource_discovery_requested(parsed_msg)) {
        sl_wisun_coap_resource_discovery_response(sockd_meter,
                                                  parsed_msg->msg_id,
                                                  &collector_addr);
        sl_wisun_coap_destroy_packet(parsed_msg);
        continue;
      }

      // Measurement URI path
      if (sl_wisun_coap_compare_uri_path(SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH, meas_uri_path_len)) {
        r = sl_wisun_coap_meter_meas_params_and_send(sockd_meter, &collector_addr);
        printf("Measurement data has %s (%ld)\n", r == -1 ? "NOT SENT" : "SENT", r);

        // LED URI path
      } else if (sl_wisun_coap_compare_uri_path(SL_WISUN_COAP_METER_COLLECTOR_LED_TOGGLE_URI_PATH, led_uri_path_len)
                 && parsed_msg->msg_code ==  COAP_MSG_CODE_REQUEST_PUT) {
        led_id = (int8_t)sl_wisun_mc_get_led_id_from_payload((const char *)parsed_msg->payload_ptr);
        if (sl_wisun_led_toggle(sl_wisun_coap_meter_convert_led_id(led_id)) == SL_STATUS_FAIL) {
          printf("[LED Toggle failed: %d]\n", led_id);
        }

        // URI Path not matched, prepare 'not found' response
      } else {
        printf("[%s: Request for accessing resource not valid]\n", collector_ip);
        sl_wisun_coap_meter_resource_not_found_response(sockd_meter, parsed_msg, &collector_addr);
      }

      // Destroy packet. Use this call only instead of 'free'!
      sl_wisun_coap_destroy_packet(parsed_msg);
    }
    // dispatch thread
    msleep(1);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
