/***************************************************************************//**
* @file
* @brief BT Mesh Host Provisioner Example Project - UI mode handler.
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

#include "app.h"
#include "app_conf.h"
#include "app_assert.h"
#include "app_log.h"
#include "app_sleep.h"
#include "btmesh_prov.h"
#include "sl_simple_timer.h"
#include "sl_bt_api.h"
#include "sl_btmesh_api.h"

#include <pthread.h>
#include <string.h>

// -----------------------------------------------------------------------------
// Macros

#if defined(POSIX) && POSIX == 1
  #define CLEARSCR "clear"
  #include <stdio.h>
  #include <sys/select.h>
  #include <unistd.h>
#else
  #define CLEARSCR "cls"
  #include <windows.h>
  #include <conio.h>
#endif // defined(POSIX) && POSIX == 1

/// Buffer length for user input
#define INPUT_BUFFER_LEN 100
/// Time to sleep between user input checking in microseconds
#define INPUT_SLEEP_US 10000

// -----------------------------------------------------------------------------
// Enums

/// Current UI state, either processing something or waiting for user input
static enum {
  WAITING_FOR_INPUT,
  PROCESSING
} ui_state = WAITING_FOR_INPUT;

// -----------------------------------------------------------------------------
// Static Function Declarations

/***************************************************************************//**
* Node removal job status callback
*
* @param[in] job The job this function is called from
*******************************************************************************/
static void remove_node_job_status_ui(const btmesh_conf_job_t *job);

/***************************************************************************//**
* Set the next command and its state.
*
* @param[in] new_command Next command
* @param[in] new_state Next state
*
* This function sets command and command_state in app.c as well.
*******************************************************************************/
static void set_command_and_state_ui(command_t new_command, command_state_t new_state);

/***************************************************************************//**
* Helper script to print a given UUID
*
* @param[in] uuid Pointer to the UUID
*******************************************************************************/
static void print_uuid(uuid_128 *uuid);

/***************************************************************************//**
* The user input handler called in a new thread
*
* @param[in] ptr Unused
*******************************************************************************/
static void *handle_input(void *ptr);

/***************************************************************************//**
* Peek the standard input
*
* @param[out] buf Character array to save the input to
* @param[in] buffer_length Length of buffer
* @param[out] num_read Actual number of characters read
* @return Status of the peek.
* @retval true If user input is ready
* @retval false If user input is still being processed
*******************************************************************************/
static bool peek_stdin(char *buf, int buffer_length, int *num_read);

/***************************************************************************//**
* Callback for the timer used during scanning
*
* @param[in] timer Pointer to the timer used
* @param[in] data Data from the timer
*******************************************************************************/
static void on_scan_timer(sl_simple_timer_t *timer, void *data);

/***************************************************************************//**
* Callback for the timer used during provisioner reset
*
* @param[in] timer Pointer to the timer used
* @param[in] data Data from the timer
*******************************************************************************/
static void on_reset_timer(sl_simple_timer_t *timer, void *data);

// -----------------------------------------------------------------------------
// Static Variables

/// The state of the current command
static command_state_t command_state = UI_START;
/// The command in use
static command_t command = NONE;
/// Flag indicating the user input thread has to run
/// and check for data
static volatile bool run = true;
/// Flag indicating the user input is ready to be processed
static volatile bool input_ready = false;
/// User input handler thread
static pthread_t thread_input;
/// Buffer for user input
static char input_buffer[INPUT_BUFFER_LEN];
/// Number of networks already present on the provisioner at startup
static uint16_t networks_on_startup;
/// The UUID to be used throughout node removal process
static uuid_128 remove_uuid;
/// Timer for scanning for unprovisioned devices
static sl_simple_timer_t scan_timer;
/// Timer for provisioner node reset
static sl_simple_timer_t reset_timer;

// -----------------------------------------------------------------------------
// Function definitions

