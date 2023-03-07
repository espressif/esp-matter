/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "em_chip.h"
#include "app_log.h"
#include "ember.h"
#include "poll.h"
#include "app_process.h"
#include "app_framework_common.h"
#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_simple_led_instances.h"
#endif
#include "sl_simple_button_instances.h"
#include "stack-info.h"
#include "sl_light_switch.h"
#include "em_system.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_component_catalog.h"
#include "sl_power_manager.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Handle the tasks in relation with connecting to a network
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void handle_connection_to_a_network(void);

/**************************************************************************//**
 * Send "light toggle" command to the Light node
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void toggle_light(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
///This structure contains all the flags used in the state machine
switch_application_flags_t state_machine_flags = {
  .error_detected = false,
  .init_success   = false,
  .join_request   = false,
  .joined_network = false,
  .leave_request  = false
};
/// report timing period
uint16_t switch_report_period_ms =  (1 * MILLISECOND_TICKS_PER_SECOND);
/// TX options set up for the network
volatile EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED;
/// In the starting state, the switch tries to connect to a network
light_switch_state_machine_t state = S_INIT;
/// Indicates when light toggle action is required
bool light_toggle_required = false;
/// Global flag set by a button push to allow or disallow entering to sleep
bool enable_sleep;
/// The event handler signal of the state machine
EmberEventControl *state_machine_event;
/// Connect security key id
psa_key_id_t security_key_id = 0;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Destination of the currently processed sink node
static EmberNodeId light_node_id = EMBER_COORDINATOR_ADDRESS;
/// Store the Connect's status
static EmberStatus stack_status;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(void)
{
  ///////////////////////////////////////////////////////////////////////////
  // Put your application code here!                                       //
  // This is called infinitely.                                            //
  // Do not call blocking functions from here!                             //
  ///////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * This function handles the main state machine
 *****************************************************************************/
void state_machine_handler(void)
{
  emberEventControlSetInactive(*state_machine_event);
  switch (state) {
    case S_INIT:
      if (state_machine_flags.init_success) {
        state = S_STANDBY;
      } else if (state_machine_flags.error_detected) {
        state = S_ERROR;
      }

      break;
    case S_STANDBY:
      if (state_machine_flags.join_request) {
        state_machine_flags.join_request = false;
        // Start the network connection process
        handle_connection_to_a_network();
        state = S_NETWORK;
      } else if (state_machine_flags.leave_request) {
        // if the node has joined the network already,
        state_machine_flags.leave_request = false;
      } else if (stack_status == EMBER_NETWORK_UP) {
        state = S_OPERATE;
        app_log_info("After powerup, start to operate\n");
      } else if (state_machine_flags.error_detected) {
        state_machine_flags.error_detected = false;
        state = S_ERROR;
      }

      break;
    case S_NETWORK:
      // Wait until the device is manage to connect to the network
      if (state_machine_flags.joined_network) {
        state_machine_flags.joined_network = false;
        state = S_OPERATE;
      } else if (state_machine_flags.leave_request) {
        state_machine_flags.leave_request = false;
        state = S_STANDBY;
      } else if (state_machine_flags.error_detected) {
        state_machine_flags.error_detected = false;
        state = S_ERROR;
      }

      break;
    case S_OPERATE:
      if (light_toggle_required) {
        light_toggle_required = false;
        //send "toggle light" command to the Light node
        toggle_light();
      }
      if (state_machine_flags.leave_request) {
        state_machine_flags.leave_request = false;
        state = S_STANDBY;
      } else if (state_machine_flags.error_detected) {
        state_machine_flags.error_detected = false;
        state = S_ERROR;
      }

      break;
    case S_ERROR:
      app_log_error("Error occurred\n");
      halReboot();
      break;

    default:
      app_log_info("Invalid state\n");
      state = S_ERROR;
      break;
  }

  emberEventControlSetDelayMS(*state_machine_event, STATE_MACHINE_TIMER_MS);
}

/***************************************************************************//**
 * A callback called in interrupt context whenever a button changes its state.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  // Check if any button was pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      enable_sleep = !enable_sleep;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      if (enable_sleep) {
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
      } else {
        sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM2);
        sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
      }
#endif
    }
    if (&sl_button_btn1 == handle && state == S_OPERATE) {
      light_toggle_required = true;
    }
  }
}

/**************************************************************************//**
 * Entering sleep is approved or denied in this callback, depending on user
 * demand.
 *****************************************************************************/
bool emberAfCommonOkToEnterLowPowerCallback(bool enter_em2, uint32_t duration_ms)
{
  (void) enter_em2;
  (void) duration_ms;
  return enable_sleep;
}

/**************************************************************************//**
 * This function is called to indicate whether an outgoing message was
 * successfully transmitted or to indicate the reason of failure.
 *****************************************************************************/
