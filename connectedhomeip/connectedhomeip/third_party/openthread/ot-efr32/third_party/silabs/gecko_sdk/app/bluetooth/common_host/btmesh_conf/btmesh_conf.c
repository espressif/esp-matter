/***************************************************************************/ /**
 * @file
 * @brief BT Mesh Configurator Component
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

#include "btmesh_conf.h"
#include "btmesh_conf_config.h"
#include "btmesh_conf_distributor.h"
#include "btmesh_conf_job.h"
#include "btmesh_conf_task.h"
#include "btmesh_db.h"

#include <stdlib.h>
#include <stdint.h>

#include "sl_status.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"
#include "sl_btmesh_model_specification_defs.h"

#include "app_log.h"
#include "app_assert.h"

// The BT Mesh Configurator dynamic memory allocation and deallocation is
// managed by create and destroy functions. The following rules were established
// to have consistent, uniform and correct interface and implementation:
// Create functions:
//  => Req01: Allocation and initialization of objects.
//  => Req02: The return type of the function shall be a pointer type to the
//            created struct. The parameters are object dependent. (~constructor)
//  => Req03: Create shall return a pointer to the allocated and initialized object
//  => Req04: Create shall validate the parameters first and return NULL in case
//            of invalid parameters.
//  => Req05: If the parameters are valid then create shall allocate the dynamic
//            memory for itself.
//      - This can mean multiple dynamic memory allocation calls.
//      - Example: Allocation of struct and allocation of array member of the
//                 allocated struct
//  => Req06: Create shall invoke the create function of other classes to
//            initialize aggregated objects.
//  => Req07: If any allocation fails except for the allocation of the object
//            itself then the destroy function shall be called and NULL shall
//            be returned. Every unallocated field shall be NULL when the destroy
//            function is called.
//      - Note: The destroy function shall be able to distinguish the allocated
//              and not allocated pointers. If malloc allocates dynamic memory
//              then the memory of the object is uninitialized so the pointers
//              in the objects are uninitialized as well.
//  => Req08: The attributes of the object shall be initialized after allocation
//            was successful.
//
// Destroy function:
//  => Req20: Deallocation and deinitialization of objects.
//  => Req21: Destroy function shall have void return type and it shall have
//            one parameter which is a pointer to the struct which shall be
//            deallocated. This parameter shall be named to self.
//  => Req22: The destroy function shall return immediately if the self
//            parameter is NULL. (same behavior as free stdlib function)
//  => Req23: The memory deallocation of dynamic memory of the object and
//            destroy function calls of aggregated objects shall be executed
//            in reverse order of allocation.
//  => Req24: The destroy function shall support the deallocation of partially
//            created objects.
//     - This is necessary in order to support the call of destroy function
//       inside create function when the allocation fails. (see Req06)

/***************************************************************************//**
 * @addtogroup btmesh_conf BT Mesh Configurator Component
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_conf_internal BT Mesh Configurator Internal
 * @brief Internal type definitions, variables and functions.
 * @{
 ******************************************************************************/
/// Local log new line shortcut definition
#define NL APP_LOG_NL

/// BT Mesh configuration server model ID
#define BTMESH_CONF_CONFIG_SERVER_MODEL_ID  0x0000
/// BT Mesh configuration client model ID
#define BTMESH_CONF_CONFIG_CLIENT_MODEL_ID  0x0001
/// BT Mesh health server model ID
#define BTMESH_CONF_HEALTH_SERVER_MODEL_ID  0x0002
/// BT Mesh health client model ID
#define BTMESH_CONF_HEALTH_CLIENT_MODEL_ID  0x0003

/// BT Mesh Stack BGAPI class ID mask
#define BTMESH_CONF_CLASS_ID_MASK  (0x00FF0000UL)
/// BT Mesh Stack BGAPI class ID shift
#define BTMESH_CONF_CLASS_ID_SHIFT (16)
/// Return BT Mesh Stack BGAPI class ID from the event identifier
#define BTMESH_CONF_GET_CLASS_ID(event_id) (((event_id) & BTMESH_CONF_CLASS_ID_MASK) \
                                            >> BTMESH_CONF_CLASS_ID_SHIFT)
/// Class ID of BT Mesh Stack BGAPI Configuration Client class
#define BTMESH_CONF_CLASS_ID  BTMESH_CONF_GET_CLASS_ID(sl_btmesh_cmd_config_client_get_dcd_id)

