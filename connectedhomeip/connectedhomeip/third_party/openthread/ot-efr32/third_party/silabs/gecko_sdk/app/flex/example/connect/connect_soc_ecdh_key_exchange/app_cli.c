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
#include "sl_component_catalog.h"
#include "sl_cli.h"
#include "app_framework_common.h"
#include "app_log.h"
#include "app_process.h"

#include "em_system.h"
#include "psa/crypto.h"

#include "app.h"
#include "sl_connect_ecdh_key_exchange.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Literal constant for print operations
#define ENABLED  "enabled"
#define DISABLED "disabled"

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
extern EmberMessageOptions tx_options;
extern EmberKeyData connect_network_key;

psa_key_id_t requestor_key_pair;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Displays network parameters
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
  app_log_info("          eui64: 0x");
  for (uint8_t i = 0; i < EUI64_SIZE; i++) {
    app_log_info("%02X", emberGetEui64()[i]);
  }
  app_log_info("\n");
  app_log_info("        Node id: 0x%04X\n", emberGetNodeId());
  app_log_info("         Pan id: 0x%04X\n", emberGetPanId());
  app_log_info("        Channel: %d\n", (uint16_t)emberGetRadioChannel());
  app_log_info("          Power: %d\n", (int16_t)emberGetRadioPower());
  app_log_info("     TX options: MAC acks %s, security %s, priority %s\n", is_ack, is_security, is_high_prio);
}

/******************************************************************************
 * Resets the network parameters
 *****************************************************************************/
void cli_leave(sl_cli_command_arg_t *arguments)
{
  (void) arguments;

  emberResetNetworkState();
}

/******************************************************************************
 * Assigns short address to the device
 *****************************************************************************/
void cli_commission(sl_cli_command_arg_t *arguments)
{
  EmberNodeType node_type = EMBER_DIRECT_DEVICE;
  EmberNetworkParameters parameters = {
    .panId = sl_connect_ecdh_key_exchange_get_pan_id(),
    .radioChannel = emberGetRadioChannel(),
    .radioTxPower = emberGetRadioPower(),
  };
  EmberNodeId node_id = sl_cli_get_argument_uint16(arguments, 0);
  EmberStatus status = emberJoinCommissioned(node_type, node_id, &parameters);
  if (status == EMBER_SUCCESS) {
    app_log_info("Connect: address commission succeed\n");
  } else {
    app_log_error("Connect: address commission failed (status: %ld)\n", status);
  }
}

/******************************************************************************
 * Send a message to the specified short address
 *****************************************************************************/
void cli_send(sl_cli_command_arg_t *arguments)
{
  EmberNodeId destination = sl_cli_get_argument_uint16(arguments, 0);
  size_t message_length = 0;
  uint8_t *message = sl_cli_get_argument_hex(arguments, 1, &message_length);
  emberMessageSend(destination,
                   SL_CONNECT_DATA_ENDPOINT,
                   0,
                   message_length,
                   message,
                   tx_options);
  app_log_info("Message queued (length: %u)\n", message_length);
}

/******************************************************************************
 * Sets an arbitrary network key.
 *****************************************************************************/
