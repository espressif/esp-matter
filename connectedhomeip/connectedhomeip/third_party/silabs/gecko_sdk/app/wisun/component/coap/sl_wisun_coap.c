/***************************************************************************//**
 * @file
 * @brief Wi-SUN CoAP handler
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

#include "sl_wisun_coap.h"
#include <stdio.h>
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Release coap mutex and return
#define __release_wisun_coap_mtx_and_ret() \
  do {                                     \
    _wisun_coap_mutex_release();           \
    return;                                \
  } while (0)

/// Release coap mutex and return with value
#define __release_wisun_coap_mtx_and_retval(__value) \
  do {                                               \
    _wisun_coap_mutex_release();                     \
    return (__value);                                \
  } while (0)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Default CoAP TX implementation
 * @details Empty function, for coap lib
 * @param[in,out] packet_data Packet data
 * @param[out] packet_data_size Packaet size
 * @param[out] addr Address
 * @param[in,out] param Param
 * @return uint8_t res
 *****************************************************************************/
static uint8_t _default_coap_tx_callback(uint8_t *packet_data,
                                         uint16_t packet_data_size,
                                         sn_nsdl_addr_s *addr,
                                         void *param);

/**************************************************************************//**
 * @brief Default CoAP RX implementation
 * @details Empty function for CoAP lib
 * @param[in,out] header Header
 * @param[in,out] addr Address
 * @param[in,out] param Params
 * @return int8_t res
 *****************************************************************************/
static int8_t _default_coap_rx_callback(sn_coap_hdr_s *header,
                                        sn_nsdl_addr_s *addr,
                                        void *param);

/**************************************************************************//**
 * @brief Internal Mutex acquire
 * @details Simple inline function with assert
 *****************************************************************************/
static inline void _wisun_coap_mutex_acquire(void);

/**************************************************************************//**
 * @brief Internal Mutex release
 * @details Simple inline function with assert
 *****************************************************************************/
static inline void _wisun_coap_mutex_release(void);

/**************************************************************************//**
 * @brief Store the last parsed message URI path
 * @details internal usage
 * @param[in] packet packet
 * @return const char* pointer to the uri path buffer
 *****************************************************************************/
static const char *_wisun_coap_store_uri_path(const sl_wisun_coap_packet_t *packet);

/**************************************************************************//**
 * @brief Internal pretty buffer printer
 * @details Print buffers with given length, optionally in hex format
 * @param[in] buff_ptr buffer pointer to print
 * @param[in] len size of the buffer
 * @param[in] hex_format enable or disable hex format
 *****************************************************************************/
static inline void _pretty_buff_print(const uint8_t *buff_ptr,
                                      const uint16_t len,
                                      const bool hex_format);

/**************************************************************************//**
 * @brief Get String lenght from fixed size buffer
 * @details Helper function for internal usage
 * @param[in] str string pointer
 * @param[in] max_buff_size buffer size
 * @return int32_t -1 on error, otherwise the string length, 0 is also possible
 *****************************************************************************/
static int32_t __str_n_len(const char *str, const uint16_t max_buff_size);

/**************************************************************************//**
 * @brief Memory copy
 * @details Internal memory buffer copy
 * @param[in,out] dest destination
 * @param[in] src source
 * @param[in] len length in bytes
 *****************************************************************************/
static void __memcpy(void *dest, const void *src, const uint32_t len);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Wisun internal CoAP handler
 *****************************************************************************/
static sl_wisun_coap_t _coap;

/**************************************************************************//**
 * @brief Wi-SUN CoAP mutex
 *****************************************************************************/
static osMutexId_t _wisun_coap_mtx;

/**************************************************************************//**
 * @brief Temporary URI path string buffer
 *****************************************************************************/
static char _tmp_uri_path[SL_WISUN_COAP_URI_PATH_MAX_SIZE] = { 0 };

/**************************************************************************//**
 * @brief CoAP lib state variable
 *****************************************************************************/
static bool _is_initialized = false;

/**************************************************************************//**
 * @brief Wi-SUN CoAP mutex control block
 *****************************************************************************/
__ALIGNED(8) static uint8_t _wisun_coap_mtx_cb[osMutexCbSize] = { 0 };

/**************************************************************************//**
 * @brief Wi-SUN CoAP mutex attribute
 *****************************************************************************/
