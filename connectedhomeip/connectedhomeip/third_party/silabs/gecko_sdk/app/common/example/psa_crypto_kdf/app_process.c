/***************************************************************************//**
 * @file app_process.c
 * @brief Top level application functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include "app_process.h"

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Retrieve input character from VCOM port.
 ******************************************************************************/
static void app_iostream_usart_process_action(void);

/***************************************************************************//**
 * Print key storage.
 ******************************************************************************/
static void print_key_storage(void);

/***************************************************************************//**
 * Import a master key.
 ******************************************************************************/
static void import_master_key(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

/***************************************************************************//**
 * Compare HKDF key with expected key data.
 *
 * @returns Returns 0 if equal.
 ******************************************************************************/
static int compare_expect_okm(void);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Space press status
static bool space_press;

/// Enter press status
static bool enter_press;

/// State machine state variable
static state_t app_state = PSA_CRYPTO_INIT;

/// String for example
static uint8_t example_string[] = "PSA Crypto HKDF Example";

/// Symmetric key storage selection
static uint8_t symmetric_key_storage_select;

static const char *symmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED"
};

/// Symmetric key size selection
static uint8_t symmetric_key_size_select;

static const size_t symmetric_key_size[] = {
  128,
  192,
  256
};

/// Hash algorithm selection
static uint8_t hash_algo_select;

static const char *hash_algo_string[] = {
  "SHA1",
  "SHA224",
  "SHA256",
  "SHA384",
  "SHA512"
};

static const psa_algorithm_t hash_algo[] = {
  PSA_ALG_SHA_1,
  PSA_ALG_SHA_224,
  PSA_ALG_SHA_256,
  PSA_ALG_SHA_384,
  PSA_ALG_SHA_512
};

/// Test vector for HKDF
/// Input key material (ikm)
static const uint8_t hkdf_ikm[] = {
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
  0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b
};

/// Salt
static const uint8_t hkdf_salt[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c
};

/// Information
static const uint8_t hkdf_info[] = {
  0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7,
  0xf8, 0xf9
};

/// Expect SHA1 output key material (okm)
static const uint8_t expect_sha1_hkdf_okm[] = {
  0xd6, 0x00, 0x0f, 0xfb, 0x5b, 0x50, 0xbd, 0x39,
  0x70, 0xb2, 0x60, 0x01, 0x77, 0x98, 0xfb, 0x9c,
  0x8d, 0xf9, 0xce, 0x2e, 0x2c, 0x16, 0xb6, 0xcd,
  0x70, 0x9c, 0xca, 0x07, 0xdc, 0x3c, 0xf9, 0xcf
};

/// Expect SHA224 output key material (okm)
static const uint8_t expect_sha224_hkdf_okm[] = {
  0x2f, 0x21, 0xcd, 0x7c, 0xbc, 0x81, 0x8c, 0xa5,
  0xc5, 0x61, 0xb9, 0x33, 0x72, 0x8e, 0x2e, 0x08,
  0xe1, 0x54, 0xa8, 0x7e, 0x14, 0x32, 0x39, 0x9a,
  0x82, 0x0d, 0xee, 0x13, 0xaa, 0x22, 0x2d, 0x0c
};

/// Expect SHA256 output key material (okm)
static const uint8_t expect_sha256_hkdf_okm[] = {
  0x3c, 0xb2, 0x5f, 0x25, 0xfa, 0xac, 0xd5, 0x7a,
  0x90, 0x43, 0x4f, 0x64, 0xd0, 0x36, 0x2f, 0x2a,
  0x2d, 0x2d, 0x0a, 0x90, 0xcf, 0x1a, 0x5a, 0x4c,
  0x5d, 0xb0, 0x2d, 0x56, 0xec, 0xc4, 0xc5, 0xbf
};

/// Expect SHA384 output key material (okm)
static const uint8_t expect_sha384_hkdf_okm[] = {
  0x9b, 0x50, 0x97, 0xa8, 0x60, 0x38, 0xb8, 0x05,
  0x30, 0x90, 0x76, 0xa4, 0x4b, 0x3a, 0x9f, 0x38,
  0x06, 0x3e, 0x25, 0xb5, 0x16, 0xdc, 0xbf, 0x36,
  0x9f, 0x39, 0x4c, 0xfa, 0xb4, 0x36, 0x85, 0xf7
};