/// BT Mesh Model Information
typedef struct {
  const char *name;         ///< Name of the model based
  uint16_t id;              ///< Model identifier (BT Mesh specification)
  bool appkey_binding : 1;  ///< Model application key binding support
  bool subscription : 1;    ///< Model subscription support
  bool publication : 1;     ///< Model publication support
} btmesh_conf_model_info_t;

/***************************************************************************//**
 * Provides model information belonging to the specified Bluetooth SIG model ID.
 *
 * @param[in] sig_model_id Bluetooth SIG model identifier
 * @returns Pointer to the model information
 * @retval NULL if the model ID does not exists in @ref model_info_array
 ******************************************************************************/
static const btmesh_conf_model_info_t *get_model_info(uint16_t sig_model_id);

/***************************************************************************//**
 * Submit a configuration job for execution.
 * If @auto_destroy_on_submit_failure parameter is true and submit operation
 * fails then the configuration job is deallocated (destroyed).
 *
 * @param[in] job Configuration job which aggregates the configuration tasks
 * @param[in] auto_destroy_on_submit_failure If it is true then the configuration
 *   job is deallocated after the submit operation fails.
 * @returns Status of the job submit operation.
 * @retval SL_STATUS_OK If the job is submitted successfully.
 * @retval SL_STATUS_INVALID_STATE If the BT Mesh configurator is not initialized.
 ******************************************************************************/
static sl_status_t submit_job(btmesh_conf_job_t *job,
                              bool auto_destroy_on_submit_failure);

/// BT Mesh model information array which contains the information about
/// Bluetooth SIG Mesh models.
/// @warning The model information shall be in ascending order of model ID.
static const btmesh_conf_model_info_t model_info_array[];

/// Number of elements in model_info_array
static const uint16_t model_info_count;

/// BT Mesh Configuration Distributor singleton pointer
static btmesh_conf_distributor_t *btmesh_conf_dist = NULL;
/// BT Mesh Configurator callback active flag
static bool btmesh_conf_callback_active = false;

/** @} (end addtogroup btmesh_conf_internal) */

sl_status_t btmesh_conf_init(void)
{
  sl_status_t sc = SL_STATUS_OK;
  for (uint16_t idx = 1; idx < model_info_count; idx++) {
    // Model information array shall be ordered by model id to use binary search
    if (model_info_array[idx].id <= model_info_array[idx - 1].id) {
      sc = SL_STATUS_INVALID_CONFIGURATION;
      break;
    }
  }

  if (SL_STATUS_OK == sc) {
    btmesh_conf_dist = btmesh_conf_distributor_create(SL_BTMESH_CONF_EXECUTOR_COUNT_CFG_VAL);
    if (NULL == btmesh_conf_dist) {
      sc = SL_STATUS_ALLOCATION_FAILED;
    } else {
      sc = SL_STATUS_OK;
    }
  }
  return sc;
}

sl_status_t btmesh_conf_deinit(void)
{
  sl_status_t deinit_status;
  if (false != btmesh_conf_callback_active) {
    deinit_status = SL_STATUS_INVALID_STATE;
  } else {
    btmesh_conf_distributor_destroy(btmesh_conf_dist);
    btmesh_conf_dist = NULL;
    deinit_status = SL_STATUS_OK;
  }
  return deinit_status;
}

sl_status_t btmesh_conf_submit_job(btmesh_conf_job_t *job)
{
  bool auto_destroy_on_submit_failure = false;

#if 0 != SL_BTMESH_CONF_JOB_AUTO_DESTROY_ON_SUBMIT_FAILURE_CFG_VAL
  if (false != job->auto_destroy) {
    auto_destroy_on_submit_failure = true;
  }
#endif

  return submit_job(job, auto_destroy_on_submit_failure);
}