void emberAfMessageSentCallback(EmberStatus status,
                                EmberOutgoingMessage *message)
{
  (void) message;
  if (status != EMBER_SUCCESS) {
    app_log_error("Transmit failed: 0x%02X\n", status);
  }
}

/**************************************************************************//**
 * This function is called when the stack status changes.
 *****************************************************************************/
void emberAfStackStatusCallback(EmberStatus status)
{
  stack_status = status;
  switch (status) {
    case EMBER_NETWORK_UP:
      app_log_info("Network up\n");
      state_machine_flags.joined_network = true;
      break;
    case EMBER_NETWORK_DOWN:
      app_log_info("Network down\n");
      break;
    case EMBER_JOIN_SCAN_FAILED:
      app_log_warning("Scanning during join failed\n");
      state_machine_flags.error_detected = true;
      break;
    case EMBER_JOIN_DENIED:
      app_log_warning("Joining to the network rejected!\n");
      state_machine_flags.error_detected = true;
      break;
    case EMBER_JOIN_TIMEOUT:
      app_log_warning("Join process timed out!\n");
      state_machine_flags.error_detected = true;
      break;
    default:
      app_log_info("Stack status: 0x%02X\n", status);
      break;
  }
}

bool set_security_key(uint8_t* key, size_t key_length)
{
  bool success = false;
  EmberStatus emstatus = EMBER_ERR_FATAL;

  psa_key_attributes_t key_attr;
  psa_status_t status;

  if (security_key_id != 0) {
    psa_destroy_key(security_key_id);
    app_log_info("Previous security key is destroyed.\n");
  }

  key_attr = psa_key_attributes_init();
  psa_set_key_type(&key_attr, PSA_KEY_TYPE_AES);
  psa_set_key_bits(&key_attr, 128);
  psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
  psa_set_key_algorithm(&key_attr, PSA_ALG_ECB_NO_PADDING);
#ifdef PSA_KEY_LOCATION_SLI_SE_OPAQUE
  psa_set_key_lifetime(&key_attr,
                       PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
                         PSA_KEY_LIFETIME_VOLATILE,
                         PSA_KEY_LOCATION_SLI_SE_OPAQUE));
#else
  psa_set_key_lifetime(&key_attr,
                       PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
                         PSA_KEY_LIFETIME_VOLATILE,
                         PSA_KEY_LOCATION_LOCAL_STORAGE));
#endif

  status = psa_import_key(&key_attr,
                          key,
                          key_length,
                          &security_key_id);

  if (status == PSA_SUCCESS) {
    app_log_info("Security key import successful, key id: %lu\n", security_key_id);
  } else {
    app_log_info("Security Key import failed: 0x%02lx\n", status);
  }

  emstatus = emberSetPsaSecurityKey(security_key_id);

  if (emstatus == EMBER_SUCCESS) {
    app_log_info("Security key set successful\n");
    success = true;
  } else {
    app_log_info("Security key set failed 0x%02X\n", emstatus);
  }

  return success;
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Handle the tasks in relation with connecting to a network
 *****************************************************************************/
static void handle_connection_to_a_network()
{
  EmberStatus status;
  EmberNetworkParameters parameters;

  MEMSET(&parameters, 0, sizeof(EmberNetworkParameters));
  parameters.radioTxPower = LIGHT_SWITCH_TX_POWER;
  parameters.radioChannel = sl_get_channel();
  parameters.panId = sl_get_pan_id();
  status = emberJoinNetwork(EMBER_STAR_SLEEPY_END_DEVICE, &parameters);
  if (status == EMBER_SUCCESS) {
    app_log_info("join sleepy to the network on channel: %d, PAN ID: 0x%04X \n", parameters.radioChannel, parameters.panId);
  } else {
    app_log_error("Error during join, error code:0x%02X\n", status);
  }
}

/**************************************************************************//**
 * Send "light toggle" command to the Light node
 *****************************************************************************/
static void toggle_light()
{
  uint8_t buffer[LIGHT_SWITCH_MAXIMUM_DATA_LENGTH] = { 0 };

  uint64_t uid = SYSTEM_GetUnique();

  memcpy(&buffer[1], &uid, sizeof(uid));

  // The first bit indicate the toggle requirement
  buffer[LIGHT_SWITCH_MESSAGE_CONTROL_BYTE] = 0x01;
  EmberStatus status = emberMessageSend(light_node_id,
                                        LIGHT_SWITCH_ENDPOINT, // endpoint
                                        0, // messageTag
                                        LIGHT_SWITCH_MAXIMUM_DATA_LENGTH,
                                        buffer,
                                        tx_options);

  if (status == EMBER_SUCCESS) {
    app_log_info("TX: Data to 0x%04X:\n", light_node_id);
  }
}