void app_ui_on_event(sl_btmesh_msg_t *evt)
{
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_prov_device_provisioned_id: {
      app_log("Provisioning finished" APP_LOG_NEW_LINE);
      sl_btmesh_evt_prov_device_provisioned_t *prov_evt =
        &evt->data.evt_prov_device_provisioned;
      if (PROVISION == command) {
        sl_status_t sc = app_conf_start_node_configuration(APP_NETKEY_IDX,
                                                           prov_evt->address);
        if (SL_STATUS_OK != sc) {
          app_log_status_error_f(sc, "Failed to start configuration procedure." APP_LOG_NEW_LINE);
          command_state = UI_FINISHED;
        }
      } else {
        command_state = UI_FINISHED;
      }
      break;
    }
    case sl_btmesh_evt_prov_provisioning_failed_id:
      app_log("Provisioning failed" APP_LOG_NEW_LINE);
      command_state = UI_FINISHED;
      break;
    case sl_btmesh_evt_prov_initialized_id: {
      sl_btmesh_evt_prov_initialized_t *initialized_evt;
      initialized_evt = (sl_btmesh_evt_prov_initialized_t *)&(evt->data);
      networks_on_startup = initialized_evt->networks;
      break;
    }
    case sl_btmesh_evt_prov_ddb_list_id: {
      if (NONE != command) {
        sl_btmesh_evt_prov_ddb_list_t *ddb_list_evt;
        ddb_list_evt = (sl_btmesh_evt_prov_ddb_list_t *)&(evt->data);
        uuid_128 uuid = ddb_list_evt->uuid;
        uint16_t address = ddb_list_evt->address;
        uint8_t elements = ddb_list_evt->elements;
        app_log("Address: 0x%04x" APP_LOG_NEW_LINE, address);
        app_log("Element count: %d" APP_LOG_NEW_LINE, elements);
        print_uuid(&uuid);
        app_log_nl();
      }
      break;
    }
  }
}

void handle_ui(void)
{
  switch (command_state) {
    case UI_START: {
      system(CLEARSCR);
      app_log("Please select a functionality" APP_LOG_NEW_LINE);
      app_log_nl();
      app_log("1. Scan available nodes" APP_LOG_NEW_LINE);
      app_log("2. Provision a beaconing node" APP_LOG_NEW_LINE);
      app_log("3. List nodes in the network" APP_LOG_NEW_LINE);
      app_log("4. Information about a node in the network" APP_LOG_NEW_LINE);
      app_log("5. Remove node from the network" APP_LOG_NEW_LINE);
      app_log_nl();
      app_log("9. Reset provisioner node" APP_LOG_NEW_LINE);
      app_log("0. Exit application" APP_LOG_NEW_LINE);
      app_log_nl();
      input_ready = false;
      ui_state = WAITING_FOR_INPUT;
      if (0 == thread_input) {
        int ret = pthread_create(&thread_input, NULL, handle_input, NULL);
        if (0 != ret) {
          app_log_critical("Failed to create input thread!" APP_LOG_NL);
          exit(EXIT_FAILURE);
        }
      }
      command_state = UI_IN_PROGRESS;
      break;
    }
    case UI_IN_PROGRESS:
      if (true == input_ready) {
        char c = input_buffer[0];
        switch (c) {
          case '1':
            set_command_and_state_ui(SCAN, UI_START);
            break;
          case '2':
            set_command_and_state_ui(PROVISION, UI_START);
            break;
          case '3':
            set_command_and_state_ui(NODELIST, UI_START);
            break;
          case '4':
            set_command_and_state_ui(NODEINFO, UI_START);
            break;
          case '5':
            set_command_and_state_ui(REMOVE_NODE, UI_START);
            break;
          case '9':
            set_command_and_state_ui(RESET, UI_START);
            break;
          case '0':
            command_state = UI_FINISHED;
            run = false;
            break;
          default:
            break;
        }
        input_ready = false;
        ui_state = PROCESSING;
      }
      break;
    case UI_FINISHED:
      run = false;
      pthread_join(thread_input, NULL);
      exit(EXIT_SUCCESS);
      break;
    default:
      break;
  }
}

