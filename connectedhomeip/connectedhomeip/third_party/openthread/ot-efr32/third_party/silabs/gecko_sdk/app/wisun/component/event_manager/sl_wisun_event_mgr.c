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
#include <stddef.h>
#include <stdio.h>
#include <assert.h>
#include "cmsis_os2.h"
#include "sl_cmsis_os2_common.h"
#include "sl_wisun_api.h"
#include "sl_wisun_event_mgr.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Event index for lookup
 *****************************************************************************/
typedef enum {
  EVENT_IDX_NOTVALID = -1,
  EVENT_IDX_NETWORK_UPDATE,
  EVENT_IDX_CONNECTED,
  EVENT_IDX_SOCKET_DATA,
  EVENT_IDX_SOCKET_DATA_AVAILABLE,
  EVENT_IDX_SOCKET_CONNECTED,
  EVENT_IDX_SOCKET_CONNECTION_AVAILABLE,
  EVENT_IDX_SOCKET_CLOSING,
  EVENT_IDX_DISCONNECTED,
  EVENT_IDX_CONNECTION_LOST,
  EVENT_IDX_SOCKET_DATA_SENT,
  EVENT_IDX_ERROR,
  EVENT_IDX_JOIN_STATE,
  EVENT_IDX_REGULATION_TX_LEVEL
} app_wisun_event_id_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief Event manager mutex lock
 *****************************************************************************/
static void app_wisun_event_mgr_mutex_lock(void);

/**************************************************************************//**
 * @brief Event manager mutex unlock
 *****************************************************************************/
static void app_wisun_event_mgr_mutex_unlock(void);

/**************************************************************************//**
 * @brief Decode indication event id to event lookup index
 * @details Using the app_wisun_event_id_t index definitions.
 *          It must be matched with _wisun_events lookup order
 * @param[in] ind Indication event ID
 * @return lookup index, if decoded, otherwise -1
 *****************************************************************************/
static inline app_wisun_event_id_t _decode_ind(const sl_wisun_msg_ind_id_t ind);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * @brief App framework mutex
 *****************************************************************************/
static osMutexId_t _app_wisun_event_mgr_mtx;

/**************************************************************************//**
 * @brief App framework mutex control block
 *****************************************************************************/
__ALIGNED(8) static uint8_t _app_wisun_event_mgr_mtx_cb[osMutexCbSize] = { 0 };

/**************************************************************************//**
 * @brief App framework mutex attribute
 *****************************************************************************/
static const osMutexAttr_t _app_wisun_event_mgr_mtx_attr = {
  .name      = "AppWisunEventMgrMutex",
  .attr_bits = osMutexRecursive,
  .cb_mem    = _app_wisun_event_mgr_mtx_cb,
  .cb_size   = sizeof(_app_wisun_event_mgr_mtx_cb)
};

/**************************************************************************//**
 * @brief Event Lookup table
 *        Lookup table indexes must be matched with app_wisun_event_id_t!
 *****************************************************************************/
static event_handler_t _wisun_events[] = {
  { .id = SL_WISUN_MSG_NETWORK_UPDATE_IND_ID,
    .callback = sl_wisun_network_update_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_CONNECTED_IND_ID,
    .callback = sl_wisun_connected_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_DATA_IND_ID,
    .callback = sl_wisun_socket_data_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND_ID,
    .callback = sl_wisun_socket_data_available_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID,
    .callback = sl_wisun_socket_connected_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID,
    .callback = sl_wisun_socket_connection_available_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_CLOSING_IND_ID,
    .callback = sl_wisun_socket_closing_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_DISCONNECTED_IND_ID,
    .callback = sl_wisun_disconnected_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_CONNECTION_LOST_IND_ID,
    .callback = sl_wisun_connection_lost_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID,
    .callback = sl_wisun_socket_data_sent_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_ERROR_IND_ID,
    .callback = sl_wisun_error_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_JOIN_STATE_IND_ID,
    .callback = sl_wisun_join_state_event_hnd, .custom_callback = NULL },
  { .id = SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID,
    .callback = sl_wisun_regulation_tx_level_hnd, .custom_callback = NULL },
};

/**************************************************************************//**
 * @brief Handled event count
 *****************************************************************************/
static const uint16_t _wisun_event_size = sizeof(_wisun_events) / sizeof(event_handler_t);

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/* Event Manager initialization */
void app_wisun_event_mgr_init(void)
{
  _app_wisun_event_mgr_mtx = osMutexNew(&_app_wisun_event_mgr_mtx_attr);
  assert(_app_wisun_event_mgr_mtx != NULL);
}

/* Event Manager custom callback register */
sl_status_t app_wisun_em_custom_callback_register(sl_wisun_msg_ind_id_t id,
                                                  custom_wisun_event_callback_t callback)
{
  osKernelState_t kernel_state = osKernelLocked;

  if (callback == NULL) {
    return SL_STATUS_FAIL; // parameter error
  }

  kernel_state = osKernelGetState();

  // check kernel state to avoid mutex acquire issue
  if (kernel_state == osKernelRunning) {
    app_wisun_event_mgr_mutex_lock();
  }

  for (uint16_t i = 0; i < _wisun_event_size; ++i) {
    if (_wisun_events[i].id == id) {
      _wisun_events[i].custom_callback = callback;
      break;
    }
  }

  if (kernel_state == osKernelRunning) {
    app_wisun_event_mgr_mutex_unlock();
  }

  return SL_STATUS_OK;
}

