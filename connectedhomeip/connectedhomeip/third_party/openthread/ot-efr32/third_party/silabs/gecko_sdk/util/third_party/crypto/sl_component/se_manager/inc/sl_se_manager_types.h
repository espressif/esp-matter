/***************************************************************************//**
 * @file
 * @brief Silicon Labs Secure Engine Manager API types
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
#ifndef SL_SE_MANAGER_TYPES_H
#define SL_SE_MANAGER_TYPES_H

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager
/// @{

#include "sl_se_manager_defines.h"
#include "em_se.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Typedefs

/// @addtogroup sl_se_manager_util
/// @{

/// OTP key types
typedef enum {
  SL_SE_KEY_TYPE_IMMUTABLE_BOOT = 0,
  SL_SE_KEY_TYPE_IMMUTABLE_AUTH,
#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
  SL_SE_KEY_TYPE_IMMUTABLE_AES_128,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
  SL_SE_KEY_TYPE_IMMUTABLE_ATTESTATION,
  SL_SE_KEY_TYPE_IMMUTABLE_SE_ATTESTATION,
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
#endif // SEMAILBOX_PRESENT
} sl_se_device_key_type_t;

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// SE tamper signal levels
typedef uint8_t sl_se_tamper_level_t;

/// SE tamper signals
typedef uint32_t sl_se_tamper_signals_t;

/// SE tamper filter timeout period
typedef uint8_t sl_se_tamper_filter_period_t;

/// Number of tamper counts to trigger the filter signal
typedef uint8_t sl_se_tamper_filter_threshold_t;
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT

/// Certificate size data structure
typedef struct {
  uint32_t batch_id_size;    ///< size in bytes of the Batch certificate
  uint32_t se_id_size;       ///< size in bytes of the SE ID certificate
  uint32_t host_id_size;     ///< size in bytes of the Host ID certificate
} sl_se_cert_size_type_t;

/// SE certificate types
typedef uint8_t sl_se_cert_type_t;

/// OTP initialization data structure
typedef struct {
  /// Enable secure boot for the host.
  bool enable_secure_boot;
  /// Require certificate based secure boot signing.
  bool verify_secure_boot_certificate;
  /// Enable anti-rollback for host application upgrades.
  bool enable_anti_rollback;
  /// Set flag to enable locking down all flash pages that cover the
  /// secure-booted image, except the last page if end of signature is not
  /// page-aligned.
  bool secure_boot_page_lock_narrow;
  /// Set flag to enable locking down all flash pages that cover the
  /// secure-booted image, including the last page if end of signature is not
  /// page-aligned.
  bool secure_boot_page_lock_full;
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
  /// List of tamper levels to configure for the different tamper sources.
  sl_se_tamper_level_t tamper_levels[SL_SE_TAMPER_SIGNAL_NUM_SIGNALS];
  /// Reset period for the tamper filter counter.
  sl_se_tamper_filter_period_t tamper_filter_period;
  /// Activation threshold for the tamper filter.
  sl_se_tamper_filter_threshold_t tamper_filter_threshold;
  /// Tamper flags.
  uint8_t tamper_flags;
  /// Tamper reset halt threshold.
  uint8_t tamper_reset_threshold;
#endif // _SILICON_LABS_SECURITY_FEATURE_VAULT
} sl_se_otp_init_t;

/// @} (end addtogroup sl_se_manager_util)

/// @addtogroup sl_se_manager_core
/// @{

/***************************************************************************//**
 * @brief          SE mailbox command context
 *
 * @details
 *   This structure defines the common SE mailbox command context used for
 *   all SE Manager API functions that execute SE mailbox commands. The
 *   members of this context structure should be considered internal to the
 *   SE Manager and should not be read or written directly by the user
 *   application. For members that are relevant for the user, the user can
 *   access them via corresponding set and get API functions, e.g.
 *   sl_se_set_yield().
 ******************************************************************************/
typedef struct sl_se_command_context_t {
  SE_Command_t  command;             ///< SE mailbox command struct
#if defined(SL_SE_MANAGER_YIELD_WHILE_WAITING_FOR_COMMAND_COMPLETION) || defined(DOXYGEN)
  bool          yield;               ///< If true, yield the CPU core while
                                     ///< waiting for the SE mailbox command
                                     ///< to complete. If false, busy-wait, by
                                     ///< polling the SE mailbox response
                                     ///< register.
#endif // SEMAILBOX_PRESENT
} sl_se_command_context_t;