/// Expect SHA512 output key material (okm)
static const uint8_t expect_sha512_hkdf_okm[] = {
  0x83, 0x23, 0x90, 0x08, 0x6c, 0xda, 0x71, 0xfb,
  0x47, 0x62, 0x5b, 0xb5, 0xce, 0xb1, 0x68, 0xe4,
  0xc8, 0xe2, 0x6a, 0x1a, 0x16, 0xed, 0x34, 0xd9,
  0xfc, 0x7f, 0xe9, 0x2c, 0x14, 0x81, 0x57, 0x93
};

/// Test vector (SECP256R1) for combining key agreement and key derivation
/// Client private key
static const uint8_t client_private_key[] = {
  0xc8, 0x8f, 0x01, 0xf5, 0x10, 0xd9, 0xac, 0x3f,
  0x70, 0xa2, 0x92, 0xda, 0xa2, 0x31, 0x6d, 0xe5,
  0x44, 0xe9, 0xaa, 0xb8, 0xaf, 0xe8, 0x40, 0x49,
  0xc6, 0x2a, 0x9c, 0x57, 0x86, 0x2d, 0x14, 0x33
};

/// Client public key
static const uint8_t client_public_key[] = {
  0x04,         // Uncompressed point format
  0xda, 0xd0, 0xb6, 0x53, 0x94, 0x22, 0x1c, 0xf9,
  0xb0, 0x51, 0xe1, 0xfe, 0xca, 0x57, 0x87, 0xd0,
  0x98, 0xdf, 0xe6, 0x37, 0xfc, 0x90, 0xb9, 0xef,
  0x94, 0x5d, 0x0c, 0x37, 0x72, 0x58, 0x11, 0x80,
  0x52, 0x71, 0xa0, 0x46, 0x1c, 0xdb, 0x82, 0x52,
  0xd6, 0x1f, 0x1c, 0x45, 0x6f, 0xa3, 0xe5, 0x9a,
  0xb1, 0xf4, 0x5b, 0x33, 0xac, 0xcf, 0x5f, 0x58,
  0x38, 0x9e, 0x05, 0x77, 0xb8, 0x99, 0x0b, 0xb3
};

/// Server private key
static const uint8_t server_private_key[] = {
  0xc6, 0xef, 0x9c, 0x5d, 0x78, 0xae, 0x01, 0x2a,
  0x01, 0x11, 0x64, 0xac, 0xb3, 0x97, 0xce, 0x20,
  0x88, 0x68, 0x5d, 0x8f, 0x06, 0xbf, 0x9b, 0xe0,
  0xb2, 0x83, 0xab, 0x46, 0x47, 0x6b, 0xee, 0x53
};

/// Server public key
static const uint8_t server_public_key[] = {
  0x04,         // Uncompressed point format
  0xd1, 0x2d, 0xfb, 0x52, 0x89, 0xc8, 0xd4, 0xf8,
  0x12, 0x08, 0xb7, 0x02, 0x70, 0x39, 0x8c, 0x34,
  0x22, 0x96, 0x97, 0x0a, 0x0b, 0xcc, 0xb7, 0x4c,
  0x73, 0x6f, 0xc7, 0x55, 0x44, 0x94, 0xbf, 0x63,
  0x56, 0xfb, 0xf3, 0xca, 0x36, 0x6c, 0xc2, 0x3e,
  0x81, 0x57, 0x85, 0x4c, 0x13, 0xc5, 0x8d, 0x6a,
  0xac, 0x23, 0xf0, 0x46, 0xad, 0xa3, 0x0f, 0x83,
  0x53, 0xe7, 0x4f, 0x33, 0x03, 0x98, 0x72, 0xab
};

/// ECDH shared secret
static const uint8_t raw_shared_secret[] = {
  0xd6, 0x84, 0x0f, 0x6b, 0x42, 0xf6, 0xed, 0xaf,
  0xd1, 0x31, 0x16, 0xe0, 0xe1, 0x25, 0x65, 0x20,
  0x2f, 0xef, 0x8e, 0x9e, 0xce, 0x7d, 0xce, 0x03,
  0x81, 0x24, 0x64, 0xd0, 0x4b, 0x94, 0x42, 0xde
};

/// master key identifier
static psa_key_id_t master_key_id;

