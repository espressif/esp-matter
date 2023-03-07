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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------

#include "socket_hnd.h"
#include "sl_malloc.h"
#include "em_common.h"
#include "sl_cmsis_os2_common.h"
#include "em_common.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Unlock and set errno internal macro function
 *****************************************************************************/
#define __unlock_set_errno_ret_error(__errno_val__) \
  do {                                              \
    _unlock();                                      \
    _set_errno(__errno_val__);                      \
    return RETVAL_ERROR;                            \
  } while (0)

/**************************************************************************//**
 * @brief Set fifo ptr to default macro function
 *****************************************************************************/
#define _set_fifo_ptr_to_default(hnd_ptr)                               \
  do {                                                                  \
    hnd_ptr->_fifo._head = hnd_ptr->_fifo._tail = hnd_ptr->_fifo._buff; \
  } while (0)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Memory copy
 * @details Memcopy for internal usage
 * @param[out] dest Destination ptr
 * @param[in] src Source ptr
 * @param[in] len Data size
 *****************************************************************************/
static void __memcpy(void *dest, const void *src, const uint32_t len);

/**************************************************************************//**
 * @brief Destroy socket handler
 * @details Set socket handler ptr to NULL and reset content
 * @param[in,out] socket_hnd socket handler ptr
 *****************************************************************************/
static inline void _destroy_socket_hnd(_socket_handler_t *socket_hnd);

/**************************************************************************//**
 * @brief Mutex lock
 * @details Lock socket handler mutex
 * @param[in,out] socket_hnd socket handler ptr
 *****************************************************************************/
static inline void _lock(void);

/**************************************************************************//**
 * @brief Mutex unlock
 * @details Release socket handler mutex
 * @param[in,out] socket_hnd socket handler ptr
 *****************************************************************************/
static inline void _unlock(void);

/**************************************************************************//**
 * @brief Destroy the socket handler (internal)
 * @details Set socket handler values to default without mutex lock/unlock
 * @param[in,out] socket_hnd socket handler
 *****************************************************************************/
static inline void _destroy_socket_hnd(_socket_handler_t *socket_hnd);

/**************************************************************************//**
 * @brief Init fifo (internal)
 * @details Internal fifo init to set fifo ptrs and flag to default
 * @param hnd socket handler
 * @param buff_ptr buff ptr
 * @param size size of the fifo
 *****************************************************************************/
static inline void _init_fifo(_socket_handler_t *hnd, uint8_t *buff_ptr, const uint32_t size);

/**************************************************************************//**
 * @brief Reset fifo buffer pointers and flags
 * @param hnd
 *****************************************************************************/
static inline void _reset_fifo(_socket_handler_t *hnd);

/**************************************************************************//**
 * @brief Set socket storage
 * @details static function to set handler structure pointer
 * @param[in] idx index of storage
 * @param[in] domain domain, eg. AF_WISUN
 * @param[in] socket_id socket id
 * @param[in] socket_addr_struct socket address structure pointer
 * @param[in] socket_addr_len length of socket address structure
 *****************************************************************************/
static inline void _set_socket_storage(const uint8_t idx, const uint16_t domain, const int32_t socket_id, void *socket_addr_struct, const uint8_t socket_addr_len);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Socket storage background container
 *****************************************************************************/
static _socket_handler_t _socket_storage[SOCKET_HND_MAX_SOCKET_NUMBER] = { 0 };

/**************************************************************************//**
 * @brief Socket storage pointer array
 *****************************************************************************/
static _socket_handler_t *_sockets[SOCKET_HND_MAX_SOCKET_NUMBER] = { NULL };

#if SOCKET_HND_USE_STATIC_BUFFER == true
/**************************************************************************//**
 * @brief Allocate a common socket buffer if malloc not prefered
 *****************************************************************************/
static uint8_t _buff[SOCKET_HND_MAX_SOCKET_NUMBER * SOCKET_BUFFER_SIZE] = { 0 };

