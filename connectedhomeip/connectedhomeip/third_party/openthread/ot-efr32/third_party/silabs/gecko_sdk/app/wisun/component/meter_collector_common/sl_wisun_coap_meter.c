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
#include "em_device.h"
#include "sl_wisun_coap_meter.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_TEMP_SENSOR_PRESENT)
  #include "sl_wisun_rht_measurement.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// JSON formated measurement data maximum size
#define JSON_MEASUREMENT_DATA_SIZE        (100U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Create formated json string from measurement packet
 * @details Use snprintf to static buffer
 * @param[in] packet packet
 * @return const char* pointer to the static buff
 *****************************************************************************/
static const char *sl_wisun_meter_packet2json(const sl_wisun_meter_packet_t *packet);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Internal CoAP Meter handler
static sl_wisun_meter_hnd_t _coap_meter_hnd = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void sl_wisun_coap_meter_init(void)
{
  // Init meter-collector common component
#if !defined(SL_CATALOG_TEMP_SENSOR_PRESENT)
  sl_wisun_meter_set_initializer(&_coap_meter_hnd, NULL);
#else
  sl_wisun_meter_set_initializer(&_coap_meter_hnd, sl_wisun_rht_init);
#endif
  sl_wisun_meter_init_hnd(&_coap_meter_hnd);
}

/* measure and send packet */
int32_t sl_wisun_coap_meter_meas_params_and_send(const int32_t sockd_meter, const sockaddr_in6_t *collector_addr)
{
  sl_wisun_meter_packet_t packet  = { 0 };                  // static packet
  wisun_addr_t dest               = { 0 };                  // destination address on other port
  socklen_t len                   = sizeof(sockaddr_in6_t); // socket len
  sl_wisun_coap_packet_t response = { 0 };                  // prepare a static CoAP packet
  const char *json_content        = NULL;                   // Json content length
  uint8_t *message_buff           = NULL;                   // dynamic message buffer
  uint16_t message_len            = 0;                      // message length
  int32_t retval                  = 0;                      // return value
  int16_t builder_res             = RETVAL_ERROR;           // builder result

  // fill packet
  sl_wisun_meter_gen_packet_id(&packet);
  sl_wisun_meter_get_temperature(&packet);
  sl_wisun_meter_get_humidity(&packet);
  sl_wisun_meter_get_light(&packet);

  // lock protected buffers
  sl_wisun_mc_mutex_acquire(_coap_meter_hnd);

  // prepare payload
  json_content = sl_wisun_meter_packet2json(&packet);

  // create response packet
  memset(&response, 0, sizeof(response));
  response.uri_path_ptr     = (uint8_t *) SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH; // CoAP URI path
  response.uri_path_len     = strlen(SL_WISUN_COAP_METER_COLLECTOR_MEASUREMENT_URI_PATH);     // CoAP URI path length
  response.msg_code         = COAP_MSG_CODE_RESPONSE_CONTENT;                                 // CoAP message code
  response.content_format   = COAP_CT_JSON;                                                   // CoAP content format
  response.payload_ptr      = (uint8_t *) json_content;                                       // CoAP Payload
  response.payload_len      = (uint16_t) strlen(json_content);                                // CoAP Payload length
  response.options_list_ptr = NULL;                                                           // CoAP option list
  response.msg_id           = SL_WISUN_COAP_METER_COLLECTOR_DEFAULT_MESSAGE_ID;               // CoAP message ID

  printf("[Response packet]\n");
  sl_wisun_coap_print_packet(&response, false);

  // build CoAP message
  message_len  = sl_wisun_coap_builder_calc_size(&response);
  message_buff = (uint8_t *)sl_wisun_coap_malloc(message_len);

  // set retval to correct result
  retval = message_len;

  if (message_buff == NULL) {
    printf("[Coap message buffer cannot be allocated]\n");
    sl_wisun_mc_release_mtx_and_return_val(_coap_meter_hnd, RETVAL_ERROR);
  }

  memcpy(&dest, collector_addr, sizeof(wisun_addr_t));
  dest.sin6_port = htons((SL_WISUN_METER_USE_CUSTOM_COLLECTOR_PORT
                          ? SL_WISUN_COLLECTOR_PORT
                          : collector_addr->sin6_port));

  // build raw CoAP message
  builder_res = sl_wisun_coap_builder(message_buff, &response);
  if (builder_res < 0) {
    printf("[Coap builder error: %s]\n", builder_res == RETVAL_ERROR ? "Message Header structure" : "NULL ptr arg");
    retval = RETVAL_ERROR;
    // send built CoAP message
  } else if (sendto(sockd_meter, message_buff, message_len, 0,
                    (const struct sockaddr *) &dest, len) == RETVAL_ERROR) {
    printf("[Response sending failure]\n");
    sl_wisun_coap_free(message_buff);
    sl_wisun_mc_release_mtx_and_return_val(_coap_meter_hnd, RETVAL_ERROR);
  }

  // cleanup
  sl_wisun_coap_free(message_buff);
  sl_wisun_mc_mutex_release(_coap_meter_hnd);

  return retval;
}

void sl_wisun_coap_meter_resource_not_found_response(const int32_t sockd_meter, const sl_wisun_coap_packet_t *request, const sockaddr_in6_t *collector_addr)
{
  uint8_t *message_buff = NULL;
  sl_wisun_coap_packet_t *response = NULL;
  int16_t builder_res = -1;
  uint16_t message_len = 0;
  socklen_t len = sizeof(sockaddr_in6_t);

  // Build resource not found response
  response = sl_wisun_coap_build_response(request, COAP_MSG_CODE_RESPONSE_NOT_FOUND);
  message_len = sl_wisun_coap_builder_calc_size(response);

  // Allocate message buffer
  message_buff = (uint8_t *)sl_wisun_coap_malloc(message_len);
  if (message_buff == NULL) {
    printf("[Resource not found message buffer cannot be allocated!]");
    sl_wisun_coap_destroy_packet(response);
    return;
  }

  // build raw CoAP message buffer
  builder_res = sl_wisun_coap_builder(message_buff, response);

  // Check CoAP message builder result
  if (builder_res < 0) {
    printf("[Coap builder error: %s]\n",
           builder_res == -1 ? "Message Header structure" : "NULL ptr arg");
  } else if (sendto(sockd_meter, message_buff, message_len, 0,
                    (const struct sockaddr *) &collector_addr, len) == -1) {
    printf("[Response sending failure]\n");
  }

  // Destroy packet
  sl_wisun_coap_destroy_packet(response);

  // free allocated message buffer
  sl_wisun_coap_free(message_buff);
}

sl_wisun_led_id_t sl_wisun_coap_meter_convert_led_id(const uint8_t led_id)
{
  switch (led_id) {
    case SL_WISUN_METER_LED0: return SL_WISUN_LED0_ID;
    case SL_WISUN_METER_LED1: return SL_WISUN_LED1_ID;
    default:                  return SL_WISUN_LED_UNKNOW_ID;
  }
}

static const char *sl_wisun_meter_packet2json(const sl_wisun_meter_packet_t *packet)
{
  static char buff[JSON_MEASUREMENT_DATA_SIZE] = { 0 };
  snprintf(buff, JSON_MEASUREMENT_DATA_SIZE,
           "  {\n    \"id\": %u,\n    \"temp\": %lu.%lu,\n    \"hum\": %lu.%lu,\n    \"lx\": %u\n  }\n",
           packet->id,
           packet->temperature / 1000,
           (packet->temperature % 1000) / 10,
           packet->humidity / 1000,
           (packet->humidity % 1000) / 10,
           packet->light);
  return buff;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
