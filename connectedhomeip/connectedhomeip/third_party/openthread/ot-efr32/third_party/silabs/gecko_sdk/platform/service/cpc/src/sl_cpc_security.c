/***************************************************************************/ /**
 * @file
 * @brief CPC Security Endpoint API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#include "mbedtls/bignum.h"
#include "psa/crypto.h"
#include "psa/crypto_sizes.h"

#include "sl_cpc.h"
#include "sli_cpc.h"
#include "sli_cpc_hdlc.h"
#include "sl_atomic.h"
#include "sli_cpc_security.h"

#include "em_core.h"
#include "sl_assert.h"

/*******************************************************************************
 *********************************   DEFINES   *********************************
 ******************************************************************************/
#define SLI_CPC_SECURITY_PSA_CRYPTO_BINDING_KEY_ID 0x00004200

#if !defined(SLI_CPC_SECURITY_NONCE_FRAME_COUNTER_RESET_VALUE)
#define SLI_CPC_SECURITY_NONCE_FRAME_COUNTER_RESET_VALUE 0
#endif

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/

static sl_cpc_security_state_t security_state = SL_CPC_SECURITY_STATE_NOT_READY;

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
bool security_endpoint_initialized = false;
sl_cpc_endpoint_handle_t security_ep;
#else
static sl_cpc_endpoint_handle_t security_ep;
#endif

static sli_cpc_security_protocol_cmd_t security_protocol_response;
static sli_cpc_security_protocol_cmd_t security_protocol_request;
static bool security_write_completed = true;
static bool security_request_pending = false;
static bool security_endpoint_in_error = false;

#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_ECDH)
static uint8_t peer_public_key[SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES];
static uint8_t our_public_key[SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES];
static size_t our_public_key_len = SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES;
#endif

static bool is_bound = false;
static psa_key_id_t session_key_id;
static const psa_key_id_t binding_key_id = SLI_CPC_SECURITY_PSA_CRYPTO_BINDING_KEY_ID;

static sli_cpc_security_nonce_t nonce_primary;
static sli_cpc_security_nonce_t nonce_secondary;

static sl_slist_node_t *unbind_observers;

const sli_cpc_security_protocol_cmd_info_t sli_cpc_security_command[] = {
  [BINDING_REQUEST_ID] =  {
    .request_len = sizeof(uint8_t),
    .response_len = sizeof(sl_status_t),
    .command_id = BINDING_REQUEST_ID
  },
  [PLAIN_TEXT_KEY_SHARE_ID] =  {
    .request_len = SLI_SECURITY_BINDING_KEY_LENGTH_BYTES,
    .response_len = sizeof(sl_status_t),
    .command_id = PLAIN_TEXT_KEY_SHARE_ID
  },
  [PUBLIC_KEY_SHARE_ID] =  {
    .request_len = SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
    .response_len = sizeof(sl_status_t) + SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
    .command_id = PUBLIC_KEY_SHARE_ID
  },
  [SESSION_INIT_ID] =  {
    .request_len = SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES,
    .response_len = sizeof(sl_status_t) + SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES,
    .command_id = SESSION_INIT_ID
  },
  [UNBIND_REQUEST_ID] =  {
    .request_len = 0x0000,
    .response_len = sizeof(sl_status_t),
    .command_id = UNBIND_REQUEST_ID
  },
};
#endif

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/
static void set_state(sl_cpc_security_state_t new_value);

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
static sl_status_t initialize_session(uint8_t *random1, uint8_t *random2);

static bool security_open_endpoint(void);

static psa_algorithm_t get_algorithm(void);

static void security_nonce_increment(sli_cpc_security_nonce_t *nonce);

static sl_status_t store_binding_key(uint8_t *key, uint16_t key_size);

static sl_status_t erase_binding_key(void);

static void security_recover_endpoint(void);

static sl_status_t psa_status_to_sl_status(psa_status_t status);

static void process_security_command_rx(sli_cpc_security_protocol_cmd_t *cmd);

static void send_request(sli_cpc_security_protocol_cmd_t *request);

static psa_status_t encrypt(const uint8_t *header,
                            const size_t header_len,
                            uint8_t *payload,
                            const size_t payload_len,
                            uint8_t *tag,
                            const size_t tag_len);

static psa_status_t decrypt(const uint8_t *header,
                            const size_t header_len,
                            uint8_t *payload,
                            const size_t buffer_size,
                            const size_t payload_len,
                            size_t *output_len);
#endif

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Init Security endpoint
 ******************************************************************************/
