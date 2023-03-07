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
#include <stdio.h>
#include "app_log.h"
#include "app_framework_common.h"

#include "app.h"
#include "sl_connect_ecdh_key_exchange.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
static void process_ecdh_key_exchange_request(EmberIncomingMessage *message);
static void send_ecdh_key_exchange_reply(EmberIncomingMessage *message);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// TX options set up for the network
EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED | EMBER_OPTIONS_SECURITY_ENABLED;

EmberKeyData connect_network_key;
psa_key_id_t connect_network_key_id;

extern psa_key_id_t requestor_key_pair;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void emberAfTickCallback(void)
{
  ///////////////////////////////////////////////////////////////////////////
  // Put your application code here!                                       //
  // This is called infinitely.                                            //
  // Do not call blocking functions from here!                             //
  ///////////////////////////////////////////////////////////////////////////
}

void emberAfIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint == SL_CONNECT_DATA_ENDPOINT) {
    app_log_info("RX: Data from 0x%04x:", message->source);
    app_log_hexdump_info(message->payload, message->length);
    app_log_append_info("\n");
  }
  if (message->endpoint == SL_CONNECT_ECDH_KEY_EXCHANGE_ENDPOINT) {
    sl_connect_ecdh_state ecdh_state;
    memcpy(&ecdh_state, message->payload, sizeof(ecdh_state));
    if (ecdh_state == SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_REQUEST_KEY) {
      app_log_info("Connect: key exchange request received\n");
      process_ecdh_key_exchange_request(message);
    }
    if (ecdh_state == SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_SEND_KEY) {
      app_log_info("Connect: reply to key exchange request received\n");
      send_ecdh_key_exchange_reply(message);
    }
  }
}

void emberAfMessageSentCallback(EmberStatus status,
                                EmberOutgoingMessage *message)
{
  (void) message;
  if (status != EMBER_SUCCESS) {
    app_log_error("TX: 0x%02x\n", status);
  }
}

void emberAfStackStatusCallback(EmberStatus status)
{
  switch (status) {
    case EMBER_NETWORK_UP:
      app_log_info("Network up\n");
      break;
    case EMBER_NETWORK_DOWN:
      app_log_info("Network down\n");
      break;
    default:
      app_log_info("Stack status: 0x%02x\n", status);
      break;
  }
}