/// @} (end addtogroup sl_se_manager_core)

/// @addtogroup sl_se_manager_util
/// @{

/// SE Debug lock flags
typedef uint32_t sl_se_debug_flags_t;

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
/// Debug lock options
typedef struct {
  /// Non-Secure, Invasive debug access enabled if true. If false, it is not
  /// possible to debug the non-secure state in a way that is intrusive to
  /// program execution (DBGLOCK locked).
  bool non_secure_invasive_debug;
  /// Non-Secure, Non-Invasive debug access enabled if true. If false, it is
  /// not possible to debug the non-secure state in a way that is intrusive to
  /// program execution (NIDLOCK locked).
  bool non_secure_non_invasive_debug;
  /// Secure, Invasive debug access enabled if true. If false, it is not
  /// possible to debug the secure TrustZone state in a way that is intrusive
  /// to program execution (SPIDLOCK locked).
  bool secure_invasive_debug;
  /// Secure, Non-Invasive debug access enabled if true. If false, it is not
  /// possible to observe the secure TrustZone state using trace.
  /// (SPNIDLOCK is locked. However if SPIDLOCK is open, SPNIDLOCK will also
  /// remain open.)
  bool secure_non_invasive_debug;
} sl_se_debug_options_t;
#endif

/// Debug status
typedef struct {
  /// Whether device erase is enabled
  bool device_erase_enabled;
  /// Whether secure debug is enabled with @ref sl_se_enable_secure_debug().
  bool secure_debug_enabled;
  /// Whether the debug port has been locked with @ref sl_se_apply_debug_lock().
  /// This parameter does not indicate if the debug port has been unlocked by
  /// calling @ref sl_se_open_debug().
  bool debug_port_lock_applied;
  /// Current state of the debug port.
  /// True if locked with @ref sl_se_apply_debug_lock().
  /// False if new clean, erased or unlocked with @ref sl_se_open_debug().
  bool debug_port_lock_state;
  #if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)
  /// Debug option configuration as set by @ref sl_se_set_debug_options().
  sl_se_debug_options_t options_config;
  /// Current state of debug options, locked by @ref sl_se_set_debug_options() and
  /// unlocked by @ref sl_se_open_debug().
  sl_se_debug_options_t options_state;
  #endif
} sl_se_debug_status_t;

/// @} (end addtogroup sl_se_manager_util)

#if defined(SEMAILBOX_PRESENT) || defined(DOXYGEN)

/// @addtogroup sl_se_manager_key_handling
/// @{

/// Supported key types
typedef uint32_t sl_se_key_type_t;

/// Key storage method. Can have one of @ref SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT,
/// @ref SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED,
/// @ref SL_SE_KEY_STORAGE_INTERNAL_VOLATILE or
/// @ref SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE.
typedef uint32_t sl_se_storage_method_t;

/// Internal SE key slot
typedef uint32_t sl_se_key_slot_t;

/// Describes where the key is or should be stored
typedef struct {
  uint8_t* pointer; ///< Pointer to a key buffer.
  uint32_t size;    ///< Size of buffer.
} sl_se_buffer_t;

/// Describes the storage location of keys
typedef struct {
  /// Key storage method. Sets meaning of data in location.
  sl_se_storage_method_t method;
  /// Describes key storage location. @ref sl_se_buffer_t is used if @ref sl_se_key_storage_t.method is
  /// @ref SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT or
  /// @ref SL_SE_KEY_STORAGE_EXTERNAL_WRAPPED, while @ref sl_se_key_slot_t is
  /// used for @ref SL_SE_KEY_STORAGE_INTERNAL_VOLATILE or
  /// @ref SL_SE_KEY_STORAGE_INTERNAL_IMMUTABLE.
  union {
    sl_se_buffer_t buffer;
    sl_se_key_slot_t slot;
  } location;
} sl_se_key_storage_t;

