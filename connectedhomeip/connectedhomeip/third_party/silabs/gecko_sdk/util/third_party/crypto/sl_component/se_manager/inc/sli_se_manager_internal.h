/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager internal API.
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
#ifndef SE_MANAGER_INTERNAL_H
#define SE_MANAGER_INTERNAL_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)

#include "sl_status.h"
#include "em_se.h"
#include "sl_se_manager.h"
#include "sl_se_manager_key_handling.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Defines

// -------------------------------
// SE status codes

/// Response status codes for the Secure Engine
#define SLI_SE_RESPONSE_MASK                    0x000F0000UL
/// Command executed successfully or signature was successfully validated.
#define SLI_SE_RESPONSE_OK                      0x00000000UL

/// Command was not recognized as a valid command, or is not allowed in the
/// current context.
#define SLI_SE_RESPONSE_INVALID_COMMAND         0x00010000UL
/// User did not provide the required credentials to be allowed to execute the
/// command.
#define SLI_SE_RESPONSE_AUTHORIZATION_ERROR     0x00020000UL
/// Signature validation command (e.g. SE_COMMAND_SIGNATURE_VERIFY) failed to
/// verify the given signature as being correct.
#define SLI_SE_RESPONSE_INVALID_SIGNATURE       0x00030000UL
/// A command started in non-secure mode is trying to access secure memory.
#define SLI_SE_RESPONSE_BUS_ERROR               0x00040000UL
/// Internal error
#define SLI_SE_RESPONSE_INTERNAL_ERROR          0x00050000UL
/// An internal error was raised and the command did not execute.
#define SLI_SE_RESPONSE_CRYPTO_ERROR            0x00060000UL
/// One of the passed parameters is deemed invalid (e.g. out of bounds).
#define SLI_SE_RESPONSE_INVALID_PARAMETER       0x00070000UL
/// Failure while checking the host for secure boot
#define SLI_SE_RESPONSE_SECUREBOOT_ERROR        0x00090000UL
/// Failure during selftest
#define SLI_SE_RESPONSE_SELFTEST_ERROR          0x000A0000UL
/// Feature/item not initialized or not present
#define SLI_SE_RESPONSE_NOT_INITIALIZED         0x000B0000UL
/// Abort status code is given when no operation is attempted.
#define SLI_SE_RESPONSE_ABORT                   0x00FF0000UL
#if defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)
/// Root Code Mailbox is invalid.
#define SLI_SE_RESPONSE_MAILBOX_INVALID         0x00FE0000UL
#endif

// -------------------------------
// SE command words
// Commands are grouped based on availability

#define SLI_SE_COMMAND_CHECK_SE_IMAGE           0x43020000UL
#define SLI_SE_COMMAND_APPLY_SE_IMAGE           0x43030000UL
#define SLI_SE_COMMAND_STATUS_SE_IMAGE          0x43040000UL
#define SLI_SE_COMMAND_CHECK_HOST_IMAGE         0x43050001UL
#define SLI_SE_COMMAND_APPLY_HOST_IMAGE         0x43060001UL
#define SLI_SE_COMMAND_STATUS_HOST_IMAGE        0x43070000UL

#define SLI_SE_COMMAND_READ_OTP                 0xFE040000UL

#define SLI_SE_COMMAND_INIT_OTP                 0xFF000001UL
#define SLI_SE_COMMAND_INIT_PUBKEY              0xFF070001UL
#define SLI_SE_COMMAND_READ_PUBKEY              0xFF080001UL

#define SLI_SE_COMMAND_READ_PUBKEY              0xFF080001UL
#define SLI_SE_COMMAND_READ_OTP                 0xFE040000UL

#define SLI_SE_COMMAND_DBG_LOCK_APPLY           0x430C0000UL

