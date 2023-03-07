/***************************************************************************//**
 * @file
 * @brief Low Power Node implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "em_common.h"
#include "sl_status.h"

#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_config.h"

#include <stdio.h>
#include "app_assert.h"
#include "sl_simple_timer.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_APP_LOG_PRESENT
#include "app_log.h"
#endif // SL_CATALOG_APP_LOG_PRESENT

#include "sl_btmesh_lpn.h"
#include "sl_btmesh_lpn_config.h"

// Warning! The app_btmesh_util shall be included after the component configuration
// header file in order to provide the component specific logging macro.
#include "app_btmesh_util.h"

/***************************************************************************//**
 * @addtogroup lpn
 * @{
 ******************************************************************************/

/// High Priority
#define HIGH_PRIORITY                  0
/// No Timer Options
#define NO_FLAGS                       0
/// Callback has not parameters
#define NO_CALLBACK_DATA               (void *)NULL

/// periodic timer handle
static sl_simple_timer_t lpn_friend_find_timer;
static sl_simple_timer_t lpn_node_configured_timer;

/// periodic timer callback
static void lpn_friend_find_timer_cb(sl_simple_timer_t *handle,
                                     void *data);
static void lpn_node_configured_timer_cb(sl_simple_timer_t *handle,
                                         void *data);

/// Flag for indicating that lpn feature is active
static uint8_t lpn_active = 0;
static uint8_t num_mesh_proxy_conn = 0;

// Stores the netkey index of the network which the low power node belongs to
static uint16_t lpn_friend_netkey_idx = 0;

static void lpn_establish_friendship(void);

static void set_configuration_timer(uint32_t delay);

/***************************************************************************//**
 * Initialize LPN functionality with configuration and friendship establishment.
 ******************************************************************************/
void sl_btmesh_lpn_feature_init(void)
{
  sl_status_t result = SL_STATUS_OK;
  size_t netkey_bytes_written;

  // The sl_btmesh_node_get_networks API does not copy any netkey indexes if the
  // buffer is not long enough for all netkey indexes. It is expected that only
  // one netkey is present in the device but this array is allocated to provide
  // space for SL_BTMESH_CONFIG_MAX_NETKEYS netkey indexes to be on the safe
  // side.
  uint8_t netkey_bytes[SL_BTMESH_CONFIG_MAX_NETKEYS * 2];

  // Do not initialize LPN if lpn is currently active
  // or any GATT proxy connection is opened
  if (lpn_active || num_mesh_proxy_conn) {
    return;
  }

  // Initialize LPN functionality.
  result = sl_btmesh_lpn_init();
  if (result) {
    log("LPN initialization failed (0x%lx)\r\n", result);
    return;
  }
  lpn_active = 1;
  log("LPN initialized\r\n");
  sl_btmesh_lpn_on_init();

  // Configure LPN minimum friend queue length
  result = sl_btmesh_lpn_config(sl_btmesh_lpn_queue_length,
                                SL_BTMESH_LPN_MIN_QUEUE_LENGTH_CFG_VAL);
  if (result) {
    log("LPN queue configuration failed (0x%lx)\r\n", result);
    return;
  }
  // Configure LPN poll timeout
  result = sl_btmesh_lpn_config(sl_btmesh_lpn_poll_timeout,
                                SL_BTMESH_LPN_POLL_TIMEOUT_CFG_VAL);
  if (result) {
    log("LPN poll timeout configuration failed (0x%lx)\r\n", result);
    return;
  }
  // Configure LPN receive delay
  result = sl_btmesh_lpn_config(sl_btmesh_lpn_receive_delay,
                                SL_BTMESH_LPN_RECEIVE_DELAY_CFG_VAL);
  if (result) {
    log("LPN receive delay configuration failed (0x%lx)\r\n", result);
    return;
  }
  // Configure LPN request retries
  result = sl_btmesh_lpn_config(sl_btmesh_lpn_request_retries,
                                SL_BTMESH_LPN_REQUEST_RETRIES_CFG_VAL);
  if (result) {
    log("LPN request retries configuration failed (0x%lx)\r\n", result);
    return;
  }
  // Configure LPN retry interval
  result = sl_btmesh_lpn_config(sl_btmesh_lpn_retry_interval,
                                SL_BTMESH_LPN_RETRY_INTERVAL_CFG_VAL);
  if (result) {
    log("LPN retry interval configuration failed (0x%lx)\r\n", result);
    return;
  }

  // It is necessary to determine the netkey index because it is assigned by the
  // provisioner device and it can't be assumed that it is always 0 or other
  // constant value. If the node is part of multiple networks then it tries to
  // establish friendship with the first netkey in the list.
  result = sl_btmesh_node_get_networks(sizeof(netkey_bytes),
                                       &netkey_bytes_written,
                                       &netkey_bytes[0]);

  if (result != SL_STATUS_OK) {
    log("LPN get networks request failed (0x%lx)\r\n", result);
  } else if (netkey_bytes_written < 2) {
    // Defensive programming: this should not occur because at this point the
    // node is provisioned therefore at least one netkey shall be saved in the
    // node
    log("LPN get networks provided invalid number of netkey bytes (0x%zu)\r\n",
        netkey_bytes_written);
  } else {
    // The get networks API provides the netkeys in little endian format
    lpn_friend_netkey_idx = (netkey_bytes[1] << 8) | netkey_bytes[0];

    // Establish friendship with the lpn_friend_netkey_idx network key
    // The lpn_establish_friendship function uses global variable to identify
    // the network index because it does not change after provisioning but the
    // this function is called after a proxy connection is closed too and the
    // sl_btmesh_lpn_terminate_friendship API needs the netkey index as well so
    // the netkey index shall be stored.
    lpn_establish_friendship();
  }
}

