/***************************************************************************//**
 * @file
 * @brief SoC Certificate Generator
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdio.h>

#include "sl_status.h"
#include "sl_component_catalog.h"
#include "sl_system_init.h"

#include "app_assert.h"
#include "app_log.h"

#include "ecode.h"
#include "em_common.h"
#include "em_system.h"
#include "nvm3.h"

#include "psa/crypto.h"
#include "psa/crypto_values.h"

#include "csr_generator.h"
#include "csr_generator_config.h"

#include "der.h"

// -----------------------------------------------------------------------------
// Defines

#define POS_DEVICE_CERTIFICATE  0
#define POS_DEVICE_EC_KEY       1
#define POS_STATIC_AUTH_DATA    2
#define NVM3_ID_CNT             3

#define DEVICE_CERTIFICATE_NVM3_START_KEY 1

#define CSR_NVM3        (nvm3_defaultHandle)

#define CSR_NVM3_STATIC_AUTH_DATA 0x00fd
#define CSR_NVM3_DEVICE_EC_KEY    0x00fe

#define CSR_NVM3_PRODDATA_CONTROL_BLOCK 0x0400
#define CHAIN_LINK_DATA_LEN             192

#define DEVICE_EC_KEY_ID      (CSR_GENERATOR_NVM3_REGION | CSR_NVM3_DEVICE_EC_KEY)
#define STATIC_AUTH_DATA_ID   (CSR_GENERATOR_NVM3_REGION | CSR_NVM3_STATIC_AUTH_DATA)

#define UUID_LEN   (16)

#define CRYPTO_EC_PRIVATE_KEY_LEN 32

#define PROVISIONING_CONTROL_BLOCK_INIT  { 0,                               \
                                           CSR_NVM3_PRODDATA_CONTROL_BLOCK, \
                                           0,                               \
                                           { 0, 0, 0 },                     \
                                           CHAIN_LINK_DATA_LEN }

// -----------------------------------------------------------------------------
// Type definitions.

// Provisioning record control block
typedef struct __attribute__((__packed__)) provisioning_record_control_block {
  uint16_t header;
  uint16_t next;
  uint64_t bitmap;
  uint8_t nvm3_id[NVM3_ID_CNT];
  uint16_t max_link_data_len;
} provisioning_record_control_block_t;

typedef struct  __attribute__((__packed__)) csr_output {
  uint8_t completed;
  uint8_t static_auth[CRYPTO_AUTH_256_LEN];
  uint16_t csr_len; /* DER is binary format so length is required */
  uint8_t csr[];
} csr_output_t;

typedef struct csr_config_t {
  csr_output_t *output;                                   // Position of RAM structure for communicating with PLT
  bool generate_static_auth;                              // If True, generate 256-bit static authentication data
  bool generate_key;                                      // If True, generate device EC key and corresponding CSR
  bool add_auth_data_to_csr;                              // If True, and both of the above are true, add auth data extension to CSR
  bool certificate_on_device;                             // If True, mark certificate presence on device in control block
  uint8_t certificate_pos;                                // For recording certificate position in NVM3 to control block
  size_t subject_name_field_count;                        // CSR subject name, excluding CN (see below)
  const subject_name_field_t *subject_name_field_array;
} csr_config_t;

// -----------------------------------------------------------------------------
// Module variables.

// UUID Namespace
static const uint8_t uid64_namespace[16] = {
  0x70, 0x0b, 0xaf, 0xdf, 0xd5, 0xec, 0xc3, 0x9b,
  0x37, 0x04, 0xa1, 0x2c, 0x07, 0x67, 0x86, 0x9c,
};

// Statich authentication data buffer
static uint8_t auth_data[CRYPTO_AUTH_256_LEN] = { 0 };

static char btmesh_common_name_uuid[37] = { 0 };