sl_status_t btmesh_conf_dcd_get(uint16_t netkey_index,
                                uint16_t server_address,
                                uint8_t page,
                                btmesh_conf_on_job_notification_t on_job_notification)
{
  if (NULL == btmesh_conf_dist) {
    // BT Mesh Configurator is uninitialized
    return SL_STATUS_INVALID_STATE;
  }

  btmesh_conf_task_t *task = btmesh_conf_task_dcd_get_create(page);
  if (NULL == task) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  btmesh_conf_job_t *job = btmesh_conf_job_create(netkey_index,
                                                  server_address,
                                                  task,
                                                  on_job_notification,
                                                  BTMESH_CONF_VARG_NULL,
                                                  true,
                                                  NULL);
  if (NULL == job) {
    task->destroy(task);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // The job is created internally and therefore if the submit fails then it
  // shall be destroyed internally as well so auto_destroy_on_submit_failure
  // parameter is set to true.
  return submit_job(job, true);
}

sl_status_t btmesh_conf_reset_node(uint16_t netkey_index,
                                   uint16_t server_address,
                                   btmesh_conf_on_job_notification_t on_job_notification)
{
  if (NULL == btmesh_conf_dist) {
    // BT Mesh Configurator is uninitialized
    return SL_STATUS_INVALID_STATE;
  }

  btmesh_conf_task_t *task = btmesh_conf_task_reset_node_create();
  if (NULL == task) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  btmesh_conf_job_t *job = btmesh_conf_job_create(netkey_index,
                                                  server_address,
                                                  task,
                                                  on_job_notification,
                                                  BTMESH_CONF_VARG_NULL,
                                                  true,
                                                  NULL);
  if (NULL == job) {
    task->destroy(task);
    return SL_STATUS_ALLOCATION_FAILED;
  }

  // The job is created internally and therefore if the submit fails then it
  // shall be destroyed internally as well so auto_destroy_on_submit_failure
  // parameter is set to true.
  return submit_job(job, true);
}

void btmesh_conf_on_event(const sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_prov_initialized_id:
    {
      sl_status_t sc;
      sc = btmesh_conf_init();
      app_assert_status_f(sc, "Failed to init configurator component." NL);

      sc = sl_btmesh_config_client_set_default_timeout(SL_BTMESH_CONF_REQUEST_TIMEOUT_MS_CFG_VAL,
                                                       SL_BTMESH_CONF_LPN_REQUEST_TIMEOUT_MS_CFG_VAL);
      app_assert_status_f(sc, "Failed to set config client default timeout.");
      break;
    }
  }

  if (NULL != btmesh_conf_dist) {
    // Forward events to the distributor if it exists (initialized)
    btmesh_conf_distributor_on_event(btmesh_conf_dist, evt);
  }
}

void btmesh_conf_step(void)
{
  if (NULL != btmesh_conf_dist) {
    // Execute distributor step function if it exists (initialized)
    btmesh_conf_distributor_step(btmesh_conf_dist);
  }
}

void btmesh_conf_on_job_notification(btmesh_conf_job_t *const job)
{
  // If job status notification callback is set then the application is notified
  // about the changed job status
  if (NULL != job->on_job_notification) {
    btmesh_conf_callback_active = true;
    job->on_job_notification(job);
    btmesh_conf_callback_active = false;
  }

  // If the auto destroy is set then the application does not need the config
  // job instance after the notification so it shall be deallocated
  if (false != job->auto_destroy) {
    btmesh_conf_job_destroy(job);
  }
}

sl_status_t btmesh_conf_get_sig_model_attributes(uint16_t model_id,
                                                 btmesh_conf_sig_model_attr_bitmask_t *attributes)
{
  if (NULL == attributes) {
    return SL_STATUS_NULL_POINTER;
  }

  const btmesh_conf_model_info_t *model_info = get_model_info(model_id);
  if (NULL == model_info) {
    return SL_STATUS_NOT_FOUND;
  }

  *attributes =
    (model_info->appkey_binding ? BTMESH_CONF_SIG_MODEL_SUPPORTS_APPKEY_BINDING : 0)
    | (model_info->subscription ? BTMESH_CONF_SIG_MODEL_SUPPORTS_SUBSCRIPTION : 0)
    | (model_info->publication ? BTMESH_CONF_SIG_MODEL_SUPPORTS_PUBLICATION : 0);
  return SL_STATUS_OK;
}

const char *btmesh_conf_sig_model_id_to_string(uint16_t model_id)
{
  const btmesh_conf_model_info_t *model_info = get_model_info(model_id);
  if (NULL == model_info) {
    // If the model ID is not found then return valid string in order to make
    // easier to use this function with snprintf like functions
    return "UnknownSigModel";
  } else {
    return model_info->name;
  }
}