// Commands limited to SE devices
#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
  #define SLI_SE_COMMAND_CREATE_KEY               0x02000000UL
  #define SLI_SE_COMMAND_READPUB_KEY              0x02010000UL

  #define SLI_SE_COMMAND_HASH                     0x03000000UL
  #define SLI_SE_COMMAND_HASHUPDATE               0x03010000UL
  #define SLI_SE_COMMAND_HMAC                     0x03020000UL

  #define SLI_SE_COMMAND_AES_ENCRYPT              0x04000000UL
  #define SLI_SE_COMMAND_AES_DECRYPT              0x04010000UL
  #define SLI_SE_COMMAND_AES_GCM_ENCRYPT          0x04020000UL
  #define SLI_SE_COMMAND_AES_GCM_DECRYPT          0x04030000UL
  #define SLI_SE_COMMAND_AES_CMAC                 0x04040000UL
  #define SLI_SE_COMMAND_AES_CCM_ENCRYPT          0x04050000UL
  #define SLI_SE_COMMAND_AES_CCM_DECRYPT          0x04060000UL

  #define SLI_SE_COMMAND_SIGNATURE_SIGN           0x06000000UL
  #define SLI_SE_COMMAND_SIGNATURE_VERIFY         0x06010000UL
  #define SLI_SE_COMMAND_EDDSA_SIGN               0x06020000UL
  #define SLI_SE_COMMAND_EDDSA_VERIFY             0x06030000UL

  #define SLI_SE_COMMAND_TRNG_GET_RANDOM          0x07000000UL

  #define SLI_SE_COMMAND_JPAKE_R1_GENERATE        0x0B000000UL
  #define SLI_SE_COMMAND_JPAKE_R1_VERIFY          0x0B000100UL
  #define SLI_SE_COMMAND_JPAKE_R2_GENERATE        0x0B010000UL
  #define SLI_SE_COMMAND_JPAKE_R2_VERIFY          0x0B010100UL
  #define SLI_SE_COMMAND_JPAKE_GEN_SESSIONKEY     0x0B020000UL

  #define SLI_SE_COMMAND_DH                       0x0E000000UL

  #define SLI_SE_COMMAND_STATUS_SE_VERSION        0x43080000UL
  #define SLI_SE_COMMAND_STATUS_OTP_VERSION       0x43080100UL
  #define SLI_SE_COMMAND_WRITE_USER_DATA          0x43090000UL
  #define SLI_SE_COMMAND_ERASE_USER_DATA          0x430A0000UL
  #define SLI_SE_COMMAND_DBG_LOCK_ENABLE_SECURE   0x430D0000UL
  #define SLI_SE_COMMAND_DBG_LOCK_DISABLE_SECURE  0x430E0000UL
  #define SLI_SE_COMMAND_DEVICE_ERASE             0x430F0000UL
  #define SLI_SE_COMMAND_DEVICE_ERASE_DISABLE     0x43100000UL
  #define SLI_SE_COMMAND_DBG_LOCK_STATUS          0x43110000UL
  #define SLI_SE_COMMAND_DBG_SET_RESTRICTIONS     0x43120000UL
  #define SLI_SE_COMMAND_PROTECTED_REGISTER       0x43210000UL
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
// SLI_SE_COMMAND_STATUS_READ_RSTCAUSE is only available on xG21 devices (series-2-config-1)
  #define SLI_SE_COMMAND_STATUS_READ_RSTCAUSE     0x43220000UL