// CSR Subject name fields
static subject_name_field_t subject_name_fields[] =
{
  {
    .name_len = 1,
    .value_len = (sizeof(CSR_GENERATOR_SUBJECT_COUNTRY) - 1),
    .name = "C",      // shorthand for "countryName"
    .value = CSR_GENERATOR_SUBJECT_COUNTRY
  },
  {
    .name_len = 2,
    .value_len = (sizeof(CSR_GENERATOR_SUBJECT_STATE) - 1),
    .name = "ST",     // shorthand for "stateOrProvinceName"
    .value = CSR_GENERATOR_SUBJECT_STATE
  },
  {
    .name_len = 1,
    .value_len = (sizeof(CSR_GENERATOR_SUBJECT_LOCALITY) - 1),
    .name = "L",      // shorthand for "locality"
    .value = CSR_GENERATOR_SUBJECT_LOCALITY
  },
  {
    .name_len = 1,
    .value_len = (sizeof(CSR_GENERATOR_SUBJECT_ORGANIZATION) - 1),
    .name = "O",      // shorthand for "organization"
    .value = CSR_GENERATOR_SUBJECT_ORGANIZATION
  },
  {
    .name_len = 2,
    .value_len = (sizeof(CSR_GENERATOR_SUBJECT_ORGANIZATION_UNIT) - 1),
    .name = "OU",     // shorthand for "organizationalUnitName"
    .value = CSR_GENERATOR_SUBJECT_ORGANIZATION_UNIT
  },
  {
    .name_len = 2,
    .value_len = 36, // 128-bit Mesh device UUID as hexes and dashes
    .name = "CN", // shorthand for "commonName"
    .value = btmesh_common_name_uuid // To be filled in
  },
};

// CSR configuration
static volatile const csr_config_t config = {
  .output = (csr_output_t *)((intptr_t)(CSR_GENERATOR_CSR_RAM_ADDRESS)),
  .generate_static_auth = CSR_GENERATOR_CONFIG_GENERATE_STATIC_AUTH,
  .generate_key = CSR_GENERATOR_CONFIG_GENERATE_EC_KEY,
  .add_auth_data_to_csr = CSR_GENERATOR_CONFIG_ADD_STATIC_AUTH_TO_CSR,
  .certificate_on_device = CSR_GENERATOR_CONFIG_CERTIFICATE_ON_DEVICE,
  .certificate_pos = DEVICE_CERTIFICATE_NVM3_START_KEY,
  .subject_name_field_count = 6,
  .subject_name_field_array = subject_name_fields,
};

static provisioning_record_control_block_t control_block = PROVISIONING_CONTROL_BLOCK_INIT;

// -----------------------------------------------------------------------------
// Private function declarations.

static sl_status_t error_code_to_sl_status(Ecode_t ecode);

static void update_provisoning_control_block(int32_t index,
                                             bool present,
                                             uint8_t nvm3_id);

static sl_status_t crypto_init(void);

static sl_status_t create_static_auth_data(void);

static sl_status_t create_device_ec_key(mbedtls_svc_key_id_t *ecdh_key_ptr,
                                        mbedtls_svc_key_id_t *signing_key_ptr);

static sl_status_t generate_uuid(uint8_t *const uuid);

static sl_status_t read_raw_nvm3(uint32_t domain,
                                 uint16_t tag,
                                 uint8_t *buf,
                                 size_t *len);
static sl_status_t write_raw_nvm3(uint32_t domain,
                                  uint16_t tag,
                                  uint8_t *buf,
                                  size_t len);
static sl_status_t erase_all_nvm3(void);

static sl_status_t read_provisioning_record_control_block(provisioning_record_control_block_t *block);
static sl_status_t write_provisioning_record_control_block(provisioning_record_control_block_t *block);

// -----------------------------------------------------------------------------
// Public function definitions.

/**************************************************************************//**
 * This function is generating the device EC key pair, the signing request
 * for the device certificate, and other related data.
 *****************************************************************************/