bool set_security_key(uint8_t *key, size_t key_length)
{
  bool success = false;
  EmberStatus emstatus = EMBER_ERR_FATAL;

  psa_key_attributes_t key_attr;
  psa_status_t status;

  if (connect_network_key_id != 0) {
    psa_destroy_key(connect_network_key_id);
    app_log_info("Previous security key is destroyed\n");
  }
  emberRemovePsaSecurityKey();
  connect_network_key_id = 0;

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
                          &connect_network_key_id);

  if (status == PSA_SUCCESS) {
    app_log_info("Security key import successful, key id: %lu\n", connect_network_key_id);
  } else {
    app_log_info("Security Key import failed: 0x%02lx\n", status);
  }

  emstatus = emberSetPsaSecurityKey(connect_network_key_id);

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
static void process_ecdh_key_exchange_request(EmberIncomingMessage *message)
{
  psa_status_t psa_status;
  psa_key_id_t key_pair;
  psa_key_id_t hkdf_key_id;

  sl_connect_ecdh_state ecdh_state;

  uint8_t public_key[SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE];
  size_t public_key_length;

  uint8_t peer_public_key[SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE];
  size_t peer_public_key_length;

  uint8_t iv[SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE];
  size_t iv_length;

  uint8_t encrypted_connect_network_key[SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE];
  size_t encrypted_connect_network_key_length;

  memcpy(&peer_public_key_length, message->payload + sizeof(ecdh_state), sizeof(peer_public_key_length));
  memcpy(peer_public_key, message->payload + sizeof(ecdh_state) + sizeof(peer_public_key_length), peer_public_key_length);

  app_log_info("PSA: generating public/private key pair\n");
  psa_status = sl_connect_ecdh_key_exchange_generate_key_pair(&key_pair);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: generate key pair failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: exporting public key\n");
  psa_status = sl_connect_ecdh_key_exchange_export_public_key(key_pair, public_key, sizeof(public_key), &public_key_length);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: export public key failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: generating shared key\n");
  psa_status = sl_connect_ecdh_key_exchange_generate_shared_key(key_pair, peer_public_key, peer_public_key_length, &hkdf_key_id);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: generate shared key failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: destroying public/private key pair\n");
  psa_status = sl_connect_ecdh_key_exchange_destroy_key(key_pair);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: destroy key pair failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: encrypting network key\n");
  psa_status = sl_connect_ecdh_key_exchange_encrypt_message(
    hkdf_key_id,
    connect_network_key.contents,
    sizeof(connect_network_key),
    encrypted_connect_network_key,
    sizeof(encrypted_connect_network_key),
    &encrypted_connect_network_key_length,
    iv,
    sizeof(iv),
    &iv_length);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: encrypt network key failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: plain text connect network key: ");
  app_log_hexdump_info(connect_network_key.contents, EMBER_ENCRYPTION_KEY_SIZE);
  app_log_append_info("\n");
  app_log_info("PSA: encrypted connect network key:  ");
  app_log_hexdump_info(encrypted_connect_network_key, encrypted_connect_network_key_length);
  app_log_append_info("\n");
  app_log_info("PSA: initial vector:                 ");
  app_log_hexdump_info(iv, iv_length);
  app_log_append_info("\n");

  app_log_info("PSA: destroying shared key\n");
  psa_status = sl_connect_ecdh_key_exchange_destroy_key(hkdf_key_id);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: destroy HKDF key failed (status: %ld)\n", psa_status);
    return;
  }

  uint8_t ecdh_message_size =
    sizeof(ecdh_state)
    + sizeof(public_key_length)
    + public_key_length
    + sizeof(iv_length)
    + iv_length
    + sizeof(encrypted_connect_network_key_length)
    + encrypted_connect_network_key_length;

  if (ecdh_message_size > SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH) {
    app_log_error("Connect: message is too long, max length: %d\n", SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH);
    return;
  }

  uint8_t ecdh_message[SL_CONNECT_ECDH_KEY_EXCHANGE_MAX_MESSAGE_LENGTH];
  uint8_t ecdh_message_offset = 0;

  ecdh_state = SL_CONNECT_ECDH_KEY_EXCHANGE_STATE_SEND_KEY;
  memcpy(ecdh_message, &ecdh_state, sizeof(ecdh_state));
  ecdh_message_offset += sizeof(ecdh_state);

  memcpy(ecdh_message + ecdh_message_offset, &public_key_length, sizeof(public_key_length));
  ecdh_message_offset += sizeof(public_key_length);

  memcpy(ecdh_message + ecdh_message_offset, public_key, public_key_length);
  ecdh_message_offset += public_key_length;

  memcpy(ecdh_message + ecdh_message_offset, &iv_length, sizeof(iv_length));
  ecdh_message_offset += sizeof(iv_length);

  memcpy(ecdh_message + ecdh_message_offset, iv, iv_length);
  ecdh_message_offset += iv_length;

  memcpy(ecdh_message + ecdh_message_offset, &encrypted_connect_network_key_length, sizeof(encrypted_connect_network_key_length));
  ecdh_message_offset += sizeof(encrypted_connect_network_key_length);

  memcpy(ecdh_message + ecdh_message_offset, encrypted_connect_network_key, encrypted_connect_network_key_length);
  ecdh_message_offset += encrypted_connect_network_key_length;

  app_log_info("Connect: sending public key, initial vector, network key\n");
  EmberStatus em_status = emberMessageSend(
    message->source,
    SL_CONNECT_ECDH_KEY_EXCHANGE_ENDPOINT,
    0,
    ecdh_message_size,
    ecdh_message,
    tx_options & ~EMBER_OPTIONS_SECURITY_ENABLED);
  if (em_status != EMBER_SUCCESS) {
    app_log_error("Connect: message send failed (status: %u)\n", em_status);
  }
}