#endif
    #define SLI_SE_COMMAND_READ_USER_CERT_SIZE      0x43FA0000UL
    #define SLI_SE_COMMAND_READ_USER_CERT           0x43FB0000UL
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
    #define SLI_SE_COMMAND_ATTEST_PSA_IAT           0x0A030000UL
    #define SLI_SE_COMMAND_ATTEST_CONFIG            0x0A040000UL
  #endif

  #define SLI_SE_COMMAND_GET_CHALLENGE            0xFD000001UL
  #define SLI_SE_COMMAND_ROLL_CHALLENGE           0xFD000101UL
  #define SLI_SE_COMMAND_OPEN_DEBUG               0xFD010001UL

  #define SLI_SE_COMMAND_READ_SERIAL              0xFE000000UL
  #define SLI_SE_COMMAND_GET_STATUS               0xFE010000UL
  #define SLI_SE_COMMAND_READ_PUBKEYBOOT          0xFE020001UL
  #define SLI_SE_COMMAND_SET_UPGRADEFLAG_SE       0xFE030000UL
  #define SLI_SE_COMMAND_SET_UPGRADEFLAG_HOST     0xFE030001UL

  #define SLI_SE_COMMAND_INIT_PUBKEY_SIGNATURE    0xFF090001UL
  #define SLI_SE_COMMAND_READ_PUBKEY_SIGNATURE    0xFF0A0001UL
  #define SLI_SE_COMMAND_INIT_AES_128_KEY         0xFF0B0001UL
#endif // SEMAILBOX_PRESENT

// Commands limited to SE Vault High devices
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
    #define SLI_SE_COMMAND_WRAP_KEY                 0x01000000UL
    #define SLI_SE_COMMAND_UNWRAP_KEY               0x01020000UL
    #define SLI_SE_COMMAND_DELETE_KEY               0x01050000UL
    #define SLI_SE_COMMAND_TRANSFER_KEY             0x01060000UL

    #define SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_HMAC   0x02020002UL
    #define SLI_SE_COMMAND_DERIVE_KEY_HKDF          0x02020003UL
    #define SLI_SE_COMMAND_DERIVE_KEY_PBKDF2_CMAC   0x02020010UL

    #define SLI_SE_COMMAND_CHACHAPOLY_ENCRYPT       0x0C000000UL
    #define SLI_SE_COMMAND_CHACHAPOLY_DECRYPT       0x0C010000UL
    #define SLI_SE_COMMAND_CHACHA20_ENCRYPT         0x0C020000UL
    #define SLI_SE_COMMAND_CHACHA20_DECRYPT         0x0C030000UL
    #define SLI_SE_COMMAND_POLY1305_KEY_MAC         0x0C040000UL

    #define SLI_SE_COMMAND_DISABLE_TAMPER           0xFD020001UL
  #endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

// -------------------------------
// SE command options
// Commands are grouped based on availability

/// Secure boot pubkey
#define SLI_SE_KEY_TYPE_BOOT                    0x00000100UL
/// Secure authorization (debug) pubkey
#define SLI_SE_KEY_TYPE_AUTH                    0x00000200UL

// Options limited to SE devices
#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
/// Root pubkey
  #define SLI_SE_KEY_TYPE_ROOT                    0x00000300UL
  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// Attestation pubkey
    #define SLI_SE_KEY_TYPE_ATTEST                0x00000400UL
  #endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
/// BGL encryption key
  #define SLI_SE_IMMUTABLE_KEY_TYPE_AES_128       0x00000500UL

/// Use MD5 as hash algorithm
  #define SLI_SE_COMMAND_OPTION_HASH_MD5          0x00000100UL
/// Use SHA1 as hash algorithm
  #define SLI_SE_COMMAND_OPTION_HASH_SHA1         0x00000200UL
/// Use SHA224 as hash algorithm
  #define SLI_SE_COMMAND_OPTION_HASH_SHA224       0x00000300UL
/// Use SHA256 as hash algorithm
  #define SLI_SE_COMMAND_OPTION_HASH_SHA256       0x00000400UL

/// Execute algorithm in ECB mode
  #define SLI_SE_COMMAND_OPTION_MODE_ECB          0x00000100UL
/// Execute algorithm in CBC mode
  #define SLI_SE_COMMAND_OPTION_MODE_CBC          0x00000200UL
/// Execute algorithm in CTR mode
  #define SLI_SE_COMMAND_OPTION_MODE_CTR          0x00000300UL
/// Execute algorithm in CFB mode
  #define SLI_SE_COMMAND_OPTION_MODE_CFB          0x00000400UL