void csr_generate(void)
{
  sl_status_t sc;
  int mbedtls_ret = 0;
  size_t auth_len = 0;

  mbedtls_svc_key_id_t signing_key_id = 0;
  mbedtls_svc_key_id_t ecdh_key_id = 0;

  uint8_t uuid[UUID_LEN + 1] = { 0 };

  app_log_info("Starting BT CSR creation" APP_LOG_NL);

  // Clear the RAM output area
  memset(config.output, 0, sizeof(csr_config_t));

  sc = crypto_init();
  app_assert_status(sc);

  // Check if provisioning record control block exists
  sc = read_provisioning_record_control_block(&control_block);
  switch (sc) {
    case SL_STATUS_OK:
      app_log_info("Provision record control block already exists." APP_LOG_NL);
      if (config.certificate_on_device) {
        // Check if certificate exists
        uint16_t cert_nvm_tag = control_block.nvm3_id[POS_DEVICE_CERTIFICATE]
                                | CSR_NVM3_PRODDATA_CONTROL_BLOCK;
        uint8_t data;
        size_t len = sizeof(data);
        sl_status_t sc = SL_STATUS_FAIL;
        sc = read_raw_nvm3(CSR_GENERATOR_NVM3_REGION,
                           cert_nvm_tag,
                           &data,
                           &len);
        if (sc == SL_STATUS_BT_PS_KEY_NOT_FOUND) {
          app_log("Certificate not found, regenerating CSR" APP_LOG_NL);
          sc = erase_all_nvm3();
          app_assert_status(sc);
          break;
        }
      }
      return;
    case SL_STATUS_BT_PS_KEY_NOT_FOUND:
      app_log_info("Provisioning record control block does not exist yet." APP_LOG_NL);
      break;
    default:
      app_log_info("Unexpected error occured when reading the provisioning record." APP_LOG_NL);
      return;
  }

  if (!config.generate_static_auth) {
    app_log_info("Static authentication data is not requested." APP_LOG_NL);
  } else {
    // Create static authentication data
    sc = create_static_auth_data();
    app_assert((sc == SL_STATUS_OK) || (sc == SL_STATUS_ALREADY_EXISTS),
               "Failed to create static authentication data." APP_LOG_NL);
    if (sc == SL_STATUS_OK) {
      app_log_info("Static authentication data created successfully." APP_LOG_NL);
    } else {
      app_log_info("Static authentication data already exists." APP_LOG_NL);
      sc = SL_STATUS_OK;
    }

    update_provisoning_control_block(POS_STATIC_AUTH_DATA, true, 0);

    if (sc == SL_STATUS_OK) {
      sc = export_static_auth_data(auth_data, sizeof(auth_data), &auth_len);
      app_assert((sc == SL_STATUS_OK), "Failed to read static authentication data." APP_LOG_NL);
      memcpy(config.output->static_auth, auth_data, auth_len);
      app_log_info("Data: ");
      app_log_hexdump_info(auth_data, CRYPTO_AUTH_256_LEN);
      app_log_append(APP_LOG_NL);
    }
  }

  if (!config.generate_key) {
    app_log_info("EC key creation is not requested." APP_LOG_NL);
  } else {
    // Generate EC key

    // Create persistent key for ECDH, temporary key for CSR signing
    sc = create_device_ec_key(&ecdh_key_id, &signing_key_id);
    app_assert((sc == SL_STATUS_OK) || (sc == SL_STATUS_ALREADY_EXISTS),
               "Failed to create EC key ITS." APP_LOG_NL);
    if (sc == SL_STATUS_ALREADY_EXISTS) {
      app_log_info("Key already exists." APP_LOG_NL);
      // If it already exists (and is not exportable) we can't create an equivalent
      // signing key anymore
      return;
    } else {
      app_log_info("EC key created successfully in ITS, signing key ID is %04X" APP_LOG_NL,
                   signing_key_id);
    }

    update_provisoning_control_block(POS_DEVICE_EC_KEY, true, 0);

    // Generate UUID
    sc = generate_uuid(uuid);
    app_assert((sc == SL_STATUS_OK),
               "Failed to generate UUID, e: %d" APP_LOG_NL, sc);
    app_log_info("Generated UUID: ");
    app_log_hexdump_info(uuid, UUID_LEN);
    app_log_append(APP_LOG_NL);

    // Generate CSR
    unsigned char csr_der_buf[1024] = { 0 };
    size_t csr_der_len = 0;

    /* Fill in CN to subject name */
    char* p = subject_name_fields[5].value;

    for (int i = 0; i < UUID_LEN; i++) {
      if ((i > 3) && (i < 11) && ((i & 1) == 0)) { /* index 4, 6, 8, 10) */
        *p++ = '-';
      }
      sprintf(p, "%02x", uuid[i]);
      p += 2;
    }

    mbedtls_ret = der_encode_csr(config.subject_name_field_array,
                                 config.subject_name_field_count,
                                 signing_key_id,
                                 config.add_auth_data_to_csr,
                                 csr_der_buf,
                                 sizeof(csr_der_buf),
                                 &csr_der_len);
    app_assert((mbedtls_ret == 0), "Failed to write CSR PEM file, error: %d" APP_LOG_NL, mbedtls_ret);

    app_log_info("CSR created successfully." APP_LOG_NL);

    // Set device certificate as present with the configured NVM3 ID.
    update_provisoning_control_block(POS_DEVICE_CERTIFICATE,
                                     config.certificate_on_device,
                                     config.certificate_pos);

    config.output->csr_len = csr_der_len;
    memcpy(config.output->csr, csr_der_buf, csr_der_len);
  }

  // Store the provisioning record control block
  sc = write_provisioning_record_control_block(&control_block);
  app_assert((sc == SL_STATUS_OK), "Could not create the provisioning control block." APP_LOG_NL);

  app_log_info("Provisioning control block successfully created." APP_LOG_NL);
  app_log_info("RAM output address 0x%08X" APP_LOG_NL, config.output);
  config.output->completed = 1;

  return;
}