bool btmesh_conf_is_configuration_event(uint32_t event_id)
{
  uint8_t class_id = BTMESH_CONF_GET_CLASS_ID(event_id);
  return (class_id == BTMESH_CONF_CLASS_ID);
}

sl_status_t btmesh_conf_get_handle_from_event(const sl_btmesh_msg_t *evt,
                                              uint32_t *handle)
{
  sl_status_t sc = SL_STATUS_OK;
  if ((NULL == handle) || (NULL == evt)) {
    sc = SL_STATUS_NULL_POINTER;
  } else {
    uint32_t event_id = SL_BT_MSG_ID(evt->header);
    switch (event_id) {
      case sl_btmesh_evt_config_client_request_modified_id:
        *handle = evt->data.evt_config_client_request_modified.handle;
        break;
      case sl_btmesh_evt_config_client_netkey_status_id:
        *handle = evt->data.evt_config_client_netkey_status.handle;
        break;
      case sl_btmesh_evt_config_client_netkey_list_id:
        *handle = evt->data.evt_config_client_netkey_list.handle;
        break;
      case sl_btmesh_evt_config_client_netkey_list_end_id:
        *handle = evt->data.evt_config_client_netkey_list_end.handle;
        break;
      case sl_btmesh_evt_config_client_appkey_status_id:
        *handle = evt->data.evt_config_client_appkey_status.handle;
        break;
      case sl_btmesh_evt_config_client_appkey_list_id:
        *handle = evt->data.evt_config_client_appkey_list.handle;
        break;
      case sl_btmesh_evt_config_client_appkey_list_end_id:
        *handle = evt->data.evt_config_client_appkey_list_end.handle;
        break;
      case sl_btmesh_evt_config_client_binding_status_id:
        *handle = evt->data.evt_config_client_binding_status.handle;
        break;
      case sl_btmesh_evt_config_client_bindings_list_id:
        *handle = evt->data.evt_config_client_bindings_list.handle;
        break;
      case sl_btmesh_evt_config_client_bindings_list_end_id:
        *handle = evt->data.evt_config_client_bindings_list_end.handle;
        break;
      case sl_btmesh_evt_config_client_model_pub_status_id:
        *handle = evt->data.evt_config_client_model_pub_status.handle;
        break;
      case sl_btmesh_evt_config_client_model_sub_status_id:
        *handle = evt->data.evt_config_client_model_sub_status.handle;
        break;
      case sl_btmesh_evt_config_client_subs_list_id:
        *handle = evt->data.evt_config_client_subs_list.handle;
        break;
      case sl_btmesh_evt_config_client_subs_list_end_id:
        *handle = evt->data.evt_config_client_subs_list_end.handle;
        break;
      case sl_btmesh_evt_config_client_heartbeat_pub_status_id:
        *handle = evt->data.evt_config_client_heartbeat_pub_status.handle;
        break;
      case sl_btmesh_evt_config_client_heartbeat_sub_status_id:
        *handle = evt->data.evt_config_client_heartbeat_sub_status.handle;
        break;
      case sl_btmesh_evt_config_client_beacon_status_id:
        *handle = evt->data.evt_config_client_beacon_status.handle;
        break;
      case sl_btmesh_evt_config_client_default_ttl_status_id:
        *handle = evt->data.evt_config_client_default_ttl_status.handle;
        break;
      case sl_btmesh_evt_config_client_gatt_proxy_status_id:
        *handle = evt->data.evt_config_client_gatt_proxy_status.handle;
        break;
      case sl_btmesh_evt_config_client_relay_status_id:
        *handle = evt->data.evt_config_client_relay_status.handle;
        break;
      case sl_btmesh_evt_config_client_network_transmit_status_id:
        *handle = evt->data.evt_config_client_network_transmit_status.handle;
        break;
      case sl_btmesh_evt_config_client_identity_status_id:
        *handle = evt->data.evt_config_client_identity_status.handle;
        break;
      case sl_btmesh_evt_config_client_friend_status_id:
        *handle = evt->data.evt_config_client_friend_status.handle;
        break;
      case sl_btmesh_evt_config_client_key_refresh_phase_status_id:
        *handle = evt->data.evt_config_client_key_refresh_phase_status.handle;
        break;
      case sl_btmesh_evt_config_client_lpn_polltimeout_status_id:
        *handle = evt->data.evt_config_client_lpn_polltimeout_status.handle;
        break;
      case sl_btmesh_evt_config_client_dcd_data_id:
        *handle = evt->data.evt_config_client_dcd_data.handle;
        break;
      case sl_btmesh_evt_config_client_dcd_data_end_id:
        *handle = evt->data.evt_config_client_dcd_data_end.handle;
        break;
      case sl_btmesh_evt_config_client_reset_status_id:
        *handle = evt->data.evt_config_client_reset_status.handle;
        break;
      default:
        app_assert(false == btmesh_conf_is_configuration_event(event_id),
                   "Unhandled configuration event.");
        sc = SL_STATUS_NOT_FOUND;
        break;
    }
  }
  return sc;
}

