/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP resource handler
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include "sl_string.h"
#include "sl_wisun_coap_resource_handler.h"
#include "sli_wisun_coap_mem.h"
#include "sl_wisun_coap.h"
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_status.h"
#include "socket.h"
#include "socket_hnd.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// link format URI field head
#define SL_WISUN_COAP_RESOURCE_URI_PATH_HEAD               "<"
/// link format URI field tail
#define SL_WISUN_COAP_RESOURCE_URI_PATH_TAIL               ">"
/// link format Resource Type field head
#define SL_WISUN_COAP_RESOURCE_RT_HEAD                     ";rt=\""
/// link format Resource Type field tail
#define SL_WISUN_COAP_RESOURCE_RT_TAIL                     "\""
/// link format Interface head
#define SL_WISUN_COAP_RESOURCE_IF_HEAD                     ";if=\""
/// link format Interface tail
#define SL_WISUN_COAP_RESOURCE_IF_TAIL                     "\""
/// link format Content Type Field (fixed)
#define SL_WISUN_COAP_RESOURCE_CT_STR                      ";ct=40" // application/link-format
/// maximum number string check length
#define SL_WISUN_COAP_RESOURCE_STR_CHECK_MAX               SL_WISUN_COAP_URI_PATH_MAX_SIZE

/**************************************************************************//**
 * @brief Unlock the CoAP resource mutex and return.
 *****************************************************************************/
#define _coap_resource_mutex_release_and_return_val(__val) \
  do {                                                     \
    _coap_resource_mutex_release();                        \
    return (__val);                                        \
  } while (0)

/**************************************************************************//**
 * @brief Unlock the CoAP resource mutex and return with check if OS is running.
 *****************************************************************************/
#define _coap_resource_mutex_check_release_and_return_val(__val) \
  do {                                                           \
    _coap_resource_mutex_check_release();                        \
    return (__val);                                              \
  } while (0)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Resource mutex lock and return
 * @return none
 *****************************************************************************/
static inline void _coap_resource_mutex_check_acquire(void);

/**************************************************************************//**
 * @brief Resouce mutex unlock and return
 * @return none
 *****************************************************************************/
static inline void _coap_resource_mutex_check_release(void);

/**************************************************************************//**
 * @brief Resource mutex lock
 * @return none
 *****************************************************************************/
static inline void _coap_resource_mutex_acquire(void);

/**************************************************************************//**
 * @brief Resource mutex unlock
 * @return none
 *****************************************************************************/
static inline void _coap_resource_mutex_release(void);

/**************************************************************************//**
 * @brief Appends a character to the buffer currently filled out.
 * @details It can be used to check if test if the next character fits in
 * @param[inout] buffer the pointer to the buffer being filled out
 * @param[in] chr character to insert
 * @param[inout] remanining pointer to the variable to update the space left
 * @param[in] op_mode only_calc: checks space but no actual write is done
 *                    copy_and_calc : copies into the buffer and updates space left
 * @return none
 *****************************************************************************/
static void _buffer_append(char** buffer,
                           char chr,
                           int16_t* remanining,
                           const insert_mode_t op_mode);

/**************************************************************************//**
 * @brief Adds one field to the resource discovery response.
 * @details It can write an element into the buffer or just help to calculate
 * the space needed.
 * @param[inout] buffer_ptr pointer to the buffer that is being filled out.
 * @param[in] head_str pointer to the string that the element starts with
 * @param[in] field_ptr pointer to the buffer containing the element value
 * @param[in] tail_str pointer to the string that elements ends with
 * @param[in] len length of the text pointed by field_ptr
 * @param[inout] remanining pointer to the varaible to update the space left
 * @param[in] op_mode only_calc: checks space but no actual write is done
 *                    copy_and_calc : copies into the buffer and updates space left
 * @return none
 *****************************************************************************/
static void _insert_field(char** buffer_ptr,
                          char* head_str,
                          char* field_ptr,
                          char* tail_str,
                          const uint16_t len,
                          int16_t* remanining,
                          const insert_mode_t op_mode);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wi-SUN CoAP Resource Handler mutex control block
 *****************************************************************************/
__ALIGNED(8) static uint8_t _wisun_coap_resource_mtx_cb[osMutexCbSize] = { 0 };

/**************************************************************************//**
 * @brief Wi-SUN CoAP Resource Handler mutex attribute
 *****************************************************************************/