/// Contains a full description of a key used by an SE command.
typedef struct {
  /// Key type
  sl_se_key_type_t type;
  /// Key size, applicable if key_type == SYMMETRIC
  size_t size;
  /// Flags describing restrictions, permissions and attributes of the key.
  uint32_t flags;
  /// Storage location for this key
  sl_se_key_storage_t storage;
  /// Optional password for key usage (8 bytes). If no password is provided
  /// (NULL pointer), any key not stored as plaintext will be stored with a
  /// password of all-zero bytes.
  uint8_t* password;
  /// Pointer to domain descriptor if this key contains an asymmetric key on a
  /// custom domain The reason for pointing instead of containing is to make
  /// it possible to have the parameters in ROM.
  const void* domain;
} sl_se_key_descriptor_t;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// Custom Weierstrass curve structure.
typedef struct {
  /// Domain size in bytes.
  const size_t size;
  /// Modulus p (zero-padded from MSB, right-adjusted to extend to 32-bit
  /// alignment up from domain size)
  const uint8_t* p;
  /// Order N (zero-padded from MSB, right-adjusted to extend to 32-bit
  /// alignment up from domain size)
  const uint8_t* N;
  /// Generator X-coordinate  (zero-padded from MSB, right-adjusted to extend
  /// to 32-bit alignment up from domain size)
  const uint8_t* Gx;
  /// Generator Y-coordinate  (zero-padded from MSB, right-adjusted to extend
  /// to 32-bit alignment up from domain size)
  const uint8_t* Gy;
  /// Parameter a  (zero-padded from MSB, right-adjusted to extend to 32-bit
  /// alignment up from domain size)
  const uint8_t* a;
  /// Parameter b  (zero-padded from MSB, right-adjusted to extend to 32-bit
  /// alignment up from domain size)
  const uint8_t* b;
  /// Set if a equals 0
  bool a_is_zero;
  /// Set if a equals -3
  bool a_is_minus_three;
} sl_se_custom_weierstrass_prime_domain_t;
#endif

/// @} (end addtogroup sl_se_manager_key_handling)

/// @addtogroup sl_se_manager_util
/// @{

/// SE challenge storage
typedef uint8_t sl_se_challenge_t[SL_SE_CHALLENGE_SIZE];

/// SE status
typedef struct {
  /// Boot status code / error code (Bits [7:0]).
  uint32_t boot_status;
  /// SE firmware version.
  uint32_t se_fw_version;
  /// Host firmware version (if available).
  uint32_t host_fw_version;
  /// Debug lock status.
  sl_se_debug_status_t debug_status;
  /// Secure boot enabled.
  bool secure_boot_enabled;
  /// Recorded tamper status. Reset on status read.
  uint32_t tamper_status;
  /// Currently active tamper sources.
  uint32_t tamper_status_raw;
} sl_se_status_t;

/// @} (end addtogroup sl_se_manager_util)

/// @addtogroup sl_se_manager_cipher
/// @{

/// Cipher operation types
typedef enum {
  SL_SE_ENCRYPT,
  SL_SE_DECRYPT
} sl_se_cipher_operation_t;

/// CMAC streaming context
typedef struct {
  uint8_t                      state[16];       ///< CMAC state
  uint8_t                      data_in[16];     ///< Unprocessed data
  uint8_t                      data_out[16];    ///< Last 16 bytes of cipher-text
  size_t                       length;          ///< Length of all processed and unprocessed data
} sl_se_cmac_multipart_context_t;

/// CMAC streaming context. Deprecated.
typedef struct {
  sl_se_command_context_t      *cmd_ctx;        ///< Pointer to command context object
  const sl_se_key_descriptor_t *key;            ///< Pointer to key object
  sl_se_cmac_multipart_context_t cmac_ctx;    ///< CMAC streaming context
} sl_se_cmac_streaming_context_t;

/// CCM streaming context.
typedef struct {
  uint32_t processed_message_length;///< Current length of the encrypted/decrypted data
  uint32_t total_message_length;    ///< Total length of data to be encrypted/decrypted
  uint8_t  iv[13];                  ///< Nonce (MAX size is 13 bytes)
  uint32_t tag_len;                 ///< Tag length
  sl_se_cipher_operation_t     mode;///< CCM mode (decrypt or encrypt)
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG == 1)
  uint8_t nonce_counter[16];        ///< Counter to keep CTR state
  uint8_t iv_len;                   ///< Nonce length
  uint8_t cbc_mac_state[16];        ///< State of authenication/MAC
  uint8_t final_data[16];           ///< Input data saved for finish operation
  #else
  uint8_t  se_ctx[32];              ///< SE encryption state
  union {
    uint8_t tagbuf[16];             ///< Tag
    uint8_t final_data[16];         ///< Input data saved for finish operation
  } mode_specific_buffer;
  #endif
  uint8_t final_data_length;        ///< Length of data saved
} sl_se_ccm_multipart_context_t;

