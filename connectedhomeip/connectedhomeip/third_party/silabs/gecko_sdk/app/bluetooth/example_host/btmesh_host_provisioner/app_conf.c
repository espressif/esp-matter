/***************************************************************************//**
 * @file
 * @brief BT Mesh Host Configurator Application Code
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
// Includes
#include <stdint.h>

#include "app.h"
#include "app_conf.h"

#include "sl_status.h"
#include "sl_btmesh_api.h"
#include "app_log.h"
#include "app_assert.h"

#include "btmesh_conf.h"
#include "btmesh_prov.h"
#include "btmesh_db.h"

/***************************************************************************//**
 * @addtogroup btmesh_host_provisioner BT Mesh Host Provisioner Example
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup btmesh_host_provisioner_conf \
 * BT Mesh Host Provisioner Example - Node configuration
 * @{
 ******************************************************************************/

// -----------------------------------------------------------------------------
// Macros

/// Local log new line shortcut definition
#define NL APP_LOG_NEW_LINE

/// Company identifier of Silicon Labs
#define SILABS_COMPANY_ID                           0x02ff

/// Product identifier of Silicon Labs NCP Empty example
#define SILABS_PRODUCT_ID_NCP_EMPTY_EXAMPLE         0x0000

/// Product identifier of Silicon Labs SoC Empty example
#define SILABS_PRODUCT_ID_SOC_EMPTY_EXAMPLE         0x0001

/// Product identifier of Silicon Labs SoC HSL Light example
#define SILABS_PRODUCT_ID_HSL_EXAMPLE               0x0002

/// Product identifier of Silicon Labs SoC Light example
#define SILABS_PRODUCT_ID_LIGHT_EXAMPLE             0x0003

/// Product identifier of Silicon Labs SoC Sensor Client example
#define SILABS_PRODUCT_ID_SENSOR_CLIENT_EXAMPLE     0x0004

/// Product identifier of Silicon Labs SoC Sensor Server example
#define SILABS_PRODUCT_ID_SENSOR_SERVER_EXAMPLE     0x0005

/// Product identifier of Silicon Labs SoC Switch example
#define SILABS_PRODUCT_ID_SWITCH_EXAMPLE            0x0006

/// Product identifier of Silicon Labs SoC Low Power Switch example
#define SILABS_PRODUCT_ID_LOW_POWER_SWITCH_EXAMPLE  0x0007

/// BT Mesh DCD page to query
#define APP_CONF_DCD_PAGE                           0

/// Default value of model publication group address
#define APP_CONF_PUB_ADDR                           0xC000
/// Default value of model publication application key index
#define APP_CONF_PUB_APPKEY_IDX                     0
/// Default model publication friendship credentials
///   - 0: normal credentials
///   - 1: frienship credentials
#define APP_CONF_PUB_CREDENTIALS                    0
/// Default value of model publication TTL (Time to Live)
#define APP_CONF_PUB_TTL                            5
/// Default value of model publication period
#define APP_CONF_PUB_PERIOD_MS                      0
/// Default value of model publication retransmission count (0-7 range)
#define APP_CONF_PUB_RETRANSMIT_COUNT               0
/// Default value of model publication retransmission interval in milliseconds
/// Range: 0 to 1600 ms with 50 ms resolution
#define APP_CONF_PUB_RETRANSMIT_INTERVAL_MS         0
/// Default value of model subscription group address
/// Note: Only one group address is added to model subscription list in this
///   example however a model is able to subscribe to multiple group addresses.
/// The maximum length of the model subscription list is configured by the
/// SL_BTMESH_CONFIG_MAX_SUBSCRIPTIONS (sl_btmesh_config.h) define on each node.
#define APP_CONF_SUB_ADDR                           0xC000

/// Default value of node default TTL
#define APP_CONF_DEFAULT_TTL                        5

/// Default value of node proxy feature state
#define APP_CONF_PROXY_ACTIVE                       1