/// HKDF key identifier
static psa_key_id_t hkdf_key_id;

/// HKDF key copy
static uint8_t hkdf_key_copy[32];

/// Derive key process
static uint32_t derive_process;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  // Retrieve input character from VCOM port
  app_iostream_usart_process_action();

  switch (app_state) {
    case PSA_CRYPTO_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . PSA Crypto initialization... ");
      if (init_psa_crypto() == PSA_SUCCESS) {
        print_key_storage();
      } else {
        app_state = PSA_CRYPTO_IDLE;
      }
      break;

    case SELECT_KEY_STORAGE:
      if (space_press) {
        space_press = false;
        symmetric_key_storage_select++;
        if (symmetric_key_storage_select > KEY_STORAGE_MAX) {
          symmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
        printf("  + Current HKDF key is a %s key.\n",
               symmetric_key_storage_string[symmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        printf("\n  . Current HKDF key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
        printf("  + Press SPACE to select HKDF key length (%d or %d or %d), "
               "press ENTER to next option.\n", symmetric_key_size[0],
               symmetric_key_size[1],
               symmetric_key_size[2]);
        app_state = SELECT_KEY_SIZE;
      }
      break;

    case SELECT_KEY_SIZE:
      if (space_press) {
        space_press = false;
        symmetric_key_size_select++;
        if (symmetric_key_size_select > KEY_SIZE_MAX) {
          symmetric_key_size_select = 0;
        }
        printf("  + Current HKDF key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        derive_process = DERIVE_KEY_ALGO;
        if ((derive_process == PSA_ALG_STREAM_CIPHER || derive_process == PSA_ALG_CHACHA20_POLY1305)
            && (symmetric_key_size_select != KEY_SIZE_MAX)) {
          printf("  + CHACHA20 algorithm can only use 256-bit key, press SPACE "
                 "to select the correct key length.\n");
          break;
        }
        printf("\n  . Current Hash algorithm for HKDF is %s.\n",
               hash_algo_string[hash_algo_select]);
        printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or %s "
               "or %s) for HKDF, press ENTER to run.\n", hash_algo_string[0],
               hash_algo_string[1], hash_algo_string[2], hash_algo_string[3],
               hash_algo_string[4]);
        app_state = SELECT_HASH_ALGO;
      }
      break;

    case SELECT_HASH_ALGO:
      if (space_press) {
        space_press = false;
        hash_algo_select++;
        if (hash_algo_select > HASH_ALGO_MAX) {
          hash_algo_select = 0;
        }
        printf("  + Current Hash algorithm for HKDF is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        derive_process = 0;
        app_state = IMPORT_MASTER_KEY;
      }
      break;

    case IMPORT_MASTER_KEY:
      import_master_key();
      break;

    case SETUP_HKDF:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting a HKDF operation... ");
      // Algorithm (PSA_ALG_HKDF or PSA_ALG_KEY_AGREEMENT) from IMPORT_MASTER_KEY
      if (setup_key_derivation() == PSA_SUCCESS) {
        app_state = SET_CAPACITY;
      }
      break;

    case SET_CAPACITY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting HKDF key size... ");
      if (set_key_derivation_capacity(symmetric_key_size[symmetric_key_size_select])
          == PSA_SUCCESS) {
        app_state = INPUT_SALT;
      }
      break;

    case INPUT_SALT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Inputting a salt... ");
      set_input_buf_ptr((uint8_t *)hkdf_salt);
      set_input_len(sizeof(hkdf_salt));
      if (provide_key_derivation_input(PSA_KEY_DERIVATION_INPUT_SALT)
          == PSA_SUCCESS) {
        app_state = INPUT_KEY;
      }
      break;

    case INPUT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      // The key_buf and key_len had already set in IMPORT_MASTER_KEY
      if (derive_process == 0) {
        printf("  + Inputting a %d-bit master key... ", sizeof(hkdf_ikm) * 8);
        if (provide_key_derivation_input_key(PSA_KEY_DERIVATION_INPUT_SECRET)
            == PSA_SUCCESS) {
          app_state = INPUT_INFO;
        }
      } else if (derive_process == 1) {
        printf("  + Inputting the %d-bit raw shared secret (ECDH)... ",
               sizeof(raw_shared_secret) * 8);
        if (provide_key_derivation_input_key(PSA_KEY_DERIVATION_INPUT_SECRET)
            == PSA_SUCCESS) {
          app_state = INPUT_INFO;
        }
      } else if (derive_process == 2) {
        // ECDH key agreement from client private key and server public key
        printf("  + Inputting the %d-bit ECDH key agreement (client private "
               "key)... ", sizeof(client_private_key) * 8);
        set_input_buf_ptr((uint8_t *)server_public_key);
        set_input_len(sizeof(server_public_key));
        if (derive_key_agreement(PSA_KEY_DERIVATION_INPUT_SECRET)
            == PSA_SUCCESS) {
          app_state = INPUT_INFO;
        }
      } else {
        // ECDH key agreement from server private key and client public key
        printf("  + Inputting the %d-bit ECDH key agreement (server private "
               "key)... ", sizeof(client_private_key) * 8);
        set_input_buf_ptr((uint8_t *)client_public_key);
        set_input_len(sizeof(client_public_key));
        if (derive_key_agreement(PSA_KEY_DERIVATION_INPUT_SECRET)
            == PSA_SUCCESS) {
          app_state = INPUT_INFO;
        }
      }
      break;

    case INPUT_INFO:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Inputting an information... ");
      set_input_buf_ptr((uint8_t *)hkdf_info);
      set_input_len(sizeof(hkdf_info));
      if (provide_key_derivation_input(PSA_KEY_DERIVATION_INPUT_INFO)
          == PSA_SUCCESS) {
        app_state = DERIVE_HKDF_KEY;
      }
      break;

    case DERIVE_HKDF_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Deriving a %d-bit %s HKDF key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      // Derived key is exportable for verification
      if (derive_key_attr(CREATE_KEY_OFFSET + symmetric_key_storage_select,
                          symmetric_key_size[symmetric_key_size_select],
                          DERIVE_KEY_ID,
                          PSA_KEY_USAGE_EXPORT,
                          DERIVE_KEY_ALGO) == PSA_SUCCESS) {
        if (derive_key(get_kdf_op_ptr()) == PSA_SUCCESS) {
          // Free resources
          reset_key_attr();
          // Save derived key identifier
          hkdf_key_id = get_key_id();
          printf("  + Aborting a HKDF operation... ");
          if (abort_key_derivation() == PSA_SUCCESS) {
            app_state = EXPORT_HKDF_KEY;
          }
        }
      }
      break;

    case SL_HKDF_SINGLE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Deriving a %d-bit %s HKDF key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      // Derived key is exportable for verification
      if (derive_key_attr(CREATE_KEY_OFFSET + symmetric_key_storage_select,
                          symmetric_key_size[symmetric_key_size_select],
                          DERIVE_KEY_ID,
                          PSA_KEY_USAGE_EXPORT,
                          DERIVE_KEY_ALGO) == PSA_SUCCESS) {
        if (sl_derive_key(PSA_ALG_HKDF(hash_algo[hash_algo_select]),
                          master_key_id,
                          hkdf_info, sizeof(hkdf_info),
                          hkdf_salt, sizeof(hkdf_salt), 0) == PSA_SUCCESS) {
          // Free resources
          reset_key_attr();
          // Save derived key identifier
          hkdf_key_id = get_key_id();
          app_state = EXPORT_HKDF_KEY;
        }
      } else {
        printf("Failed\n");
      }
      break;

    case EXPORT_HKDF_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Exporting a %d-bit %s HKDF key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      // Clear key buffer and set key ID
      memset(get_key_buf_ptr(), 0,
             symmetric_key_size[symmetric_key_size_select] / 8);
      set_key_id(hkdf_key_id);
      if (export_key() == PSA_SUCCESS) {
        printf("  + HKDF key:");
        print_buf(get_key_buf_ptr(), get_key_len());
        if (derive_process == 1) {
          // Save HKDF key from raw ECDH shared secret for verification
          memcpy(hkdf_key_copy, get_key_buf_ptr(), get_key_len());
          app_state = DESTROY_MASTER_KEY;
        } else {
          app_state = COMPARE_HKDF_KEY;
        }
      }
      break;

    case COMPARE_HKDF_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Comparing a %d-bit %s HKDF key with expected key data... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      if (compare_expect_okm() == 0) {
        printf("OK\n");
        app_state = DESTROY_MASTER_KEY;
      } else {
        printf("Failed\n");
      }
      break;

    case DESTROY_MASTER_KEY:
      app_state = PSA_CRYPTO_EXIT;
      if (derive_process == 0) {
        printf("  + Destroying a %d-bit %s master key... ",
               sizeof(hkdf_ikm) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
      } else {
        printf("  + Destroying a %d-bit %s master key... ",
               sizeof(raw_shared_secret) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
      }
      set_key_id(master_key_id);
      if (destroy_key() == PSA_SUCCESS) {
        master_key_id = 0;              // Reset master key identifier
        app_state = DESTROY_HKDF_KEY;
      }
      break;

    case DESTROY_HKDF_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Destroying a %d-bit %s HKDF key... ",
             symmetric_key_size[symmetric_key_size_select],
             symmetric_key_storage_string[symmetric_key_storage_select]);
      set_key_id(hkdf_key_id);
      if (destroy_key() == PSA_SUCCESS) {
        hkdf_key_id = 0;                // Reset HKDF key identifier
        // PSA Crypto doesn't support wrapped key on ECDH and HKDF
        if (symmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
          print_key_storage();
          break;
        }
        if (++derive_process == 4) {            // Next process
          derive_process = 0;
          print_key_storage();
        } else {
          app_state = IMPORT_MASTER_KEY;
        }
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy the keys regardless they are valid or not
      set_key_id(master_key_id);
      printf("  + Destroying a master key... ");
      destroy_key();
      set_key_id(hkdf_key_id);
      printf("  + Destroying a HKDF key... ");
      destroy_key();
      mbedtls_psa_crypto_free();
      app_state = PSA_CRYPTO_IDLE;
      break;

    case PSA_CRYPTO_IDLE:
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Retrieve input character from VCOM port.
 ******************************************************************************/
static void app_iostream_usart_process_action(void)
{
  int8_t c;

  c = getchar();
  if (c > 0) {
    if (c == ' ') {
      space_press = true;
    }
    if (c == '\r') {
      enter_press = true;
    }
  }
}

#if defined(MBEDTLS_ENTROPY_RAIL_PRESENT)
/***************************************************************************//**
 * It cancels the MCU to go to sleep when RAIL entropy is used.
 ******************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return false;
}
#endif

/***************************************************************************//**
 * Print key storage.
 ******************************************************************************/
static void print_key_storage(void)
{
  // Check if NVM3 can release any out-of-date objects to free up memory.
  while (nvm3_repackNeeded(nvm3_defaultHandle)) {
    printf("\n  . Repacking NVM... ");
    if (nvm3_repack(nvm3_defaultHandle) == ECODE_NVM3_OK) {
      printf("OK\n");
    } else {
      printf("Failed\n");
    }
  }

  printf("\n  . Current HKDF key is a %s key.\n",
         symmetric_key_storage_string[symmetric_key_storage_select]);
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s key, press ENTER to "
         "next option.\n",
         symmetric_key_storage_string[0],
         symmetric_key_storage_string[1],
         symmetric_key_storage_string[2],
         symmetric_key_storage_string[3]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", symmetric_key_storage_string[0],
         symmetric_key_storage_string[1]);
#endif
  app_state = SELECT_KEY_STORAGE;
}

/***************************************************************************//**
 * Import a master key.
 ******************************************************************************/
static void import_master_key(void)
{
  uint8_t *data = NULL;
  size_t key_size = 0;
  psa_key_type_t type = 0;
  psa_algorithm_t algo = 0;

  app_state = PSA_CRYPTO_EXIT;
  switch (derive_process) {
    case 0:
      data = (uint8_t *)hkdf_ikm;
      key_size = sizeof(hkdf_ikm);
      type = PSA_KEY_TYPE_NONE; // Set to PSA_KEY_TYPE_DERIVE during import
      algo = PSA_ALG_HKDF(hash_algo[hash_algo_select]); // HKDF only
      printf("\n  . Derive a HKDF (%s) key from a master key\n",
             hash_algo_string[hash_algo_select]);
      printf("  + Importing a %d-bit %s master key... ", key_size * 8,
             symmetric_key_storage_string[symmetric_key_storage_select]);
      break;

    case 1:
      data = (uint8_t *)raw_shared_secret;
      key_size = sizeof(raw_shared_secret);
      type = PSA_KEY_TYPE_NONE; // Set to PSA_KEY_TYPE_DERIVE during import
      algo = PSA_ALG_HKDF(hash_algo[hash_algo_select]); // HKDF only
      printf("\n  . Derive a HKDF (%s) key from the raw shared secret (ECDH)\n",
             hash_algo_string[hash_algo_select]);
      printf("  + Importing the %d-bit %s raw shared secret (ECDH)... ",
             key_size * 8,
             symmetric_key_storage_string[symmetric_key_storage_select]);
      break;

    case 2:
      data = (uint8_t *)client_private_key;
      key_size = sizeof(client_private_key);
      type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1); // For ECDH
      algo = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,        // ECDH and HKDF
                                   PSA_ALG_HKDF(hash_algo[hash_algo_select]));
      printf("\n  . Derive a HKDF (%s) key from the ECDH key agreement (client "
             "private key)\n", hash_algo_string[hash_algo_select]);
      printf("  + Importing a %d-bit %s client private key... ", key_size * 8,
             symmetric_key_storage_string[symmetric_key_storage_select]);
      break;

    case 3:
      data = (uint8_t *)server_private_key;
      key_size = sizeof(server_private_key);
      type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1); // For ECDH
      algo = PSA_ALG_KEY_AGREEMENT(PSA_ALG_ECDH,        // ECDH and HKDF
                                   PSA_ALG_HKDF(hash_algo[hash_algo_select]));
      printf("\n  . Derive a HKDF (%s) key from the ECDH key agreement (server "
             "private key)\n", hash_algo_string[hash_algo_select]);
      printf("  + Importing a %d-bit %s server private key... ", key_size * 8,
             symmetric_key_storage_string[symmetric_key_storage_select]);
      break;

    default:
      break;
  }

  // Setup buffer and length for key import
  memcpy(get_key_buf_ptr(), data, key_size);
  set_key_len(key_size);
  // Key algorithm is PSA_ALG_HKDF or PSA_ALG_KEY_AGREEMENT
  if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                        type,
                        key_size * 8,
                        MASTER_KEY_ID,
                        DEFAULT_KEY_USAGE,
                        algo) == PSA_SUCCESS) {
    // Free resources
    reset_key_attr();
    // Save master key identifier
    master_key_id = get_key_id();
    app_state = SETUP_HKDF;
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    // Using Silicon Labs custom API for wrapped key
    if (symmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
      app_state = SL_HKDF_SINGLE;
    }
#endif
  }
}

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len)
{
  uint32_t i;
  uint8_t hex_array[16] = "0123456789ABCDEF";

  for (i = 0; i < len; i++) {
    if ((i % 16) == 0) {
      printf("\n    ");
    }
    printf("%c", hex_array[(buf[i] >> 4) & 0x0f]);
    printf("%c ", hex_array[buf[i] & 0x0f]);
  }
  printf("\n");
}

/***************************************************************************//**
 * Compare HKDF key with expected key data.
 ******************************************************************************/
static int compare_expect_okm(void)
{
  if (derive_process == 0) {
    // Key algorithm is PSA_ALG_HKDF
    switch (hash_algo[hash_algo_select]) {
      case PSA_ALG_SHA_1:
        return(memcmp(get_key_buf_ptr(), (uint8_t *)expect_sha1_hkdf_okm,
                      get_key_len()));

      case PSA_ALG_SHA_224:
        return(memcmp(get_key_buf_ptr(), (uint8_t *)expect_sha224_hkdf_okm,
                      get_key_len()));

      case PSA_ALG_SHA_256:
        return(memcmp(get_key_buf_ptr(), (uint8_t *)expect_sha256_hkdf_okm,
                      get_key_len()));

      case PSA_ALG_SHA_384:
        return(memcmp(get_key_buf_ptr(), (uint8_t *)expect_sha384_hkdf_okm,
                      get_key_len()));

      case PSA_ALG_SHA_512:
        return(memcmp(get_key_buf_ptr(), (uint8_t *)expect_sha512_hkdf_okm,
                      get_key_len()));

      default:
        return(1);
    }
  } else {
    // Key algorithm is PSA_ALG_KEY_AGREEMENT
    return(memcmp(get_key_buf_ptr(), hkdf_key_copy, get_key_len()));
  }
}