/***************************************************************************//**
 * Deinitialize LPN functionality.
 ******************************************************************************/
void sl_btmesh_lpn_feature_deinit(void)
{
  sl_status_t result = 0;

  if (!lpn_active) {
    return; // lpn feature is currently inactive
  }

  // Cancel friend finding timer
  sl_status_t sc = sl_simple_timer_stop(&lpn_friend_find_timer);
  app_assert_status_f(sc, "Failed to stop timer\n");

  // Terminate friendship if exist
  result = sl_btmesh_lpn_terminate_friendship(lpn_friend_netkey_idx);
  if (result) {
    log("Friendship termination failed (0x%lx)\r\n", result);
  }
  // turn off lpn feature
  result = sl_btmesh_lpn_deinit();
  if (result) {
    log("LPN deinit failed (0x%lx)\r\n", result);
  }
  lpn_active = 0;
  log("LPN deinitialized\r\n");
  sl_btmesh_lpn_on_deinit();
}

/*******************************************************************************
 *  Handling of mesh lpn events.
 *  It handles:
 *   - lpn_friendship_established_id
 *   - lpn_friendship_failed_id
 *   - lpn_friendship_terminated_id
 *
 *  @param[in] evt  Pointer to incoming lpn event.
 ******************************************************************************/
void sl_btmesh_lpn_on_event(sl_btmesh_msg_t* evt)
{
  sl_btmesh_evt_node_initialized_t *data;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      data = (sl_btmesh_evt_node_initialized_t *)&(evt->data);
      if (data->provisioned) {
        sl_btmesh_lpn_feature_init();
      }
      break;

    case sl_btmesh_evt_node_provisioned_id:
      set_configuration_timer(SL_BTMESH_LPN_TIMEOUT_AFTER_PROVISIONED_CFG_VAL);
      break;

    case sl_btmesh_evt_node_model_config_changed_id:
      set_configuration_timer(SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_MODEL_CHANGED_CFG_VAL);
      break;

    case sl_btmesh_evt_node_config_set_id:
      set_configuration_timer(SL_BTMESH_LPN_TIMEOUT_AFTER_CONFIG_SET_CFG_VAL);
      break;

    case sl_btmesh_evt_node_key_added_id:
      set_configuration_timer(SL_BTMESH_LPN_TIMEOUT_AFTER_KEY_CFG_VAL);
      break;

    case sl_btmesh_evt_lpn_friendship_established_id:
      sl_btmesh_lpn_on_friendship_established(
        evt->data.evt_lpn_friendship_established.friend_address);
      break;

    case sl_btmesh_evt_lpn_friendship_failed_id: {
      sl_btmesh_lpn_on_friendship_failed(
        evt->data.evt_lpn_friendship_failed.reason);

      // try again after timer expires
      sl_status_t sc = sl_simple_timer_start(&lpn_friend_find_timer,
                                             SL_BTMESH_LPN_FRIEND_FIND_TIMEOUT_CFG_VAL,
                                             lpn_friend_find_timer_cb,
                                             NO_CALLBACK_DATA,
                                             false);
      app_assert_status_f(sc, "Failed to start timer\n");

      break;
    }

    case sl_btmesh_evt_lpn_friendship_terminated_id:
      sl_btmesh_lpn_on_friendship_terminated(
        evt->data.evt_lpn_friendship_terminated.reason);

      if (num_mesh_proxy_conn == 0) {
        // try again after timer expires
        sl_status_t sc = sl_simple_timer_start(&lpn_friend_find_timer,
                                               SL_BTMESH_LPN_FRIEND_FIND_TIMEOUT_CFG_VAL,
                                               lpn_friend_find_timer_cb,
                                               NO_CALLBACK_DATA,
                                               false);
        app_assert_status_f(sc, "Failed to start timer\n");
      }
      break;

    /* Proxy Events*/
    case sl_btmesh_evt_proxy_connected_id:
      num_mesh_proxy_conn++;
      // turn off lpn feature after GATT proxy connection is opened
      sl_btmesh_lpn_feature_deinit();
      break;

    case sl_btmesh_evt_proxy_disconnected_id:
      if (num_mesh_proxy_conn > 0) {
        if (--num_mesh_proxy_conn == 0) {
          // Initialize lpn when there is no active proxy connection
          sl_btmesh_lpn_feature_init();
        }
      }
      break;

    default:
      break;
  }
}