/**************************************************************************//**
 * @brief Allocate a common buffer for storing the current remote address
 *****************************************************************************/
static uint8_t _remote_addr_storage[SOCKET_HND_MAX_SOCKET_NUMBER * SOCKADDR_STORAGE_SIZE] = { 0 };
#endif

/**************************************************************************//**
 * @brief Socket counter
 *****************************************************************************/
static uint8_t _cnt = 0;

/**************************************************************************//**
 * @brief Socket handler mutex
 *****************************************************************************/
static osMutexId_t _socket_hnd_mtx = NULL;

/**************************************************************************//**
 * @brief Socket handler mutex control block
 *****************************************************************************/
__ALIGNED(8) static uint8_t _socket_hnd_mtx_cb[osMutexCbSize] = { 0 };

/**************************************************************************//**
 * @brief Socket handler mutex attributes
 *****************************************************************************/
static const osMutexAttr_t  _socket_hnd_mtx_attr = {
  .name      = "SocketHndMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _socket_hnd_mtx_cb,
  .cb_size   = sizeof(_socket_hnd_mtx_cb)
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* socket handler fifo init */
void socket_handler_fifo_init(_socket_handler_t *hnd, uint8_t *buff_ptr, const uint32_t size)
{
  if (hnd == NULL) {
    return;
  }
  _lock();
  _init_fifo(hnd, buff_ptr, size);
  _unlock();
}

/* socket handler fifo reset */
void socket_handler_fifo_reset(_socket_handler_t *hnd)
{
  if (hnd == NULL) {
    return;
  }
  _lock();
  _reset_fifo(hnd);
  _unlock();
}

/* socket handler fifo write */
int32_t socket_handler_fifo_write(_socket_handler_t *hnd, const uint8_t *data, uint32_t * const size)
{
  int32_t retval = RETVAL_OK;
  uint32_t usedspace = 0;
  if (hnd == NULL) {
    return RETVAL_ERROR;
  }

  _lock();

  // Reset fifo befor write if overwrite options is enabled
  if (hnd->_fifo._enable_overwrite) {
    _reset_fifo(hnd);
  }

  usedspace = hnd->_fifo._head - hnd->_fifo._buff;

  if (usedspace + *size > hnd->_fifo._size) {
    retval = RETVAL_ERROR; // overflow
    hnd->_state._fifo_overflow = true;
    *size = hnd->_fifo._size - usedspace;
  }

  if (*size) {
    __memcpy(hnd->_fifo._head, data, *size);
    hnd->_fifo._head += *size;
  }

  _unlock();
  return retval;
}

/* Socket handler fifo read */
int32_t socket_handler_fifo_read(_socket_handler_t *hnd, uint8_t *data, uint32_t *size)
{
  int32_t retval = RETVAL_OK;
  uint32_t available = 0;

  if (hnd == NULL) {
    return RETVAL_ERROR;
  }

  _lock();

  available = hnd->_fifo._head - hnd->_fifo._tail; // calculate available data
  if (available < *size) {
    retval = RETVAL_ERROR; // underflow
    hnd->_state._fifo_underflow = true;
    *size = available; // trim size
  }

  if (*size) {
    __memcpy(data, hnd->_fifo._tail, *size);
    hnd->_fifo._tail += *size;
  }

  // if head is at tail (fifo empty),
  // set pointers to default to increse possible write size to the max
  if (hnd->_fifo._head == hnd->_fifo._tail) {
    _set_fifo_ptr_to_default(hnd);
  }
  _unlock();

  return retval;
}

int32_t socket_handler_fifo_set_overwrite(_socket_handler_t *hnd, const bool enabled)
{
  if (hnd == NULL) {
    return RETVAL_ERROR;
  }

  _lock();
  hnd->_fifo._enable_overwrite = enabled;
  _unlock();

  return RETVAL_OK;
}

int32_t socket_handler_fifo_get_overwrite(const _socket_handler_t *hnd, bool * const dest_val)
{
  if (hnd == NULL) {
    return RETVAL_ERROR;
  }

  _lock();
  *dest_val = hnd->_fifo._enable_overwrite;
  _unlock();

  return RETVAL_OK;
}

/* Init socket handler */
void socket_handler_init(void)
{
  osKernelState_t kernel_state = osKernelLocked;

  // get kernel status to avoid mutex assert failure
  kernel_state = osKernelGetState();

  // Create socket handler mutex
  _socket_hnd_mtx = osMutexNew(&_socket_hnd_mtx_attr);

  if (kernel_state == osKernelRunning) {
    _lock(); // lock socket handler
  }

  for (uint8_t i = 0; i < SOCKET_HND_MAX_SOCKET_NUMBER; ++i) {
    // reset content
    _destroy_socket_hnd(&_socket_storage[i]);
    // pointer set to NULL to indicate free slot
    _sockets[i] = NULL;
  }
  // reset counter
  _cnt = 0;

  if (kernel_state == osKernelRunning) {
    _unlock(); // unlock socket handler
  }
}

/* Add socket handler */
int32_t socket_handler_add(const uint16_t domain, const int32_t socket_id, void *socket_addr_struct, const uint8_t socket_addr_len)
{
  int32_t retval = RETVAL_ERROR;
  int16_t first_free_idx = -1;

  if (socket_id < 0) { // socket handler can be higher than zero
    return retval;
  }

  _lock(); // lock socket handler

  for (int16_t i = SOCKET_HND_MAX_SOCKET_NUMBER - 1; i >= 0; --i) {
    if (_sockets[i] == NULL) { // find the first free slot
      first_free_idx = i;
    } else if (_sockets[i]->_socket_id == socket_id) { // check socket id is already exist in the storage
      __unlock_set_errno_ret_error(EINVAL);
    }
  }
  if (first_free_idx == -1) {
    __unlock_set_errno_ret_error(EINVAL); // no free slot found
  }
  _set_socket_storage((uint8_t) first_free_idx, domain, socket_id, socket_addr_struct, socket_addr_len);

  ++_cnt;
  retval = _cnt;

  _unlock(); // unlock socket handler

  return retval;
}

/* Get socket handler pointer */
_socket_handler_t *socket_handler_get(const int32_t socket_id)
{
  _socket_handler_t *retval = NULL;
  _lock(); // lock socket handler

  for (uint8_t i = 0; i < SOCKET_HND_MAX_SOCKET_NUMBER; ++i) {
    if (_sockets[i] == NULL) {
      continue;
    }

    if (_sockets[i]->_socket_id == socket_id) {
      retval =  _sockets[i];
      break;
    }
  }

  _unlock(); // unlock socket handler

  return retval;
}

/* Remove socket handler */
int32_t socket_handler_remove(const int32_t socket_id)
{
  int32_t retval = RETVAL_ERROR;

  _lock(); // lock socket handler

  for (uint8_t i = 0; i < SOCKET_HND_MAX_SOCKET_NUMBER; ++i) {
    if (_sockets[i] == NULL) {
      continue;
    }

    if (_sockets[i]->_socket_id == socket_id) {
      _destroy_socket_hnd(_sockets[i]); /*Clear storage element*/
      _sockets[i] = NULL; /*Set pointer to NULL as 'free'*/
      --_cnt;
      retval = _cnt;
      break;
    }
  }

  _unlock(); // unlock socket handler

  return retval;
}

/* get count of sockets in handler */
uint8_t socket_handler_get_cnt(void)
{
  uint8_t retval = 0;

  _lock(); // lock socket handler
  retval = _cnt;
  _unlock(); //unlock socket handler

  return retval;
}

/* Set custom info pointer */
int32_t socket_handler_set_custom_info(const int32_t socket_id, custom_info_t *info)
{
  int32_t retval = RETVAL_ERROR;

  _lock(); // lock socket handler

  for (uint8_t i = 0; i < SOCKET_HND_MAX_SOCKET_NUMBER; ++i) {
    if (_sockets[i] == NULL) {
      continue;
    }

    if (_sockets[i]->_socket_id == socket_id) {
      _sockets[i]->_custom_info = info; // set info pointer
      retval = RETVAL_OK;
      break;
    }
  }

  _unlock(); // unlock socket handler

  return retval;
}

/* set socket handler state */
int32_t socket_handler_set_state(_socket_handler_t *handler, const _socket_state_id_t state_id, bool value)
{
  int32_t retval;

  #define __set_state_value_and_break(handler_state) \
  do {                                               \
    handler_state = (value);                         \
    retval =  RETVAL_OK;                             \
  } while (0)

  if (handler == NULL) {
    return RETVAL_ERROR;
  }

  _lock();
  switch (state_id) {
    case SOCKET_STATE_CONNECTION_AVAILABLE:
      __set_state_value_and_break(handler->_state._connection_avalaible);
      break;
    case SOCKET_STATE_DATA_RECEIVED:
      __set_state_value_and_break(handler->_state._data_received);
      break;
    default: retval = RETVAL_ERROR; break;
  }
  _unlock();

  #undef __set_state_value_and_break
  return retval;
}

/* get socket state */
int32_t socket_handler_get_state(const _socket_handler_t *handler, const _socket_state_id_t state_id, bool *dest_value)
{
  int32_t retval;

  #define __set_dest_value_and_break(state_value) \
  do {                                            \
    *dest_value = (state_value);                  \
    retval =  RETVAL_OK;                          \
  } while (0)

  if (handler == NULL || dest_value == NULL) {
    return RETVAL_ERROR;
  }

  _lock();

  switch (state_id) {
    case SOCKET_STATE_CONNECTION_AVAILABLE:
      __set_dest_value_and_break(handler->_state._connection_avalaible);
      break;
    case SOCKET_STATE_DATA_RECEIVED:
      __set_dest_value_and_break(handler->_state._data_received);
      break;
    default: retval = RETVAL_ERROR; break;
  }

  _unlock();

  #undef __set_dest_value_and_break
  return retval;
}

int32_t socket_handler_write_remote_addr(_socket_handler_t *hnd, const void *data, const uint8_t size, const uint8_t offset)
{
  if (hnd == NULL || data == NULL || ((size + offset) > SOCKADDR_STORAGE_SIZE)) {
    return RETVAL_ERROR;
  }
  _lock(); // lock
  __memcpy(hnd->_remote_addr + offset, data, size);
  _unlock(); // lock
  return RETVAL_OK;
}

int32_t socket_handler_read_remote_addr(const _socket_handler_t *hnd, void *data, const uint8_t size, const uint8_t offset)
{
  if (hnd == NULL || data == NULL || ((size + offset) > SOCKADDR_STORAGE_SIZE)) {
    return RETVAL_ERROR;
  }
  _lock(); // lock
  __memcpy(data, hnd->_remote_addr + offset, size);
  _unlock(); //unlock
  return RETVAL_OK;
}

void socket_handler_set_sockaddr(_socket_handler_t *handler, const void *sockaddr_struct, const uint8_t socket_addr_len)
{
  if (handler == NULL || sockaddr_struct == NULL) {
    return;
  }
  handler->_raw_socket_addr = (void *)sockaddr_struct;   // store the address ptr
  handler->_socket_addr_len = socket_addr_len;           // length of complete structure size with domain
}

void socket_handler_get_sockaddr(const _socket_handler_t *handler, void *dest_sockaddr_struct, const uint8_t socket_addr_len)
{
  if (handler == NULL || dest_sockaddr_struct == NULL) {
    return;
  }
  __memcpy(dest_sockaddr_struct, handler->_raw_socket_addr, socket_addr_len);
}

/* Weak implementation of fifo overflow handling */
SL_WEAK void socket_fifo_overflow_handler(const int32_t socketid,
                                          const uint8_t *buffer,
                                          const uint32_t size,
                                          const uint8_t *remaining_buffer)
{
  (void) socketid;
  (void) buffer;
  (void) size;
  (void) remaining_buffer;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Set socket storage */
static inline void _set_socket_storage(const uint8_t idx, const uint16_t domain, const int32_t socket_id, void *socket_addr_struct, const uint8_t socket_addr_len)
{
  _sockets[idx] = &_socket_storage[idx]; // Gluing storage element to the pointer
  _sockets[idx]->_socket_id = socket_id;
  _sockets[idx]->_domain = domain;
  _sockets[idx]->_raw_socket_addr = socket_addr_struct;
  _sockets[idx]->_socket_addr_len = socket_addr_len;

# if SOCKET_HND_USE_STATIC_BUFFER
  _init_fifo(_sockets[idx], _buff + (idx * SOCKET_BUFFER_SIZE), SOCKET_BUFFER_SIZE);
  // set remote address storage
  _sockets[idx]->_remote_addr = _remote_addr_storage + (idx * SOCKADDR_STORAGE_SIZE);
# else
  _init_fifo(_sockets[idx], (uint8_t *) sl_malloc(SOCKET_BUFFER_SIZE), SOCKET_BUFFER_SIZE);
  _sockets[idx]->_remote_addr = (uint8_t *) sl_malloc(SOCKADDR_STORAGE_SIZE);
# endif
}

/* memcopy */
static void __memcpy(void *dest, const void *src, const uint32_t len)
{
  if (dest == NULL || src == NULL) {
    return;
  }
  for (uint32_t i = 0; i < len;
       *((uint8_t *)dest + i) = *((const uint8_t *)src + i), ++i) {
    ;
  }
}

/* lock mutex*/
static inline void _lock()
{
  assert(osMutexAcquire(_socket_hnd_mtx, osWaitForever) == osOK);
}

/* unlock mutex */
static inline void _unlock()
{
  assert(osMutexRelease(_socket_hnd_mtx) == osOK);
}

/* Destroy socket handler */
static inline void _destroy_socket_hnd(_socket_handler_t *socket_hnd)
{
  socket_hnd->_domain = SOCKET_DOMAIN_NOT_SET;
  socket_hnd->_socket_id = SOCKET_INVALID_ID;
  socket_hnd->_raw_socket_addr = NULL;
  socket_hnd->_socket_addr_len = 0;
  socket_hnd->_custom_info = NULL;

  // reset states
  socket_hnd->_state._connection_avalaible = false;
  socket_hnd->_state._data_received = false;

#if SOCKET_HND_USE_STATIC_BUFFER
  // fifo init
  socket_hnd->_fifo._buff = socket_hnd->_fifo._head = socket_hnd->_fifo._tail = NULL;
  socket_hnd->_remote_addr = NULL;
#else
  sl_free(socket_hnd->_fifo._buff);
  sl_free(socket_hnd->_remote_addr);
#endif
  socket_hnd->_fifo._size = SOCKET_BUFFER_SIZE;
}

/* init fifo internal */
static inline void _init_fifo(_socket_handler_t *hnd, uint8_t *buff_ptr, const uint32_t size)
{
  hnd->_state._fifo_overflow = false; // reset states
  hnd->_state._fifo_underflow = false;
  hnd->_fifo._enable_overwrite = false; // overwrite is disabled
  hnd->_fifo._buff = hnd->_fifo._head = hnd->_fifo._tail = buff_ptr;
  hnd->_fifo._size = size;
}

/* reset fifo */
static inline void _reset_fifo(_socket_handler_t *hnd)
{
  hnd->_state._fifo_overflow = false;
  hnd->_state._fifo_underflow = false;
  _set_fifo_ptr_to_default(hnd);
}