static const osMutexAttr_t _wisun_coap_mtx_attr = {
  .name      = "WisunCoapMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _wisun_coap_mtx_cb,
  .cb_size   = sizeof(_wisun_coap_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Init CoAP descriptor */
bool sl_wisun_coap_init(const sl_wisun_coap_tx_callback tx_callback,
                        const sl_wisun_coap_rx_callback rx_callback,
                        const sl_wisun_coap_version_t   version)
{
  bool rval = false;

  _is_initialized = false;

  // init wisun coap mutex
  _wisun_coap_mtx = osMutexNew(&_wisun_coap_mtx_attr);
  assert(_wisun_coap_mtx != NULL);

  _wisun_coap_mutex_acquire();

#if SL_WISUN_COAP_MEM_USE_STATIC_MEMORY_POOL
  // init memory
  _wisun_coap_mem_init();
#endif
  // init handler
  _coap.malloc      = sl_wisun_coap_malloc;                    // malloc
  _coap.free        = sl_wisun_coap_free;                      // free
  _coap.tx_callback = tx_callback == NULL
                      ? &_default_coap_tx_callback : tx_callback; // tx callback
  _coap.rx_callback = rx_callback == NULL
                      ? &_default_coap_rx_callback : rx_callback; // rx callback
  _coap.version     = version;                                 // coap version

  // lib handler init
  _coap.handler     = sn_coap_protocol_init(_coap.malloc,
                                            _coap.free,
                                            _coap.tx_callback,
                                            _coap.rx_callback);
  _is_initialized = _coap.handler == NULL ? false : true;
  rval = _is_initialized;
  _wisun_coap_mutex_release();

  // return with the result of protocol init
  return rval;
}

/* Get state */
bool sl_wisun_coap_is_initialized(void)
{
  bool rval = false;
  _wisun_coap_mutex_acquire();
  rval = _is_initialized;
  _wisun_coap_mutex_release();

  return rval;
}

/* Wi-SUN CoAP malloc */
void *sl_wisun_coap_malloc(uint16_t size)
{
  void *p_mem;
  _wisun_coap_mutex_acquire();
  p_mem = _wisun_coap_mem_malloc(size);
  _wisun_coap_mutex_release();
  return p_mem;
}

/* Wi-SUN CoAP free */
void sl_wisun_coap_free(void *addr)
{
  _wisun_coap_mutex_acquire();
  _wisun_coap_mem_free(addr);
  _wisun_coap_mutex_release();
}

/* Wi-SUN CoAP parser */
sl_wisun_coap_packet_t* sl_wisun_coap_parser(uint16_t packet_data_len,
                                             uint8_t *packet_data)
{
  sl_wisun_coap_packet_t* res;
  res = sn_coap_parser(_coap.handler,
                       packet_data_len,
                       packet_data,
                       &_coap.version);
  // Store automatically the parsed message URI path
  (void) _wisun_coap_store_uri_path(res);
  return res;
}

/* Wi-SUN CoAP builder calculate size */
uint16_t sl_wisun_coap_builder_calc_size(const sl_wisun_coap_packet_t *message)
{
  uint16_t res;
  res = sn_coap_builder_calc_needed_packet_data_size(message);
  return res;
}

/* Wi-SUN CoAP builder */
int16_t sl_wisun_coap_builder(uint8_t *dest_buff,
                              const sl_wisun_coap_packet_t *message)
{
  int16_t res;
  res = sn_coap_builder(dest_buff, message);
  return res;
}

/* Wi-SUN CoAP parser */
sl_wisun_coap_packet_t *sl_wisun_coap_build_response(const sl_wisun_coap_packet_t *req,
                                                     sl_wisun_coap_message_code_t msg_code)
{
  sl_wisun_coap_packet_t * res;
  res = sn_coap_build_response(_coap.handler, req, (uint8_t) msg_code);
  return res;
}

/* Wi-SUN CoAP get parsed uri path */
const char *sl_wisun_coap_get_parsed_uri_path(char *dest_buff, const uint16_t buff_length)
{
  int32_t uri_path_len = 0;

  if (dest_buff == NULL) {
    return NULL;
  }

  _wisun_coap_mutex_acquire();

  uri_path_len = __str_n_len(_tmp_uri_path, SL_WISUN_COAP_URI_PATH_MAX_SIZE);

  if (uri_path_len <= 0
      || (uri_path_len - 1)  > (int32_t)SL_WISUN_COAP_URI_PATH_MAX_SIZE
      || uri_path_len > (buff_length - 1)) {
    __release_wisun_coap_mtx_and_retval(NULL);
  }
  // copy mem
  __memcpy(dest_buff, _tmp_uri_path, uri_path_len + 1);

  _wisun_coap_mutex_release();

  return (const char *) dest_buff;
}

/* get payload */
bool sl_wisun_coap_get_payload(const sl_wisun_coap_packet_t *source_packet,
                               uint8_t *dest_buff,
                               const uint16_t buff_length)
{
  if (source_packet == NULL) {
    return false;
  }

  _wisun_coap_mutex_acquire();

  if (source_packet->payload_ptr == NULL
      || source_packet->payload_len > buff_length) {
    __release_wisun_coap_mtx_and_retval(false);
  }
  __memcpy(dest_buff, source_packet->payload_ptr, buff_length);

  _wisun_coap_mutex_release();

  return true;
}

/* Set payload */
void sl_wisun_coap_set_payload(sl_wisun_coap_packet_t *dest_packet,
                               uint8_t *source_buff,
                               const uint16_t buff_length)
{
  if (dest_packet == NULL) {
    return;
  }
  _wisun_coap_mutex_acquire();
  dest_packet->payload_ptr = source_buff;
  dest_packet->payload_len = buff_length;
  _wisun_coap_mutex_release();
}

/* get token */
bool sl_wisun_coap_get_token(const sl_wisun_coap_packet_t *source_packet,
                             uint8_t *dest_buff,
                             const uint16_t buff_length)
{
  if (source_packet == NULL) {
    return false;
  }

  _wisun_coap_mutex_acquire();

  if (source_packet->token_ptr == NULL
      || source_packet->token_len > buff_length) {
    __release_wisun_coap_mtx_and_retval(false);
  }
  __memcpy(dest_buff, source_packet->token_ptr, buff_length);

  _wisun_coap_mutex_release();

  return true;
}

/* Set token */
void sl_wisun_coap_set_token(sl_wisun_coap_packet_t *dest_packet,
                             uint8_t *source_buff,
                             const uint8_t buff_length)
{
  if (dest_packet == NULL) {
    return;
  }
  _wisun_coap_mutex_acquire();
  dest_packet->token_ptr = source_buff;
  dest_packet->token_len = buff_length;
  _wisun_coap_mutex_release();
}

/* Get option list */
bool sl_wisun_coap_get_options_list(const sl_wisun_coap_packet_t *source_packet,
                                    sl_wisun_coap_option_list_t *dest)
{
  if (source_packet == NULL) {
    return false;
  }

  _wisun_coap_mutex_acquire();

  if (source_packet->options_list_ptr == NULL) {
    __release_wisun_coap_mtx_and_retval(false);
  }
  __memcpy(dest, source_packet->options_list_ptr,
           sizeof(sl_wisun_coap_option_list_t));

  _wisun_coap_mutex_release();

  return true;
}

/* Set options list */
void sl_wisun_coap_set_options_list(sl_wisun_coap_packet_t *dest_packet,
                                    sl_wisun_coap_option_list_t *source)
{
  if (dest_packet == NULL) {
    return;
  }
  _wisun_coap_mutex_acquire();
  dest_packet->options_list_ptr = source;
  _wisun_coap_mutex_release();
}

/* Wi-SUN CoAP print packet */
void sl_wisun_coap_print_packet(const sl_wisun_coap_packet_t *packet, const bool hex_format)
{
  if (packet == NULL) {
    return;
  }
  _wisun_coap_mutex_acquire();

  // according to JSON shape the print put together
  // start of the JSON
  printf("{\n");

  printf("  \"token_len\": %u,\n", packet->token_len);
  printf("  \"coap_status\": %u,\n", packet->coap_status);
  printf("  \"msg_code\": %u,\n", packet->msg_code);
  printf("  \"msg_type\": %u,\n", packet->msg_type);
  printf("  \"content_format\": %u,\n", packet->content_format);
  printf("  \"msg_id\": %u,\n", packet->msg_id);
  printf("  \"payload_len\": %u,\n", packet->payload_len);
  printf("  \"uri_path_len\": %u,\n", packet->uri_path_len);

  // token
  printf("  \"token\": \"");
  if (!packet->token_len) {
    printf("n/a\",\n");
  } else {
    _pretty_buff_print(packet->token_ptr, (uint16_t) packet->token_len, hex_format);
    printf(",\"\n");
  }

  // uri path
  printf("  \"uri_path\": \"");
  if (!packet->uri_path_len) {
    printf("n/a\",\n");
  } else {
    _pretty_buff_print(packet->uri_path_ptr, packet->uri_path_len, hex_format);
    printf("\",\n");
  }

  // payload
  printf("  \"payload\": ");
  if (!packet->payload_len) {
    printf("\"n/a\",\n");
  } else {
    printf("%s", packet->content_format == COAP_CT_JSON ? "\n" : "\"");
    _pretty_buff_print(packet->payload_ptr, packet->payload_len, hex_format);
    printf("%s", packet->content_format == COAP_CT_JSON ? "\n" : "\"");
  }
  // end of JSON
  printf("}\n");

  _wisun_coap_mutex_release();
}

/* Wi-SUN CoAP destroy packet */
void sl_wisun_coap_destroy_packet(sl_wisun_coap_packet_t *packet)
{
  sn_coap_parser_release_allocated_coap_msg_mem(_coap.handler, packet);
}

/* Wi-SUN CoAP get lib handler */
const sl_wisun_coap_handle_t* sl_wisun_coap_get_lib_handler(void)
{
  return _coap.handler;
}

/* Wi-SUN CoAP compare uri path */
bool sl_wisun_coap_compare_uri_path(const char *uri_path, const uint16_t buff_max_len)
{
  int32_t parsed_uri_path_len = -1;
  int32_t uri_path_len = -1;

  if (uri_path == NULL || buff_max_len == 0) {
    return false;
  }
  _wisun_coap_mutex_acquire();

  parsed_uri_path_len = __str_n_len(_tmp_uri_path, SL_WISUN_COAP_URI_PATH_MAX_SIZE);
  uri_path_len = __str_n_len(uri_path, buff_max_len);

  if (parsed_uri_path_len <= 0
      || uri_path_len <= 0
      || parsed_uri_path_len != uri_path_len) {
    __release_wisun_coap_mtx_and_retval(false);
  }
  for (uint16_t i = 0; i < uri_path_len; ++i) {
    if ( *(_tmp_uri_path + i) != *(uri_path + i)) {
      __release_wisun_coap_mtx_and_retval(false);
    }
  }

  _wisun_coap_mutex_release();

  return true;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Str n len */
static int32_t __str_n_len(const char *str, const uint16_t max_buff_size)
{
  int32_t res = -1;

  if (str == NULL) {
    return res;
  }

  for (uint32_t i = 0; i < max_buff_size; ++i) {
    if (!str[i]) {
      res = i;
      return res;
    }
  }
  return res;
}

/* memcopy */
static void __memcpy(void *dest, const void *src, const uint32_t len)
{
  if (dest == NULL || src == NULL || len == 0) {
    return;
  }
  for (uint32_t i = 0; i < len;
       *((uint8_t *)dest + i) = *((const uint8_t *)src + i), ++i) {
    ;
  }
}

/* Default CoAP TX callback */
static uint8_t _default_coap_tx_callback(uint8_t *packet_data, uint16_t packet_data_size,
                                         sn_nsdl_addr_s *addr, void *param)
{
  (void) packet_data;
  (void) packet_data_size;
  (void) addr;
  (void) param;
  return 0;
}

/* Default CoAP RX callback */
static int8_t _default_coap_rx_callback(sn_coap_hdr_s *header, sn_nsdl_addr_s *addr, void *param)
{
  (void) header;
  (void) addr;
  (void) param;
  return 0;
}

/* Mutex acquire */
static inline void _wisun_coap_mutex_acquire(void)
{
  assert(osMutexAcquire(_wisun_coap_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static inline void _wisun_coap_mutex_release(void)
{
  assert(osMutexRelease(_wisun_coap_mtx) == osOK);
}

/* Pretty buffer printer */
static inline void _pretty_buff_print(const uint8_t *buff_ptr, const uint16_t len, const bool hex_format)
{
  if (buff_ptr == NULL || len == 0) {
    return;
  }
  for (uint16_t i = 0; i < len; ++i) {
    printf(hex_format ? "0x%02x " : "%c", buff_ptr[i]);
  }
}

/* Wi-SUN CoAP store uri path as string */
static const char *_wisun_coap_store_uri_path(const sl_wisun_coap_packet_t *packet)
{
  char *p = _tmp_uri_path;
  uint16_t max_size = SL_WISUN_COAP_URI_PATH_MAX_SIZE;
  _wisun_coap_mutex_acquire();
  // check packet
  if (packet == NULL
      || packet->uri_path_ptr == NULL
      || packet->uri_path_len == 0) {
    __release_wisun_coap_mtx_and_retval(NULL);
  }

  // add leading '/' if missing
  if (*packet->uri_path_ptr != '/') {
    *p = '/';
    ++p;
    --max_size;
  }

  if (packet->uri_path_len > (max_size - 1)) {
    __release_wisun_coap_mtx_and_retval(NULL);
  }

  __memcpy(p, packet->uri_path_ptr, packet->uri_path_len);
  *(p + packet->uri_path_len) = 0; // terminate

  _wisun_coap_mutex_release();

  return (const char *) _tmp_uri_path;
}

/* Wi-SUN CoAP compare */
bool sl_wisun_coap_compare(const char* str1,
                           const uint16_t len1,
                           const char* str2,
                           const uint16_t len2)
{
  if (str1 == NULL || str2 == NULL || len1 != len2) {
    return false;
  }

  if (osKernelGetState() == osKernelRunning) {
    _wisun_coap_mutex_acquire();
  }

  for (uint16_t i = 0U; i < len1; ++i) {
    if ( str1[i] != str2[i] ) {
      return false;
    }
  }

  if (osKernelGetState() == osKernelRunning) {
    _wisun_coap_mutex_release();
  }

  return true;
}