/* Event Manager custom callback removing */
sl_status_t app_wisun_em_custom_callback_remove(sl_wisun_msg_ind_id_t id)
{
  osKernelState_t kernel_state = osKernelLocked;

  // check kernel state to avoid mutex acquire issue
  if (kernel_state == osKernelRunning) {
    app_wisun_event_mgr_mutex_lock();
  }

  for (uint16_t i = 0; i < _wisun_event_size; ++i) {
    if (_wisun_events[i].id == id) {
      _wisun_events[i].custom_callback = NULL;
      break;
    }
  }

  if (kernel_state == osKernelRunning) {
    app_wisun_event_mgr_mutex_unlock();
  }

  return SL_STATUS_OK;
}

/* Wi-SUN event callback */
void sl_wisun_on_event(sl_wisun_evt_t *evt)
{
  app_wisun_event_id_t idx = _decode_ind((sl_wisun_msg_ind_id_t) evt->header.id);

  app_wisun_event_mgr_mutex_lock();

  if (EVENT_IDX_NOTVALID == idx) {
    printf("[Unknown event: %d]\r\n", evt->header.id);
  } else {
    _wisun_events[idx].callback(evt);
    if (_wisun_events[idx].custom_callback != NULL) {
      _wisun_events[idx].custom_callback(evt);
    }
  }

  app_wisun_event_mgr_mutex_unlock();
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/* Mutex acquire */
static void app_wisun_event_mgr_mutex_lock(void)
{
  assert(osMutexAcquire(_app_wisun_event_mgr_mtx, osWaitForever) == osOK);
}

/* Mutex release */
static void app_wisun_event_mgr_mutex_unlock(void)
{
  assert(osMutexRelease(_app_wisun_event_mgr_mtx) == osOK);
}

/* Decode wisun event id to lookup index */
static inline app_wisun_event_id_t _decode_ind(const sl_wisun_msg_ind_id_t ind)
{
  switch (ind) {
    case SL_WISUN_MSG_NETWORK_UPDATE_IND_ID:                return EVENT_IDX_NETWORK_UPDATE;
    case SL_WISUN_MSG_CONNECTED_IND_ID:                     return EVENT_IDX_CONNECTED;
    case SL_WISUN_MSG_SOCKET_DATA_IND_ID:                   return EVENT_IDX_SOCKET_DATA;
    case SL_WISUN_MSG_SOCKET_DATA_AVAILABLE_IND_ID:         return EVENT_IDX_SOCKET_DATA_AVAILABLE;
    case SL_WISUN_MSG_SOCKET_CONNECTED_IND_ID:              return EVENT_IDX_SOCKET_CONNECTED;
    case SL_WISUN_MSG_SOCKET_CONNECTION_AVAILABLE_IND_ID:   return EVENT_IDX_SOCKET_CONNECTION_AVAILABLE;
    case SL_WISUN_MSG_SOCKET_CLOSING_IND_ID:                return EVENT_IDX_SOCKET_CLOSING;
    case SL_WISUN_MSG_DISCONNECTED_IND_ID:                  return EVENT_IDX_DISCONNECTED;
    case SL_WISUN_MSG_CONNECTION_LOST_IND_ID:               return EVENT_IDX_CONNECTION_LOST;
    case SL_WISUN_MSG_SOCKET_DATA_SENT_IND_ID:              return EVENT_IDX_SOCKET_DATA_SENT;
    case SL_WISUN_MSG_ERROR_IND_ID:                         return EVENT_IDX_ERROR;
    case SL_WISUN_MSG_JOIN_STATE_IND_ID:                    return EVENT_IDX_JOIN_STATE;
    case SL_WISUN_MSG_REGULATION_TX_LEVEL_IND_ID:           return EVENT_IDX_REGULATION_TX_LEVEL;
    default:                                                return EVENT_IDX_NOTVALID;
  }
}

// ------------------------- event callbacks -----------------------------------

/**************************************************************************//**
 * @brief Wi-SUN network update event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_network_update_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Wi-SUN network connected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_connected_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Data event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_data_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Data avialable event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_data_available_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Socket connected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_connected_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Socket connection available event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_connection_available_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Socket closing event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_closing_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Wi-SUN network disconnected event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_disconnected_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Socket connection lost event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_connection_lost_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Socket data sent event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_socket_data_sent_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Wi-SUN error occured event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_error_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Wi-SUN join state event handler
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_join_state_event_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}

/**************************************************************************//**
 * @brief Wi-SUN regulation TX level indication
 * @details
 * @param[in] evt event ptr
 *****************************************************************************/
SL_WEAK void sl_wisun_regulation_tx_level_hnd(sl_wisun_evt_t *evt)
{
  (void) evt;
  assert(false);
}