static const osMutexAttr_t _wisun_coap_resource_mtx_attr = {
  .name      = "WisunCoapResourceMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _wisun_coap_resource_mtx_cb,
  .cb_size   = sizeof(_wisun_coap_resource_mtx_cb)
};

/**************************************************************************//**
 * @brief Wi-SUN CoAP Resource manager mutex
 *****************************************************************************/
static osMutexId_t _wisun_coap_resource_mtx;

/// Internal Resource Manager
static sl_wisun_coap_resource_t _resource_table[SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES] = { 0 };

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/* Mutex acquire with check if OS is running */
static inline void _coap_resource_mutex_check_acquire(void)
{
  if (osKernelGetState() == osKernelRunning) {
    assert(osMutexAcquire(_wisun_coap_resource_mtx, osWaitForever) == osOK);
  }
}

/* Mutex release with check if OS is running */
static inline void _coap_resource_mutex_check_release(void)
{
  if (osKernelGetState() == osKernelRunning) {
    assert(osMutexRelease(_wisun_coap_resource_mtx) == osOK);
  }
}

/* Mutex acquire */
static inline void _coap_resource_mutex_acquire(void)
{
  assert(osMutexAcquire(_wisun_coap_resource_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _coap_resource_mutex_release(void)
{
  assert(osMutexRelease(_wisun_coap_resource_mtx) == osOK);
}

/* append cahracter to buffer */
static void _buffer_append(char** buffer,
                           char chr,
                           int16_t* remanining,
                           const insert_mode_t op_mode)
{
  if (op_mode == copy_and_calc) {
    **buffer = chr;
    (*buffer)++;
  }
  (*remanining)--;
}

/* insert field to the currently constructed resource info */
static void _insert_field(char** buffer_ptr,
                          char* head_str,
                          char* field_ptr,
                          char* tail_str,
                          const uint16_t len,
                          int16_t* remanining,
                          const insert_mode_t op_mode)
{
  uint16_t head_len = sl_strnlen(head_str, SL_WISUN_COAP_RESOURCE_STR_CHECK_MAX);
  uint16_t tail_len = sl_strnlen(tail_str, SL_WISUN_COAP_RESOURCE_STR_CHECK_MAX);

  if (*remanining >= (head_len + len + tail_len)) {
    if (op_mode == copy_and_calc) {
      memcpy(*buffer_ptr, head_str, head_len);
      (*buffer_ptr) += head_len;
      memcpy(*buffer_ptr, field_ptr, len);
      (*buffer_ptr) += len;
      memcpy(*buffer_ptr, tail_str, tail_len);
      (*buffer_ptr) += tail_len;
    }
    (*remanining) -= (head_len + len + tail_len);
  } else {
    (*remanining) = -1;
  }
}

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

void sl_wisun_coap_resource_init(void)
{
  // init mutex
  _wisun_coap_resource_mtx = osMutexNew(&_wisun_coap_resource_mtx_attr);
  assert(_wisun_coap_resource_mtx != NULL);
  sl_wisun_coap_resource_init_table();
}

/* init resource table */
void sl_wisun_coap_resource_init_table(void)
{
  _coap_resource_mutex_check_acquire();
  for (uint16_t i = 0; i < SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES; ++i) {
    _resource_table[i].used = false; // all slot unused
  }
  _coap_resource_mutex_check_release();
}

/* Register resource */
bool sl_wisun_coap_resource_register(const sl_wisun_coap_resource_data_t* resource_data,
                                     const sl_wisun_coap_discovery_visibility_t discoverable)
{
  int32_t first_free_idx = -1;

  _coap_resource_mutex_check_acquire();

  for (int32_t idx = SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES; idx >= 0; --idx) {
    // check if resource exists
    if (_resource_table[idx].used == true
        && sl_wisun_coap_compare(_resource_table[idx].data.uri_path,
                                 _resource_table[idx].data.uri_path_len,
                                 resource_data->uri_path,
                                 resource_data->uri_path_len) ) {
      _coap_resource_mutex_check_release_and_return_val(false);
    } else if (_resource_table[idx].used == false) {
      first_free_idx = idx;
    }
  }

  if (first_free_idx != -1) {
    // check if URI path is empty
    if (resource_data->uri_path == NULL || resource_data->uri_path_len == 0) {
      _coap_resource_mutex_check_release_and_return_val(false);
    }
    _resource_table[first_free_idx].used = true;
    _resource_table[first_free_idx].discoverable = discoverable;
    memcpy(&_resource_table[first_free_idx].data,
           resource_data,
           sizeof(sl_wisun_coap_resource_data_t));
    _coap_resource_mutex_check_release_and_return_val(true);
  }

  _coap_resource_mutex_check_release();
  return false;
}

/* Remove resource by URI path */
bool sl_wisun_coap_resource_remove_uri(const char* uri_path, const uint16_t uri_path_len)
{
  _coap_resource_mutex_acquire();

  for (uint16_t i = 0; i < SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES; ++i) {
    if (sl_wisun_coap_compare(_resource_table[i].data.uri_path,
                              _resource_table[i].data.uri_path_len,
                              uri_path,
                              uri_path_len)) {
      _resource_table[i].used = false;
      _coap_resource_mutex_release_and_return_val(true);
    }
  }

  _coap_resource_mutex_release();
  return false;
}

/* Get resource by URI path */
sl_wisun_coap_resource_t* sl_wisun_coap_resource_get_uri(const char* uri_path,
                                                         const uint16_t uri_path_len)
{
  _coap_resource_mutex_acquire();

  for (uint16_t i = 0; i < SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES; ++i) {
    if (!_resource_table[i].used) {
      continue;
    }
    if (sl_wisun_coap_compare(uri_path,
                              uri_path_len,
                              _resource_table[i].data.uri_path,
                              _resource_table[i].data.uri_path_len)) {
      _coap_resource_mutex_release();
      return &_resource_table[i];
    }
  }

  _coap_resource_mutex_release();
  return NULL;
}

/* Check if request is a resource discovery request */
bool sl_wisun_coap_resource_discovery_requested(const sl_wisun_coap_packet_t* parsed_msg)
{
  bool retval = false;

  if (parsed_msg == NULL) {
    return false;
  }

  // check if the request is approripate
  if ((parsed_msg->msg_code == COAP_MSG_CODE_REQUEST_GET)
      && (sl_wisun_coap_compare((char*)parsed_msg->uri_path_ptr,
                                parsed_msg->uri_path_len,
                                SL_WISUN_COAP_RESOURCE_DISCOVERY_STR,
                                sizeof(SL_WISUN_COAP_RESOURCE_DISCOVERY_STR) - 1))) {
    retval = true;
  }

  return retval;
}

/* response to a GET /.well-known/core request */
int16_t sl_wisun_coap_resource_discovery(char* buffer,
                                         const uint16_t buffer_size,
                                         insert_mode_t op_mode)
{
  // Inidcate first returned resource
  bool first = true;
  uint16_t i = 0U;
  // Remaining buffer space
  int16_t buffer_remanining = (int16_t)buffer_size;
  char* buffer_ptr = buffer;

  // check inputs if the actual operation is carried out
  if (op_mode == copy_and_calc && ((buffer == NULL) || (buffer_size == 0u))) {
    // return a value to indicate error with an invalid remaning space
    return -1;
  }

  _coap_resource_mutex_acquire();

  while ( (buffer_remanining > 0) && (i < SL_WISUN_COAP_RESOURCE_HANDLER_MAX_RESOURCES) ) {
    if (_resource_table[i].used && _resource_table[i].discoverable == discoverable) {
      if (!first) {
        _buffer_append(&buffer_ptr, ',', &buffer_remanining, op_mode);
      } else {
        first = false;
      }
      _insert_field(&buffer_ptr,
                    SL_WISUN_COAP_RESOURCE_URI_PATH_HEAD,
                    _resource_table[i].data.uri_path,
                    SL_WISUN_COAP_RESOURCE_URI_PATH_TAIL,
                    _resource_table[i].data.uri_path_len,
                    &buffer_remanining,
                    op_mode);
      if (buffer_remanining > 0) {
        _insert_field(&buffer_ptr,
                      SL_WISUN_COAP_RESOURCE_RT_HEAD,
                      _resource_table[i].data.resource_type,
                      SL_WISUN_COAP_RESOURCE_RT_TAIL,
                      _resource_table[i].data.resource_type_len,
                      &buffer_remanining,
                      op_mode);
      }
      if (buffer_remanining > 0) {
        _insert_field(&buffer_ptr,
                      SL_WISUN_COAP_RESOURCE_IF_HEAD,
                      _resource_table[i].data.interface,
                      SL_WISUN_COAP_RESOURCE_IF_TAIL,
                      _resource_table[i].data.interface_len,
                      &buffer_remanining,
                      op_mode);
      }
      if (buffer_remanining > 0) {
        _insert_field(&buffer_ptr,
                      "",
                      SL_WISUN_COAP_RESOURCE_CT_STR,
                      "",
                      sizeof(SL_WISUN_COAP_RESOURCE_CT_STR),
                      &buffer_remanining,
                      op_mode);
      }
    }
    ++i;
  }

  _coap_resource_mutex_release();

  return ((int16_t)buffer_size - buffer_remanining);
}

/* send a response to a resource discovery request */
sl_status_t sl_wisun_coap_resource_discovery_response(const int32_t sock_id,
                                                      const uint16_t message_id,
                                                      const sockaddr_in6_t *dest_addr)
{
  sl_status_t retval              = SL_STATUS_OK;           // assume success of operation
  wisun_addr_t dest               = { 0 };                  // destination address on other port
  sl_wisun_coap_packet_t response = { 0 };                  // static CoAP packet
  uint8_t *message_buff           = NULL;                   // dynamic message buffer
  uint16_t message_len            = 0;                      // message length
  char *payload_buff              = NULL;                   // dynamic payload buffer
  int16_t payload_len             = 0;                      // payload length
  socklen_t len                   = sizeof(sockaddr_in6_t);
  int16_t builder_res             = RETVAL_ERROR;           // builder result

  if (dest_addr == NULL) {
    return SL_STATUS_FAIL;
  }

  _coap_resource_mutex_acquire();

  // calculate the needed space for the payload
  payload_len = sl_wisun_coap_resource_discovery(NULL,
                                                 SL_WISUN_COAP_RESOURCE_MAXIMUM_PAYLOAD,
                                                 only_calc);
  // create payload if applicable
  if (payload_len > 0) {
    // Try to allocate space
    payload_buff = (char*)sl_wisun_coap_malloc(payload_len);
    if (payload_buff == NULL) {
      printf("[Could not allocate memory for payload!]");
      return SL_STATUS_FAIL;
    }
    // Fill out the buffer
    (void)sl_wisun_coap_resource_discovery(payload_buff, (uint16_t)payload_len, copy_and_calc);
  } else if (payload_len == SL_WISUN_COAP_RESOURCE_NOT_ENOUGH_SPACE) {
    printf("[Payload of response exceeds maximum payload capacity!\nAdjust SL_WISUN_COAP_RESOURCE_MAXIMUM_PAYLOAD!]");
    return SL_STATUS_FAIL;
  }

  // create response packet
  memset(&response, 0, sizeof(response));
  response.uri_path_ptr     = NULL;                             // CoAP URI path
  response.uri_path_len     = 0;                                // CoAP URI path length
  response.msg_code         = COAP_MSG_CODE_RESPONSE_CONTENT;   // CoAP message code
  response.content_format   = COAP_CT_LINK_FORMAT;              // CoAP content format
  response.payload_ptr      = (uint8_t*)payload_buff;                     // CoAP Payload
  response.payload_len      = (uint16_t)payload_len;            // CoAP Payload length
  response.options_list_ptr = NULL;                             // CoAP option list
  response.msg_id           = message_id;                       // CoAP message ID of the original request

  // Build Content response
  message_len = sl_wisun_coap_builder_calc_size(&response);

  message_buff = (uint8_t*)sl_wisun_coap_malloc(message_len);
  if (message_buff == NULL) {
    printf("[Could not allocate memory for message!]");
    return SL_STATUS_FAIL;
  }

  memcpy(&dest, dest_addr, sizeof(wisun_addr_t));
  dest.sin6_port = htons(dest_addr->sin6_port);

  // build raw CoAP message
  builder_res = sl_wisun_coap_builder(message_buff, &response);
  if (builder_res < 0) {
    printf("[Coap builder error: %s]\n", builder_res == RETVAL_ERROR ? "Message Header structure" : "NULL ptr arg");
    retval = RETVAL_ERROR;
    // send built CoAP message
  } else if (sendto(sock_id, message_buff, message_len, 0,
                    (const struct sockaddr *) &dest, len) == RETVAL_ERROR) {
    printf("[Response sending failure]\n");
    sl_wisun_coap_free(payload_buff);
    sl_wisun_coap_free(message_buff);
    _coap_resource_mutex_release_and_return_val(RETVAL_ERROR);
  }

  // free allocated payload buffer
  sl_wisun_coap_free(payload_buff);
  sl_wisun_coap_free(message_buff);

  _coap_resource_mutex_release();

  return retval;
}