// -----------------------------------------------------------------------------
// Private function definitions.

static sl_status_t error_code_to_sl_status(Ecode_t ecode)
{
  sl_status_t sc = SL_STATUS_FAIL;
  switch (ecode) {
    case ECODE_OK:
      sc = SL_STATUS_OK;
      break;
    case ECODE_NVM3_ERR_PARAMETER:
    case ECODE_NVM3_ERR_KEY_INVALID:
      sc = SL_STATUS_INVALID_PARAMETER;
      break;
    case ECODE_NVM3_ERR_KEY_NOT_FOUND:
      sc = SL_STATUS_BT_PS_KEY_NOT_FOUND;
      break;
    case ECODE_NVM3_ERR_STORAGE_FULL:
      sc = SL_STATUS_BT_PS_STORE_FULL;
      break;
    default:
      sc = (sl_status_t) (SL_STATUS_BLUETOOTH_SPACE + 0x80 + (ecode & (~ECODE_EMDRV_NVM3_BASE)));
      break;
  }
  return sc;
}

sl_status_t psa_status_to_sl_status(psa_status_t sc)
{
  switch (sc) {
    case PSA_SUCCESS:                     return SL_STATUS_OK;
    case PSA_ERROR_GENERIC_ERROR:         return SL_STATUS_FAIL;
    case PSA_ERROR_NOT_SUPPORTED:         return SL_STATUS_NOT_SUPPORTED;
    case PSA_ERROR_NOT_PERMITTED:         return SL_STATUS_PERMISSION;
    case PSA_ERROR_BUFFER_TOO_SMALL:      return SL_STATUS_WOULD_OVERFLOW;
    case PSA_ERROR_ALREADY_EXISTS:        return SL_STATUS_ALREADY_EXISTS;
    case PSA_ERROR_DOES_NOT_EXIST:        return SL_STATUS_FAIL;
    case PSA_ERROR_BAD_STATE:             return SL_STATUS_INVALID_STATE;
    case PSA_ERROR_INVALID_ARGUMENT:      return SL_STATUS_INVALID_PARAMETER;
    case PSA_ERROR_INSUFFICIENT_MEMORY:   return SL_STATUS_NO_MORE_RESOURCE;
    case PSA_ERROR_INSUFFICIENT_STORAGE:  return SL_STATUS_NO_MORE_RESOURCE;
    case PSA_ERROR_COMMUNICATION_FAILURE: return SL_STATUS_IO;
    case PSA_ERROR_STORAGE_FAILURE:       return SL_STATUS_BT_HARDWARE;
    case PSA_ERROR_HARDWARE_FAILURE:      return SL_STATUS_BT_HARDWARE;
    case PSA_ERROR_CORRUPTION_DETECTED:   return SL_STATUS_BT_DATA_CORRUPTED;
    case PSA_ERROR_INSUFFICIENT_ENTROPY:  return SL_STATUS_BT_CRYPTO;
    case PSA_ERROR_INVALID_SIGNATURE:     return SL_STATUS_INVALID_SIGNATURE;
    case PSA_ERROR_INVALID_PADDING:       return SL_STATUS_BT_CRYPTO;
    case PSA_ERROR_INSUFFICIENT_DATA:     return SL_STATUS_BT_CRYPTO;
    case PSA_ERROR_INVALID_HANDLE:        return SL_STATUS_INVALID_HANDLE;
    case PSA_ERROR_DATA_CORRUPT:          return SL_STATUS_BT_DATA_CORRUPTED;
    case PSA_ERROR_DATA_INVALID:          return SL_STATUS_BT_CRYPTO;
    default:                              return SL_STATUS_BT_UNSPECIFIED;
  }
}

static void update_provisoning_control_block(int32_t index,
                                             bool present,
                                             uint8_t nvm3_id)
{
  if (present) {
    control_block.bitmap |= (1 << index);
    control_block.nvm3_id[index] = nvm3_id;
  } else {
    control_block.bitmap &= ~(1 << index);
    control_block.nvm3_id[index] = 0;
  }
}