/// Default value of node relay feature state
#define APP_CONF_RELAY_ACTIVE                       1
/// Default value of node relay retransmission count (0-7 range)
#define APP_CONF_RELAY_RETRANSMIT_COUNT             0
/// Default value of node relay retransmission interval in milliseconds
/// Range: 0 to 320 ms with 10 ms resolution
#define APP_CONF_RELAY_RETRANSMIT_INT_MS            0

/// Default value of node network transmission count (1-8 range)
#define APP_CONF_NW_TRANSMIT_COUNT                  1
/// Default value of node network transmission interval in milliseconds
/// Range: 0 to 320 ms with 10 ms resolution
#define APP_CONF_NW_TRANSMIT_INT_MS                 0

/// Default value of node friend feature state
#define APP_CONF_FRIEND_ACTIVE                      1

// -----------------------------------------------------------------------------
// Enums

/// State of configurator application
typedef enum {
  APP_CONF_STATE_IDLE,        ///< No configuration procedure is in progress
  APP_CONF_STATE_SUCCESS,     ///< Configuration procedure was successful
  APP_CONF_STATE_FAILURE,     ///< Configuration procedure failed
  APP_CONF_STATE_GET_DCD,     ///< Configuration procedure get DCD phase
  APP_CONF_STATE_NODE_CONFIG, ///< Configuration procedure configuration phase
  APP_CONF_STATE_COUNT,       ///< Number of configurator states
} app_conf_state_t;

// -----------------------------------------------------------------------------
// Static Function Declarations

/***************************************************************************//**
 * Execute configurator application state transition.
 *
 * @param[in] next_state Next state of configurator app
 *
 * The configurator application state is set.
 * If the configuration procedure ends then @ref app_on_node_configuration_end
 * and @ref app_ui_on_node_configuration_end functions are called to notify the
 * main application logic.
 ******************************************************************************/
static void app_conf_state_transition(app_conf_state_t next_state);

/***************************************************************************//**
 * Return the string representation of the Silicon Labs example product id.
 *
 * @param[in] product_id Product identifier of Silicon Labs example
 * @returns Name of Silicon Labs example belonging to the passed product id.
 * @retval "Unknown" If the product id is not recognized.
 *
 * It is guaranteed that a valid string is returned which makes easier to use
 * this function with snprintf like functions because no NULL check is necessary.
 * This function shall be called if the company identifier paired with the
 * product identifier is the company id of Silicon Labs.
 ******************************************************************************/
static const char *btmesh_example_product_id_to_string(uint16_t product_id);

/***************************************************************************//**
 * Create configuration job with default configuration values.
 *
 * @param[in] netkey_index Network key index of node which shall be configured
 * @param[in] server_address Server address of node which shall be configured
 * @returns Created configuration job.
 * @retval NULL If the configuration job creation fails.
 ******************************************************************************/
static btmesh_conf_job_t *create_node_conf_job_default(uint16_t netkey_index,
                                                       uint16_t server_address);

/***************************************************************************//**
 * Configuration job status notification handler.
 *
 * @param[in] job Configuration job with changed status
 *
 * This callback is added to the configuration job which is submitted for
 * execution to the BT Mesh configurator component (btmesh_conf).
 * If status of the configuration job changes then this callback is called.
 ******************************************************************************/
static void app_conf_on_job_notification(const btmesh_conf_job_t *job);

/***************************************************************************//**
 * Print node information.
 *
 * @param[in] node Pointer to the node
 *
 * If DCD of the node is not available in the BT Mesh database then the DCD is
 * queried first and it is printed afterwards otherwise the node information is
 * printed based on the cached data.
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called when the node information procedure ends to notify the main application
 * logic.
 ******************************************************************************/
static void app_conf_print_nodeinfo(const btmesh_db_node_t *const node);

/***************************************************************************//**
 * Node information notification handler.
 *
 * @param[in] job Configuration job with changed status
 *
 * This callback is added to the configuration job with get DCD configuration
 * task which is submitted for execution to the BT Mesh configurator component
 * (btmesh_conf). It provides status of the DCD query.
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called to notify the main application logic about the end of node information
 * procedure.
 ******************************************************************************/
static void app_conf_nodeinfo_on_job_notification(const btmesh_conf_job_t *job);