/***************************************************************************//**
 * Establishes friendship and logs if the request fails
 ******************************************************************************/
static void lpn_establish_friendship(void)
{
  sl_status_t result;

  log(SL_BTMESH_LPN_START_FRIEND_SEARCH_LOG_TEXT_CFG_VAL);
  result = sl_btmesh_lpn_establish_friendship(lpn_friend_netkey_idx);

  if (result != SL_STATUS_OK) {
    log(SL_BTMESH_LPN_FRIEND_NOT_FOUND_LOG_TEXT_CFG_VAL, result);
  }
}

/***************************************************************************//**
 *  Set the timer that delay LPN initialization to enable quick configuration
 *  over advertising bearer.
 *
 *  @param[in] delay  Time to set for the timer.
 ******************************************************************************/
static void set_configuration_timer(uint32_t delay)
{
  sl_status_t sc = sl_simple_timer_start(&lpn_node_configured_timer,
                                         delay,
                                         lpn_node_configured_timer_cb,
                                         NO_CALLBACK_DATA,
                                         false);
  app_assert_status_f(sc, "Failed to start timer\n");
}

/**************************************************************************//**
 * @addtogroup btmesh_lpn_tmr_cb Timer Callbacks
 * @{
 *****************************************************************************/
static void lpn_friend_find_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;

  lpn_establish_friendship();
}

static void  lpn_node_configured_timer_cb(sl_simple_timer_t *handle, void *data)
{
  (void)data;
  (void)handle;

  if (!lpn_active) {
    log(SL_BTMESH_LPN_START_INIT_LOG_TEXT_CFG_VAL);
    sl_btmesh_lpn_feature_init();
  }
}

/** @} (end addtogroup btmesh_lpn_tmr_cb) */

/// @addtogroup btmesh_lpn_weak_cb Weak implementation of Callbacks
/// @{

SL_WEAK void sl_btmesh_lpn_on_init(void)
{
}

SL_WEAK void sl_btmesh_lpn_on_deinit(void)
{
}

SL_WEAK void sl_btmesh_lpn_on_friendship_established(uint16_t node_address)
{
  (void) node_address;
}

SL_WEAK void sl_btmesh_lpn_on_friendship_failed(uint16_t reason)
{
  (void) reason;
}

SL_WEAK void sl_btmesh_lpn_on_friendship_terminated(uint16_t reason)
{
  (void) reason;
}

/// @} (end addtogroup btmesh_lpn_weak_cb)

/** @} (end addtogroup lpn) */