void cli_set_network_key(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  size_t network_key_length;
  uint8_t *netwok_key = sl_cli_get_argument_hex(arguments, 0, &network_key_length);

  if (network_key_length != EMBER_ENCRYPTION_KEY_SIZE) {
    app_log_error("Connect: network key length must be %d bytes\n", EMBER_ENCRYPTION_KEY_SIZE);
    return;
  }

  memcpy(connect_network_key.contents, netwok_key, EMBER_ENCRYPTION_KEY_SIZE);

  app_log_hexdump_info(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
  app_log_append_info("\n");

  if (set_security_key(connect_network_key.contents, (size_t)EMBER_ENCRYPTION_KEY_SIZE) == true) {
    app_log_info("Connect: set network key succeed\n");
  } else {
    app_log_error("Connect: set network key failed\n");
  }
}

/******************************************************************************
 * Generates a random value and sets as the network key
 *****************************************************************************/
void cli_generate_random_network_key(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  psa_status_t psa_status;

  psa_status = psa_generate_random(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: generate random network key succeed\n");
    app_log_info("PSA: random network key length: %u\n", EMBER_ENCRYPTION_KEY_SIZE);
    app_log_info("PSA: random network key: ");
    app_log_hexdump_info(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
    app_log_append_info("\n");
    if (set_security_key(connect_network_key.contents, (size_t)EMBER_ENCRYPTION_KEY_SIZE) == true) {
      app_log_info("Connect: set the random key as network key succeed\n");
    } else {
      app_log_error("Connect: set the random key as network key failed\n");
    }
  } else {
    app_log_error("PSA: generate random network key (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Initiate key exchange and retrieve network key then apply it as network key
 * Note: retrieving and applying the key happens in the incoming callback
 *****************************************************************************/
void cli_request_network_key(sl_cli_command_arg_t *arguments)
{
  EmberNodeId destination = sl_cli_get_argument_uint16(arguments, 0);

  uint8_t public_key[SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE];
  size_t public_key_length;

  psa_status_t psa_status;

  app_log_info("Connect: initiating key exchange\n");
  app_log_info("PSA: generating public/private key pair\n");
  psa_status = sl_connect_ecdh_key_exchange_generate_key_pair(&requestor_key_pair);

  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: generated key ID: %lu\n", requestor_key_pair);
  } else {
    app_log_error("PSA: key pair generation and public key export failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: exporting public key\n");
  psa_status = sl_connect_ecdh_key_exchange_export_public_key(
    requestor_key_pair,
    public_key,
    SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE,
    &public_key_length);

  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: public key export succeed\n");
    app_log_info("PSA: public key length: %u\n", public_key_length);
    app_log_info("PSA: public key: ");
    app_log_hexdump_info(public_key, public_key_length);
    app_log_append_info("\n");
  } else {
    app_log_error("PSA: export public key failed (status: %ld)\n", psa_status);
    return;
  }

  sl_connect_ecdh_state ecdh_state = SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_REQUEST_KEY;
  size_t ecdh_message_length =
    sizeof(ecdh_state)
    + sizeof(public_key_length)
    + public_key_length;

  if (ecdh_message_length > SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH) {
    app_log_error("Connect: message is too long, max length: %d\n", SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH);
    return;
  }

  uint8_t ecdh_message[SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH];
  memcpy(ecdh_message, &ecdh_state, sizeof(ecdh_state));
  memcpy(ecdh_message + sizeof(ecdh_state), &public_key_length, sizeof(public_key_length));
  memcpy(ecdh_message + sizeof(ecdh_state) + sizeof(public_key_length), public_key, public_key_length);
  app_log_info("Connect: requestor message length %u\n", ecdh_message_length);

  app_log_info("Connect: sending public key\n");
  EmberStatus em_status = emberMessageSend(
    destination,
    SL_CONNECT_ECDH_KEY_EXCHANGE_ENDPOINT,
    0,
    ecdh_message_length,
    ecdh_message,
    // public key must be sent non-encrypted, as the Connect network key is not
    // shared yet
    tx_options & ~EMBER_OPTIONS_SECURITY_ENABLED);
  if (em_status != EMBER_SUCCESS) {
    app_log_error("Connect: message send failed (status: %u)\n", em_status);
  }
}

/******************************************************************************
 * Generates local public/private key pair
 *****************************************************************************/
void cli_generate_key_pair(sl_cli_command_arg_t *arguments)
{
  (void) arguments;
  psa_status_t psa_status;
  psa_key_id_t key_id;

  psa_status = sl_connect_ecdh_key_exchange_generate_key_pair(&key_id);

  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: generated key ID: %lu\n", key_id);
  } else {
    app_log_error("PSA: key pair generation and public key export failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Exports the public part of the local key pair
 *****************************************************************************/
void cli_export_public_key(sl_cli_command_arg_t *arguments)
{
  psa_status_t psa_status;
  psa_key_id_t key_id = sl_cli_get_argument_uint32(arguments, 0);

  uint8_t public_key[SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE];
  size_t public_key_length;

  psa_status = sl_connect_ecdh_key_exchange_export_public_key(
    key_id,
    public_key,
    SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE,
    &public_key_length);

  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: public key export succeed\n");
    app_log_info("PSA: public key length: %u\n", public_key_length);
    app_log_info("PSA: public key: ");
    app_log_hexdump_info(public_key, public_key_length);
    app_log_append_info("\n");
  } else {
    app_log_error("PSA: export public key failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Generates shared secret from the local private and peer public key
 *****************************************************************************/
void cli_generate_shared_key(sl_cli_command_arg_t *arguments)
{
  psa_status_t psa_status;

  size_t peer_public_key_length = 0;
  psa_key_id_t key_id = sl_cli_get_argument_uint32(arguments, 0);
  uint8_t *peer_public_key = sl_cli_get_argument_hex(arguments, 1, &peer_public_key_length);
  psa_key_id_t hkdf_id;

  psa_status = sl_connect_ecdh_key_exchange_generate_shared_key(
    key_id,
    peer_public_key,
    peer_public_key_length,
    &hkdf_id);
  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: generate shared key succeed\n");
    app_log_info("PSA: generated shared key ID: %lu\n", hkdf_id);
  } else {
    app_log_error("PSA: generate shared key failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Encrypts message with the provided key
 *****************************************************************************/
void cli_encrypt_message(sl_cli_command_arg_t *arguments)
{
  psa_status_t psa_status;

  psa_key_id_t key_id = sl_cli_get_argument_uint32(arguments, 0);

  size_t plain_text_length = 0;
  uint8_t *plain_text = sl_cli_get_argument_hex(arguments, 1, &plain_text_length);

  uint8_t cipher_text[128];
  size_t cipher_text_length = 0;

  uint8_t iv[PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)];
  size_t iv_length;

  psa_status = sl_connect_ecdh_key_exchange_encrypt_message(
    key_id,
    plain_text,
    plain_text_length,
    cipher_text,
    sizeof(cipher_text),
    &cipher_text_length,
    iv,
    sizeof(iv),
    &iv_length);
  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: encrypt succeed\n");
    app_log_info("PSA: initial vector: ");
    app_log_hexdump_info(iv, iv_length);
    app_log_append_info("\n");
    app_log_info("PSA: encrypted message: ");
    app_log_hexdump_info(cipher_text, cipher_text_length);
    app_log_append_info("\n");
  } else {
    app_log_info("PSA: encrypt failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Decrypts message with the provided key
 *****************************************************************************/
void cli_decrypt_message(sl_cli_command_arg_t *arguments)
{
  psa_status_t psa_status;

  psa_key_id_t key_id = sl_cli_get_argument_uint32(arguments, 0);

  size_t iv_length;
  uint8_t *iv = sl_cli_get_argument_hex(arguments, 1, &iv_length);

  size_t cipher_text_length = 0;
  uint8_t *cipher_text = sl_cli_get_argument_hex(arguments, 2, &cipher_text_length);

  uint8_t plain_text[128];
  size_t plain_text_length = 0;

  psa_status = sl_connect_ecdh_key_exchange_decrypt_message(
    key_id,
    iv,
    iv_length,
    cipher_text,
    cipher_text_length,
    plain_text,
    sizeof(plain_text),
    &plain_text_length);
  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: decrypt succeed\n");
    app_log_info("PSA: decrypted message: ");
    app_log_hexdump_info(plain_text, plain_text_length);
    app_log_append_info("\n");
  } else {
    app_log_error("PSA: encrypt failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * Destroys previously generated key
 *****************************************************************************/
void cli_destroy_key(sl_cli_command_arg_t *arguments)
{
  psa_status_t psa_status;
  psa_key_id_t key_id = sl_cli_get_argument_uint32(arguments, 0);

  psa_status = sl_connect_ecdh_key_exchange_destroy_key(key_id);

  if (psa_status == PSA_SUCCESS) {
    app_log_info("PSA: destroy key succeed\n");
  } else {
    app_log_info("PSA: destroy key failed (status: %ld)\n", psa_status);
  }
}

/******************************************************************************
 * CLI - set PAN ID
 *****************************************************************************/
void cli_set_pan_id(sl_cli_command_arg_t *arguments)
{
  uint16_t pan_id = sl_cli_get_argument_uint16(arguments, 0);
  uint8_t network_state = (uint8_t)emberNetworkState();

  if (network_state == EMBER_NO_NETWORK) {
    sl_connect_ecdh_key_exchange_set_pan_id(pan_id);
    app_log_info("Pan id set to 0x%04X\n", sl_connect_ecdh_key_exchange_get_pan_id());
  } else {
    app_log_info("Leave the network before setting pan id\n");
  }
}

/******************************************************************************
 * CLI - set the communication channel
 *****************************************************************************/
void cli_set_channel(sl_cli_command_arg_t *arguments)
{
  uint16_t channel = sl_cli_get_argument_uint16(arguments, 0);
  EmberStatus status = emberSetRadioChannelExtended(channel, false);

  if (status == EMBER_SUCCESS) {
    app_log_info("Channel set to %d\n", (uint16_t)emberGetRadioChannel());
  } else {
    app_log_info("Channel set failed, status: 0x%02X\n", (uint8_t)status);
  }
}

/******************************************************************************
 * CLI - set transmission power
 *****************************************************************************/
void cli_set_power(sl_cli_command_arg_t *arguments)
{
  int16_t power = sl_cli_get_argument_int16(arguments, 0);
  EmberStatus status = emberSetRadioPower(power, false);

  if (status == EMBER_SUCCESS) {
    app_log_info("Tx power set to %d\n", (int16_t)emberGetRadioPower());
  } else {
    app_log_info("Tx power set failed, status: 0x%02X\n", (uint8_t)status);
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