sl_status_t sli_cpc_security_init(void)
{
#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_CUSTOMER_SPECIFIC)
  sl_status_t status;
  uint8_t *key = NULL;
  uint16_t key_size = 0;
#endif

  psa_status_t psa_status =  psa_crypto_init();
  EFM_ASSERT(psa_status == PSA_SUCCESS);

  if (psa_is_key_present_in_storage(binding_key_id)) {
    is_bound = true;
  } else {
#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_CUSTOMER_SPECIFIC)
    sl_cpc_security_fetch_user_specified_binding_key(&key, &key_size);
    EFM_ASSERT(key != NULL);
    EFM_ASSERT(key_size == 16);

    status = store_binding_key(key, key_size);
    EFM_ASSERT(status == SL_STATUS_OK);
#else
    is_bound = false;
#endif
  }

  set_state(SL_CPC_SECURITY_STATE_INITIALIZING);
  security_open_endpoint();

#else
  set_state(SL_CPC_SECURITY_STATE_DISABLED);
#endif
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Get Security State
 ******************************************************************************/
sl_cpc_security_state_t sl_cpc_security_get_state(void)
{
  sl_cpc_security_state_t local_state;

  sl_atomic_load(local_state, security_state);

  return local_state;
}

/***************************************************************************//**
 * Callback for getting the user binding key
 ******************************************************************************/
__WEAK void sl_cpc_security_fetch_user_specified_binding_key(uint8_t **key, uint16_t *key_size_in_bytes)
{
  (void)key;
  (void)key_size_in_bytes;
  EFM_ASSERT(0); // User must provide the binding key by overriding this function
}

/***************************************************************************//**
 * Callback to allow unbind request from the Host
 ******************************************************************************/
__WEAK uint64_t sl_cpc_security_on_unbind_request(bool is_link_encrypted)
{
  (void) is_link_encrypted;
  return 0; // Not allowed to unbind by default
}

#if (SL_CPC_ENDPOINT_SECURITY_ENABLED >= 1)
/***************************************************************************//**
 * Process security endpoint
 ******************************************************************************/
void sli_cpc_security_process(void)
{
  sl_status_t status;
  void *read_buffer;
  uint16_t size;

#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
  if (security_endpoint_initialized == false) {
    return;
  }
#endif

  sli_cpc_security_protocol_cmd_t *rx_security_cmd;

  // Recover the security endpoint if it failed
  if (security_endpoint_in_error) {
    security_recover_endpoint();
    if (security_endpoint_in_error) {
      return; // Could not recover the security endpoint retry the process later on
    }
  }

  if (security_write_completed == true) {
    status = sl_cpc_read(&security_ep, &read_buffer, &size, 0u, SL_CPC_FLAG_NO_BLOCK);

    if (status == SL_STATUS_OK) {
      rx_security_cmd = (sli_cpc_security_protocol_cmd_t *)read_buffer;
      process_security_command_rx(rx_security_cmd);
    } else if (status == SL_STATUS_EMPTY) {
      if (security_request_pending) {
        send_request(&security_protocol_request);
        security_request_pending = false;
      }
    } else {
      security_recover_endpoint();
    }
  }
}

sl_status_t sl_cpc_security_unbind(void)
{
  sl_cpc_unbind_notification_handle_t *handle;

  SL_SLIST_FOR_EACH_ENTRY(unbind_observers, handle, sl_cpc_unbind_notification_handle_t, node) {
    EFM_ASSERT(handle->fnct != NULL);
    handle->fnct(handle->data);
  }

  return erase_binding_key();
}

sl_status_t sl_cpc_security_unbind_subscribe(sl_cpc_unbind_notification_handle_t *handle,
                                             sl_cpc_unbind_notification_t callback,
                                             void *data)
{
  if (handle == NULL || callback == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  handle->fnct = callback;
  handle->data = data;

  CORE_ATOMIC_SECTION(sl_slist_push_back(&unbind_observers, &handle->node); );

  return SL_STATUS_OK;
}

sl_status_t sl_cpc_security_unbind_unsubscribe(sl_cpc_unbind_notification_handle_t *handle)
{
  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  handle->fnct = NULL;
  handle->data = NULL;
  sl_slist_remove(&unbind_observers, &handle->node);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Encrypt data
 ******************************************************************************/
sl_status_t sli_cpc_security_encrypt(const uint8_t address,
                                     const uint8_t *header, const size_t header_len,
                                     uint8_t *payload, const size_t payload_len,
                                     uint8_t *tag, const size_t tag_len)
{
  psa_status_t ret;

  nonce_secondary.endpoint_id = address;

  ret = encrypt(header, header_len,
                payload, payload_len,
                tag, tag_len);

  if (ret == PSA_SUCCESS) {
    security_nonce_increment(&nonce_secondary);
  }

  nonce_secondary.endpoint_id = 0;

  return psa_status_to_sl_status(ret);
}

/***************************************************************************//**
 * Decrypt data
 ******************************************************************************/
sl_status_t sli_cpc_security_decrypt(const uint8_t *header, const size_t header_len,
                                     uint8_t *payload, const size_t buffer_len,
                                     const size_t payload_len, size_t *output_len)
{
  psa_status_t ret = decrypt(header, header_len, payload,
                             buffer_len, payload_len, output_len);

  if (ret == PSA_SUCCESS) {
    security_nonce_increment(&nonce_primary);
  }

  return psa_status_to_sl_status(ret);
}

/***************************************************************************//**
 * Packet drop notification
 ******************************************************************************/
void sli_cpc_security_drop_incoming_packet(void)
{
  sl_cpc_security_state_t security_state = sl_cpc_security_get_state();

  if (security_state == SL_CPC_SECURITY_STATE_INITIALIZED) {
    nonce_primary.frame_counter++;
  }
}

/***************************************************************************//**
 * Init Nonce
 ******************************************************************************/
static void security_nonce_init(sli_cpc_security_nonce_t *nonce)
{
  nonce->endpoint_id = 0;
  nonce->frame_counter = SLI_CPC_SECURITY_NONCE_FRAME_COUNTER_RESET_VALUE;
}

/***************************************************************************//**
 * Security error
 ******************************************************************************/
static void on_security_error(uint8_t endpoint_id, void *arg)
{
  (void)arg;

  EFM_ASSERT(endpoint_id == SL_CPC_ENDPOINT_SECURITY);

  if (sl_cpc_get_endpoint_state(&security_ep) != SL_CPC_STATE_OPEN) {
    security_endpoint_in_error = true;
  }
}

/***************************************************************************//**
 * Write completed
 ******************************************************************************/
static void on_security_write_completed(sl_cpc_user_endpoint_id_t endpoint_id,
                                        void *buffer, void *arg, sl_status_t status)
{
  sl_cpc_security_state_t state;
  (void) endpoint_id;
  (void) arg;
  (void) buffer;

  EFM_ASSERT(endpoint_id == SL_CPC_ENDPOINT_SECURITY);

  if (status != SL_STATUS_OK) {
    security_endpoint_in_error = true;
  }

  security_write_completed = true;

  state = sl_cpc_security_get_state();
  if (state == SL_CPC_SECURITY_STATE_WAITING_ON_TX_COMPLETE) {
    set_state(SL_CPC_SECURITY_STATE_INITIALIZED);
  }
}

/***************************************************************************//**
 * Command is a response
 ******************************************************************************/
static bool cmd_is_a_response(sli_cpc_security_protocol_cmd_t *cmd)
{
  if (cmd->command_id & SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK) {
    return true;
  }
  return false;
}

/***************************************************************************//**
 * Send request
 ******************************************************************************/
static void send_request(sli_cpc_security_protocol_cmd_t *request)
{
  EFM_ASSERT(security_write_completed == true);
  security_write_completed = false;

  sl_cpc_write(&security_ep, request,
               request->len + SLI_SECURITY_PROTOCOL_HEADER_LENGTH,
               0,
               (void *)on_security_write_completed);
}

/***************************************************************************//**
 * Write response
 ******************************************************************************/
static void send_response(sli_cpc_security_protocol_cmd_t *response)
{
  EFM_ASSERT(security_write_completed == true);
  security_write_completed = false;

  sl_cpc_write(&security_ep, response,
               response->len + SLI_SECURITY_PROTOCOL_HEADER_LENGTH,
               0,
               (void *)on_security_write_completed);
}

/***************************************************************************//**
 * On Bind command
 ******************************************************************************/
static void on_binding_cmd(sli_cpc_security_protocol_cmd_t *cmd)
{
  uint8_t binding_type;
  sl_status_t status;

  binding_type = cmd->payload[0];
  security_protocol_response.len = sli_cpc_security_command[BINDING_REQUEST_ID].response_len;
  security_protocol_response.command_id = cmd->command_id | SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK;

  if (is_bound) {
    status = SL_STATUS_ALREADY_INITIALIZED;
  } else {
    switch (binding_type) {
      case SLI_CPC_SECURITY_PROTOCOL_BINDING_TYPE_PLAINTEXT:
#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_PLAINTEXT_SHARE)
        status = SL_STATUS_OK;
#else
        status = SL_STATUS_INVALID_MODE;
#endif
        break;
      case SLI_CPC_SECURITY_PROTOCOL_BINDING_TYPE_ECDH:
#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_ECDH)
        status = SL_STATUS_OK;
#else
        status = SL_STATUS_INVALID_MODE;
#endif
        break;
      default:
        status = SL_STATUS_INVALID_MODE;
        break;
    }
  }

  memcpy(&security_protocol_response.payload, &status, sizeof(status));
  send_response(&security_protocol_response);
}

/***************************************************************************//**
 * On plaintext key share command
 ******************************************************************************/
static void on_plaintext_key_share_cmd(sli_cpc_security_protocol_cmd_t *cmd)
{
  sl_status_t status = SL_STATUS_FAIL;

#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_PLAINTEXT_SHARE)
  status = store_binding_key(cmd->payload, SLI_SECURITY_BINDING_KEY_LENGTH_BYTES);
#else
  status = SL_STATUS_INVALID_MODE;
#endif

  security_protocol_response.len = sli_cpc_security_command[cmd->command_id].response_len;
  security_protocol_response.command_id = cmd->command_id | SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK;
  memcpy(&security_protocol_response.payload, &status, sizeof(status));
  send_response(&security_protocol_response);
}

/***************************************************************************//**
 * ECDH Exchange
 ******************************************************************************/
#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_ECDH)
static sl_status_t ecdh_exchange(uint8_t* peer_key, size_t peer_key_len)
{
  psa_key_id_t private_key;
  psa_status_t psa_status;
  psa_key_attributes_t key_attributes;
  sl_status_t status;

  mbedtls_mpi peer_key_big_endian;
  mbedtls_mpi our_public_key_big_endian;
  uint8_t* sha256_output;
  uint8_t* shared_secret;
  mbedtls_mpi shared_secret_big_endian;
  size_t hash_out_len;
  size_t shared_secret_len;

  if (peer_key_len != SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  mbedtls_mpi_init(&peer_key_big_endian);
  mbedtls_mpi_init(&our_public_key_big_endian);
  mbedtls_mpi_init(&shared_secret_big_endian);

  sha256_output = malloc(SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  shared_secret = malloc(SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  EFM_ASSERT(sha256_output != NULL);
  EFM_ASSERT(shared_secret != NULL);

  key_attributes = psa_key_attributes_init();
  psa_set_key_usage_flags(&key_attributes, PSA_KEY_USAGE_DERIVE);
  psa_set_key_algorithm(&key_attributes, PSA_ALG_ECDH);
  psa_set_key_type(&key_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_MONTGOMERY));
  psa_set_key_bits(&key_attributes, 255);

  // Generate private key
  psa_status = psa_generate_key(&key_attributes, &private_key);
  if (psa_status != PSA_SUCCESS) {
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  // Obtain the public key from the keypair
  psa_status = psa_export_public_key(private_key,
                                     our_public_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
                                     &our_public_key_len);
  if (psa_status != PSA_SUCCESS) {
    EFM_ASSERT(psa_destroy_key(private_key) == PSA_SUCCESS);
    private_key = MBEDTLS_SVC_KEY_ID_INIT;
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  // Convert our public key to big endian and the peer key to little endian
  // This is done to work around a limitation in MbedTLS 2.16 LTS that only handles big endian keys with ECP-Montgomery
  mbedtls_mpi_read_binary_le(&our_public_key_big_endian, our_public_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  mbedtls_mpi_write_binary(&our_public_key_big_endian, our_public_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  mbedtls_mpi_read_binary(&peer_key_big_endian, peer_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  mbedtls_mpi_write_binary_le(&peer_key_big_endian, peer_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);

  // Compute ECDH shared secret
  psa_status = psa_raw_key_agreement(PSA_ALG_ECDH,
                                     private_key,
                                     peer_key,
                                     peer_key_len,
                                     shared_secret,
                                     SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
                                     &shared_secret_len);
  if (psa_status != PSA_SUCCESS) {
    EFM_ASSERT(psa_destroy_key(private_key) == PSA_SUCCESS);
    private_key = MBEDTLS_SVC_KEY_ID_INIT;
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  mbedtls_mpi_read_binary(&shared_secret_big_endian, shared_secret, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  mbedtls_mpi_write_binary_le(&shared_secret_big_endian, shared_secret, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);

  psa_status = psa_hash_compute(PSA_ALG_SHA_256, shared_secret, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
                                sha256_output, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES,
                                &hash_out_len);
  if (psa_status != PSA_SUCCESS || hash_out_len != SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES) {
    EFM_ASSERT(psa_destroy_key(private_key) == PSA_SUCCESS);
    private_key = MBEDTLS_SVC_KEY_ID_INIT;
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  // Get rid of the private_key
  psa_status = psa_destroy_key(private_key);
  if (psa_status != PSA_SUCCESS) {
    mbedtls_platform_zeroize(sha256_output, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  private_key = MBEDTLS_SVC_KEY_ID_INIT;

  // Store the binding key by truncating the first bytes from the sha256 output
  if (store_binding_key(sha256_output, SLI_SECURITY_BINDING_KEY_LENGTH_BYTES) != SL_STATUS_OK) {
    status = SL_STATUS_FAIL;
    goto cleanup;
  }

  status = SL_STATUS_OK;

  cleanup:
  mbedtls_platform_zeroize(sha256_output, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  mbedtls_platform_zeroize(shared_secret, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  free(sha256_output);
  free(shared_secret);

  return status;
}
#endif

/***************************************************************************//**
 * On public share command
 ******************************************************************************/
static void on_public_key_share_cmd(sli_cpc_security_protocol_cmd_t *cmd)
{
  sl_status_t status;

#if (SL_CPC_SECURITY_BINDING_KEY_METHOD == SL_CPC_SECURITY_BINDING_KEY_ECDH)
  memcpy(peer_public_key, &cmd->payload, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  status = ecdh_exchange(peer_public_key, SLI_SECURITY_PUBLIC_KEY_LENGTH_BYTES);
  memcpy(security_protocol_response.payload + sizeof(status), our_public_key, our_public_key_len);
#else
  status = SL_STATUS_NOT_SUPPORTED;
#endif

  security_protocol_response.len = sli_cpc_security_command[cmd->command_id].response_len;
  security_protocol_response.command_id = cmd->command_id | SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK;
  memcpy(&security_protocol_response.payload, &status, sizeof(status));
  send_response(&security_protocol_response);
}

/***************************************************************************//**
 * On Session init command
 ******************************************************************************/
static void on_session_init_cmd(sli_cpc_security_protocol_cmd_t *cmd)
{
  sli_cpc_security_session_init_response_t *response = (sli_cpc_security_session_init_response_t*)security_protocol_response.payload;

  if (is_bound) {
    response->status = initialize_session(cmd->payload, response->random2);
  } else {
    response->status = SL_STATUS_NOT_INITIALIZED;
    //Just to be pedantic and not leak anything
    mbedtls_platform_zeroize(response->random2, sizeof(response->random2));
  }

  security_protocol_response.len = sli_cpc_security_command[cmd->command_id].response_len;
  security_protocol_response.command_id = cmd->command_id | SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK;
  send_response(&security_protocol_response);
}

/***************************************************************************//**
 * On unbind command
 ******************************************************************************/
static void on_unbind_cmd(sli_cpc_security_protocol_cmd_t *cmd)
{
  sl_status_t status;
  bool allow_unbind;
  uint64_t ret;

  // According to the spec:
  // The corresponding check for the return value should be glitch-hardened
  // by checking the return value twice in different ways (to avoid the compiler from optimizing it away)
  volatile uint64_t ok_to_unbind_magic = 0xAAAAAAAAAAAAAAAA;
  ret = sl_cpc_security_on_unbind_request(sl_cpc_security_get_state() == SL_CPC_SECURITY_STATE_INITIALIZED);
  if (ret == SL_CPC_SECURITY_OK_TO_UNBIND && ret == ok_to_unbind_magic) {
    allow_unbind = true;
  } else {
    allow_unbind = false;
  }

  if (allow_unbind) {
    status = sl_cpc_security_unbind();
  } else {
    status = SL_STATUS_PERMISSION;
  }

  security_protocol_response.len = sli_cpc_security_command[cmd->command_id].response_len;
  security_protocol_response.command_id = cmd->command_id | SLI_CPC_SECURITY_PROTOCOL_RESPONSE_MASK;
  memcpy(&security_protocol_response.payload, &status, sizeof(status));
  send_response(&security_protocol_response);
}

/***************************************************************************//**
 * Process security command
 ******************************************************************************/
static void process_security_command_rx(sli_cpc_security_protocol_cmd_t *cmd)
{
  if (cmd_is_a_response(cmd)) {
    EFM_ASSERT(0); // Should not receive a response, only requests are allowed
  }

  EFM_ASSERT(cmd->command_id >= BINDING_REQUEST_ID && cmd->command_id <= UNBIND_REQUEST_ID);
  EFM_ASSERT(cmd->len == sli_cpc_security_command[cmd->command_id].request_len);

  // Check if the command is a response or a request
  switch (cmd->command_id) {
    case BINDING_REQUEST_ID:
      on_binding_cmd(cmd);
      break;
    case PLAIN_TEXT_KEY_SHARE_ID:
      on_plaintext_key_share_cmd(cmd);
      break;
    case PUBLIC_KEY_SHARE_ID:
      on_public_key_share_cmd(cmd);
      break;
    case SESSION_INIT_ID:
      on_session_init_cmd(cmd);
      break;
    case UNBIND_REQUEST_ID:
      on_unbind_cmd(cmd);
      break;
    default:
      EFM_ASSERT(0); // Unknown command
  }
}

/***************************************************************************//**
 * Security endpoint open function, will return false if endpoint is busy (CLOSING)
 ******************************************************************************/
static bool security_open_endpoint(void)
{
  sl_status_t status;

  status = sli_cpc_open_service_endpoint(&security_ep, SL_CPC_ENDPOINT_SECURITY, 0, 1);
  if (status == SL_STATUS_BUSY) {
    return false;
  }
  EFM_ASSERT(status == SL_STATUS_OK);

  status = sl_cpc_set_endpoint_option(&security_ep,
                                      SL_CPC_ENDPOINT_ON_IFRAME_WRITE_COMPLETED,
                                      (void *)on_security_write_completed);
  EFM_ASSERT(status == SL_STATUS_OK);

  status = sl_cpc_set_endpoint_option(&security_ep, SL_CPC_ENDPOINT_ON_ERROR, (void *)on_security_error);
  EFM_ASSERT(status == SL_STATUS_OK);

  return true;
}

/***************************************************************************//**
 * Recover security endpoint
 ******************************************************************************/
static void security_recover_endpoint(void)
{
  EFM_ASSERT(SL_STATUS_OK == sl_cpc_close_endpoint(&security_ep));

  if (security_open_endpoint()) {
    security_write_completed = true;
    security_endpoint_in_error = false;
  }
}

/***************************************************************************//**
 * Store binding key
 ******************************************************************************/
static sl_status_t store_binding_key(uint8_t *key, uint16_t key_size)
{
  if (key == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  if (key_size != SLI_SECURITY_BINDING_KEY_LENGTH_BYTES) {
    // Only 128-bits key is supported
    return SL_STATUS_NOT_SUPPORTED;
  }

  if (is_bound) {
    return SL_STATUS_ALREADY_INITIALIZED;
  }
#define PSA_ALG_NONE ((psa_algorithm_t)0)

  // Set key attributes.
  psa_key_attributes_t attr = psa_key_attributes_init();
  psa_set_key_algorithm(&attr, PSA_ALG_NONE);
  psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_EXPORT);
  psa_set_key_type(&attr, PSA_KEY_TYPE_RAW_DATA);
  psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(key_size));
  psa_set_key_id(&attr, binding_key_id);
  psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
                         PSA_KEY_LIFETIME_PERSISTENT, PSA_KEY_LOCATION_LOCAL_STORAGE));

  // Import key.
  psa_key_id_t key_id;
  psa_status_t psa_status = psa_import_key(&attr,
                                           key,
                                           key_size,
                                           &key_id);

  // The key is expected to be in RAM, we need to squash it now that it's loaded in the SE
  mbedtls_platform_zeroize(key, key_size);

#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
  security_endpoint_initialized = true;
#endif

  //Big logic corruption with is_bound flash if the key already exists
  EFM_ASSERT(psa_status != PSA_ERROR_ALREADY_EXISTS);

  // Output result.
  if (psa_status == PSA_SUCCESS) {
    // When registering a persistent key, the returned key_id should be equal to
    // the one set with psa_set_key_id. Something is terribly wrong if it isn't
    EFM_ASSERT(key_id == binding_key_id);
    is_bound = true;
    return SL_STATUS_OK;
  } else {
    return SL_STATUS_FAIL;
  }
}

/***************************************************************************//**
 * Erase binding key
 ******************************************************************************/
sl_status_t erase_binding_key(void)
{
  //Check if the key is already erased
  if (psa_is_key_present_in_storage(binding_key_id) == 0) {
    //Just a sanity check to make sure 'is_bound' flag is consistent with the key status
    EFM_ASSERT(is_bound == false);
    return (sl_status_t) SL_STATUS_NOT_INITIALIZED;
  }

  if (psa_destroy_persistent_key(binding_key_id) != PSA_SUCCESS) {
    return (sl_status_t)SL_STATUS_FAIL;
  }

  //Just a confirmation
  if (psa_is_key_present_in_storage(binding_key_id) == 1) {
    return (sl_status_t)SL_STATUS_FAIL;
  }

  is_bound = false;
  return (sl_status_t)SL_STATUS_OK;
}

/***************************************************************************//**
 * Initialize security session
 ******************************************************************************/
sl_status_t initialize_session(uint8_t *random1, uint8_t *random2)
{
  psa_status_t psa_status;
  const size_t half_random_len = SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES / 2;
  uint8_t random3[SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES];
  uint8_t sha256_random3[SLI_SECURITY_SHA256_LENGTH_BYTES];
  uint8_t random4[SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES + SLI_SECURITY_BINDING_KEY_LENGTH_BYTES];
  uint8_t tmp_session_key[SLI_SECURITY_SESSION_KEY_LENGTH_BYTES] = { 0 };

  // Generate Session ID and Session Key
  {
    // Both devices will construct a string of bits:  Rand-3 = Rand-1[0:255] || Rand-2[0:255]
    {
      // random1 is generated by the primary and supplied to us
      memcpy(&random3[0],
             random1,
             half_random_len);

#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
      // When testing, we need a known random number to assert on the testint side
      size_t i;
      for (i = 0; i != SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES; i++) {
        random2[i] = i;
      }
#else
      // random2 is generated by the secondary
      psa_status = psa_generate_random(random2,
                                       SLI_SECURITY_SESSION_INIT_RANDOM_LENGTH_BYTES);
      if (psa_status != PSA_SUCCESS) {
        return SL_STATUS_FAIL;
      }
#endif

      memcpy(&random3[half_random_len],
             random2,
             half_random_len);
    }

    // Both devices will perform SHA256 on Rand-3
    {
      size_t hash_length;

      psa_status = psa_hash_compute(PSA_ALG_SHA_256,
                                    random3,
                                    sizeof(random3),
                                    sha256_random3,
                                    sizeof(sha256_random3),
                                    &hash_length);

      EFM_ASSERT(hash_length == sizeof(sha256_random3));

      if (psa_status != PSA_SUCCESS) {
        return SL_STATUS_FAIL;
      }
    }

    // The resulting 32-byte number will be split into two 8-byte values as follows: Result = Session-ID-Host || Session-ID-NCP || Discarded data
    {
      memcpy(nonce_primary.session_id,
             &sha256_random3[0],
             SLI_SECURITY_SESSION_ID_LENGTH_BYTES);

      memcpy(nonce_secondary.session_id,
             &sha256_random3[SLI_SECURITY_SESSION_ID_LENGTH_BYTES + 1],
             SLI_SECURITY_SESSION_ID_LENGTH_BYTES);
    }

    // To generate the session key a second string of bits is constructed:
    // Rand-4 = Rand-1[256:511] || Rand-2[256:511] || Binding Key[0:128]
    {
      size_t key_length;

      //Rand-4 = Rand-1[256:511]
      memcpy(&random4[0],
             &random1[half_random_len],
             half_random_len);

      //Rand-4 = || Rand-2[256:511]
      memcpy(&random4[half_random_len],
             &random2[half_random_len],
             half_random_len);

      //Rand-4 = || Binding Key[0:128]
      psa_status = psa_export_key(binding_key_id,
                                  &random4[2 * half_random_len], //
                                  SLI_SECURITY_BINDING_KEY_LENGTH_BYTES,
                                  &key_length);

      // Something terribly wrong if the retrived binding key length is not the
      // same as the one when it was stored
      EFM_ASSERT(key_length == SLI_SECURITY_BINDING_KEY_LENGTH_BYTES);

      if (psa_status != PSA_SUCCESS) {
        // If for whatever reason the export failed, squash random4 anyway
        mbedtls_platform_zeroize(random4, sizeof(random4));
        return SL_STATUS_FAIL;
      }
    }

    // Both devices perform SHA256 on RAND-4
    {
      size_t hash_length;

      psa_status = psa_hash_compute(PSA_ALG_SHA_256,
                                    random4,
                                    sizeof(random4),
                                    tmp_session_key,
                                    sizeof(tmp_session_key),
                                    &hash_length);

      // Squash the random4 containing the binding key in plain text now that its
      // hash has been computed
      mbedtls_platform_zeroize(random4, sizeof(random4));

      // Something is terribly wrong if sha256 doesn't give a 256 length (session key length)
      EFM_ASSERT(hash_length == SLI_SECURITY_SESSION_KEY_LENGTH_BYTES);

      if (psa_status != PSA_SUCCESS) {
        // If for whatever reason the hash failed, squash tmp_session_key anyway
        mbedtls_platform_zeroize(tmp_session_key, sizeof(tmp_session_key));
        return SL_STATUS_FAIL;
      }
    }

    // Destroy previous key if security session is being reset
    if (sl_cpc_security_get_state() == SL_CPC_SECURITY_STATE_RESETTING) {
      psa_destroy_key(session_key_id);
    }

    // The resulting 256 bit number is then used as the session key
    {
      // Set key attributes
      psa_key_attributes_t attr = psa_key_attributes_init();
      psa_set_key_algorithm(&attr, get_algorithm());
      psa_set_key_usage_flags(&attr, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
      psa_set_key_type(&attr, PSA_KEY_TYPE_AES);
      psa_set_key_bits(&attr, PSA_BYTES_TO_BITS(sizeof(tmp_session_key)));
      // psa_set_key_id() is not used because it is a volatile key, and using it
      // would implicitly mark it as persistent. psa_import_key will assign us
      // a key_id
      psa_set_key_lifetime(&attr, PSA_KEY_LIFETIME_VOLATILE);

      // Create the key and retrieve its key_id in session_key_id
      psa_status_t status = psa_import_key(&attr,
                                           tmp_session_key,
                                           sizeof(tmp_session_key),
                                           &session_key_id);

      // Squash the session_key temp buffer now that it is managed by psa_crypto
      mbedtls_platform_zeroize(tmp_session_key, sizeof(tmp_session_key));

      if (status != PSA_SUCCESS) {
        return SL_STATUS_FAIL;
      }
    }
  }

  // Initialize the nonce
  {
    security_nonce_init(&nonce_primary);
    security_nonce_init(&nonce_secondary);
  }

  set_state(SL_CPC_SECURITY_STATE_WAITING_ON_TX_COMPLETE);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Status conversion
 ******************************************************************************/
static sl_status_t psa_status_to_sl_status(psa_status_t status)
{
  switch (status) {
    case PSA_SUCCESS:
      return SL_STATUS_OK;
      break;
    case PSA_ERROR_GENERIC_ERROR:
      return SL_STATUS_FAIL;
      break;
    case PSA_ERROR_NOT_SUPPORTED:
      return SL_STATUS_NOT_SUPPORTED;
      break;
    case PSA_ERROR_NOT_PERMITTED:
      return SL_STATUS_PERMISSION;
      break;
    case PSA_ERROR_BUFFER_TOO_SMALL:
      return SL_STATUS_WOULD_OVERFLOW;
      break;
    case PSA_ERROR_ALREADY_EXISTS:
      return SL_STATUS_ALREADY_EXISTS;
      break;
    case PSA_ERROR_DOES_NOT_EXIST:
      return SL_STATUS_NOT_FOUND;
      break;
    case PSA_ERROR_BAD_STATE:
      return SL_STATUS_INVALID_MODE;
      break;
    case PSA_ERROR_INVALID_ARGUMENT:
      return SL_STATUS_INVALID_PARAMETER;
      break;
    case PSA_ERROR_INSUFFICIENT_MEMORY:
      return SL_STATUS_NO_MORE_RESOURCE;
      break;
    case PSA_ERROR_COMMUNICATION_FAILURE:
      return SL_STATUS_IO;
      break;
    case PSA_ERROR_INVALID_SIGNATURE:
      return SL_STATUS_INVALID_SIGNATURE;
      break;
    case PSA_ERROR_INVALID_HANDLE:
      return SL_STATUS_INVALID_HANDLE;
      break;
    // no specific sl_status_t error code for those
    case PSA_ERROR_INSUFFICIENT_STORAGE:
    case PSA_ERROR_STORAGE_FAILURE:
    case PSA_ERROR_DATA_CORRUPT:
    case PSA_ERROR_DATA_INVALID:
    case PSA_ERROR_HARDWARE_FAILURE:
    case PSA_ERROR_CORRUPTION_DETECTED:
    case PSA_ERROR_INSUFFICIENT_ENTROPY:
    case PSA_ERROR_INVALID_PADDING:
    case PSA_ERROR_INSUFFICIENT_DATA:
    default:
      return SL_STATUS_FAIL;
      break;
  }
}

/***************************************************************************//**
 * Get encryption algorithm
 ******************************************************************************/
static psa_algorithm_t get_algorithm(void)
{
  return PSA_ALG_AEAD_WITH_SHORTENED_TAG(
    PSA_ALG_GCM,
    SLI_SECURITY_TAG_LENGTH_BYTES);
}

/***************************************************************************//**
 * Increment nonce's internal frame counter. Triggers a reset of the security
 * session if the frame counter reaches its maximum allowed value.
 ******************************************************************************/
static void security_nonce_increment(sli_cpc_security_nonce_t *nonce)
{
  nonce->frame_counter++;

  if (nonce->frame_counter == SLI_SECURITY_NONCE_FRAME_COUNTER_MAX_VALUE) {
    if (sl_cpc_security_get_state() == SL_CPC_SECURITY_STATE_INITIALIZED) {
      set_state(SL_CPC_SECURITY_STATE_RESETTING);
    }
  }
}

/***************************************************************************//**
 * Get security flag
 ******************************************************************************/
static size_t __sli_cpc_security_get_tag_length(void)
{
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_status_t ret;

  ret = psa_get_key_attributes(session_key_id, &attributes);
  EFM_ASSERT(ret == PSA_SUCCESS);

  return PSA_AEAD_TAG_LENGTH(
    psa_get_key_type(&attributes),
    psa_get_key_bits(&attributes),
    get_algorithm());
}

/***************************************************************************//**
 * Encrypt data
 ******************************************************************************/
static psa_status_t encrypt(const uint8_t *header,
                            const size_t header_len,
                            uint8_t *payload,
                            const size_t payload_len,
                            uint8_t *tag,
                            const size_t tag_len)
{
  psa_aead_operation_t op = PSA_AEAD_OPERATION_INIT;
  psa_status_t ret;
  size_t out_cypher_len;
  size_t out_tag_len;
  size_t out_len;

  ret = psa_aead_encrypt_setup(&op, session_key_id, get_algorithm());
  if (ret != PSA_SUCCESS) {
    return ret;
  }

  ret = psa_aead_set_lengths(&op, header_len, payload_len);
  if (ret != PSA_SUCCESS) {
    psa_aead_abort(&op);
    return ret;
  }

  ret = psa_aead_set_nonce(&op, (uint8_t*)&nonce_secondary, sizeof(nonce_secondary));
  if (ret != PSA_SUCCESS) {
    psa_aead_abort(&op);
    return ret;
  }

  ret = psa_aead_update_ad(&op, header, header_len);
  if (ret != PSA_SUCCESS) {
    psa_aead_abort(&op);
    return ret;
  }

  ret = psa_aead_update(&op,
                        payload, payload_len,
                        payload, payload_len,
                        &out_len);
  if (ret != PSA_SUCCESS) {
    psa_aead_abort(&op);
    return ret;
  }

  ret = psa_aead_finish(&op,
                        &(payload[out_len]),
                        payload_len - out_len,
                        &out_cypher_len,
                        tag,
                        tag_len,
                        &out_tag_len);
  if (ret != PSA_SUCCESS) {
    psa_aead_abort(&op);
    return ret;
  }

  // We expect the encrypted text to be the same length as the input buffer
  EFM_ASSERT(out_len + out_cypher_len == payload_len);
  EFM_ASSERT(out_tag_len == __sli_cpc_security_get_tag_length());

  return ret;
}

/***************************************************************************//**
 * Decrypt data
 ******************************************************************************/
static psa_status_t decrypt(const uint8_t *header,
                            const size_t header_len,
                            uint8_t *payload,
                            const size_t buffer_size,
                            const size_t payload_len,
                            size_t *output_len)
{
  psa_status_t ret;

  nonce_primary.endpoint_id = sli_cpc_hdlc_get_address(header);

  ret = psa_aead_decrypt(session_key_id,
                         get_algorithm(),
                         (uint8_t*)&nonce_primary,
                         sizeof(nonce_primary),
                         header, header_len,
                         payload, buffer_size,
                         payload, payload_len,
                         output_len);

  if (ret == PSA_SUCCESS) {
    EFM_ASSERT(*output_len + __sli_cpc_security_get_tag_length() == payload_len);
  }

  return ret;
}

#if (defined(SLI_CPC_DEVICE_UNDER_TEST))
/***************************************************************************//**
 * Test session initialization
 ******************************************************************************/
sl_status_t sli_cpc_security_initialize_session(uint8_t *random1, uint8_t *random2)
{
  return initialize_session(random1, random2);
}

/***************************************************************************//**
 * Test store binding key
 ******************************************************************************/
sl_status_t sli_cpc_security_store_binding_key(uint8_t * key, uint16_t key_size)
{
  return store_binding_key(key, key_size);
}

/***************************************************************************//**
 * Test erase binding key
 ******************************************************************************/
sl_status_t sli_cpc_security_erase_binding_key(void)
{
  return erase_binding_key();
}
#endif

#endif // SL_CPC_ENDPOINT_SECURITY_ENABLED

/***************************************************************************//**
 * Set Security State
 ******************************************************************************/
static void set_state(sl_cpc_security_state_t new_value)
{
  sl_atomic_store(security_state, new_value);
}