bool btmesh_conf_any_severe_log_level(uint8_t start_level)
{
  bool enabled = false;
  for (int16_t level = start_level; 0 <= level; level--) {
    enabled = enabled || _app_log_check_level(level);
  }
  return enabled;
}

/***************************************************************************//**
 * @addtogroup btmesh_conf_internal BT Mesh Configurator Internal
 * @{
 ******************************************************************************/
static sl_status_t submit_job(btmesh_conf_job_t *job,
                              bool auto_destroy_on_submit_failure)
{
  sl_status_t sc;
  if (NULL == btmesh_conf_dist) {
    // BT Mesh Configurator is uninitialized
    sc = SL_STATUS_INVALID_STATE;
  } else {
    sc = btmesh_conf_distributor_submit_job(btmesh_conf_dist, job);

    if ((SL_STATUS_OK != sc) && (false != auto_destroy_on_submit_failure)) {
      btmesh_conf_job_destroy(job);
    }
  }
  return sc;
}

static const btmesh_conf_model_info_t *get_model_info(uint16_t sig_model_id)
{
  int32_t start_idx = 0;
  int32_t end_idx = model_info_count - 1;
  int32_t middle_idx;
  // Binary search in the model information array which is ordered by ascending
  // model identifiers. The ascending order is checked by btmesh_conf_init.
  while (start_idx <= end_idx) {
    middle_idx = (start_idx + end_idx) / 2;
    if (sig_model_id < model_info_array[middle_idx].id) {
      end_idx = middle_idx - 1;
    } else if (model_info_array[middle_idx].id < sig_model_id) {
      start_idx = middle_idx + 1;
    } else {
      return &model_info_array[middle_idx];
    }
  }
  return NULL;
}