/***************************************************************************//**
 * Print Device Composition Data based on the stored value in BT Mesh database
 *
 * @param[in] node Pointer to the node
 ******************************************************************************/
static void app_conf_print_nodeinfo_dcd(const btmesh_db_node_t *const node);

/***************************************************************************//**
 * Finish node information procedure
 *
 * The @ref app_on_nodeinfo_end and @ref app_ui_on_nodeinfo_end functions are
 * called to notify the main application logic about the end of node information
 * procedure.
 ******************************************************************************/
static void app_conf_finish_nodeinfo(void);

// -----------------------------------------------------------------------------
// Static Variables

/// Configurator application data structure
static struct {
  app_conf_state_t state;  ///< Configurator application state
  uint16_t netkey_index;   ///< Network key index of configuration procedure
  uint16_t server_address; ///< Server address of configuration procedure
} app_conf = { .state = APP_CONF_STATE_IDLE };

// -----------------------------------------------------------------------------
// Function definitions - Node Configuration

sl_status_t app_conf_start_node_configuration(uint16_t netkey_index,
                                              uint16_t server_address)
{
  sl_status_t sc;
  if ((APP_CONF_STATE_IDLE == app_conf.state)
      || (APP_CONF_STATE_SUCCESS == app_conf.state)
      || (APP_CONF_STATE_FAILURE == app_conf.state)) {
    app_conf.netkey_index = netkey_index;
    app_conf.server_address = server_address;

    // Get DCD of the node first in order to figure out the company and product
    // identifier and determine what kind of models are present on the node and
    // which elements these models belong to.
    // The app_conf_on_job_notification callback is called when the status of
    // the DCD get configuration procedure changes.
    sc = btmesh_conf_dcd_get(netkey_index,
                             server_address,
                             APP_CONF_DCD_PAGE,
                             app_conf_on_job_notification);

    if (SL_STATUS_OK == sc) {
      app_conf_state_transition(APP_CONF_STATE_GET_DCD);
      app_log("Configuration of node (netkey_idx=%d,addr=0x%04x) is started." NL,
              app_conf.netkey_index,
              app_conf.server_address);
    } else {
      app_log_status_error_f(sc,
                             "Failed to start dcd get procedure to node "
                             "(netkey_idx=%d,addr=0x%04x)." NL,
                             netkey_index,
                             server_address);
    }
  } else {
    // Another node configuration procedure is already in progress
    sc = SL_STATUS_INVALID_STATE;
  }
  return sc;
}

static void app_conf_state_transition(app_conf_state_t next_state)
{
  // Make sure that next_state parameter is valid
  app_assert(next_state < APP_CONF_STATE_COUNT,
             "Invalid app configurator state (%d)." NL,
             next_state);

  switch (next_state) {
    case APP_CONF_STATE_IDLE:
    case APP_CONF_STATE_SUCCESS:
    case APP_CONF_STATE_FAILURE:
      // If the node configuration procedure ends then the main application logic
      // shall be notified.
      app_on_node_configuration_end(app_conf.netkey_index,
                                    app_conf.server_address);
      app_ui_on_node_configuration_end(app_conf.netkey_index,
                                       app_conf.server_address);
      break;

    default:
      break;
  }
  app_conf.state = next_state;
}

