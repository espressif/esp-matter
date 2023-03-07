/***************************************************************************//**
 * @file
 * @brief Socket Handler
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
#ifndef _SOCKET_HND_H_
#define _SOCKET_HND_H_

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include <inttypes.h>
#include <stdbool.h>
#include <assert.h>
#include "cmsis_os2.h"
#include "errno.h"
#include "socket_config.h"
#include "em_common.h"

/**************************************************************************//**
 * @defgroup SL_WISUN_SOCKET_HND Socket Handler
 * @ingroup SL_WISUN_SOCKET
 * @{
 *****************************************************************************/

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

#ifndef NULL
/**
 * @brief NULL ptr definition, if necessary
 *
 */
#  define NULL ((void *)0)
#endif

/**************************************************************************//**
 * @brief Using static buffer or not
 *****************************************************************************/
#define SOCKET_HND_USE_STATIC_BUFFER       (true)

/**************************************************************************//**
 * @brief Size of the sockaddress storage general container
 *****************************************************************************/
#define SOCKADDR_STORAGE_SIZE              (28U)

/**************************************************************************//**
 * @brief Return value of error
 *****************************************************************************/
#define RETVAL_ERROR            (-1)

/**************************************************************************//**
 * @brief Return value of correct execution
 *****************************************************************************/
#define RETVAL_OK               (0)

/**************************************************************************//**
 * @brief Socket domain is not set indicator
 *****************************************************************************/
#define SOCKET_DOMAIN_NOT_SET   (0)

/**************************************************************************//**
 * @brief Invalid socket id
 *****************************************************************************/
#define SOCKET_INVALID_ID       (-1)

/**************************************************************************//**
 * @brief Socket EOF for closed socket indication
 *****************************************************************************/
#define SOCKET_EOF              (0)

/**************************************************************************//**
 * @brief Set errno number macro function
 *****************************************************************************/
#define _set_errno(err) \
  do {                  \
    errno = (err);      \
  } while (0)

/**************************************************************************//**
 * @brief Set errno and return with value macro function
 *****************************************************************************/
#define _set_errno_ret_error(err) \
  do {                            \
    _set_errno(err);              \
    return RETVAL_ERROR;          \
  } while (0)

/**************************************************************************//**
 * @defgroup SL_WISUN_SOCKET_HND_TYPES Socket Handler type definitions
 * @ingroup SL_WISUN_SOCKET_HND
 * @{
 *****************************************************************************/

/// Custom info type def
typedef void* custom_info_t;

/// Socket state ID enum
typedef enum {
  /// Socket State Connection available ID
  SOCKET_STATE_CONNECTION_AVAILABLE,
  /// Socket State Data received ID
  SOCKET_STATE_DATA_RECEIVED,
  /// Socket State FIFO underflow ID
  SOCKET_FIFO_UNDERFLOW,
  /// Socket State FIFO overflow ID
  SOCKET_FIFO_OVERFLOW
} _socket_state_id_t;

/// Socket state structure
typedef struct _socket_state {
  /// Socket State Connection available ID
  bool _connection_avalaible;
  /// Socket State Data received ID
  bool _data_received;
  /// Socket State FIFO underflow ID
  bool _fifo_underflow;
  /// Socket State FIFO overflow ID
  bool _fifo_overflow;
} _socket_state_t;

/// Socket FIFO handler structure
typedef struct _socket_fifo {
  /// Overwrite previous fifo content.
  /// Fifo is reset and start writing from begining of the buffer
  bool _enable_overwrite;
  /// head pointer to write
  uint8_t *_head;
  /// tail pointer to read
  uint8_t *_tail;
  /// destination buffer pointer
  uint8_t *_buff;
  /// size of the FIFO
  uint32_t _size;
} _socket_fifo_t;

/// Socket handler structure
typedef struct _socket_handler {
  /// socket id
  int32_t _socket_id;
  /// custom info slot
  custom_info_t _custom_info;
  /// socket address length
  uint8_t _socket_addr_len;
  /// socket state
  _socket_state_t _state;
  /// socket FIFO handler
  _socket_fifo_t _fifo;
  /// socket buffer ptr
  uint8_t *_socket_buff;
  /// storing the current remote address structure
  uint8_t *_remote_addr;
  /// socket family : AF_INET, AF_INET6, AF_WISUN
  uint16_t _domain;
  /// socket address pointer
  void *_raw_socket_addr;
} _socket_handler_t;

/** @} (end SL_WISUN_SOCKET_HND_TYPES) */

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Socket handler init.
 * @details Socket handler initialization with default data and null ptr
 *****************************************************************************/
void socket_handler_init(void);

/**************************************************************************//**
 * @brief Add socket to the internal container.
 * @details Push socket id, socket address with length of address info to storage
 * @param[in] domain The address domain or family, it can be AF_WISUN, AF_INET and AF_INET6
 * @param[in] socket_id socket id
 * @param[in] socket_addr_struct socket address pointer
 * @param[in] socket_addr_len size of address structure
 * @return count of sockets if successful, -1 otherwise
 *****************************************************************************/
