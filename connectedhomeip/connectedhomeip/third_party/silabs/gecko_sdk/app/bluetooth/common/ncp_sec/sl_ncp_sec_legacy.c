/***************************************************************************//**
 * @file
 * @brief Secure NCP functionality
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include <string.h>
#include "sl_ncp_sec.h"
#include "mbedtls/ecp.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/sha256.h"
#include "mbedtls/ccm.h"
#include "em_core.h"

#define AES_CCM_KEY_SIZE 16
#define ECDH_PUBLIC_KEY_SIZE 32
#define NONCE_SIZE 13

enum user_cmd_codes{
  USER_CMD_INCREASE_SECURITY
};

PACKSTRUCT(struct user_msg_ncp_increase_security_t{
  uint8_t public_keys[64];
  uint8_t host_iv_to_target[4];
  uint8_t host_iv_to_host[4];
});

PACKSTRUCT(struct user_cmd_packet{
  uint8_t code;
  union {
    struct user_msg_ncp_increase_security_t ncp_increase_security;
  } data;
});

typedef struct conn_nonce{
  uint32_t counter;
  uint8_t counter_hi;
  uint8_t host_iv[4];
  uint8_t target_iv[4];
}conn_nonce_t;

typedef struct public_key{
  uint8_t x[ECDH_PUBLIC_KEY_SIZE];
  uint8_t y[ECDH_PUBLIC_KEY_SIZE];
}public_key_t;

enum ncp_sec_state {
  NCP_SEC_UNENCRYPTED = 0,
  NCP_SEC_ENCRYPTED,
  NCP_SEC_ENCRYPTED_AUTHENTICATED
};

// -----------------------------------------------------------------------------
// Private variables

static uint8_t ccm_key[AES_CCM_KEY_SIZE];

static conn_nonce_t ncp_sec_counter_in;
static conn_nonce_t ncp_sec_counter_out;
static mbedtls_ecp_point public;
static mbedtls_mpi private;
static mbedtls_ecp_group ecp_grp;
static mbedtls_ccm_context ccm_ctx;

static uint8_t cipher_msg[CMD_BUF_SIZE];

static enum ncp_sec_state ncp_sec_state;

// -----------------------------------------------------------------------------
// Private function declarations

static void increase_counter(conn_nonce_t *counter);
static int verify_public_key(const uint8_t *pub_x,
                             const uint8_t *pub_y);
static int get_random_data(void *ctx, uint8_t *rand, size_t len);
static int generate_ecc_keypair();
static int ecdh(const uint8_t *host_x, const uint8_t *host_y);
static sl_status_t increase_security(uint8_t *public_keys,
                                     uint8_t *host_iv_to_target,
                                     uint8_t *host_iv_to_host);
static bool validate_security(volatile uint8_t *msg);
static void update_payload_length(sl_bt_msg_t *msg, uint16_t length);
static sl_bt_msg_t* encrypt_message(sl_bt_msg_t *msg);
static sl_bt_msg_t* decrypt_command(sl_bt_msg_t *msg);
static enum ncp_sec_state get_state();

// -----------------------------------------------------------------------------
// Public function definitions

uint32_t sl_ncp_sec_command_handler(uint8_t *data)
{
  sl_bt_msg_t *response = NULL;
  sl_bt_msg_t *command = NULL;
  sl_status_t result = SL_STATUS_FAIL;
  bool cmd_encrypted;
  bool valid_security;

  command = (sl_bt_msg_t *)data;

  if (SL_BT_MSG_ID(command->header) == sl_bt_cmd_user_message_to_target_id) {
    uint8_t user_event_data_len = command->data.evt_user_message_to_host.message.len;
    uint8_t *user_event_data = command->data.evt_user_message_to_host.message.data;

    struct user_cmd_packet *cmd = (struct user_cmd_packet *)user_event_data;
    switch (cmd->code) {
      case USER_CMD_INCREASE_SECURITY:
        if ((user_event_data_len - 1) != sizeof(struct user_msg_ncp_increase_security_t)) {
          // Assemble fail message to host and return
          sl_bt_send_rsp_user_message_to_target(SL_STATUS_FAIL, 0, NULL);
          return SL_NCP_SEC_RSP_PROCESS;
        }
        result = increase_security(
          cmd->data.ncp_increase_security.public_keys,
          cmd->data.ncp_increase_security.host_iv_to_target,
          cmd->data.ncp_increase_security.host_iv_to_host);
        // Assemble response message to host and return
        sl_bt_send_rsp_user_message_to_target(
          result,
          sizeof(struct user_msg_ncp_increase_security_t) + 1,
          user_event_data);
        return SL_NCP_SEC_RSP_PROCESS;
      default:
        // Other message arrived, decrypt it if needed
        break;
    }
  }

  // -------------------------------
  // De-crypt received message

  cmd_encrypted = sl_ncp_sec_is_encrypted(command);
  valid_security = validate_security((uint8_t *)command);

  if ((cmd_encrypted && get_state() == NCP_SEC_UNENCRYPTED)
      || !valid_security) {
    sl_bt_send_system_error(SL_STATUS_BT_APPLICATION_MISMATCHED_OR_INSUFFICIENT_SECURITY,
                            0, NULL);
    return SL_NCP_SEC_EVT_PROCESS;
  } else {
    response = decrypt_command(command);
    if (response == NULL) {
      sl_bt_send_system_error(SL_STATUS_BT_APPLICATION_ENCRYPTION_DECRYPTION_ERROR,
                              0, NULL);
      return SL_NCP_SEC_EVT_PROCESS;
    } else {
      memcpy(data, response, CMD_BUF_SIZE);
      return (SL_NCP_SEC_CMD_PROCESS | SL_NCP_SEC_RSP_PROCESS);
    }
  }
}

bool sl_ncp_sec_is_encrypted(volatile void *msg)
{
  uint32_t cmd_header = *((uint32_t *)msg);
  return (SL_BT_MSG_ENCRYPTED(cmd_header) !=  0);
}

sl_bt_msg_t* sl_ncp_sec_process_event(sl_bt_msg_t *msg)
{
  if (ncp_sec_state == NCP_SEC_UNENCRYPTED || SL_BT_MSG_ID(msg->header)
      == sl_bt_evt_system_error_id) {
    return msg;
  }

  return encrypt_message(msg);
}

sl_bt_msg_t* sl_ncp_sec_process_response(sl_bt_msg_t *msg,
                                         bool cmd_was_encrypted)
{
  if (ncp_sec_state == NCP_SEC_UNENCRYPTED || !cmd_was_encrypted) {
    return msg;
  }

  return encrypt_message(msg);
}

// -----------------------------------------------------------------------------
// Private function definitions

static void increase_counter(conn_nonce_t *counter)
{
  if (counter->counter == UINT32_MAX) {
    counter->counter_hi++;
  }
  counter->counter++;
}

static int verify_public_key(const uint8_t *pub_x,
                             const uint8_t *pub_y)
{
  mbedtls_ecp_point public_key;
  mbedtls_ecp_point_init(&public_key);

  mbedtls_mpi_read_binary(&public_key.MBEDTLS_PRIVATE(X), pub_x, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_read_binary(&public_key.MBEDTLS_PRIVATE(Y), pub_y, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_lset(&public_key.MBEDTLS_PRIVATE(Z), 1);

  int ret = mbedtls_ecp_check_pubkey(&ecp_grp, &public_key);
  mbedtls_ecp_point_free(&public_key);

  return ret;
}

static int get_random_data(void *ctx, uint8_t *rand, size_t len)
{
  (void)ctx;
  size_t i = 0;
  uint8_t data[16];
  size_t out_len;
  while (len > 0) {
    if (len > 16) {
      sl_bt_system_get_random_data(16, 16, &out_len, data);
      memcpy(rand + i, data, out_len);
    } else {
      sl_bt_system_get_random_data(len, 16, &out_len, data);
      memcpy(rand + i, data, out_len);
    }
    i += out_len;
    len -= out_len;
  }
  return 0;
}

static int generate_ecc_keypair()
{
  int ret = 0;
  mbedtls_mpi_free(&private);
  mbedtls_mpi_init(&private);
  mbedtls_ecp_point_free(&public);
  mbedtls_ecp_point_init(&public);

  ret = mbedtls_ecdh_gen_public(&ecp_grp, &private, &public,
                                get_random_data, NULL);
  return ret;
}

static int ecdh(const uint8_t *host_x, const uint8_t *host_y)
{
  int ret = 0;
  uint8_t sha265_input[ECDH_PUBLIC_KEY_SIZE];
  uint8_t sha265_output[ECDH_PUBLIC_KEY_SIZE];
  mbedtls_ecp_point public_key;
  mbedtls_ecp_point_init(&public_key);

  mbedtls_mpi_read_binary(&public_key.MBEDTLS_PRIVATE(X), host_x, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_read_binary(&public_key.MBEDTLS_PRIVATE(Y), host_y, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_lset(&public_key.MBEDTLS_PRIVATE(Z), 1);

  mbedtls_mpi shared_secred;
  mbedtls_mpi_init(&shared_secred);    //shared secret
  mbedtls_mpi_lset(&shared_secred, 0);

  ret = mbedtls_ecdh_compute_shared(&ecp_grp, &shared_secred, &public_key,
                                    &private, get_random_data,
                                    NULL);
  mbedtls_ecp_point_free(&public_key);
  if (ret) {
    mbedtls_mpi_free(&shared_secred);
    return ret;
  }

  mbedtls_mpi_write_binary(&shared_secred, sha265_input, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_free(&shared_secred);

  ret = mbedtls_sha256(sha265_input, ECDH_PUBLIC_KEY_SIZE, sha265_output, 0);
  memcpy(ccm_key, sha265_output, AES_CCM_KEY_SIZE);

  return ret;
}

/***************************************************************************//**
 * @brief
 *   Increases security on NCP if command arrives. Checks the got keys.
 *
 * @param[in] msg
 *   Pointer to the public key.
 *   Pointer to the host_iv_to_target.
 *   Pointer to the host_iv_to_host.
 *
 * @return
 *   Ok if the keys are valid and secure connection can be established
 *   Error otherwise.
 ******************************************************************************/
