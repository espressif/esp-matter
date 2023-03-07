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
#include "app_framework_common.h"
#include "sl_simple_led_instances.h"
#include "app_process.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "stack-info.h"
#include "sl_light_switch.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define SL_EXPECTED_SWITCH_PAYLOAD_LENGHT_BYTE (9)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Handle the network forming related tasks
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void handle_network_form();

/**************************************************************************//**
 * Process the incoming message
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void process_message();

/*******************************************************************************
 * Send the first indication in the queue
 ******************************************************************************/
extern void sl_send_bluetooth_indications (void);

/*******************************************************************************
 * Notify the connected mobile device about the characteristic changes
 ******************************************************************************/
extern void notify_connected_ble_device(sl_direction_t in_direction, uint8_t* address);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

/// The event handler signal of the state machine
EmberEventControl *state_machine_event;
/// In the starting state, the switch tries to connect to a network
light_switch_state_machine_t state = S_INIT;
/// TX options set up for the network
volatile EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED;
/// Connect security key id
psa_key_id_t security_key_id = 0;
///This structure contains all the flags used in the state machine
light_application_flags_t state_machine_flags = { false };
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Indicates when new message has income
static bool is_there_new_message = false;
/// Store the Connect's status
static EmberStatus stack_status;
/// Data buffer for the incoming messages
static EmberIncomingMessage incoming_message;
/// message from the connected switch device
static uint8_t message_from_connect[SL_EXPECTED_SWITCH_PAYLOAD_LENGHT_BYTE] = { 0 };

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Toggle the light state, and blink the LED on the board
 *****************************************************************************/
void toggle_light_state()
{
  (sl_get_light_state() == DEMO_LIGHT_ON) ? sl_led_led0.turn_off(sl_led_led0.context)
  : sl_led_led0.turn_on(sl_led_led0.context);
  (sl_get_light_state() == DEMO_LIGHT_ON) ? (sl_set_light_state(DEMO_LIGHT_OFF))
  : (sl_set_light_state(DEMO_LIGHT_ON));
}

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
 * This function is called when a message is received.
 *****************************************************************************/
void emberAfIncomingMessageCallback(EmberIncomingMessage *message)
{
  is_there_new_message = true;
  if ((message->endpoint == LIGHT_SWITCH_ENDPOINT) && (message != NULL)) {
    memcpy(&message_from_connect, message->payload, sizeof(message_from_connect));
  }
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
 * This function is called by the application framework from the stack status
 * handler.
 *****************************************************************************/
void emberAfStackStatusCallback(EmberStatus status)
{
  stack_status = status;
  switch (status) {
    case EMBER_NETWORK_UP:
      state_machine_flags.network_formed = true;
      app_log_info("Network up\n");
      break;
    case EMBER_NETWORK_DOWN:
      state_machine_flags.network_formed = false;
      app_log_info("Network down\n");
      break;
    default:
      app_log_info("Stack status: 0x%02X\n", status);
      break;
  }
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
      if (state_machine_flags.form_network_request == true) {
        state_machine_flags.form_network_request = false;
        // Start the network connection process
        handle_network_form();
        state = S_NETWORK;
      } else if (stack_status == EMBER_NETWORK_UP) {
        state = S_OPERATE;
        sl_set_light_state(DEMO_LIGHT_OFF);
        app_log_info("After powerup, start to operate\n");
      } else if (state_machine_flags.error_detected) {
        state_machine_flags.error_detected = false;
        state = S_ERROR;
      }

      break;
    case S_NETWORK:
      // Wait until the device is manage to connect to the network
      if (state_machine_flags.network_formed) {
        state_machine_flags.network_formed = false;
        state = S_OPERATE;
      } else if (state_machine_flags.error_detected) {
        state_machine_flags.error_detected = false;
        state = S_ERROR;
      }

      break;
    case S_OPERATE:
      if (is_there_new_message) {
        is_there_new_message = false;
        //process the incoming message
        process_message();
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
      app_log_warning("Invalid state\n");
      state = S_ERROR;
      break;
  }

  sl_send_bluetooth_indications();
  emberEventControlSetDelayMS(*state_machine_event, 100);
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
// -----------------------------------------------------------------------------s

/**************************************************************************//**
 * Handle the tasks in relation with connecting to a network
 *****************************************************************************/
static void handle_network_form()
{
  EmberStatus status;
  EmberNetworkParameters parameters;

  MEMSET(&parameters, 0, sizeof(EmberNetworkParameters));
  parameters.radioTxPower = LIGHT_SWITCH_TX_POWER;
  parameters.radioChannel = sl_get_channel();
  parameters.panId = sl_get_pan_id();
  status = emberFormNetwork(&parameters);
  if (status == EMBER_SUCCESS) {
    app_log_info("network formed, radio_channel: %d PAN ID: 0x%04X\n", parameters.radioChannel, parameters.panId);
  } else {
    app_log_error("network form unsuccessful, error code: 0x%02X\n", status);
  }
  emberPermitJoining(UNLIMETED_CONNECTION_TIME);
}

/**************************************************************************//**
 * Process the incoming message
 *****************************************************************************/
static void process_message()
{
  if (message_from_connect[LIGHT_SWITCH_MESSAGE_CONTROL_BYTE] & 0x01) {
    toggle_light_state();
    uint8_t switch_endianness_switch_id[8] = { 0 };

    // change the endianness of the received switch connect id
    for (uint8_t i = 0; i < (SL_EXPECTED_SWITCH_PAYLOAD_LENGHT_BYTE - 1); i++ ) {
      switch_endianness_switch_id[i] = ((message_from_connect[i + 1] >> 4) & 0x0F) | ((message_from_connect[i + 1] << 4) & 0xF0);
    }
    notify_connected_ble_device(SL_DIRECTION_PROPRIETARY, &switch_endianness_switch_id[0]);
    app_log_info("Toggle message from node: %d, light is %s\n", incoming_message.source,
                 (sl_get_light_state() == DEMO_LIGHT_ON)
                 ? "on"
                 : "off");
  }
}
