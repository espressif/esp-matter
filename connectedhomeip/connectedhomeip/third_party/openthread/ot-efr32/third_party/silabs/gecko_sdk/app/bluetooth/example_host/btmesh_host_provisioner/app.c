/***************************************************************************//**
* @file
* @brief BT Mesh Host Provisioner Example Project - CLI mode handler.
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
*******************************************************************************/

// -----------------------------------------------------------------------------
// Includes

// standard library headers
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// app-related headers
#include "app.h"
#include "app_conf.h"
#include "app_log.h"
#include "ncp_host.h"
#include "app_log_cli.h"
#include "app_assert.h"
#include "sl_simple_timer.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"

#include "system.h"
#include "sl_bt_api.h"
#include "sl_btmesh_ncp_host.h"
#include "sl_bt_ncp_host.h"
#include "sl_ncp_evt_filter_common.h"
#include "btmesh_conf.h"
#include "btmesh_conf_job.h"
#include "btmesh_prov.h"
#include "btmesh_db.h"

// -----------------------------------------------------------------------------
// Macros

// Optstring argument for getopt.
#define OPTSTRING      NCP_HOST_OPTSTRING "h"

// Usage info.
#define USAGE          "\n%s " NCP_HOST_USAGE \
  " [-h]"                                     \
  " [--nodeinfo <UUID>]"                      \
  " [--nodelist]"                             \
  " [--provision <UUID>]"                     \
  " [--remove <UUID>]"                        \
  " [--reset]"                                \
  " [--scan]\n"

// Options info.
#define OPTIONS                                                                 \
  "\nOPTIONS\n"                                                                 \
  "NCP Host-related commands:\n"                                                \
  NCP_HOST_OPTIONS                                                              \
  "    -h  Print this help message.\n"                                          \
  "\nHost Provisioner-related commands:\n"                                      \
  "    -i --nodeinfo   Print DCD information about a node in the network\n"     \
  "                    <UUID>     The unique identifier of the node.\n"         \
  "    -l --nodelist   List all nodes present in the provisioner's device "     \
  "database (DDB)\n"                                                            \
  "    -p --provision  Provision a node\n"                                      \
  "                    <UUID>     The UUID of the node to be provisioned. "     \
  "Can be acquired by --scan.\n"                                                \
  "    -r --remove     Remove the given node from the Mesh network\n"           \
  "                    <UUID>     The UUID of the node to be removed\n"         \
  "    -e --reset      Factory reset the provisioner. Note: This command does " \
  "not remove existing devices from the network.\n"                             \
  "    -s --scan       Scan and list unprovisioned beaconing nodes\n"           \
  "\nUUID shall be a string containing 16 separate octets, e.g. "               \
  "\"00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff\"\n"                       \
  "The separator can be any character, but in case of a whitespace character "  \
  "this example requires quotation marks around the string.\n\n"                \
  "Running the program without a Host Provisioner-related command will "        \
  "trigger UI mode.\n\n"

// This characteristic handle value has to match the value in gatt_db.h of
// NCP empty example running on the connected WSTK.
#define GATTDB_SYSTEM_ID 18

// -----------------------------------------------------------------------------
// Static Function Declarations

/***************************************************************************//**
* Handle --nodeinfo functionality
*
*******************************************************************************/
static void handle_nodeinfo(void);

/***************************************************************************//**
* Handle --nodelist functionality
*
*******************************************************************************/
static void handle_nodelist(void);

/***************************************************************************//**
* Handle --provision functionality
*
*******************************************************************************/
static void handle_provision(void);

/***************************************************************************//**
* Handle --remove functionality
*
*******************************************************************************/
static void handle_remove(void);

/***************************************************************************//**
* Handle --reset functionality
*
*******************************************************************************/
static void handle_reset(void);

/***************************************************************************//**
* Handle --scan functionality
*
*******************************************************************************/
static void handle_scan(void);

/***************************************************************************//**
*  Node removal job status callback
*
* @param[in] job The job this function is called from
*******************************************************************************/
static void app_on_remove_node_job_status(const btmesh_conf_job_t *job);

/***************************************************************************//**
* Callback for the timer used during scanning
*
* @param[in] timer Pointer to the timer used
* @param[in] data Data from the timer
*******************************************************************************/
static void app_on_scan_timer(sl_simple_timer_t *timer, void *data);