/// GCM streaming context. Deprecated.
typedef struct {
  sl_se_command_context_t *cmd_ctx; ///< Pointer to command context object
  const sl_se_key_descriptor_t *key;///< Pointer to key object
  uint64_t len;                     ///< Total length of the encrypted data
  uint64_t add_len;                 ///< Total length of the additional data
  uint8_t  se_ctx_enc[32];          ///< SE encryption state
  uint8_t  se_ctx_dec[32];          ///< SE decryption state
  uint8_t  tagbuf[16];              ///< Tag
  int      mode;                    ///< GCM mode
  bool     last_op;                 ///< Last operation / update
} sl_se_gcm_streaming_context_t;

typedef struct {
  uint64_t len;                     ///< Total length of the encrypted data
  uint64_t add_len;                 ///< Total length of the additional data
  #if (_SILICON_LABS_32B_SERIES_2_CONFIG < 3)
  uint8_t  tagbuf[16];              ///< Tag
  uint8_t previous_se_ctx[32];      ///< SE state from previous operation
  #endif
  uint8_t se_ctx[32];               ///< SE state
  uint8_t final_data[16];           ///< Input data saved for finish operation
  uint8_t final_data_length;        ///< Length of data saved
  sl_se_cipher_operation_t     mode;///< GCM mode
  bool    first_operation;          ///< First operation
} sl_se_gcm_multipart_context_t;

/// @} (end addtogroup sl_se_manager_cipher)

/// @addtogroup sl_se_manager_hash
/// @{

/// Hash algorithms
typedef enum {
  SL_SE_HASH_NONE,      ///< No hash
  SL_SE_HASH_SHA1,      ///< SHA-1
  SL_SE_HASH_SHA224,    ///< SHA-224
  SL_SE_HASH_SHA256,    ///< SHA-256
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
  SL_SE_HASH_SHA384,    ///< SHA-384
  SL_SE_HASH_SHA512,    ///< SHA-512
#endif
} sl_se_hash_type_t;

/// Generic hash streaming context. Deprecated.
typedef struct {
  sl_se_command_context_t *cmd_ctx;   ///< Pointer to command context object
  sl_se_hash_type_t        hash_type; ///< Hash type
  size_t                   size;      ///< Hash output size
  void                    *hash_type_ctx; ///< Pointer to hash specific context
} sl_se_hash_streaming_context_t;

/// SHA-1 streaming context. Safe to use.
typedef struct {
  sl_se_hash_type_t      hash_type; ///< Hash streaming context
  uint32_t total[2];                ///< number of bytes processed
  uint8_t  state[32];               ///< intermediate digest state
  uint8_t  buffer[64];              ///< data block being processed
} sl_se_sha1_multipart_context_t;

/// SHA-1 streaming context. Deprecated.
typedef struct {
  uint32_t total[2];                ///< number of bytes processed
  uint8_t  state[32];               ///< intermediate digest state
  uint8_t  buffer[64];              ///< data block being processed
} sl_se_sha1_streaming_context_t;

/// SHA-224 streaming context. Safe to use.
typedef struct {
  sl_se_hash_type_t      hash_type; ///< Hash streaming context
  uint32_t total[2];                ///< Number of bytes processed
  uint8_t  state[32];               ///< Intermediate digest state
  uint8_t  buffer[64];              ///< Data block being processed
} sl_se_sha224_multipart_context_t;

/// SHA-224 streaming context. Deprecated.
typedef struct {
  uint32_t total[2];                ///< Number of bytes processed
  uint8_t  state[32];               ///< Intermediate digest state
  uint8_t  buffer[64];              ///< Data block being processed
} sl_se_sha224_streaming_context_t;

