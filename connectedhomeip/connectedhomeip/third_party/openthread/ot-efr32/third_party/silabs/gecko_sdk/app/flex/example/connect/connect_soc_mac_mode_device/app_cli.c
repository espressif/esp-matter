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
#include "em_chip.h"
#include "app_log.h"
#include "app_common.h"
#include "app_init.h"
#include "sl_cli.h"
#include "stack-info.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static bool check_channel(uint16_t channel);
static bool set_security_key(uint8_t* key, size_t key_length);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Connect security key id
static psa_key_id_t security_key_id = 0;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * CLI - active_scan message: Start active scanning on selected channel
 *****************************************************************************/
void cli_active_scan(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t channel_to_scan = sl_cli_get_argument_uint8(arguments, 0);
  status = emberStartActiveScan(channel_to_scan);
  app_log_info("Start active scanning: channel %d, status=0x%02X\n", channel_to_scan, status);
}

/******************************************************************************
 * CLI - commission message: Commission node parameters
 *****************************************************************************/
void cli_commission(sl_cli_command_arg_t *arguments)
{
  EmberNetworkParameters parameters;
  EmberStatus status;
  uint16_t channel = sl_cli_get_argument_uint8(arguments, 4);
  // Abort if the channel is invalid for this PHY
  if (check_channel(channel) == false) {
    return;
  }

  uint8_t node_type = sl_cli_get_argument_uint8(arguments, 0);
  EmberNodeId node_id = sl_cli_get_argument_uint16(arguments, 1);
  parameters.panId = sl_cli_get_argument_uint16(arguments, 2);
  parameters.radioTxPower = sl_cli_get_argument_int16(arguments, 3);
  parameters.radioChannel = channel;

  status = emberJoinCommissioned(node_type, node_id, &parameters);

  if (status == EMBER_SUCCESS) {
    app_log_info("Node parameters commissioned\n");
  } else {
    app_log_error("Commissioning failed, status 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - energy_scan message: Start energy scanning on the given channel with samples (num)
 *****************************************************************************/
void cli_energy_scan(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t channel_to_scan = sl_cli_get_argument_uint8(arguments, 0);
  uint8_t samples = sl_cli_get_argument_uint8(arguments, 1);
  status = emberStartEnergyScan(channel_to_scan, samples);
  app_log_info("Start energy scanning: channel %d, samples %d, status=0x%02X\n", channel_to_scan, samples, status);
}

/******************************************************************************
 * CLI - form message: Form network
 *****************************************************************************/
void cli_form(sl_cli_command_arg_t *arguments)
{
  EmberNetworkParameters parameters;
  EmberStatus status;
  uint16_t channel = sl_cli_get_argument_uint8(arguments, 2);
  // Abort if the channel is invalid for this PHY
  if (check_channel(channel) == false) {
    return;
  }

  parameters.panId = sl_cli_get_argument_uint16(arguments, 0);
  parameters.radioTxPower = sl_cli_get_argument_int16(arguments, 1);
  parameters.radioChannel = channel;

  status = emberMacFormNetwork(&parameters);

  if (status == EMBER_SUCCESS) {
    app_log_info("Network formed\n");
  } else {
    app_log_info("Network form failed, status 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - info message: List network and device params
 *****************************************************************************/
void cli_info(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
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
  app_log_info("     TX options: MAC acks %s, security %s, priority %s\n",
               ((tx_options & EMBER_OPTIONS_ACK_REQUESTED) ? "enabled" : "disabled"),
               ((tx_options & EMBER_OPTIONS_SECURITY_ENABLED) ? "enabled" : "disabled"),
               ((tx_options & EMBER_OPTIONS_HIGH_PRIORITY) ? "enabled" : "disabled"));
}

/******************************************************************************
 * CLI - join message: Start to join network
 *****************************************************************************/
void cli_join_network(sl_cli_command_arg_t *arguments)
{
  EmberNetworkParameters parameters;
  EmberStatus status;
  uint16_t channel = sl_cli_get_argument_uint8(arguments, 3);
  // Abort if the channel is invalid for this PHY
  if (check_channel(channel) == false) {
    return;
  }

  uint8_t node_type = sl_cli_get_argument_uint8(arguments, 0);
  parameters.panId = sl_cli_get_argument_uint16(arguments, 1);
  parameters.radioTxPower = sl_cli_get_argument_int16(arguments, 2);
  parameters.radioChannel = channel;
  bool use_long_addressing = (sl_cli_get_argument_uint8(arguments, 4) > 0);

  status = emberJoinNetworkExtended(
    node_type,
    (use_long_addressing ? EMBER_USE_LONG_ADDRESS : EMBER_NULL_NODE_ID),
    &parameters);

  if (status == EMBER_SUCCESS) {
    app_log_info("Started the joining process\n");
  } else {
    app_log_error("Join network failed, status 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - leave message: Leave current network, resets states
 *****************************************************************************/
void cli_leave(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  emberResetNetworkState();
}

/******************************************************************************
 * CLI - pjoin message: Permit join set with duration
 *****************************************************************************/
void cli_set_permit_join(sl_cli_command_arg_t *arguments)
{
  uint8_t duration = sl_cli_get_argument_uint8(arguments, 0);
  EmberStatus status = emberPermitJoining(duration);

  if (status == EMBER_SUCCESS) {
    app_log_info("Permit join set 0x%02X\n", duration);
  } else {
    app_log_error("Permit join failed\n");
  }
}

/******************************************************************************
 * CLI - poll message: Get poll status
 *****************************************************************************/
void cli_poll(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  EmberStatus status = emberPollForData();
  app_log_info("Poll status 0x%02X", status);
}

/******************************************************************************
 * CLI - purge_indirect message: Indirect purge
 *****************************************************************************/
void cli_purge_indirect(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  EmberStatus status = emberPurgeIndirectMessages();
  if (status == EMBER_SUCCESS) {
    app_log_info("Purge indirect success\n");
  } else {
    app_log_error("Purge indirect failed, 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - reset message: Reset device
 *****************************************************************************/
void cli_reset(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  NVIC_SystemReset();
}

/******************************************************************************
 * CLI - send message: Send message
 * Params:
 * 0: a "nibble mask" indicating which fields are specified, specifically:
 *    0x000F - source ID mode (0x00 = none, 0x02 = short, 0x03 = long)
 *    0x00F0 - destination ID mode (0x00 = none, 0x02 = short, 0x03 = long)
 *    0x0F00 - the source pan ID is specified (0x01) or not (0x00).
 *    0xF000 - the destination pan ID is specified (0x01) or not (0x00).
 * 1: the source short ID (if specified)
 * 2: the source long ID (if specified)
 * 3: the destination short ID (if specified)
 * 4: the destination long ID (if specified)
 * 5: the source PAN ID (if specified)
 * 6: the destination PAN ID (if specified)
 * 7: MAC payload length
 *****************************************************************************/
void cli_send(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberMacFrame mac_frame;
  size_t length;
  uint16_t mac_frame_info = sl_cli_get_argument_uint16(arguments, 0);
  EmberNodeId short_src_id = sl_cli_get_argument_uint16(arguments, 1);
  EmberNodeId short_dest_id = sl_cli_get_argument_uint16(arguments, 3);
  EmberPanId src_pan_id = sl_cli_get_argument_uint16(arguments, 5);
  EmberPanId dst_pan_id = sl_cli_get_argument_uint16(arguments, 6);
  uint8_t *message = sl_cli_get_argument_hex(arguments, 7, &length);

  if ((mac_frame_info & 0x000F) == EMBER_MAC_ADDRESS_MODE_SHORT) {
    mac_frame.srcAddress.addr.shortAddress = short_src_id;
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_SHORT;
  } else if ((mac_frame_info & 0x000F) == EMBER_MAC_ADDRESS_MODE_LONG) {
    uint8_t *hex_value = 0;
    size_t hex_length = 0;
    hex_value = sl_cli_get_argument_hex(arguments, 2, &hex_length);
    if (EUI64_SIZE != hex_length) {
      app_log_error("Source MAC address set failed, not correct length\n");
      return;
    }
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      mac_frame.srcAddress.addr.longAddress[i] = hex_value[i];
    }
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_LONG;
  } else {
    mac_frame.srcAddress.mode = EMBER_MAC_ADDRESS_MODE_NONE;
  }

  if (((mac_frame_info & 0x00F0) >> 4) == EMBER_MAC_ADDRESS_MODE_SHORT) {
    mac_frame.dstAddress.addr.shortAddress = short_dest_id;
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_SHORT;
  } else if (((mac_frame_info & 0x00F0) >> 4) == EMBER_MAC_ADDRESS_MODE_LONG) {
    uint8_t *hex_value = 0;
    size_t hex_length = 0;
    hex_value = sl_cli_get_argument_hex(arguments, 4, &hex_length);
    if (EUI64_SIZE != hex_length) {
      app_log_error("Destination MAC address set failed, not correct length\n");
      return;
    }
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      mac_frame.dstAddress.addr.longAddress[i] = hex_value[i];
    }
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_LONG;
  } else {
    mac_frame.dstAddress.mode = EMBER_MAC_ADDRESS_MODE_NONE;
  }

  if (mac_frame_info & 0x0F00) {
    mac_frame.srcPanId = src_pan_id;
    mac_frame.srcPanIdSpecified = true;
  } else {
    mac_frame.srcPanIdSpecified = false;
  }

  if (mac_frame_info & 0xF000) {
    mac_frame.dstPanId = dst_pan_id;
    mac_frame.dstPanIdSpecified = true;
  } else {
    mac_frame.dstPanIdSpecified = false;
  }

  status = emberMacMessageSend(&mac_frame,
                               0x00, // messageTag
                               length,
                               message,
                               tx_options);

  if (status == EMBER_SUCCESS) {
    app_log_info("MAC frame submitted\n");
  } else {
    app_log_error("MAC frame submission failed, status=0x%02X\n",
                  status);
  }
}

/******************************************************************************
 * CLI - set_beacon_payload message: Set beacon payload
 *****************************************************************************/
void cli_set_beacon_payload(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  size_t length;
  uint8_t *contents = sl_cli_get_argument_hex(arguments, 0, &length);

  status = emberSetApplicationBeaconPayload(length, contents);

  app_log_info("Set beacon payload: {");
  for (uint8_t i = 0; i < length; i++) {
    app_log_info("%02X", contents[i]);
  }
  app_log_info("}: status=0x%02X\n", status);
}

/******************************************************************************
 * CLI - set_channel message: Radio channel set
 *****************************************************************************/
void cli_set_channel(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  uint8_t channel = sl_cli_get_argument_uint8(arguments, 0);
  status = emberSetRadioChannel(channel);
  if (status == EMBER_SUCCESS) {
    app_log_info("Radio channel set, status=0x%02X\n", status);
  } else {
    app_log_error("Setting radio channel failed, status=0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - set_key message: Set security key
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
 * CLI - set_options message: Set transmit options
 *****************************************************************************/
void cli_set_options(sl_cli_command_arg_t *arguments)
{
  tx_options = sl_cli_get_argument_uint8(arguments, 0);
  app_log_info("Send options set: 0x%02X\n", tx_options);
}

/******************************************************************************
 * CLI - set_poll_dest message: Set Poll Destination
 *****************************************************************************/
void cli_set_poll_destination(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberMacAddress dest_addr;
  uint8_t *hex_value = 0;
  size_t hex_length = 0;

  dest_addr.addr.shortAddress = sl_cli_get_argument_uint16(arguments, 0);
  dest_addr.mode = EMBER_MAC_ADDRESS_MODE_SHORT;

  hex_value = sl_cli_get_argument_hex(arguments, 1, &hex_length);

  if (dest_addr.addr.shortAddress == EMBER_NULL_NODE_ID) {
    if (EUI64_SIZE != hex_length) {
      app_log_info("Poll address set failed, not correct length\n");
      return;
    }
    for (uint8_t i = 0; i < EUI64_SIZE; i++) {
      dest_addr.addr.longAddress[i] = hex_value[i];
    }
    dest_addr.mode = EMBER_MAC_ADDRESS_MODE_LONG;
  }

  status = emberSetPollDestinationAddress(&dest_addr);

  if (status == EMBER_SUCCESS) {
    app_log_info("Poll address set\n");
  } else {
    app_log_info("Poll address set failed, 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - set_security_mapping command
 * The command fills up the Long address table to be able later use secure
 * packets for communications.
 *****************************************************************************/
void set_security_mapping_command(sl_cli_command_arg_t *arguments)
{
  EmberStatus status;
  EmberEUI64 long_addr;
  EmberNodeId short_addr = sl_cli_get_argument_uint16(arguments, 0);

  size_t hex_length;
  uint8_t* contents = sl_cli_get_argument_hex(arguments, 1, &hex_length);
  if (hex_length != EUI64_SIZE) {
    app_log_info("Long address is %d byte long, please check your input.\n", EUI64_SIZE);
    return;
  } else {
    memcpy(long_addr, contents, EUI64_SIZE);
  }

  status = emberMacAddShortToLongAddressMapping(short_addr, long_addr);

  if (status == EMBER_SUCCESS) {
    app_log_info("Security mapping set\n");
  } else {
    app_log_info("Security mapping set failed, 0x%02X\n", status);
  }
}

/******************************************************************************
 * CLI - clear_security_mappings command
 * The command clears the Long address table used for secure packet communications.
 *****************************************************************************/
void clear_security_mappings_command(sl_cli_command_arg_t *arguments)
{
  (void)arguments;
  EmberStatus status = emberMacClearShortToLongAddressMappings();

  if (status == EMBER_SUCCESS) {
    app_log_info("Security mappings cleared\n");
  } else {
    app_log_info("Security mappings clear failed, 0x%02X\n", status);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
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