void handle_ui_scan(void)
{
  switch (command_state) {
    case UI_START: {
      app_log_nl();
      sl_status_t sc;
      sc = btmesh_prov_start_scanning();
      if (SL_STATUS_OK != sc) {
        app_log_warning("Failed to start scanning" APP_LOG_NEW_LINE);
      } else {
        app_log("Scanning started" APP_LOG_NEW_LINE);
      }
      // Let the provisioner scan unprovisioned nodes
      sl_simple_timer_start(&scan_timer,          // Timer pointer
                            SCAN_TIMER_MS,        // Timer duration
                            on_scan_timer,        // Timer callback
                            NULL,                 // Timer data, not needed
                            false);               // Not periodic
      command_state = UI_IN_PROGRESS;
      ui_state = PROCESSING;
      break;
    }
    case UI_IN_PROGRESS:
      // Do nothing, timer callback will trigger next state
      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        sl_status_t sc;
        sc = btmesh_prov_stop_scanning();
        if (SL_STATUS_OK != sc) {
          app_log_warning("Failed to stop scanning" APP_LOG_NEW_LINE);
        } else {
          app_log("Scanning finished" APP_LOG_NEW_LINE);
        }
        sc = btmesh_prov_list_unprovisioned_nodes();
        if (SL_STATUS_EMPTY == sc) {
          app_log("No unprovisioned beaconing nodes were found" APP_LOG_NEW_LINE);
        }
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

void handle_ui_provision(void)
{
  switch (command_state) {
    case UI_START:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Select a node to provision" APP_LOG_NEW_LINE);
        app_log("Type either the ID or UUID from the list below" APP_LOG_NEW_LINE);
        app_log_nl();
        sl_status_t sc = btmesh_prov_list_unprovisioned_nodes();
        if (SL_STATUS_EMPTY == sc) {
          app_log("No unprovisioned nodes available" APP_LOG_NEW_LINE);
          app_log("Try scanning for unprovisioned nodes first" APP_LOG_NEW_LINE);
          command_state = UI_FINISHED;
        } else {
          input_ready = false;
          ui_state = WAITING_FOR_INPUT;
        }
      } else {
        if (true == input_ready) {
          uint16_t netkey_index = APP_NETKEY_IDX;
          sl_status_t sc = btmesh_prov_create_network(netkey_index, 0, 0);
          app_assert((sc == SL_STATUS_OK || sc == SL_STATUS_BT_MESH_ALREADY_EXISTS),
                     "Failed to create network" APP_LOG_NEW_LINE);
          if (SL_STATUS_OK == sc) {
            networks_on_startup++;
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
          size_t len = strlen(input_buffer);
          if (ADDRESS_LEN_WITHOUT_PREFIX > len) {
            uint16_t id = (uint16_t)atoi(input_buffer);
            sc = btmesh_prov_start_provisioning_by_id(netkey_index, id, 0);
          } else if (UUID_LEN_WITHOUT_SEPARATORS == len
                     || UUID_LEN_WITH_SEPARATORS == len) {
            uuid_128 uuid;
            app_parse_uuid(input_buffer, len, &uuid);
            sc = btmesh_prov_start_provisioning_by_uuid(netkey_index, uuid, 0);
          } else {
            app_log_error("Invalid input format!" APP_LOG_NEW_LINE);
            command_state = UI_FINISHED;
            ui_state = PROCESSING;
            break;
          }
          if (SL_STATUS_OK != sc) {
            app_log_error("Provisioning failed" APP_LOG_NEW_LINE);
            command_state = UI_FINISHED;
            ui_state = PROCESSING;
          } else {
            app_log("Starting provisioning..." APP_LOG_NEW_LINE);
            command_state = UI_IN_PROGRESS;
            ui_state = PROCESSING;
          }
        }
      }
      break;
    case UI_IN_PROGRESS:

      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

void app_ui_on_node_configuration_end(uint16_t netkey_index,
                                      uint16_t server_address)
{
  command_state = UI_FINISHED;
}

void handle_ui_nodelist(void)
{
  switch (command_state) {
    case UI_START:
      if (0 < networks_on_startup) {
        app_log("Getting node list..." APP_LOG_NEW_LINE);
        app_log_nl();
        sl_status_t sc = btmesh_prov_list_provisioned_nodes();
        if (SL_STATUS_EMPTY == sc) {
          app_log("No nodes present in the network" APP_LOG_NEW_LINE);
        }
      } else {
        app_log("No networks present on the device" APP_LOG_NEW_LINE);
      }
      command_state = UI_FINISHED;
      break;
    case UI_IN_PROGRESS:

      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

void handle_ui_nodeinfo(void)
{
  switch (command_state) {
    case UI_START:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Select a node" APP_LOG_NEW_LINE);
        app_log("Type either the ID, UUID or address from the list below" APP_LOG_NEW_LINE);
        app_log_nl();
        sl_status_t sc = btmesh_prov_list_provisioned_nodes();
        if (SL_STATUS_EMPTY == sc) {
          app_log("No provisioned nodes available" APP_LOG_NEW_LINE);
          command_state = UI_FINISHED;
        } else {
          input_ready = false;
          ui_state = WAITING_FOR_INPUT;
        }
      } else {
        if (true == input_ready) {
          command_state = UI_IN_PROGRESS;
          ui_state = PROCESSING;
          size_t len = strlen(input_buffer);
          if (ADDRESS_LEN_WITHOUT_PREFIX > len) {
            uint16_t id = (uint16_t)atoi(input_buffer);
            app_conf_print_nodeinfo_by_id(id);
          } else if (ADDRESS_LEN_WITHOUT_PREFIX == len
                     || ADDRESS_LEN_WITH_PREFIX == len) {
            uint16_t addr;
            app_parse_address(input_buffer, len, &addr);
            app_conf_print_nodeinfo_by_addr(addr);
          } else if (UUID_LEN_WITHOUT_SEPARATORS == len
                     || UUID_LEN_WITH_SEPARATORS == len) {
            uuid_128 uuid;
            app_parse_uuid(input_buffer, len, &uuid);
            app_conf_print_nodeinfo_by_uuid(uuid);
          } else {
            app_log_error("Invalid input format!" APP_LOG_NEW_LINE);
            input_ready = false;
            ui_state = WAITING_FOR_INPUT;
            command_state = UI_START;
          }
        }
      }
      break;
    case UI_IN_PROGRESS:
      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("All nodes listed" APP_LOG_NEW_LINE);
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

void app_ui_on_nodeinfo_end(void)
{
  command_state = UI_FINISHED;
}

void handle_ui_remove(void)
{
  switch (command_state) {
    case UI_START:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Select a node to unprovision" APP_LOG_NEW_LINE);
        app_log("Type either the ID, UUID or address from the list below" APP_LOG_NEW_LINE);
        app_log_nl();
        sl_status_t sc = btmesh_prov_list_provisioned_nodes();
        if (SL_STATUS_EMPTY == sc) {
          app_log("No provisioned nodes available" APP_LOG_NEW_LINE);
          command_state = UI_FINISHED;
        } else {
          input_ready = false;
          ui_state = WAITING_FOR_INPUT;
        }
      } else {
        if (true == input_ready) {
          sl_status_t sc;
          size_t len = strlen(input_buffer);
          if (ADDRESS_LEN_WITHOUT_PREFIX > len) {
            uint16_t id = (uint16_t)atoi(input_buffer);
            sc = btmesh_prov_remove_node_by_id(id, remove_node_job_status_ui, &remove_uuid);
          } else if (ADDRESS_LEN_WITHOUT_PREFIX == len
                     || ADDRESS_LEN_WITH_PREFIX == len) {
            uint16_t addr;
            app_parse_address(input_buffer, len, &addr);
            sc = btmesh_prov_remove_node_by_address(addr, remove_node_job_status_ui, &remove_uuid);
          } else if (UUID_LEN_WITHOUT_SEPARATORS == len
                     || UUID_LEN_WITH_SEPARATORS == len) {
            uuid_128 uuid;
            app_parse_uuid(input_buffer, len, &uuid);
            sc = btmesh_prov_remove_node_by_uuid(remove_uuid, remove_node_job_status_ui);
          } else {
            app_log_error("Invalid input format!" APP_LOG_NEW_LINE);
            input_ready = false;
            ui_state = WAITING_FOR_INPUT;
            break;
          }
          app_assert_status_f(sc, "Provisioning failed" APP_LOG_NEW_LINE);
          command_state = UI_IN_PROGRESS;
          ui_state = PROCESSING;
        }
      }
      break;
    case UI_IN_PROGRESS:
      // Wait for confirmation callback
      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

void handle_ui_reset(void)
{
  switch (command_state) {
    case UI_START:
      app_log("Initiating node reset" APP_LOG_NEW_LINE);
      app_log_nl();
      sl_btmesh_node_reset();
      // Timer to let the NVM clear properly
      sl_simple_timer_start(&reset_timer,         // Timer pointer
                            RESET_TIMER_MS,       // Timer duration
                            on_reset_timer,       // Timer callback
                            NULL,                 // Timer data, not needed
                            false);               // Not periodic
      command_state = UI_IN_PROGRESS;
      break;
    case UI_IN_PROGRESS:
      // Do nothing, timer callback will trigger next state
      break;
    case UI_FINISHED:
      if (WAITING_FOR_INPUT != ui_state) {
        app_log("Provisioner reset completed" APP_LOG_NEW_LINE);
        app_log("Press enter to continue..." APP_LOG_NEW_LINE);
        input_ready = false;
        ui_state = WAITING_FOR_INPUT;
      } else {
        if (true == input_ready) {
          set_command_and_state_ui(NONE, UI_START);
        }
      }
      break;
    default:
      break;
  }
}

// Handle user input
void *handle_input(void *ptr)
{
  int num_read = 0;
  while (run) {
    if (false == input_ready && WAITING_FOR_INPUT == ui_state) {
      if (peek_stdin(input_buffer, sizeof(input_buffer), &num_read)) {
        input_buffer[num_read - 1] = '\0';
        app_log_debug("Command received: %s" APP_LOG_NEW_LINE, input_buffer);
        app_log_debug("Length: %d" APP_LOG_NEW_LINE, num_read);
        input_ready = true;
      }
    }
    app_sleep_us(INPUT_SLEEP_US);
  }
  return 0;
}

static bool peek_stdin(char *buf, int buffer_length, int *num_read)
{
#if defined(POSIX) && POSIX == 1
  // Setup an instant non-blocking peek
  fd_set rfds;
  struct timeval tv = { .tv_sec = 0, .tv_usec = 0 };
  FD_ZERO(&rfds);
  FD_SET(0, &rfds);
  // Peek standard input
  int retval = select(1, &rfds, NULL, NULL, &tv);
  if (-1 == retval) {
    // Some kind of error happened
    app_log_error("Can't check stdin" APP_LOG_NEW_LINE);
    return false;
  } else if (0 == retval) {
    // No input data available
    return false;
  } else {
    // Input data available
    *num_read = read(STDIN_FILENO, buf, buffer_length);
    if (-1 == *num_read) {
      app_log_error("Can't read from stdin" APP_LOG_NEW_LINE);
    }
    return true;
  }
#else
  static int current_length = 0;
  if (_kbhit()) {
    // Get the last character while echoing it back
    buf[current_length] = _getche();
    current_length++;
    *num_read = current_length;
    if (13 == buf[current_length - 1]) { // Enter pressed
      // Fake the newline char to behave like read()
      buf[current_length - 1] = '\n';
      current_length = 0;
      // Write a newline so it behaves like POSIX
      app_log_nl();
      return true;
    } else if (8 == buf[current_length - 1]) { // Backspace
      if (0 != current_length) {
        current_length--;
      }
      return false;
    } else if (current_length == buffer_length) { // Buffer length reached
      buf[buffer_length - 1] = '\n';
      current_length = 0;
      // As a full buffer is considered as a finished command, add a newline here as well
      app_log_nl();
      return true;
    } else {
      return false;
    }
  }
  return false;
#endif // defined(POSIX) && POSIX == 1
}

void btmesh_prov_on_unprovisioned_node_list_evt(uint16_t id,
                                                uuid_128 uuid)
{
  app_log("Unprovisioned node" APP_LOG_NEW_LINE);
  app_log("ID:      %d" APP_LOG_NEW_LINE, id);
  print_uuid(&uuid);
  app_log_nl();
}

void btmesh_prov_on_provisioned_node_list_evt(uint16_t id,
                                              uuid_128 uuid,
                                              uint16_t primary_address)
{
  app_log("Provisioned node" APP_LOG_NEW_LINE);
  app_log("ID:      %d" APP_LOG_NEW_LINE, id);
  print_uuid(&uuid);
  app_log("Address: 0x%04x" APP_LOG_NEW_LINE, primary_address);
  app_log_nl();
}

void print_uuid(uuid_128 *uuid)
{
  app_log("UUID:    %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x" APP_LOG_NEW_LINE,
          uuid->data[0],
          uuid->data[1],
          uuid->data[2],
          uuid->data[3],
          uuid->data[4],
          uuid->data[5],
          uuid->data[6],
          uuid->data[7],
          uuid->data[8],
          uuid->data[9],
          uuid->data[10],
          uuid->data[11],
          uuid->data[12],
          uuid->data[13],
          uuid->data[14],
          uuid->data[15]);
}

static void set_command_and_state_ui(command_t new_command, command_state_t new_state)
{
  command = new_command;
  command_state = new_state;
  set_command_and_state(new_command, new_state);
}

void remove_node_job_status_ui(const btmesh_conf_job_t *job)
{
  if (BTMESH_CONF_JOB_RESULT_SUCCESS == job->result) {
    app_log("Node removed from network" APP_LOG_NEW_LINE);
    sl_status_t sc = btmesh_prov_delete_ddb_entry(remove_uuid);
    app_log_status_warning_f(sc, "Failed to delete DDB entry" APP_LOG_NEW_LINE);
  } else {
    app_log_error("Could not remove node from network" APP_LOG_NEW_LINE);
  }
  command_state = UI_FINISHED;
}

void btmesh_prov_on_ddb_list_ready_ui(void)
{
  command_state = UI_FINISHED;
}

// -----------------------------------------------------------------------------
// Callbacks

void btmesh_prov_on_provision_failed_evt_ui(void)
{
  command_state = UI_FINISHED;
}

static void on_scan_timer(sl_simple_timer_t *timer, void *data)
{
  command_state = UI_FINISHED;
}

static void on_reset_timer(sl_simple_timer_t *timer, void *data)
{
  command_state = UI_FINISHED;
}