/// SHA-256 streaming context. Safe to use.
typedef struct {
  sl_se_hash_type_t      hash_type; ///< Hash streaming context
  uint32_t total[2];                ///< Number of bytes processed
  uint8_t  state[32];               ///< Intermediate digest state
  uint8_t  buffer[64];              ///< Data block being processed
} sl_se_sha256_multipart_context_t;

/// SHA-256 streaming context. Deprecated.
typedef struct {
  uint32_t total[2];                ///< Number of bytes processed
  uint8_t  state[32];               ///< Intermediate digest state
  uint8_t  buffer[64];              ///< Data block being processed
} sl_se_sha256_streaming_context_t;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// SHA-384 streaming context. Safe to use.
typedef struct {
  sl_se_hash_type_t      hash_type; ///< Hash streaming context
  uint32_t total[4];                ///< Number of bytes processed
  uint8_t  state[64];               ///< Intermediate digest state
  uint8_t  buffer[128];             ///< Data block being processed
} sl_se_sha384_multipart_context_t;

/// SHA-384 streaming context. Deprecated.
typedef struct {
  uint32_t total[4];                ///< Number of bytes processed
  uint8_t  state[64];               ///< Intermediate digest state
  uint8_t  buffer[128];             ///< Data block being processed
} sl_se_sha384_streaming_context_t;

/// SHA-512 streaming context. Safe to use.
typedef struct {
  sl_se_hash_type_t      hash_type; ///< Hash streaming context
  uint32_t total[4];                ///< Number of bytes processed
  uint8_t  state[64];               ///< Intermediate digest state
  uint8_t  buffer[128];             ///< Data block being processed
} sl_se_sha512_multipart_context_t;

/// SHA-512 streaming context. Deprecated.
typedef struct {
  uint32_t total[4];                ///< Number of bytes processed
  uint8_t  state[64];               ///< Intermediate digest state
  uint8_t  buffer[128];             ///< Data block being processed
} sl_se_sha512_streaming_context_t;
#endif

/// @} (end addtogroup sl_se_manager_hash)

/// @addtogroup sl_se_manager_key_derivation
/// @{

/// Roles in the EC J-PAKE exchange
typedef enum {
  SL_SE_ECJPAKE_CLIENT = 0,           ///< Client
  SL_SE_ECJPAKE_SERVER,               ///< Server
} sl_se_ecjpake_role_t;

/**************************************************************************//**
 * EC J-PAKE context structure.
 *
 * J-PAKE is a symmetric protocol, except for the identifiers used in
 * Zero-Knowledge Proofs, and the serialization of the second message
 * (KeyExchange) as defined by the Thread spec.
 *
 * In order to benefit from this symmetry, we choose a different naming
 * convention from the Thread v1.0 spec. Correspondance is indicated in the
 * description as a pair C: client name, S: server name
 *****************************************************************************/
typedef struct {
  sl_se_command_context_t *cmd_ctx;   ///< Pointer to command context object
  uint32_t curve_flags;               ///< Curve flags to use
  sl_se_ecjpake_role_t role;          ///< Are we client or server?

  char pwd[32];                       ///< J-PAKE password
  size_t pwd_len;                     ///< J-PAKE password length

  uint8_t r[32];                      ///< Random scalar for exchange
  uint8_t Xm1[64];                    ///< Our point 1 (round 1)
  uint8_t Xm2[64];                    ///< Our point 2 (round 1)
  uint8_t Xp1[64];                    ///< Their point 1 (round 1)
  uint8_t Xp2[64];                    ///< Their point 2 (round 1)
  uint8_t Xp[64];                     ///< Their point (round 2)
} sl_se_ecjpake_context_t;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT) || defined(DOXYGEN)
/// Typedef sl_se_pbkdf2_prf_type_t to sl_se_hash_type_t in order to maintain
/// backward compatibility. Defines for mapping the PRF identifiers to the
/// underlying hash enum values exists in sl_se_manager_defines.h.
typedef sl_se_hash_type_t sl_se_pbkdf2_prf_type_t;
#endif

/// @} (end addtogroup sl_se_manager_key_derivation)

#endif // defined(SEMAILBOX_PRESENT)

#ifdef __cplusplus
}
#endif

/// @} (end addtogroup sl_se_manager)

#endif // defined(SEMAILBOX_PRESENT) || defined(CRYPTOACC_PRESENT)

#endif // SL_SE_MANAGER_TYPES_H