static btmesh_conf_job_t *create_node_conf_job_default(uint16_t netkey_index,
                                                       uint16_t server_address)
{
  sl_status_t sc;
  btmesh_conf_task_t *new_task, *last_task, *first_task;

  // The configuration job and its tasks are created based on the device
  // composition data of the node in BT Mesh database.
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(server_address);
  app_assert(NULL != node,
             "Node (addr=0x%04x) does not exist in database." NL,
             server_address);
  app_assert(false != node->dcd_available,
             "DCD of node (addr=0x%04x) is not available." NL,
             server_address);

  // If the specified node has Silicon Labs company identifier then print
  // example name which runs on the node.
  if (SILABS_COMPANY_ID == node->dcd.company_id) {
    app_log("Node (netkey_idx=%d,addr=0x%04x) runs %s example." NL,
            netkey_index,
            server_address,
            btmesh_example_product_id_to_string(node->dcd.product_id));
  }

  // Create task to set default TTL
  new_task = btmesh_conf_task_default_ttl_set_create(APP_CONF_DEFAULT_TTL);
  app_assert(NULL != new_task, "Failed to create configuration task." NL);
  first_task = new_task;
  last_task = new_task;

  // If a node supports the proxy feature then create a task to turn the proxy
  // feature on
  if (false != node->dcd.feature_proxy) {
    new_task = btmesh_conf_task_gatt_proxy_set_create(APP_CONF_PROXY_ACTIVE);
    sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
    app_assert_status_f(sc, "Failed to create configuration task." NL);
    last_task = new_task;
  }

  // If a node supports the relay feature then create a task to turn the relay
  // feature on
  if (false != node->dcd.feature_relay) {
    new_task = btmesh_conf_task_relay_set_create(APP_CONF_RELAY_ACTIVE,
                                                 APP_CONF_RELAY_RETRANSMIT_COUNT,
                                                 APP_CONF_RELAY_RETRANSMIT_INT_MS);
    sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
    app_assert_status_f(sc, "Failed to create configuration task." NL);
    last_task = new_task;
  }

  // Create task to set network transmit parameters
  new_task = btmesh_conf_task_network_transmit_set_create(APP_CONF_NW_TRANSMIT_COUNT,
                                                          APP_CONF_NW_TRANSMIT_INT_MS);
  sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
  app_assert_status_f(sc, "Failed to create configuration task." NL);
  last_task = new_task;

  // If a node supports the friend feature then create a task to turn the friend
  // feature on
  if (false != node->dcd.feature_friend) {
    new_task = btmesh_conf_task_friend_set_create(APP_CONF_FRIEND_ACTIVE);
    sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
    app_assert_status_f(sc, "Failed to create configuration task." NL);
    last_task = new_task;
  }

  // Create task to add application key to node
  // Note: Application key is created when the network is created in app.c
  //       by calling the btmesh_prov_create_appkey function
  new_task = btmesh_conf_task_appkey_add_create(APP_CONF_APPKEY_INDEX,
                                                netkey_index);
  sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
  app_assert_status_f(sc, "Failed to create configuration task." NL);
  last_task = new_task;

  // Iterate over each model on every elements of the node
  for (uint16_t elem_idx = 0; elem_idx < node->dcd.element_count; elem_idx++) {
    const btmesh_db_element_t *const element = &node->dcd.elements[elem_idx];
    for (uint16_t model_idx = 0; model_idx < element->model_count; model_idx++) {
      const btmesh_db_model_id_t *const model_id = &element->models[model_idx];
      btmesh_conf_sig_model_attr_bitmask_t attributes;
      // WARNING! This function binds the same application key and use the same
      // group address for publication and subscription and configures every
      // BT Mesh SIG model on the node.
      // This is not recommended in general due to following reasons:
      //   - Different application keys can be used for different functionalities
      //     to have multiple security domains
      //   - Some models on specific nodes should be added to separate group
      //     (for example BT Mesh Lighting models on nodes of a room could be
      //      added to its own group)
      //   - It might not be necessary to configure every models on a node.
      //     The node might support multiple usecases and therefore different
      //     models shall be configured based on the specific usecase.
      // These are quite usecase specific considerations so this default
      // implementation uses only one appkey and one group address for
      // demonstration purposes.

      if (MESH_SPEC_VENDOR_ID != model_id->vendor) {
        // The configuration of vendor model is skipped because it is unknown
        // whether it supports publication or not.
        // This code can be extended to configure the vendor model.
        continue;
      }

      // Get model attributes to determine if the model supports appkey binding,
      // publication or subscription
      sl_status_t model_attr_status =
        btmesh_conf_get_sig_model_attributes(model_id->model, &attributes);

      if (SL_STATUS_OK == model_attr_status) {
        if (attributes & BTMESH_CONF_SIG_MODEL_SUPPORTS_APPKEY_BINDING) {
          // If the model supports application key binding then create task
          // to bind an application key
          new_task = btmesh_conf_task_model_bind_create(elem_idx,
                                                        model_id->vendor,
                                                        model_id->model,
                                                        APP_CONF_APPKEY_INDEX);
          sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
          app_assert_status_f(sc, "Failed to create configuration task." NL);
          last_task = new_task;
        }
        if (attributes & BTMESH_CONF_SIG_MODEL_SUPPORTS_PUBLICATION) {
          // If the model supports publication then create task to set the
          // publication parameters
          new_task = btmesh_conf_task_model_pub_set_create(elem_idx,
                                                           model_id->vendor,
                                                           model_id->model,
                                                           APP_CONF_PUB_ADDR,
                                                           APP_CONF_PUB_APPKEY_IDX,
                                                           APP_CONF_PUB_CREDENTIALS,
                                                           APP_CONF_PUB_TTL,
                                                           APP_CONF_PUB_PERIOD_MS,
                                                           APP_CONF_PUB_RETRANSMIT_COUNT,
                                                           APP_CONF_PUB_RETRANSMIT_INTERVAL_MS);
          sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
          app_assert_status_f(sc, "Failed to create configuration task." NL);
          last_task = new_task;
        }
        if (attributes & BTMESH_CONF_SIG_MODEL_SUPPORTS_SUBSCRIPTION) {
          // If the model supports subscription then create task to add a group
          // address to the subscription list of the model
          new_task = btmesh_conf_task_model_sub_add_create(elem_idx,
                                                           model_id->vendor,
                                                           model_id->model,
                                                           APP_CONF_SUB_ADDR);
          sc = btmesh_conf_task_set_next_on_success(last_task, new_task);
          app_assert_status_f(sc, "Failed to create configuration task." NL);
          last_task = new_task;
        }
      } else {
        app_log_status_error_f(model_attr_status,
                               "Failed to get btmesh model (0x%04x) attributes." NL,
                               model_id->model);
      }
    }
  }

  // Create a configuration job with the created task tree.
  // The app_conf_on_job_notification callback is called when the status of the
  // DCD get configuration procedure changes.
  return btmesh_conf_job_create_default(netkey_index,
                                        server_address,
                                        first_task,
                                        app_conf_on_job_notification);
}

