/***************************************************************************//**
 * @file
 * @brief app_cli.c
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
#include <string.h>

#include PLATFORM_HEADER
#include "em_chip.h"
#include "ember.h"
#include "hal.h"

#include "app_init.h"
#include "sl_cli.h"
#include "app_log.h"
#include "stack-info.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define ENABLED  "enabled"
#define DISABLED "disabled"

// The destination endpoint of the outgoing message
#define DATA_ENDPOINT           1
#define TX_TEST_ENDPOINT        2

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static EmberStatus send_tx_test_packet(void);
static bool check_channel(uint16_t channel);
static bool set_security_key(uint8_t* key, size_t key_length);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Connect security key id
static psa_key_id_t security_key_id = 0;
// Connect Tx options
EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED;

extern bool tx_test_print_en;
uint8_t tx_test_packet_size;
uint8_t tx_test_inflight_packets;
uint8_t tx_test_current_inflights_packets = 0;
EmberNodeId tx_test_destination;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - info command
 * It lists the main attributes of the current state of the node
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  char* is_ack = ((tx_options & EMBER_OPTIONS_ACK_REQUESTED) ? ENABLED : DISABLED);
  char* is_security = ((tx_options & EMBER_OPTIONS_SECURITY_ENABLED) ? ENABLED : DISABLED);
  char* is_high_prio = ((tx_options & EMBER_OPTIONS_HIGH_PRIORITY) ? ENABLED : DISABLED);

  app_log_info("Info:\n");
  app_log_info("         MCU Id: 0x%016llX\n", SYSTEM_GetUnique());
  app_log_info("  Network state: 0x%02X\n", emberNetworkState());
  app_log_info("      Node type: 0x%02X\n", emberGetNodeType());
  app_log_info("        Node id: 0x%04X\n", emberGetNodeId());
  app_log_info("   Node Long id: 0x");
  for (uint8_t i = 0; i < EUI64_SIZE; i++) {
    app_log_info("%02X", emberGetEui64()[i]);
  }
  app_log_info("\n");
  app_log_info("         Pan id: 0x%04X\n", emberGetPanId());
  app_log_info("        Channel: %d\n", (uint16_t)emberGetRadioChannel());
  app_log_info("          Power: %d\n", (int16_t)emberGetRadioPower());
  app_log_info("     TX options: MAC acks %s, security %s, priority %s\n", is_ack, is_security, is_high_prio);
}

/******************************************************************************
 * CLI - leave command
 * By this API call the node forgets the current network and reverts to
 * a network status of EMBER_NO_NETWORK
 *****************************************************************************/
void cli_leave(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  emberResetNetworkState();
}

/******************************************************************************
 * CLI - commission command
 * If the node starts as a master, it forms a network and enable permit join
 *****************************************************************************/
void cli_commission(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberNodeId nodeId;
  EmberNetworkParameters params;
  uint16_t channel = sl_cli_get_argument_uint8(arguments, 2);

  // Abort if the channel is invalid for this PHY
  if (check_channel(channel) == false) {
    return;
  }

  nodeId = sl_cli_get_argument_uint16(arguments, 0);
  params.panId = sl_cli_get_argument_uint16(arguments, 1);
  params.radioChannel = channel;
  params.radioTxPower = sl_cli_get_argument_int16(arguments, 3);

  status = emberJoinCommissioned(EMBER_DIRECT_DEVICE, nodeId, &params);

  if ( status != EMBER_SUCCESS ) {
    app_log_error("Commissioning failed, 0x%02X", status);
  }
}

/******************************************************************************
 * CLI - data command
 * The node sends message to the given destination ID
 *****************************************************************************/
void cli_data(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberNodeId destination = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t *hex_value = 0;
  size_t hex_length = 0;
  hex_value = sl_cli_get_argument_hex(arguments, 1, &hex_length);

  status = emberMessageSend(destination,
                            DATA_ENDPOINT,
                            0, // messageTag
                            hex_length,
                            hex_value,
                            tx_options);

  app_log_info("TX: Data to 0x%04X:{", destination);
  for (uint8_t i = 0; i < hex_length; i++) {
    if (i == (hex_length - 1)) {
      app_log_info("%02X", hex_value[i]);
    } else {
      app_log_info("%02X ", hex_value[i]);
    }
  }
  app_log_info("}: status=0x%02X\n", status);
}

/******************************************************************************
 * CLI - set_channel command
 * Sets the channel
 *****************************************************************************/