static sl_status_t crypto_init(void)
{
  psa_status_t sc = PSA_ERROR_NOT_SUPPORTED;
  if (SYSTEM_GetSecurityCapability() == securityCapabilityVault) {
    sc = psa_crypto_init();
  }

  return psa_status_to_sl_status(sc);
}

static sl_status_t create_static_auth_data(void)
{
  sl_status_t sc = SL_STATUS_OK;

  mbedtls_svc_key_id_t key = STATIC_AUTH_DATA_ID;
  psa_algorithm_t alg = 0;
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_key_lifetime_t lifetime =
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT,
                                                   CSR_GENERATOR_KEY_LOCATION);

  psa_set_key_algorithm(&attributes, alg);
  psa_set_key_type(&attributes, PSA_KEY_TYPE_RAW_DATA);
  psa_set_key_bits(&attributes, CRYPTO_AUTH_256_LEN * 8);
  psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_EXPORT);
  psa_set_key_id(&attributes, key);
  psa_set_key_lifetime(&attributes, lifetime);

  sc = psa_status_to_sl_status(psa_generate_key(&attributes,
                                                &key));

  return sc;
}

sl_status_t export_static_auth_data(uint8_t *data, size_t data_size, size_t *data_length)
{
  psa_status_t sc;
  mbedtls_svc_key_id_t key = STATIC_AUTH_DATA_ID;

  sc = psa_export_key(key, data, data_size, data_length);

  return psa_status_to_sl_status(sc);
}

static sl_status_t create_device_ec_key(mbedtls_svc_key_id_t *ecdh_key_ptr,
                                        mbedtls_svc_key_id_t *signing_key_ptr)
{
  psa_key_lifetime_t persistent_wrapped =
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_DEFAULT,
                                                   CSR_GENERATOR_KEY_LOCATION);
  psa_key_lifetime_t volatile_wrapped =
    PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(PSA_KEY_PERSISTENCE_VOLATILE,
                                                   CSR_GENERATOR_KEY_LOCATION);

  sl_status_t sc = SL_STATUS_OK;

  mbedtls_svc_key_id_t ecdh_key = 0;
  mbedtls_svc_key_id_t signing_key = 0;

  psa_key_attributes_t ecdh_attributes = PSA_KEY_ATTRIBUTES_INIT;
  psa_key_attributes_t signing_attributes = PSA_KEY_ATTRIBUTES_INIT;

  if (!ecdh_key_ptr || !signing_key_ptr) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  psa_set_key_algorithm(&signing_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
  psa_set_key_type(&signing_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&signing_attributes, CRYPTO_EC_PRIVATE_KEY_LEN * 8);
  psa_set_key_usage_flags(&signing_attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_EXPORT);
  psa_set_key_lifetime(&signing_attributes, volatile_wrapped);

  sc = psa_status_to_sl_status(psa_generate_key(&signing_attributes, &signing_key));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  psa_set_key_algorithm(&ecdh_attributes, CSR_GENERATOR_EC_KEY_ALGO);
  psa_set_key_type(&ecdh_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1));
  psa_set_key_bits(&ecdh_attributes, CRYPTO_EC_PRIVATE_KEY_LEN * 8);
  psa_set_key_usage_flags(&ecdh_attributes, CSR_GENERATOR_EC_KEY_USAGE);
  psa_set_key_lifetime(&ecdh_attributes, persistent_wrapped);
  psa_set_key_id(&ecdh_attributes, DEVICE_EC_KEY_ID);

  uint8_t buf[32];
  size_t buf_len = sizeof(buf);
  sc = psa_status_to_sl_status(psa_export_key(signing_key, buf, buf_len, &buf_len));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  sc = psa_status_to_sl_status(psa_import_key(&ecdh_attributes, buf, sizeof(buf), &ecdh_key));
  memset(buf, 0, sizeof(buf));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  *ecdh_key_ptr = ecdh_key;
  ecdh_key = 0;
  *signing_key_ptr = signing_key;
  signing_key = 0;

  psa_destroy_key(ecdh_key);
  psa_destroy_key(signing_key);

  return SL_STATUS_OK;
}

sl_status_t export_public_key(mbedtls_svc_key_id_t key, uint8_t *data, size_t data_size, size_t *data_length)
{
  psa_status_t sc;

  if (!key || !data || !data_size || !data_length) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sc = psa_export_public_key(key, data, data_size, data_length);

  return psa_status_to_sl_status(sc);
}