int32_t socket_handler_add(uint16_t domain,
                           const int32_t socket_id,
                           void *socket_addr_struct,
                           const uint8_t socket_addr_len);

/**************************************************************************//**
 * @brief Add only the socket handler to the internal container.
 * @details Push socket id, socket address and length is initialized with zero
 * @param[in] domain The address domain or family, it can be AF_WISUN, AF_INET and AF_INET6
 * @param[in] socket_id socket id
 * @return count of sockets if successful, -1 otherwise
 *****************************************************************************/
static inline int32_t socket_handler_add_sockid(uint16_t domain,
                                                const int32_t socket_id)
{
  return socket_handler_add(domain, socket_id, NULL, 0);
}

/**************************************************************************//**
 * @brief Set custom info for socket handler.
 * @details Gluing additional info to socket handler
 * @param[in] socket_id socket id
 * @param[in] info custom, user defined data structure pointer
 * @return 0 if successful, -1 otherwise
 *****************************************************************************/
int32_t socket_handler_set_custom_info(const int32_t socket_id, custom_info_t *info);

/**************************************************************************//**
 * @brief Reset custom info for socket handler.
 * @details Remove custom info from socket handler struct, set the pointer to NULL
 * @param[in] socket_id socket id
 * @return 0 if successful, -1 otherwise
 *****************************************************************************/
static inline int32_t socket_handler_reset_custom_info(const int32_t socket_id)
{
  return socket_handler_set_custom_info(socket_id, NULL);
}

/**************************************************************************//**
 * @brief Get socket handler.
 * @details Get socket handler by socket id
 * @param[in] socket_id socket id
 * @return socket handler pointer in storage if successful, NULL ptr otherwise
 *****************************************************************************/
_socket_handler_t *socket_handler_get(const int32_t socket_id);

/**************************************************************************//**
 * @brief Check socket id in the storage.
 * @details
 * @param[in] socket_id socket id
 * @return true if socket id is in the storage already, false otherwise
 *****************************************************************************/
static inline bool socket_handler_is_exist(const int32_t socket_id)
{
  return (socket_handler_get(socket_id) == NULL) ? false : true;
}

/**************************************************************************//**
 * @brief Remove socket handler from the storage.
 * @details
 * @param[in] socket_id socket id
 * @return count of sockets in the storage if successful, -1 otherwise
 *****************************************************************************/
int32_t socket_handler_remove(const int32_t socket_id);

/**************************************************************************//**
 * @brief Get count of sockets in the storage.
 * @details
 * @return count of sockets in the storage if successful, 0 otherwise
 *****************************************************************************/
uint8_t socket_handler_get_cnt(void);

/**************************************************************************//**
 * @brief Get count of available free slots in the storage.
 * @details
 * @return count of free socket slots in the storage if successful, 0 otherwise
 *****************************************************************************/
static inline uint8_t socket_handler_get_available_cnt(void)
{
  return (SOCKET_HND_MAX_SOCKET_NUMBER - socket_handler_get_cnt());
}

/**************************************************************************//**
 * @brief Set socket handler raw socket addr ptr and domain from socket structure.
 * @details Only the raw address ptr is stored, without mem copy
 * @param[out] handler socket handler pointer
 * @param[in] sockaddr_struct the entire socket address structure ptr
 * @param[in] socket_addr_len byte size of the address structure
 *****************************************************************************/
void socket_handler_set_sockaddr(_socket_handler_t *handler,
                                 const void *sockaddr_struct,
                                 const uint8_t socket_addr_len);

/**************************************************************************//**
 * @brief Get socket handler raw socket addr ptr and domain from socket structure.
 * @details Create a full copy of the domain and raw address to the dest address
 * @param[in] handler socket handler pointer
 * @param[out] dest_sockaddr_struct destination address ptr
 * @param[in] socket_addr_len length of full address structure with domain as well
 *****************************************************************************/
void socket_handler_get_sockaddr(const _socket_handler_t *handler,
                                 void *dest_sockaddr_struct,
                                 const uint8_t socket_addr_len);

/**************************************************************************//**
 * @brief Set socket handler state.
 * @details Set socket handler state by socket state ID
 * @param[out] handler socket handler ptr
 * @param[in] state_id state ID enum
 * @param[in] value bool value of the state
 * @return 0 if the state is set, otherwise -1
 *****************************************************************************/
int32_t socket_handler_set_state(_socket_handler_t *handler,
                                 const _socket_state_id_t state_id,
                                 bool value);

/**************************************************************************//**
 * @brief Get socket handler state.
 * @details Get socket handler state by socket state ID
 * @param[in] handler socket handler ptr
 * @param[in] state_id state ID enum
 * @param[out] dest_value bool value of the state
 * @return 0 if the state is copied to dest_value, otherwise -1
 *****************************************************************************/