// WARNING! The model information shall be in ascending order of model ID.
static const btmesh_conf_model_info_t model_info_array[] = {
  {
    .id = BTMESH_CONF_CONFIG_SERVER_MODEL_ID,
    .name = "Configuration Server",
    .appkey_binding = false,
    .subscription = false,
    .publication = false,
  },
  {
    .id = BTMESH_CONF_CONFIG_CLIENT_MODEL_ID,
    .name = "Configuration Client",
    .appkey_binding = false,
    .subscription = false,
    .publication = false,
  },
  {
    .id = BTMESH_CONF_HEALTH_SERVER_MODEL_ID,
    .name = "Health Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = BTMESH_CONF_HEALTH_CLIENT_MODEL_ID,
    .name = "Health Client",
    .appkey_binding = true,
    .subscription = false,
    .publication = false,
  },
  {
    .id = MESH_GENERIC_ON_OFF_SERVER_MODEL_ID,
    .name = "Generic OnOff Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_ON_OFF_CLIENT_MODEL_ID,
    .name = "Generic OnOff Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_LEVEL_SERVER_MODEL_ID,
    .name = "Generic Level Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_LEVEL_CLIENT_MODEL_ID,
    .name = "Generic Level Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_TRANSITION_TIME_SERVER_MODEL_ID,
    .name = "Generic Default Transition Time Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_TRANSITION_TIME_CLIENT_MODEL_ID,
    .name = "Generic Default Transition Time Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_POWER_ON_OFF_SERVER_MODEL_ID,
    .name = "Generic Power OnOff Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_POWER_ON_OFF_SETUP_SERVER_MODEL_ID,
    .name = "Generic Power OnOff Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_GENERIC_POWER_ON_OFF_CLIENT_MODEL_ID,
    .name = "Generic Power OnOff Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_POWER_LEVEL_SERVER_MODEL_ID,
    .name = "Generic Power Level Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_POWER_LEVEL_SETUP_SERVER_MODEL_ID,
    .name = "Generic Power Level Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_GENERIC_POWER_LEVEL_CLIENT_MODEL_ID,
    .name = "Generic Power Level Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_BATTERY_SERVER_MODEL_ID,
    .name = "Generic Battery Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_BATTERY_CLIENT_MODEL_ID,
    .name = "Generic Battery Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_LOCATION_SERVER_MODEL_ID,
    .name = "Generic Location Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_LOCATION_SETUP_SERVER_MODEL_ID,
    .name = "Generic Location Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_GENERIC_LOCATION_CLIENT_MODEL_ID,
    .name = "Generic Location Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_ADMIN_PROPERTY_SERVER_MODEL_ID,
    .name = "Generic Admin Property Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_MANUF_PROPERTY_SERVER_MODEL_ID,
    .name = "Generic Manufacturer Property Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_USER_PROPERTY_SERVER_MODEL_ID,
    .name = "Generic User Property Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_CLIENT_PROPERTY_SERVER_MODEL_ID,
    .name = "Generic Client Property Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_GENERIC_PROPERTY_CLIENT_MODEL_ID,
    .name = "Generic Property Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SENSOR_SERVER_MODEL_ID,
    .name = "Sensor Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SENSOR_SETUP_SERVER_MODEL_ID,
    .name = "Sensor Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SENSOR_CLIENT_MODEL_ID,
    .name = "Sensor Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_TIME_SERVER_MODEL_ID,
    .name = "Time Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_TIME_SETUP_SERVER_MODEL_ID,
    .name = "Time Setup Server",
    .appkey_binding = true,
    .subscription = false,
    .publication = false,
  },
  {
    .id = MESH_TIME_CLIENT_MODEL_ID,
    .name = "Time Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SCENE_SERVER_MODEL_ID,
    .name = "Scene Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SCENE_SETUP_SERVER_MODEL_ID,
    .name = "Scene Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_SCENE_CLIENT_MODEL_ID,
    .name = "Scene Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SCHEDULER_SERVER_MODEL_ID,
    .name = "Scheduler Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_SCHEDULER_SETUP_SERVER_MODEL_ID,
    .name = "Scheduler Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_SCHEDULER_CLIENT_MODEL_ID,
    .name = "Scheduler Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_LIGHTNESS_SERVER_MODEL_ID,
    .name = "Light Lightness Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_LIGHTNESS_SETUP_SERVER_MODEL_ID,
    .name = "Light Lightness Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_LIGHTING_LIGHTNESS_CLIENT_MODEL_ID,
    .name = "Light Lightness Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_CTL_SERVER_MODEL_ID,
    .name = "Light CTL Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_CTL_SETUP_SERVER_MODEL_ID,
    .name = "Light CTL Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_LIGHTING_CTL_CLIENT_MODEL_ID,
    .name = "Light CTL Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_CTL_TEMPERATURE_SERVER_MODEL_ID,
    .name = "Light CTL Temperature Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_HSL_SERVER_MODEL_ID,
    .name = "Light HSL Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_HSL_SETUP_SERVER_MODEL_ID,
    .name = "Light HSL Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_LIGHTING_HSL_CLIENT_MODEL_ID,
    .name = "Light HSL Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_HSL_HUE_SERVER_MODEL_ID,
    .name = "Light HSL Hue Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_HSL_SATURATION_SERVER_MODEL_ID,
    .name = "Light HSL Saturation Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_XYL_SERVER_MODEL_ID,
    .name = "Light xyL Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LIGHTING_XYL_SETUP_SERVER_MODEL_ID,
    .name = "Light xyL Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = false,
  },
  {
    .id = MESH_LIGHTING_XYL_CLIENT_MODEL_ID,
    .name = "Light xyL Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LC_SERVER_MODEL_ID,
    .name = "Light LC Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LC_SETUP_SERVER_MODEL_ID,
    .name = "Light LC Setup Server",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  },
  {
    .id = MESH_LC_CLIENT_MODEL_ID,
    .name = "Light LC Client",
    .appkey_binding = true,
    .subscription = true,
    .publication = true,
  }
};

static const uint16_t model_info_count = sizeof(model_info_array)
                                         / sizeof(model_info_array[0]);

/** @} (end addtogroup btmesh_conf_internal) */
/** @} (end addtogroup btmesh_conf) */