sl_status_t calculate_sha_1(const uint8_t *ptr,
                            size_t len,
                            uint8_t *hash)
{
  psa_hash_operation_t operation = { 0 };
  psa_algorithm_t alg = PSA_ALG_SHA_1;
  size_t out_len = 0;
  uint8_t result[CRYPTO_SHA_1_LEN];

  sl_status_t sc = SL_STATUS_OK;

  if (!ptr || !len  || !hash) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  sc = psa_status_to_sl_status(psa_hash_setup(&operation, alg));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  sc = psa_status_to_sl_status(psa_hash_update(&operation, ptr, len));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  sc = psa_status_to_sl_status(psa_hash_finish(&operation, result, CRYPTO_SHA_1_LEN, &out_len));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  memcpy(hash, result, CRYPTO_SHA_1_LEN);

  return sc;
}

static sl_status_t generate_uuid(uint8_t *const uuid)
{
  sl_status_t sc = SL_STATUS_OK;

  if (!uuid) {
    return SL_STATUS_INVALID_PARAMETER;
  }

  // UUID generation procedure shall follow the standard UUID format as defined in RFC4122
  // https://www.ietf.org/rfc/rfc4122.txt

  uint64_t uid = SYSTEM_GetUnique();
  uint8_t tmp[24], digest[CRYPTO_SHA_1_LEN];
  memcpy(tmp, uid64_namespace, 16);
  tmp[16] = (uid >> 56) & 0xff;
  tmp[17] = (uid >> 48) & 0xff;
  tmp[18] = (uid >> 40) & 0xff;
  tmp[19] = (uid >> 32) & 0xff;
  tmp[20] = (uid >> 24) & 0xff;
  tmp[21] = (uid >> 16) & 0xff;
  tmp[22] = (uid >>  8) & 0xff;
  tmp[23] = (uid >>  0) & 0xff;

  sc = calculate_sha_1(tmp, sizeof(tmp), digest);

  memcpy(uuid, digest, 16);
  uuid[7] &= 0x0f; // Set the four most significant bits (bits 12 through 15) of the time_hi_and_version field
  uuid[7] |= 0x50; // to the 4-bit version number as randomly generated
  uuid[8] &= 0x3f; // Set the two most significant bits (bits 6 and 7) of the clock_seq_hi_and_reserved field
  uuid[8] |= 0x80; // to zero and one, respectively

  return sc;
}

static sl_status_t read_raw_nvm3(uint32_t domain,
                                 uint16_t tag,
                                 uint8_t *buf,
                                 size_t *len)
{
  uint32_t type;
  size_t value_len;
  size_t read_len;
  sl_status_t sc;

  sc = error_code_to_sl_status(nvm3_getObjectInfo(CSR_NVM3,
                                                  domain | tag,
                                                  &type,
                                                  &value_len));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  read_len = (value_len > *len) ? *len : value_len;

  sc = error_code_to_sl_status(nvm3_readData(CSR_NVM3,
                                             domain | tag,
                                             buf,
                                             read_len));
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  *len = value_len;
  return SL_STATUS_OK;
}

static sl_status_t write_raw_nvm3(uint32_t domain,
                                  uint16_t tag,
                                  uint8_t *buf,
                                  size_t len)
{
  return error_code_to_sl_status(nvm3_writeData(CSR_NVM3,
                                                domain | tag,
                                                buf,
                                                len));
}

static sl_status_t erase_all_nvm3(void)
{
  return error_code_to_sl_status(nvm3_eraseAll(CSR_NVM3));
}

static sl_status_t read_provisioning_record_control_block(provisioning_record_control_block_t *b)
{
  provisioning_record_control_block_t block;
  size_t len = sizeof(block);
  sl_status_t sc;

  sc = read_raw_nvm3(CSR_GENERATOR_NVM3_REGION,
                     CSR_NVM3_PRODDATA_CONTROL_BLOCK,
                     (uint8_t *)&block,
                     &len);

  if (sc == SL_STATUS_OK) {
    memcpy(b, &block, sizeof(block));
  }

  return sc;
}

static sl_status_t write_provisioning_record_control_block(provisioning_record_control_block_t *block)
{
  return write_raw_nvm3(CSR_GENERATOR_NVM3_REGION,
                        CSR_NVM3_PRODDATA_CONTROL_BLOCK,
                        (uint8_t *)block,
                        sizeof(provisioning_record_control_block_t));
}