void cli_set_channel(sl_cli_command_arg_t *arguments)
{
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);
  EmberStatus status = emberSetRadioChannel(channel);
  if (status == EMBER_SUCCESS) {
    app_log_info("Radio channel set, status=0x%02X\n", status);
  } else {
    app_log_error("Setting radio channel failed, status=0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - set_tx_options command
 * Sets the tx options
 *****************************************************************************/
void cli_set_tx_option(sl_cli_command_arg_t *arguments)
{
  tx_options = sl_cli_get_argument_uint8(arguments, 0);
  char* is_ack = ((tx_options & EMBER_OPTIONS_ACK_REQUESTED) ? ENABLED : DISABLED);
  char* is_security = ((tx_options & EMBER_OPTIONS_SECURITY_ENABLED) ? ENABLED : DISABLED);
  char* is_high_prio = ((tx_options & EMBER_OPTIONS_HIGH_PRIORITY) ? ENABLED : DISABLED);

  app_log_info("TX options set: MAC acks %s, security %s, priority %s", is_ack, is_security, is_high_prio);
}

/******************************************************************************
 * CLI - reset command
 * Resets the hardware
 *****************************************************************************/
void cli_reset(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  halReboot();
}

/******************************************************************************
 * CLI - toggle_radio command
 * The command turnes on and of the radio (for Rx)
 *****************************************************************************/
void cli_toggle_radio(sl_cli_command_arg_t *arguments)
{
  bool radio_on = (sl_cli_get_argument_uint8(arguments, 0) > 0);

  EmberStatus status = emberSetRadioPowerMode(radio_on);

  if (status == EMBER_SUCCESS) {
    app_log_info("Radio is turned %s\n", (radio_on) ? "ON" : "OFF");
  } else {
    app_log_error("Radio toggle is failed, status=0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - start_energy_scan command
 * The command scans the energy level on the given channel, e.g.
 * "start_energy_scan 0 10" results in 10 RSSI samples collected on channel 0.
 *****************************************************************************/
void cli_start_energy_scan(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t sample_num = sl_cli_get_argument_uint8(arguments, 1);
  status = emberStartEnergyScan(channel, sample_num);

  if (status == EMBER_SUCCESS) {
    app_log_info("Start energy scanning: channel %d, samples %d\n", channel, sample_num);
  } else {
    app_log_error("Start energy scanning failed, status=0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - set_security_key command
 * The command sets the security key
 *****************************************************************************/
void cli_set_security_key(sl_cli_command_arg_t *arguments)
{
#ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
  uint8_t *key_hex_value = 0;
  size_t key_hex_length = 0;
  key_hex_value = sl_cli_get_argument_hex(arguments, 0, &key_hex_length);
  if (key_hex_length != EMBER_ENCRYPTION_KEY_SIZE) {
    app_log_info("Security key length must be: %d bytes\n", EMBER_ENCRYPTION_KEY_SIZE);
    return;
  }

  set_security_key(key_hex_value, key_hex_length);

#else
  (void)arguments;
  app_log_info("Security plugin: CONNECT AES SECURITY is missing\n");
  app_log_info("Security key set failed 0x%02X\n", EMBER_ERR_FATAL);
#endif
}

void cli_unset_security_key(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  #ifdef SL_CATALOG_CONNECT_AES_SECURITY_PRESENT
  if (security_key_id != 0) {
    psa_destroy_key(security_key_id);
  }
  emberRemovePsaSecurityKey();
  security_key_id = 0;
  app_log_info("Security key unset successful\n");
  #endif
}

/******************************************************************************
 * CLI - counter command
 * The command prints out the passed stack counter
 *****************************************************************************/
void cli_counter(sl_cli_command_arg_t *arguments)
{
  uint8_t counter_type = sl_cli_get_argument_uint8(arguments, 0);
  uint32_t counter;
  EmberStatus status = emberGetCounter(counter_type, &counter);

  if (status == EMBER_SUCCESS) {
    app_log_info("Counter type=0x%02X: %d\n", counter_type, counter);
  } else {
    app_log_error("Get counter failed, status=0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - tx_test_print_enable command
 * The command turns ON/OFF printing the received test packets
 *****************************************************************************/
void cli_tx_test_print_enable(sl_cli_command_arg_t *arguments)
{
  tx_test_print_en = (sl_cli_get_argument_uint8(arguments, 0) > 0);
  app_log_info("TX test prints %s\n", (tx_test_print_en) ? ENABLED : DISABLED);
}

/******************************************************************************
 * CLI - tx_test command
 * The command sends packets (number and the length of the packets can be set)
 * to the given destination
 *****************************************************************************/
void cli_tx_test(sl_cli_command_arg_t *arguments)
{
  tx_test_packet_size = sl_cli_get_argument_uint8(arguments, 0);
  tx_test_inflight_packets = sl_cli_get_argument_uint8(arguments, 1);
  tx_test_destination = sl_cli_get_argument_uint16(arguments, 2);
  tx_test_current_inflights_packets = 0;

  for (uint8_t i = 0; i < tx_test_inflight_packets; i++) {
    if (send_tx_test_packet() == EMBER_SUCCESS) {
      tx_test_current_inflights_packets++;
    } else {
      break;
    }
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
static EmberStatus send_tx_test_packet(void)
{
  uint8_t tx_test_packet[127];

  for (uint8_t i = 0; i < tx_test_packet_size; i++) {
    tx_test_packet[i] = i;
  }

  return emberMessageSend(tx_test_destination,
                          TX_TEST_ENDPOINT,
                          0, // messageTag
                          tx_test_packet_size,
                          tx_test_packet,
                          tx_options);
}

/******************************************************************************
 * Checks if the current channel is valid for the selected PHY.
 * If the channel is invalid, it recommends the first available channel.
 * Returns false if the given channel is below the first allowed channel
 * or true otherwise.
 *****************************************************************************/
static bool check_channel(uint16_t channel)
{
  bool channel_ok = true;
  uint16_t default_channel = emberGetDefaultChannel();

  if (channel < default_channel) {
    app_log_info("Channel %d is invalid, the first valid channel is %d!\n", channel, default_channel);
    channel_ok = false;
  }
  return (channel_ok);
}

static bool set_security_key(uint8_t* key, size_t key_length)
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