static void send_ecdh_key_exchange_reply(EmberIncomingMessage *message)
{
  psa_status_t psa_status;
  psa_key_id_t hkdf_key_id;

  uint8_t peer_public_key[SL_CONNECT_ECDH_KEY_EXCHANGE_PUBLIC_KEY_SIZE];
  size_t peer_public_key_length;

  uint8_t iv[SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE];
  size_t iv_length;

  uint8_t encrypted_connect_network_key[SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE];
  size_t encrypted_connect_network_key_length;

  uint8_t decrypted_connect_network_key[SL_CONNECT_ECDH_KEY_EXCHANGE_SHARED_KEY_SIZE];
  size_t decrypted_connect_network_key_length;

  uint8_t ecdh_message_offset = sizeof(sl_connect_ecdh_state);

  memcpy(&peer_public_key_length, message->payload + ecdh_message_offset, sizeof(peer_public_key_length));
  ecdh_message_offset += sizeof(peer_public_key_length);

  memcpy(peer_public_key, message->payload + ecdh_message_offset, peer_public_key_length);
  ecdh_message_offset += peer_public_key_length;

  memcpy(&iv_length, message->payload + ecdh_message_offset, sizeof(iv_length));
  ecdh_message_offset += sizeof(iv_length);

  memcpy(iv, message->payload + ecdh_message_offset, iv_length);
  ecdh_message_offset += iv_length;

  memcpy(&encrypted_connect_network_key_length, message->payload + ecdh_message_offset, sizeof(encrypted_connect_network_key_length));
  ecdh_message_offset += sizeof(encrypted_connect_network_key_length);

  memcpy(encrypted_connect_network_key, message->payload + ecdh_message_offset, encrypted_connect_network_key_length);
  ecdh_message_offset += encrypted_connect_network_key_length;

  bool psa_fail = false;
  app_log_info("PSA: generating shared key\n");
  psa_status = sl_connect_ecdh_key_exchange_generate_shared_key(requestor_key_pair, peer_public_key, peer_public_key_length, &hkdf_key_id);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: generate shared key failed (status: %ld)\n", psa_status);
    psa_fail = true;
  }

  app_log_info("PSA: destroying public/private key pair\n");
  psa_status = sl_connect_ecdh_key_exchange_destroy_key(requestor_key_pair);
  if (psa_status != PSA_SUCCESS || psa_fail) {
    app_log_error("PSA: destroy key pair failed (status: %ld)\n", psa_status);
    return;
  }

  psa_fail = false;
  app_log_info("PSA: decrypting network key\n");
  psa_status = sl_connect_ecdh_key_exchange_decrypt_message(
    hkdf_key_id,
    iv,
    iv_length,
    encrypted_connect_network_key,
    encrypted_connect_network_key_length,
    decrypted_connect_network_key,
    sizeof(decrypted_connect_network_key),
    &decrypted_connect_network_key_length);
  if (psa_status != PSA_SUCCESS) {
    app_log_error("PSA: decrypt failed (status: %ld)\n", psa_status);
    psa_fail = true;
  }

  app_log_info("PSA: destroying shared key\n");
  psa_status = sl_connect_ecdh_key_exchange_destroy_key(hkdf_key_id);
  if (psa_status != PSA_SUCCESS || psa_fail) {
    app_log_error("PSA: destroy HKDF key failed (status: %ld)\n", psa_status);
    return;
  }

  app_log_info("PSA: received network key: ");
  app_log_hexdump_info(decrypted_connect_network_key, decrypted_connect_network_key_length);
  app_log_append_info("\n");

  memcpy(connect_network_key.contents, decrypted_connect_network_key, EMBER_ENCRYPTION_KEY_SIZE);

  app_log_info("Connect: applying received network key\n");
  if (set_security_key(connect_network_key.contents, (size_t)EMBER_ENCRYPTION_KEY_SIZE) == true) {
    app_log_info("Connect: set network key succeed\n");
  } else {
    app_log_error("Connect: set network key failed\n");
  }
}