int32_t socket_handler_get_state(const _socket_handler_t *handler,
                                 const _socket_state_id_t state_id,
                                 bool *dest_value);

/**************************************************************************//**
 * @brief Initialize socket handler FIFO.
 * @details Set FIFO pointers, size and states (thread safe)
 * @param[in,out] hnd handler
 * @param[in] buff_ptr buffer pointer for FIFO storage
 * @param[in] size size of the FIFO storage
 *****************************************************************************/
void socket_handler_fifo_init(_socket_handler_t *hnd,
                              uint8_t *buff_ptr,
                              const uint32_t size);

/**************************************************************************//**
 * @brief Reset socket handler FIFO storage
 * @details Set head and tail pointers to the buffer pointers and reset state
 * @param[in,out] hnd socket handler ptr
 *****************************************************************************/
void socket_handler_fifo_reset(_socket_handler_t *hnd);

/**************************************************************************//**
 * @brief Write data to FIFO.
 * @details Write to FIFO with error handling and size correction
 * @param[in,out] hnd socket handler ptr
 * @param[in] data data buffer to write
 * @param[in,out] size size of data to write, it will be set to the exact wrote size
 * @return return -1 if an error occurred overflow, otherwise 0
 *****************************************************************************/
int32_t socket_handler_fifo_write(_socket_handler_t *hnd, const uint8_t *data, uint32_t * const size);

/**************************************************************************//**
 * @brief Read data from FIFO.
 * @details Read data from FIFO with error handling and size correction
 * @param[in,out] hnd socket handler ptr
 * @param[in] data data buffer to read
 * @param[in,out] size size of data to read, it will be set to the exact read size
 * @return return -1 if an error occurred underflow, otherwise 0
 *****************************************************************************/
int32_t socket_handler_fifo_read(_socket_handler_t *hnd, uint8_t *data, uint32_t *size);

/**************************************************************************//**
 * @brief Destroy socket handler FIFO.
 * @details Set pointers to NULL
 * @param[in,out] hnd socket handler ptr
 *****************************************************************************/
static inline void socket_fifo_destroy(_socket_handler_t *hnd)
{
  socket_handler_fifo_init(hnd, NULL, 0);
}

/**************************************************************************//**
 * @brief Set FIFO overwrite mode
 * @details Set overwrite mode with reset FIFO
 * @param[in,out] hnd socket handler ptr
 * @param[in] enabled overwrite mode enabled or disabled bool
 * @return return -1 if an error occurred, otherwise 0
 *****************************************************************************/
int32_t socket_handler_fifo_set_overwrite(_socket_handler_t *hnd,
                                          const bool enabled);

/**************************************************************************//**
 * @brief Get FIFO overwrite mode.
 * @details Get overwrite mode with reset FIFO
 * @param[in,out] hnd socket handler ptr
 * @param[out] dest_val Result value
 * @return return -1 if an error occurred, otherwise 0
 *****************************************************************************/
int32_t socket_handler_fifo_get_overwrite(const _socket_handler_t *hnd,
                                          bool * const dest_val);

/**************************************************************************//**
 * @brief Fill socket handler remote address with data
 * @details The function uses internal memcopy
 * @param[in,out] hnd socket handler ptr
 * @param[in] data data pointer to write
 * @param[in] size size of data to write
 * @param[in] offset offset
 * @return return -1 on error, otherwise 0
 *****************************************************************************/
int32_t socket_handler_write_remote_addr(_socket_handler_t *hnd,
                                         const void *data,
                                         const uint8_t size,
                                         const uint8_t offset);

/**************************************************************************//**
 * @brief Read socket handler remote address
 * @details The function uses internal memcopy
 * @param[in] hnd socket handler ptr
 * @param[out] data destination buffer ptr
 * @param[in] size size of data to read
 * @param[in] offset offset
 * @return return -1 on error, otherwise 0
 *****************************************************************************/
int32_t socket_handler_read_remote_addr(const _socket_handler_t *hnd,
                                        void *data,
                                        const uint8_t size,
                                        const uint8_t offset);

/**************************************************************************//**
 * @brief Wi-SUN socket fifo overflow handler.
 * @details Weak implementation, it can be overrided.
 *          User can create own backup process to store remaining data
 * @param[in] socketid socket id
 * @param[in] buffer the original buffer ptr
 * @param[in] size size of the original buffer
 * @param[in] remaining_buffer remaining buffer ptr
 *****************************************************************************/
SL_WEAK void socket_fifo_overflow_handler(const int32_t socketid,
                                          const uint8_t *buffer,
                                          const uint32_t size,
                                          const uint8_t *remaining_buffer);

/** @}*/
#ifdef __cplusplus
}
#endif
#endif