/// Run the whole algorithm, all data present
  #define SLI_SE_COMMAND_OPTION_CONTEXT_WHOLE     0x00000000UL
/// Start the algorithm, but get a context to later add more data
  #define SLI_SE_COMMAND_OPTION_CONTEXT_START     0x00000001UL
/// End the algorithm, get the result
  #define SLI_SE_COMMAND_OPTION_CONTEXT_END       0x00000002UL
/// Add more data input to the algorithm. Need to supply previous context,
/// and get a context back
  #define SLI_SE_COMMAND_OPTION_CONTEXT_ADD       0x00000003UL

/// Magic paramater for deleting user data
  #define SLI_SE_COMMAND_OPTION_ERASE_UD          0xDE1E7EADUL
  #define SLI_SE_COMMAND_CERT_BATCH               0x00000100UL
  #define SLI_SE_COMMAND_CERT_SE                  0x00000200UL
  #define SLI_SE_COMMAND_CERT_HOST                0x00000300UL

  #if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// Use SHA384 as hash algorithm
    #define SLI_SE_COMMAND_OPTION_HASH_SHA384       0x00000500UL
/// Use SHA512 as hash algorithm
    #define SLI_SE_COMMAND_OPTION_HASH_SHA512       0x00000600UL
  #endif
#endif // SEMAILBOX_PRESENT

// -------------------------------
// Other defines

#if defined(SEMAILBOX_PRESENT)
// Due to a problem with the countermeasures applied to
// accelerated point multiplication over elliptic curves,
// it is possible that random errors are encountered (this
// is extremely unilkely for truly random keys).
// As a workaround for this, the affected commands will
// retry the operation in order to reduce the probability
// that the error code was returned incorrectly. This helps
// lower the error probability further when using purposely
// small or large scalars, for example during testing.
  #define SLI_SE_MAX_POINT_MULT_RETRIES   3U
#endif

// EFR32xG23+ doesn't require padding of curve elements or other keys
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG) \
  && (_SILICON_LABS_32B_SERIES_2_CONFIG < 3)
  #define SLI_SE_KEY_PADDING_REQUIRED
  #define SLI_SE_P521_PADDING_BYTES 2
#else
  #define SLI_SE_P521_PADDING_BYTES 0
#endif

// -------------------------------
// Function-like macros

/***************************************************************************//**
 * @brief
 *   Helper macro to init/reset the SE command struct of an SE command context
 *
 * @param[in] cmd_ctx
 *   Pointer to SE context containing the command to initialize/reset
 *
 * @param[out] command_word
 *   Command word to set in the SE command.
 *
 ******************************************************************************/
#define sli_se_command_init(cmd_ctx, command_word) \
  cmd_ctx->command.command = command_word;         \
  cmd_ctx->command.data_in = NULL;                 \
  cmd_ctx->command.data_out = NULL;                \
  cmd_ctx->command.num_parameters = 0;

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
/***************************************************************************//**
 * @brief
 *   Helper macros to add key parameters and input/output blocks to SE commands
 *
 * @param[in] cmd_ctx
 *   Pointer to SE context
 * @param[in] key
 *   Pointer to sl_se_key_descriptor_t structure
 * @param[out] status
 *   SL_Status_T
 *
 ******************************************************************************/

// Add keyspec to command for given key
#define sli_add_key_parameters(cmd_ctx, key, status) { \
    uint32_t keyspec;                                  \
    (status) = sli_se_key_to_keyspec((key), &keyspec); \
    if ((status) != SL_STATUS_OK) {                    \
      return (status);                                 \
    }                                                  \
    SE_addParameter(&cmd_ctx->command, keyspec);       \
}

// Add key metadata buffers to command for given key
#define sli_add_key_metadata(cmd_ctx, key, status)        \
  /* Auth data */                                         \
  SE_DataTransfer_t auth_buffer;                          \
  (status) = sli_se_get_auth_buffer((key), &auth_buffer); \
  if ((status) != SL_STATUS_OK) {                         \
    return (status);                                      \
  }                                                       \
  SE_addDataInput(&cmd_ctx->command, &auth_buffer);