/***************************************************************************//**
* Callback for the timer used during provisioner reset
*
* @param[in] timer Pointer to the timer used
* @param[in] data Data from the timer
*******************************************************************************/
static void app_on_reset_timer(sl_simple_timer_t *timer, void *data);

/***************************************************************************//**
* Add user event filter via sl_bt_user_manage_event_filter
*
* @param[in] event_id ID of the event to be filtered
* @return Status of the sl_bt_user_manage_event_filter command
*******************************************************************************/
static sl_status_t app_add_user_event_filter(const uint32_t event_id);

// -----------------------------------------------------------------------------
// Static Variables

/// The state of the current command
static command_state_t command_state = INIT;
/// The command in use
static command_t command = NONE;
/// UUID passed as an argument
static uuid_128 command_uuid;
/// Flag stating that the btmeshprov_initialized event has arrived
static bool initialized = false;
/// Number of networks already present on the provisioner at startup
static uint16_t networks_on_startup = 0;
/// Number of known devices in the DDB list
static uint16_t ddb_count = 0;
/// Timer for scanning for unprovisioned devices
static sl_simple_timer_t scan_timer;
/// Timer for provisioner node reset
static sl_simple_timer_t reset_timer;

// -----------------------------------------------------------------------------
// Function definitions

/***************************************************************************//**
* Application Init.
*******************************************************************************/
void app_init(int argc, char *argv[])
{
  sl_status_t sc;
  int opt;

  app_log_filter_threshold_set(APP_LOG_LEVEL_WARNING);

  // Initialize timer functionality
  sl_simple_timer_init();

  // Process command line options
  static struct option long_options[] = {
    { "nodeinfo", required_argument, 0, 'i' },
    { "nodelist", no_argument, 0, 'l' },
    { "provision", required_argument, 0, 'p' },
    { "remove", required_argument, 0, 'r' },
    { "reset", no_argument, 0, 'e' },
    { "scan", no_argument, 0, 's' },
    { 0, 0, 0, 0 }
  };
  int option_index = 0;
  while ((opt = getopt_long(argc, argv, "ehi:lp:r:s" OPTSTRING, long_options, &option_index)) != -1) {
    switch (opt) {
      case 'p':
        // provision selected node
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        command = PROVISION;
        command_state = START;
        char *pos = optarg;
        size_t len = strlen(optarg);
        // Parse input for UUID
        app_parse_uuid(pos, len, &command_uuid);
        break;
      case 'l':
        // List all nodes in network
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        app_log("Nodelist" APP_LOG_NEW_LINE);
        command = NODELIST;
        command_state = START;
        break;
      case 'i': {
        // Info about selected node
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        app_log("Nodeinfo: %s" APP_LOG_NEW_LINE, optarg);
        char *pos = optarg;
        size_t len = strlen(optarg);
        // Parse input for UUID
        app_parse_uuid(pos, len, &command_uuid);
        command = NODEINFO;
        command_state = START;
        break;
      }
      case 'r': {
        // Remove node from network
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        app_log("Remove: %s" APP_LOG_NEW_LINE, optarg);
        char *pos = optarg;
        size_t len = strlen(optarg);
        // Parse input for UUID
        app_parse_uuid(pos, len, &command_uuid);
        command = REMOVE_NODE;
        command_state = START;
        break;
      }
      case 's':
        // Scan for unprovisioned nodes
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        app_log("Scan" APP_LOG_NEW_LINE);
        command = SCAN;
        command_state = START;
        break;
      case 'e':
        // Factory reset
        app_log_filter_threshold_set(APP_LOG_LEVEL_INFO);
        app_log("Factory reset" APP_LOG_NEW_LINE);
        command = RESET;
        command_state = START;
        break;
      case 'h':
        // Print help.
        app_log(USAGE, argv[0]);
        app_log(OPTIONS);
        exit(EXIT_SUCCESS);

      // Process options for other modules.
      default:
        sc = ncp_host_set_option((char)opt, optarg);
        if (sc != SL_STATUS_OK) {
          app_log(USAGE, argv[0]);
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // Initialize NCP connection.
  sc = ncp_host_init();
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  app_assert_status(sc);

  SL_BTMESH_API_REGISTER();

  app_log_info("Host Provisioner initialised." APP_LOG_NEW_LINE);
  app_log_info("Resetting NCP..." APP_LOG_NEW_LINE);
  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(sl_bt_system_boot_mode_normal);

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/***************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
  if (initialized) {
    switch (command) {
      case SCAN:
        handle_scan();
        break;
      case PROVISION:
        handle_provision();
        break;
      case NODELIST:
        handle_nodelist();
        break;
      case NODEINFO:
        handle_nodeinfo();
        break;
      case REMOVE_NODE:
        handle_remove();
        break;
      case RESET:
        handle_reset();
        break;
      case NONE:
        // UI mode
        handle_ui();
        break;
      default:
        break;
    }
  }
}

/***************************************************************************//**
 * Application Deinit.
 *****************************************************************************/
void app_deinit(void)
{
  ncp_host_deinit();

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application deinit code here!                       //
  // This is called once during termination.                                 //
  /////////////////////////////////////////////////////////////////////////////
}

/***************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
    {
      // Filter legacy and new scanner report events as it would send a message
      // every 5 ms and clog UART while scanning for unprovisioned nodes
      sc = app_add_user_event_filter(sl_bt_evt_scanner_scan_report_id);
      app_assert_status_f(sc, "Failed to enable filtering on the target" APP_LOG_NEW_LINE);

      sc = app_add_user_event_filter(sl_bt_evt_scanner_legacy_advertisement_report_id);
      app_assert_status_f(sc, "Failed to enable filtering on the target" APP_LOG_NEW_LINE);

      sc = app_add_user_event_filter(sl_bt_evt_scanner_extended_advertisement_report_id);
      app_assert_status_f(sc, "Failed to enable filtering on the target" APP_LOG_NEW_LINE);

      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d" APP_LOG_NEW_LINE,
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);

      // Initialize Mesh stack in Provisioner mode,
      // wait for initialized event
      app_log_info("Provisioner init" APP_LOG_NEW_LINE);
      sc = sl_btmesh_prov_init();
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to init provisioner\n",
                 (int)sc);
      break;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/***************************************************************************//**
 * Bluetooth Mesh stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth Mesh stack.
 *****************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_prov_initialized_id: {
      // Device successfully initialized in provisioner mode
      sl_btmesh_evt_prov_initialized_t *initialized_evt;
      initialized_evt = (sl_btmesh_evt_prov_initialized_t *)&(evt->data);

      app_log_info("Network initialized" APP_LOG_NEW_LINE);
      app_log_info("Networks: %x" APP_LOG_NEW_LINE, initialized_evt->networks);
      app_log_info("Address : %x" APP_LOG_NEW_LINE, initialized_evt->address);
      app_log_info("IV Index: %x" APP_LOG_NEW_LINE, initialized_evt->iv_index);
      initialized = true;
      networks_on_startup = initialized_evt->networks;
      if (INIT == command_state) {
        command_state = START;
      }
      break;
    }

    case sl_btmesh_evt_prov_initialization_failed_id: {
      // Device failed to initialize in provisioner mode
      sl_btmesh_evt_prov_initialization_failed_t *initialization_failed_evt;
      initialization_failed_evt = (sl_btmesh_evt_prov_initialization_failed_t *)&(evt->data);
      uint16_t res = initialization_failed_evt->result;
      if (0 != res) {
        app_log_status_error_f(res, "Failed to initialize provisioning node" APP_LOG_NEW_LINE);
      }
      break;
    }
    case sl_btmesh_evt_prov_ddb_list_id:
      // DDB List event
      if (NODELIST == command) {
        // If nodelist is requested, print the information
        // This event can be fired from elsewhere, no logs needed in that case
        sl_btmesh_evt_prov_ddb_list_t *ddb_list_evt;
        ddb_list_evt = (sl_btmesh_evt_prov_ddb_list_t *)&(evt->data);
        uuid_128 uuid = ddb_list_evt->uuid;
        uint16_t address = ddb_list_evt->address;
        uint8_t elements = ddb_list_evt->elements;
        app_log_info(APP_LOG_NEW_LINE);
        app_log_info("Address: 0x%04x" APP_LOG_NEW_LINE, address);
        app_log_info("Element count: %d" APP_LOG_NEW_LINE, elements);
        app_log_info("UUID:    %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x" APP_LOG_NEW_LINE,
                     uuid.data[0],
                     uuid.data[1],
                     uuid.data[2],
                     uuid.data[3],
                     uuid.data[4],
                     uuid.data[5],
                     uuid.data[6],
                     uuid.data[7],
                     uuid.data[8],
                     uuid.data[9],
                     uuid.data[10],
                     uuid.data[11],
                     uuid.data[12],
                     uuid.data[13],
                     uuid.data[14],
                     uuid.data[15]);
      }
      break;
    case sl_btmesh_evt_node_reset_id:
      // Node reset successful
      app_log_info("Reset system" APP_LOG_NEW_LINE);
      sl_bt_system_reset(0);
      break;
    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
  // Let other modules handle their events too
  app_ui_on_event(evt);
  btmesh_prov_on_event(evt);
  btmesh_conf_on_event(evt);
}

// -----------------------------------------------------------------------------
// App logic functions

void handle_scan(void)
{
  switch (command_state) {
    case START: {
      sl_status_t sc;
      sc = btmesh_prov_start_scanning();
      if (SL_STATUS_OK != sc) {
        app_log_warning("Failed to start scanning" APP_LOG_NEW_LINE);
      } else {
        app_log_info("Scanning started" APP_LOG_NEW_LINE);
      }
      command_state = IN_PROGRESS;
      // Let the provisioner scan unprovisioned nodes
      sl_simple_timer_start(&scan_timer,       // Timer pointer
                            SCAN_TIMER_MS,     // Timer duration
                            app_on_scan_timer, // Timer callback
                            NULL,              // Timer data, not needed
                            false);            // Not periodic
      break;
    }
    case IN_PROGRESS:
      // Do nothing, timer callback will trigger next state
      break;
    case FINISHED: {
      sl_status_t sc;
      sc = btmesh_prov_stop_scanning();
      if (SL_STATUS_OK != sc) {
        app_log_warning("Failed to stop scanning" APP_LOG_NEW_LINE);
      } else {
        app_log_info("Scanning stopped" APP_LOG_NEW_LINE);
      }
      exit(EXIT_SUCCESS);
      break;
    }
    default:
      handle_ui_scan();
      break;
  }
}

void handle_provision(void)
{
  switch (command_state) {
    case START:
      app_log_info("Provisioning..." APP_LOG_NEW_LINE);
      bd_addr mac = { 0 };
      uint16_t netkey_index = APP_NETKEY_IDX;
      // Try to create a network with index 0.
      // If one is already present, SL_STATUS_BT_MESH_ALREADY_EXISTS is handled
      sl_status_t sc = btmesh_prov_create_network(netkey_index, 0, 0);
      app_assert((sc == SL_STATUS_OK || sc == SL_STATUS_BT_MESH_ALREADY_EXISTS),
                 "Failed to create network" APP_LOG_NEW_LINE);
      if (SL_STATUS_OK == sc) {
        uint8_t appkey_data[16];
        size_t appkey_length;
        // If a new network is created then application key is created as
        // well because the appkeys are bound to network keys.
        // If the network already exists then it is not necessary to create
        // appkey because it has already been created.
        // Note: Output buffer is mandatory for create appkey API function
        sl_status_t sc = btmesh_prov_create_appkey(netkey_index,
                                                   APP_CONF_APPKEY_INDEX,
                                                   0,
                                                   NULL,
                                                   sizeof(appkey_data),
                                                   &appkey_length,
                                                   &appkey_data[0]);
        app_assert_status_f(sc, "Failed to create appkey" APP_LOG_NEW_LINE);
      }
      // MAC address is unknown here, but the database requires a bd_addr struct
      // so we use a 0 here.
      // Note: this won't affect provisioning as only UUID is used there
      sc = btmesh_prov_start_provisioning(netkey_index, command_uuid, mac, 0, 0);
      app_assert_status_f(sc, "Provisioning failed" APP_LOG_NEW_LINE);
      command_state = IN_PROGRESS;
      break;
    case IN_PROGRESS:
      // Wait for provision to end. The btmesh_prov module informs the
      // application on finish.
      break;
    case FINISHED:
      app_log_info("Provisioning finished" APP_LOG_NEW_LINE);
      command_state = INIT;
      exit(EXIT_SUCCESS);
      break;
    default:
      handle_ui_provision();
      break;
  }
}
void handle_nodelist(void)
{
  switch (command_state) {
    case START:
      // Check if any networks are present on the node on startup
      if (0 < networks_on_startup) {
        sl_status_t sc;
        app_log_info("Querying DDB list" APP_LOG_NEW_LINE);
        sc = btmesh_prov_list_ddb_entries(&ddb_count);
        if (SL_STATUS_OK != sc) {
          app_log_error("Failed to list DDB entries" APP_LOG_NEW_LINE);
          command_state = FINISHED;
          break;
        }
        if (ddb_count == 0) {
          // The count is synchronous, but individual nodes' info are sent
          // via sl_btmesh_on_event
          app_log_info("No nodes present in the network" APP_LOG_NEW_LINE);
          command_state = FINISHED;
        } else {
          command_state = IN_PROGRESS;
        }
      } else {
        app_log_info("No networks present on the device" APP_LOG_NEW_LINE);
        command_state = FINISHED;
      }
      break;
    case IN_PROGRESS:
      // Wait for all nodes' information
      break;
    case FINISHED:
      exit(EXIT_SUCCESS);
      break;
    default:
      handle_ui_nodelist();
      break;
  }
}

void handle_nodeinfo(void)
{
  switch (command_state) {
    case START:
      // Nodeinfo is started automatically after startup DDB list query
      command_state = IN_PROGRESS;
      break;
    case IN_PROGRESS:
      // Wait for configurator to finish nodeinfo printing
      break;
    case FINISHED:
      command_state = INIT;
      exit(EXIT_SUCCESS);
      break;
    default:
      handle_ui_nodeinfo();
      break;
  }
}

void handle_remove(void)
{
  switch (command_state) {
    case START:
      // DDB info is queried on startup
      command_state = IN_PROGRESS;
      break;
    case IN_PROGRESS:
      // Wait for confirmation callback
      break;
    case FINISHED:
      command_state = INIT;
      exit(EXIT_SUCCESS);
      break;
    default:
      handle_ui_remove();
      break;
  }
}

void handle_reset(void)
{
  switch (command_state) {
    case START:
      app_log_info("Initiating node reset" APP_LOG_NEW_LINE);
      sl_btmesh_node_reset();
      // Timer to let the NVM clear properly
      sl_simple_timer_start(&reset_timer,       // Timer pointer
                            RESET_TIMER_MS,     // Timer duration
                            app_on_reset_timer, // Timer callback
                            NULL,               // Timer data, not needed
                            false);             // Not periodic
      command_state = IN_PROGRESS;
      break;
    case IN_PROGRESS:
      // Do nothing, timer callback will trigger next state
      break;
    case FINISHED:
      app_log_info("Resetting hardware" APP_LOG_NEW_LINE);
      // Reset the hardware
      sl_bt_system_reset(0);
      exit(EXIT_SUCCESS);
      break;
    default:
      handle_ui_reset();
      break;
  }
}

void app_parse_uuid(char *input, size_t length, uuid_128 *parsed_uuid)
{
  // Sanity check, do nothing if length is incorrect
  if (UUID_LEN_WITHOUT_SEPARATORS == length
      || UUID_LEN_WITH_SEPARATORS == length) {
    for (size_t count = 0; count < 16; count++) {
      sscanf(input, "%2hhx", &parsed_uuid->data[count]);
      // If data is in AA:BB:CC format move the pointer by 3 bytes and
      // skip the separator, otherwise move by 2 bytes
      input += (UUID_LEN_WITHOUT_SEPARATORS == length ? 2 : 3);
    }
  }
}

void app_parse_address(char *input, size_t length, uint16_t *address)
{
  // Sanity check, do nothing if length is incorrect
  if (ADDRESS_LEN_WITHOUT_PREFIX == length
      || ADDRESS_LEN_WITH_PREFIX == length) {
    if (ADDRESS_LEN_WITH_PREFIX == length) {
      // Discard prefix if address is in 0x1234 format
      input += 2;
    }
    sscanf(input, "%4hx", address);
  }
}

sl_status_t app_add_user_event_filter(const uint32_t event_id)
{
  sl_status_t sc = SL_STATUS_OK;
  uint8_t user_data[SL_NCP_EVT_FILTER_CMD_ADD_LEN];

  user_data[0] = SL_NCP_EVT_FILTER_CMD_ADD_ID;
  user_data[1] = event_id >> 0;
  user_data[2] = event_id >> 8;
  user_data[3] = event_id >> 16;
  user_data[4] = event_id >> 24;

  sc = sl_bt_user_manage_event_filter(SL_NCP_EVT_FILTER_CMD_ADD_LEN, user_data);
  return sc;
}

// -----------------------------------------------------------------------------
// Callbacks

void app_on_nodeinfo_end(void)
{
  if (command_state < UI_START) {
    command_state = FINISHED;
  }
}

void btmesh_prov_on_node_found_evt(uint8_t bearer,
                                   uuid_128 uuid,
                                   int8_t rssi)
{
  app_log_info("Bearer:  %s" APP_LOG_NEW_LINE, bearer == 1 ? "PB_GATT" : "PB_ADV");
  app_log_info("UUID:    %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x" APP_LOG_NEW_LINE,
               uuid.data[0],
               uuid.data[1],
               uuid.data[2],
               uuid.data[3],
               uuid.data[4],
               uuid.data[5],
               uuid.data[6],
               uuid.data[7],
               uuid.data[8],
               uuid.data[9],
               uuid.data[10],
               uuid.data[11],
               uuid.data[12],
               uuid.data[13],
               uuid.data[14],
               uuid.data[15]);
  app_log_info("RSSI:    %d" APP_LOG_NEW_LINE, rssi);
}

void btmesh_prov_on_device_provisioned_evt(uint16_t address, uuid_128 uuid)
{
  app_log_info("Device provisioned" APP_LOG_NEW_LINE);
  app_log_info("UUID:    %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x" APP_LOG_NEW_LINE,
               uuid.data[0],
               uuid.data[1],
               uuid.data[2],
               uuid.data[3],
               uuid.data[4],
               uuid.data[5],
               uuid.data[6],
               uuid.data[7],
               uuid.data[8],
               uuid.data[9],
               uuid.data[10],
               uuid.data[11],
               uuid.data[12],
               uuid.data[13],
               uuid.data[14],
               uuid.data[15]);
  app_log_info("Address: 0x%04x" APP_LOG_NEW_LINE, address);
  if (UI_START > command_state) {
    // If oneshot mode is used (no UI) then the configuration shall be started
    // here otherwise the configuration is started in the UI event handler
    sl_status_t sc = app_conf_start_node_configuration(APP_NETKEY_IDX,
                                                       address);
    if (SL_STATUS_OK != sc) {
      app_log_status_error_f(sc, "Failed to start configuration procedure." APP_LOG_NEW_LINE);
      command_state = FINISHED;
    }
  }
}

void app_on_node_configuration_end(uint16_t netkey_index,
                                   uint16_t server_address)
{
  if (UI_START > command_state) {
    command_state = FINISHED;
  }
}

void btmesh_prov_on_provision_failed_evt(uint8_t reason, uuid_128 uuid)
{
  if (UI_START > command_state) {
    command_state = FINISHED;
  } else {
    btmesh_prov_on_provision_failed_evt_ui();
  }
}

void btmesh_prov_on_ddb_list_ready(uint16_t count)
{
  if (0 == count) {
    app_log_debug("No nodes in DDB" APP_LOG_NEW_LINE);
  } else {
    if (NODELIST == command) {
      // DDB list requested by user
      app_log_debug("All nodes in DDB listed" APP_LOG_NEW_LINE);
      // Finish only if not requested by UI mode
      if (UI_START > command_state) {
        command_state = FINISHED;
      } else {
        btmesh_prov_on_ddb_list_ready_ui();
      }
    } else if (REMOVE_NODE == command) {
      // Node removal can only start after the database is ready
      sl_status_t sc = btmesh_prov_remove_node_by_uuid(command_uuid,
                                                       app_on_remove_node_job_status);
      if (SL_STATUS_OK != sc) {
        command_state = FINISHED;
      } else {
        command_state = IN_PROGRESS;
      }
    } else if (NODEINFO == command) {
      // Node information can only be obtained after the database is ready
      app_log_info("Querying node information" APP_LOG_NEW_LINE);
      command_state = IN_PROGRESS;
      app_conf_print_nodeinfo_by_uuid(command_uuid);
    }
  }
}

// -----------------------------------------------------------------------------
// Callbacks

void app_on_remove_node_job_status(const btmesh_conf_job_t *job)
{
  if (BTMESH_CONF_JOB_RESULT_SUCCESS == job->result) {
    app_log_info("Node removed from network" APP_LOG_NEW_LINE);
    btmesh_prov_delete_ddb_entry(command_uuid);
  } else {
    app_log_error("Could not remove node from network" APP_LOG_NEW_LINE);
  }
  command_state = FINISHED;
}

static void app_on_scan_timer(sl_simple_timer_t *timer, void *data)
{
  command_state = FINISHED;
}

static void app_on_reset_timer(sl_simple_timer_t *timer, void *data)
{
  command_state = FINISHED;
}

// -----------------------------------------------------------------------------
// Helper functions
void set_command_and_state(command_t new_command, command_state_t new_state)
{
  command_state = new_state;
  command = new_command;
}