static void app_conf_on_job_notification(const btmesh_conf_job_t *job)
{
  switch (app_conf.state) {
    case APP_CONF_STATE_GET_DCD:
    {
      if (BTMESH_CONF_JOB_RESULT_SUCCESS == job->result) {
        app_log("DCD query of node (netkey_idx=%d,addr=0x%04x) completed." NL,
                app_conf.netkey_index,
                app_conf.server_address);
        // Create configuration job with configuration tasks
        btmesh_conf_job_t *config_job =
          create_node_conf_job_default(app_conf.netkey_index,
                                       app_conf.server_address);

        app_assert(NULL != config_job,
                   "Failed to create node configuration job."
                   "(netkey_idx=%d,addr=0x%04x)." NL,
                   app_conf.netkey_index,
                   app_conf.server_address);

        // Submit the configuration job for execution to the BT Mesh
        // Configurator component.
        sl_status_t config_job_status = btmesh_conf_submit_job(config_job);

        if (SL_STATUS_OK == config_job_status) {
          app_conf_state_transition(APP_CONF_STATE_NODE_CONFIG);
        } else {
          app_log_status_error_f(config_job_status,
                                 "Failed to submit configuration job"
                                 "(netkey_idx=%d,addr=0x%04x)." NL,
                                 app_conf.netkey_index,
                                 app_conf.server_address);
          app_conf_state_transition(APP_CONF_STATE_FAILURE);
        }
      } else {
        // Failed to get the DCD of the node
        app_log_error("DCD query of node (netkey_idx=%d,addr=0x%04x) failed." NL,
                      app_conf.netkey_index,
                      app_conf.server_address);
        app_conf_state_transition(APP_CONF_STATE_FAILURE);
      }
      break;
    }

    case APP_CONF_STATE_NODE_CONFIG:
      if (BTMESH_CONF_JOB_RESULT_SUCCESS == job->result) {
        app_log("Configuration of node (netkey_idx=%d,addr=0x%04x) is successful." NL,
                app_conf.netkey_index,
                app_conf.server_address);
        app_conf_state_transition(APP_CONF_STATE_SUCCESS);
      } else {
        app_log("Configuration of node (netkey_idx=%d,addr=0x%04x) is failed." NL,
                app_conf.netkey_index,
                app_conf.server_address);
        app_conf_state_transition(APP_CONF_STATE_FAILURE);
      }
      break;

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
// Function definitions - Node Information

void app_conf_print_nodeinfo_by_addr(uint16_t server_address)
{
  const btmesh_db_node_t *node = btmesh_db_node_get_by_addr(server_address);
  if (NULL == node) {
    app_log_error("Node is not found in btmesh_db." NL);
    app_conf_finish_nodeinfo();
  } else {
    app_conf_print_nodeinfo(node);
  }
}

void app_conf_print_nodeinfo_by_uuid(uuid_128 uuid)
{
  const btmesh_db_node_t *node = btmesh_db_node_get_by_uuid(uuid);
  if (NULL == node) {
    app_log_error("Node is not found in btmesh_db." NL);
    app_conf_finish_nodeinfo();
  } else {
    app_conf_print_nodeinfo(node);
  }
}

void app_conf_print_nodeinfo_by_id(uint16_t id)
{
  uint16_t node_cnt = 0;
  uuid_128 provisioner_uuid = { 0 };
  sl_status_t sc = sl_btmesh_node_get_uuid(&provisioner_uuid);
  if (SL_STATUS_OK != sc) {
    app_log_warning("Could not get provisioner UUID" NL);
  }
  btmesh_db_node_t *node;
  sl_slist_node_t *node_list = btmesh_db_get_node_list();
  SL_SLIST_FOR_EACH_ENTRY(node_list, node, btmesh_db_node_t, list_elem) {
    if (0 == memcmp(&provisioner_uuid, &node->prov.uuid, sizeof(uuid_128))) {
      // Skip the provisioner itself as self-configuration is not supported
      continue;
    }
    if (true == node->node_available) {
      if (node_cnt == id) {
        break;
      }
      node_cnt++;
    }
  }
  if (NULL == node) {
    app_log_error("Node is not found in btmesh_db." NL);
    app_conf_finish_nodeinfo();
  } else {
    app_conf_print_nodeinfo(node);
  }
}

static void app_conf_print_nodeinfo(const btmesh_db_node_t *const node)
{
  app_assert(NULL != node, "Node pointer is NULL." NL);
  uint16_t netkey_index = node->primary_subnet->netkey_index;
  uint16_t server_address = node->prov.prim_address;

  if (false == node->dcd_available) {
    // The DCD of the node have not been read from the remote node,
    // therefore it shall be read now
    sl_status_t sc = btmesh_conf_dcd_get(netkey_index,
                                         server_address,
                                         APP_CONF_DCD_PAGE,
                                         app_conf_nodeinfo_on_job_notification);
    if (SL_STATUS_OK != sc) {
      app_log_status_error_f(sc,
                             "Failed to start dcd get procedure to node "
                             "(netkey_idx=%d,addr=0x%04x)." NL,
                             netkey_index,
                             server_address);
      app_conf_finish_nodeinfo();
    }
  } else {
    app_conf_print_nodeinfo_dcd(node);
    app_conf_finish_nodeinfo();
  }
}

static void app_conf_nodeinfo_on_job_notification(const btmesh_conf_job_t *job)
{
  btmesh_db_node_t *node = btmesh_db_node_get_by_addr(job->server_address);
  // The control can't get here if the node does not exists in the btmesh
  // database so an assert is used to check it instead of logging
  app_assert(NULL != node,
             "Node (netkey_idx=%d,addr=0x%04x) does not exist in btmesh_db." NL,
             job->enc_netkey_index,
             job->server_address);

  if (BTMESH_CONF_JOB_RESULT_SUCCESS == job->result) {
    app_conf_print_nodeinfo_dcd(node);
  } else {
    app_log_error("DCD query of node (netkey_idx=%d,addr=0x%04x) failed." NL,
                  job->enc_netkey_index,
                  job->server_address);
  }
  app_conf_finish_nodeinfo();
}

static void app_conf_finish_nodeinfo(void)
{
  app_on_nodeinfo_end();
  app_ui_on_nodeinfo_end();
}

static void app_conf_print_nodeinfo_dcd(const btmesh_db_node_t *const node)
{
  app_log("Node 0x%04x information:" NL, node->prov.prim_address);
  if (SILABS_COMPANY_ID == node->dcd.company_id) {
    app_log("Company id: 0x%04x-%s" NL,
            node->dcd.company_id,
            "Silicon Labs");
    app_log("Product id: 0x%04x-%s" NL,
            node->dcd.product_id,
            btmesh_example_product_id_to_string(node->dcd.product_id));
  } else {
    app_log("Company id: 0x%04x" NL, node->dcd.company_id);
    app_log("Product id: 0x%04x" NL, node->dcd.product_id);
  }
  app_log("Version id: 0x%04x" NL, node->dcd.version_id);
  app_log("Min replay prot list length: %u" NL, node->dcd.min_replay_prot_list_len);
  app_log("Features:" NL);
  app_log("  -Relay:  %u" NL, node->dcd.feature_relay);
  app_log("  -Proxy:  %u" NL, node->dcd.feature_proxy);
  app_log("  -Friend: %u" NL, node->dcd.feature_friend);
  app_log("  -LPN:    %u" NL, node->dcd.feature_low_power);
  app_log("Elements: (count=%u)" NL, node->dcd.element_count);
  for (uint16_t elem_idx = 0; elem_idx < node->dcd.element_count; elem_idx++) {
    const btmesh_db_element_t *const element = &node->dcd.elements[elem_idx];
    app_log("  -Element %u:" NL, elem_idx);
    app_log("    -Address: 0x%04x" NL, element->address);
    app_log("    -Location: %u" NL, element->location);
    app_log("    -Models: (count=%u)" NL, element->model_count);
    for (uint16_t mdl_idx = 0; mdl_idx < element->model_count; mdl_idx++) {
      const btmesh_db_model_id_t *const model_id = &element->models[mdl_idx];
      if (MESH_SPEC_VENDOR_ID == model_id->vendor) {
        // SIG model
        app_log("      -SIG model: 0x%04x-%s" NL,
                model_id->model,
                btmesh_conf_sig_model_id_to_string(model_id->model));
      } else {
        // Vendor model
        app_log("      -Vendor (0x%04x) model: 0x%04x" NL,
                model_id->vendor,
                model_id->model);
      }
    }
  }
}

// -----------------------------------------------------------------------------
// Helper functions

static const char *btmesh_example_product_id_to_string(uint16_t product_id)
{
  // Each Silicon Labs BT Mesh example has its own unique product identifier.
  // The product ID of the example can be found in the following file of
  // Simplicity Studio project: config/btmeshconf/dcd_config.btmeshconf
  switch (product_id) {
    case SILABS_PRODUCT_ID_NCP_EMPTY_EXAMPLE:
      return "btmesh_ncp_empty";
    case SILABS_PRODUCT_ID_SOC_EMPTY_EXAMPLE:
      return "btmesh_soc_empty";
    case SILABS_PRODUCT_ID_HSL_EXAMPLE:
      return "btmesh_soc_hsl";
    case SILABS_PRODUCT_ID_LIGHT_EXAMPLE:
      return "btmesh_soc_light";
    case SILABS_PRODUCT_ID_SENSOR_CLIENT_EXAMPLE:
      return "btmesh_soc_sensor_client";
    case SILABS_PRODUCT_ID_SENSOR_SERVER_EXAMPLE:
      return "btmesh_soc_sensor_server";
    case SILABS_PRODUCT_ID_SWITCH_EXAMPLE:
      return "btmesh_soc_switch";
    case SILABS_PRODUCT_ID_LOW_POWER_SWITCH_EXAMPLE:
      return "btmesh_soc_switch_low_power";
    default:
      return "Unknown";
  }
}

/** @} (end addtogroup btmesh_host_provisioner_conf) */
/** @} (end addtogroup btmesh_host_provisioner) */