// Add key metadata buffers with custom auth buffer to command for given key
#define sli_add_key_metadata_custom(cmd_ctx, auth_data_buf, key, status) \
  /* Auth data */                                                        \
  SE_DataTransfer_t auth_data_buf;                                       \
  (status) = sli_se_get_auth_buffer((key), &auth_data_buf);              \
  if ((status) != SL_STATUS_OK) {                                        \
    return (status);                                                     \
  }                                                                      \
  SE_addDataInput(&cmd_ctx->command, &auth_data_buf);

// Add key input buffer to given command
#define sli_add_key_input(cmd_ctx, key, status)                     \
  SE_DataTransfer_t key_input_buffer;                               \
  (status) = sli_se_get_key_input_output((key), &key_input_buffer); \
  if ((status) != SL_STATUS_OK) {                                   \
    return (status);                                                \
  }                                                                 \
  SE_addDataInput(&cmd_ctx->command, &key_input_buffer);

// Add Key output buffer to given command
#define sli_add_key_output(cmd_ctx, key, status)                     \
  SE_DataTransfer_t key_output_buffer;                               \
  (status) = sli_se_get_key_input_output((key), &key_output_buffer); \
  if ((status) != SL_STATUS_OK) {                                    \
    return (status);                                                 \
  }                                                                  \
  SE_addDataOutput(&cmd_ctx->command, &key_output_buffer);
#endif // SEMAILBOX_PRESENT

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

sl_status_t sli_se_to_sl_status(SE_Response_t res);

/***************************************************************************//**
 * @brief
 *   Take the SE lock in order to synchronize multiple threads calling into
 *   the SE Manager API concurrently.
 *
 * @return
 *   SL_STATUS_OK when successful, or else error code.
 ******************************************************************************/
sl_status_t sli_se_lock_acquire(void);

/***************************************************************************//**
 * @brief
 *   Give the SE lock in order to synchronize multiple threads calling into
 *   the SE Manager API concurrently.
 *
 * @return
 *   SL_STATUS_OK when successful, or else error code.
 ******************************************************************************/
sl_status_t sli_se_lock_release(void);

/***************************************************************************//**
 * @brief
 *   Execute and wait for mailbox command to complete.
 *
 * @param[in] cmd_ctx
 *   Pointer to an SE command context object.
 *
 * @return
 *   Status code, @ref sl_status.h.
 ******************************************************************************/
sl_status_t sli_se_execute_and_wait(sl_se_command_context_t *cmd_ctx);

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
// Key handling helper functions
sl_status_t sli_key_get_storage_size(const sl_se_key_descriptor_t* key,
                                     uint32_t *storage_size);
sl_status_t sli_key_get_size(const sl_se_key_descriptor_t* key, uint32_t* size);
sl_status_t sli_key_check_equivalent(const sl_se_key_descriptor_t* key_1,
                                     const sl_se_key_descriptor_t* key_2,
                                     bool check_key_flag,
                                     bool public_export);

sl_status_t sli_se_key_to_keyspec(const sl_se_key_descriptor_t* key,
                                  uint32_t* keyspec);
sl_status_t sli_se_keyspec_to_key(const uint32_t keyspec,
                                  sl_se_key_descriptor_t* key);
sl_status_t sli_se_get_auth_buffer(const sl_se_key_descriptor_t* key,
                                   SE_DataTransfer_t* auth_buffer);
sl_status_t sli_se_get_key_input_output(const sl_se_key_descriptor_t* key,
                                        SE_DataTransfer_t* buffer);
#endif // SEMAILBOX_PRESENT

#ifdef __cplusplus
}
#endif

#endif /* defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) */

#endif /* SE_MANAGER_INTERNAL_H */