static sl_status_t increase_security(uint8_t *public_keys,
                                     uint8_t *host_iv_to_target,
                                     uint8_t *host_iv_to_host)
{
  mbedtls_ecp_group_free(&ecp_grp);
  mbedtls_ecp_group_init(&ecp_grp);
  mbedtls_ecp_group_load(&ecp_grp, MBEDTLS_ECP_DP_SECP256R1);

  public_key_t *host_public = (public_key_t *)public_keys;
  if (verify_public_key(host_public->x, host_public->y)) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  ncp_sec_state = NCP_SEC_UNENCRYPTED;

  ncp_sec_counter_in.counter = 0;
  ncp_sec_counter_in.counter_hi = 0;
  ncp_sec_counter_out.counter = 0;
  ncp_sec_counter_out.counter_hi = 0;

  memcpy(ncp_sec_counter_in.host_iv, host_iv_to_target,
         sizeof(ncp_sec_counter_in.host_iv));
  memcpy(ncp_sec_counter_out.host_iv, host_iv_to_host,
         sizeof(ncp_sec_counter_out.host_iv));

  get_random_data(NULL, ncp_sec_counter_in.target_iv,
                  sizeof(ncp_sec_counter_in.target_iv));
  get_random_data(NULL, ncp_sec_counter_out.target_iv,
                  sizeof(ncp_sec_counter_out.target_iv));
  if (generate_ecc_keypair()) {
    memset(host_iv_to_target, 0, sizeof(ncp_sec_counter_in.target_iv));
    memset(host_iv_to_host, 0, sizeof(ncp_sec_counter_out.target_iv));
    memset(host_public, 0, sizeof(public_key_t));
    return SL_STATUS_BT_CRYPTO;
  }

  if (ecdh(host_public->x, host_public->y)) {
    memset(host_iv_to_target, 0, sizeof(ncp_sec_counter_in.target_iv));
    memset(host_iv_to_host, 0, sizeof(ncp_sec_counter_out.target_iv));
    memset(host_public, 0, sizeof(public_key_t));
    return SL_STATUS_BT_CRYPTO;
  }

  // Copy target data over the host data for response
  memcpy(host_iv_to_target, ncp_sec_counter_in.target_iv,
         sizeof(ncp_sec_counter_in.target_iv));
  memcpy(host_iv_to_host, ncp_sec_counter_out.target_iv,
         sizeof(ncp_sec_counter_out.target_iv));
  mbedtls_mpi_write_binary(&public.MBEDTLS_PRIVATE(X), host_public->x, ECDH_PUBLIC_KEY_SIZE);
  mbedtls_mpi_write_binary(&public.MBEDTLS_PRIVATE(Y), host_public->y, ECDH_PUBLIC_KEY_SIZE);

  mbedtls_ccm_free(&ccm_ctx);
  mbedtls_ccm_init(&ccm_ctx);
  mbedtls_ccm_setkey(&ccm_ctx, MBEDTLS_CIPHER_ID_AES, ccm_key,
                     AES_CCM_KEY_SIZE * 8);

  ncp_sec_state = NCP_SEC_ENCRYPTED;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * @brief
 *   Validate if received packet has right level of security.
 *
 * @details
 *   Only certain unencrypted commands are allowed when the security is
 *   encrypted or higher. No encrypted commands are allowed when the security
 *   state is unencrypted.
 *
 * @param[in] msg
 *   Pointer to NCP command packet.
 *
 * @return
 *   True if security is valid, otherwise False.
 *
 ******************************************************************************/
static bool validate_security(volatile uint8_t *msg)
{
  uint32_t cmd_header = *((uint32_t *)msg);

  uint8_t sensitive = sl_bt_is_sensitive_message(SL_BT_MSG_ID(cmd_header));
  if (sensitive && !SL_BT_MSG_ENCRYPTED(cmd_header)) {
    return false;
  }

  return true;
}

/***************************************************************************//**
 * @brief
 *   Set callback function for transmit.
 *
 * @details
 *   This function is used to update the payload length in NCP packet header.
 *
 * @param[in] msg
 *   Pointer to NCP packet.
 *
 * @param[in] length
 *   New length of payload.
 *
 ******************************************************************************/
static void update_payload_length(sl_bt_msg_t *msg,
                                  uint16_t length)
{
  uint8_t *header = (uint8_t *)&msg->header;
  header[1] = length & 0xff;
  header[0] &= ~0x07;
  header[0] |= ((length & 0x0700) >> 8);
}

static sl_bt_msg_t* encrypt_message(sl_bt_msg_t *msg)
{
  uint16_t len = SL_BT_MSG_LEN(msg->header);
  memcpy((void *)cipher_msg, msg, sizeof(msg->header));

  sl_bt_msg_t *new_msg = (sl_bt_msg_t *)cipher_msg;
  update_payload_length(new_msg, len + 9);
  new_msg->header |= SL_BT_BIT_ENCRYPTED;

  uint8_t auth_data[7];
  memcpy(auth_data, cipher_msg, 2);
  memcpy(auth_data + 2, &ncp_sec_counter_out.counter, 4);
  auth_data[6] = ncp_sec_counter_out.counter_hi;

  // Encrypt all data except two first bytes of header
  // and counter which are signed
  int ret = mbedtls_ccm_encrypt_and_tag(&ccm_ctx,
                                        len + 2,
                                        (uint8_t *)&ncp_sec_counter_out,
                                        NONCE_SIZE,
                                        auth_data, sizeof(auth_data),
                                        (uint8_t *)msg + 2,
                                        cipher_msg + 2,
                                        cipher_msg + len + 4,
                                        4);
  if (ret) {
    return NULL;
  }

  // Add counter to end
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  memcpy(cipher_msg + len + 8, auth_data + 2, 5);

  increase_counter(&ncp_sec_counter_out);
  CORE_EXIT_ATOMIC();
  return new_msg;
}

/***************************************************************************//**
 * @brief
 *   Decrypt the command packet.
 *
 * @param[in] msg
 *   Pointer to NCP packet.
 *
 * @return
 *   Pointer to the new decrypted packet.
 ******************************************************************************/
static sl_bt_msg_t* decrypt_command(sl_bt_msg_t *msg)
{
  // If not encrypted, just return the same
  if (ncp_sec_state == NCP_SEC_UNENCRYPTED
      || !SL_BT_MSG_ENCRYPTED(msg->header)) {
    return msg;
  }

  uint16_t len = SL_BT_MSG_LEN(msg->header);
  memcpy((void *)cipher_msg, msg, sizeof(msg->header));

  // Verify counter to prevent replay attacks
  conn_nonce_t nonce;
  memcpy(&nonce.counter, (uint8_t *)msg + len + 4 - 5, 4);
  memcpy(&nonce.counter_hi, (uint8_t *)msg + len + 4 - 1, 1);
  memcpy(nonce.host_iv, ncp_sec_counter_in.host_iv,
         sizeof(ncp_sec_counter_in.host_iv));
  memcpy(nonce.target_iv, ncp_sec_counter_in.target_iv,
         sizeof(ncp_sec_counter_in.target_iv));
  if ((nonce.counter_hi > ncp_sec_counter_in.counter_hi
       || (nonce.counter_hi == ncp_sec_counter_in.counter_hi
           && nonce.counter >= ncp_sec_counter_in.counter)) == 0) {
    return NULL;
  }

  uint8_t auth_data[7];
  memcpy(auth_data, cipher_msg, 2);
  memcpy(auth_data + 2, &nonce.counter, 4);
  auth_data[6] = nonce.counter_hi;

  // Decrypt all data except two header's first bytes of header
  // and counter which are signed
  int ret = mbedtls_ccm_auth_decrypt(&ccm_ctx,
                                     len + 2 - 9,
                                     (uint8_t *)&nonce, NONCE_SIZE,
                                     auth_data, sizeof(auth_data),
                                     (uint8_t *)msg + 2,
                                     cipher_msg + 2,
                                     (uint8_t *)msg + len + 4 - 9,
                                     4);
  if (ret) {
    return NULL;
  }

  sl_bt_msg_t *new_msg = (sl_bt_msg_t *)cipher_msg;
  update_payload_length(new_msg, len - 9);
  new_msg->header &= ~SL_BT_BIT_ENCRYPTED;

  // Update counter
  if (ncp_sec_counter_in.counter != nonce.counter
      || ncp_sec_counter_in.counter_hi != nonce.counter_hi) {
    ncp_sec_counter_in.counter = nonce.counter;
    ncp_sec_counter_in.counter_hi = nonce.counter_hi;
  }
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_ATOMIC();
  increase_counter(&ncp_sec_counter_in);
  CORE_EXIT_ATOMIC();

  return new_msg;
}

/***************************************************************************//**
 * @brief
 *   Gets the current NCP security state
 *
 * @return
 *   the current NCP security state
 ******************************************************************************/
static enum ncp_sec_state get_state()
{
  return ncp_sec_state;
}
