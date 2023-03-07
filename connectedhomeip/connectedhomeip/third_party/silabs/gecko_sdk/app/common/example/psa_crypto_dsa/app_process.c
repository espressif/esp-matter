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
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

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
static uint8_t example_string[] = "PSA Crypto DSA Example";

/// Asymmetric key storage selection
static uint8_t asymmetric_key_storage_select;

static const char *asymmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED",
  "BUILT-IN ECC"
};

/// Asymmetric key ECC curve selection
static uint8_t asymmetric_key_curve_select;

static const char *asymmetric_key_curve_string[] = {
  "SECPxxxR1",
  "Ed25519"
};

static const psa_ecc_family_t asymmetric_key_curve[] = {
  PSA_ECC_FAMILY_SECP_R1,
  PSA_ECC_FAMILY_TWISTED_EDWARDS
};

/// ECC SECPR1 key size selection
static uint8_t secpr1_key_size_select;

static const char *secpr1_key_size_string[] = {
  "SECP192R1",
  "SECP256R1",
  "SECP384R1",
  "SECP521R1",
};

static const size_t secpr1_key_size[] = {
  192,
  256,
  384,
  521
};

/// Hash algorithm selection
static uint8_t hash_algo_select;

static const char *hash_algo_string[] = {
  "NONE - PRECOMPUTED HASH",
  "SHA1",
  "SHA224",
  "SHA256",
  "SHA384",
  "SHA512"
};

static const psa_algorithm_t hash_algo[] = {
  0,
  PSA_ALG_SHA_1,
  PSA_ALG_SHA_224,
  PSA_ALG_SHA_256,
  PSA_ALG_SHA_384,
  PSA_ALG_SHA_512
};

/// Message size selection
static uint8_t msg_size_select;

static const uint32_t msg_size[] = {
  MSG_SIZE / 16,
  MSG_SIZE / 4,
  MSG_SIZE
};

/// Message buffer
static uint8_t msg_buf[MSG_SIZE];

/// Precomputed SHA256 hash value
static const uint8_t sha256_hash[] = {
  0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
  0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
  0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
  0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

/// Private sign key
static const uint8_t private_sign_key[] = {
  0x47, 0x94, 0xfc, 0x25, 0xb2, 0x34, 0x2b, 0x74,
  0xf8, 0x55, 0x9d, 0xa7, 0xb8, 0xf1, 0xd8, 0xaa,
  0xee, 0x89, 0x7a, 0x4c, 0x95, 0xa5, 0x59, 0x39,
  0xcc, 0x45, 0x66, 0x6d, 0x47, 0xcc, 0x01, 0xea
};

/// Public sign key
static const uint8_t public_sign_key[] = {
  0xc4, 0xaf, 0x4a, 0xc6, 0x9a, 0xab, 0x95, 0x12,
  0xdb, 0x50, 0xf7, 0xa2, 0x6a, 0xe5, 0xb4, 0x80,
  0x11, 0x83, 0xd8, 0x54, 0x17, 0xe7, 0x29, 0xa5,
  0x6d, 0xa9, 0x74, 0xf4, 0xe0, 0x8a, 0x56, 0x2c,
  0xde, 0x60, 0x19, 0xde, 0xa9, 0x41, 0x13, 0x32,
  0xdc, 0x1a, 0x74, 0x33, 0x72, 0xd1, 0x70, 0xb4,
  0x36, 0x23, 0x8a, 0x34, 0x59, 0x7c, 0x41, 0x0e,
  0xa1, 0x77, 0x02, 0x4d, 0xe2, 0x0f, 0xc8, 0x19
};

/// Private command key
static const uint8_t private_command_key[] = {
  0xac, 0x3f, 0x26, 0xd2, 0x0f, 0x24, 0x24, 0xe4,
  0xbb, 0x25, 0x6f, 0x30, 0x25, 0xe8, 0xd9, 0x75,
  0x9a, 0xaa, 0x06, 0x45, 0x1d, 0xe2, 0xdf, 0xdb,
  0x6e, 0x02, 0x7d, 0x72, 0x98, 0xcc, 0x1d, 0xb1
};

/// Public command key
static const uint8_t public_command_key[] = {
  0xb1, 0xbc, 0x6f, 0x6f, 0xa5, 0x66, 0x40, 0xed,
  0x52, 0x2b, 0x2e, 0xe0, 0xf5, 0xb3, 0xcf, 0x7e,
  0x5d, 0x48, 0xf6, 0x0b, 0xe8, 0x14, 0x8f, 0x0d,
  0xc0, 0x84, 0x40, 0xf0, 0xa4, 0xe1, 0xdc, 0xa4,
  0x7c, 0x04, 0x11, 0x9e, 0xd6, 0xa1, 0xbe, 0x31,
  0xb7, 0x70, 0x7e, 0x5f, 0x9d, 0x00, 0x1a, 0x65,
  0x9a, 0x05, 0x10, 0x03, 0xe9, 0x5e, 0x1b, 0x93,
  0x6f, 0x05, 0xc3, 0x7e, 0xa7, 0x93, 0xad, 0x63
};

/// Selected key size
static size_t selected_key_size;

/// Selected key string
static const char *selected_key_string = (char *)asymmetric_key_curve_string;

/// Algorithm for DSA key
static psa_algorithm_t key_algo;

/// Key storage for public key import
static uint8_t key_storage_public;

/// DSA process state variable
static dsa_state_t dsa_state;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  psa_status_t ret = PSA_SUCCESS;

  // Retrieve input character from VCOM port
  app_iostream_usart_process_action();

  switch (app_state) {
    case PSA_CRYPTO_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . PSA Crypto initialization... ");
      if (init_psa_crypto() == PSA_SUCCESS) {
        printf("  + Filling %d bytes message buffer with random number... ",
               MSG_SIZE);
        if (generate_random_number(msg_buf, MSG_SIZE) != PSA_SUCCESS) {
          app_state = PSA_CRYPTO_EXIT;
          break;
        }
        print_key_storage();
      } else {
        app_state = PSA_CRYPTO_IDLE;
      }
      break;

    case SELECT_KEY_STORAGE:
      if (space_press) {
        space_press = false;
        asymmetric_key_storage_select++;
        if (asymmetric_key_storage_select > KEY_STORAGE_MAX) {
          asymmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
#if defined(SEMAILBOX_PRESENT)
        if (asymmetric_key_storage_select == KEY_STORAGE_MAX) {
          printf("  + Current asymmetric key is a %s key.\n",
                 asymmetric_key_storage_string[4]);
          break;
        }
#endif
        printf("  + Current asymmetric key is a %s key.\n",
               asymmetric_key_storage_string[asymmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        key_storage_public = asymmetric_key_storage_select;
#if defined(SEMAILBOX_PRESENT)
        if (asymmetric_key_storage_select == KEY_STORAGE_MAX) {
          app_state = IMPORT_PRIVATE_KEY;
          dsa_state = ECDSA_SIGN_KEY_SIGN_HASH;
          break;
        }
#endif

#if defined(SEMAILBOX_PRESENT)
        printf("\n  . Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
        printf("  + Press SPACE to select ECC curve (%s or %s), press ENTER to "
               "next option.\n", asymmetric_key_curve_string[0],
               asymmetric_key_curve_string[1]);
        app_state = SELECT_KEY_CURVE;
      }
      break;

    case SELECT_KEY_CURVE:
      if (space_press) {
        space_press = false;
        asymmetric_key_curve_select++;
        if (asymmetric_key_curve_select > KEY_CURVE_MAX) {
          asymmetric_key_curve_select = 0;
        }
        printf("  + Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
      }
      if (enter_press) {
        enter_press = false;
        if (asymmetric_key_curve_select == 0) {
          printf("\n  . Current SECPxxxR1 key length is %d-bit (%s).\n",
                 secpr1_key_size[secpr1_key_size_select],
                 secpr1_key_size_string[secpr1_key_size_select]);
          printf("  + Press SPACE to select SECPxxxR1 key length (%d or %d or "
                 "%d or %d), press ENTER to next option.\n", secpr1_key_size[0],
                 secpr1_key_size[1], secpr1_key_size[2], secpr1_key_size[3]);
          app_state = SELECT_SECPR1_SIZE;
        } else {
          // No hash algorithm selection for EdDSA
          selected_key_size = 255;
          selected_key_string = asymmetric_key_curve_string[asymmetric_key_curve_select];
          printf("\n  . Current data length is %lu bytes.\n",
                 msg_size[msg_size_select]);
          printf("  + Press SPACE to select data length (%lu or %lu or %lu), "
                 "press ENTER to run.\n", msg_size[0], msg_size[1],
                 msg_size[2]);
          app_state = SELECT_DATA_SIZE;
        }
      }
      break;
#else
        printf("\n  . Current SECPxxxR1 key length is %d-bit (%s).\n",
               secpr1_key_size[secpr1_key_size_select],
               secpr1_key_size_string[secpr1_key_size_select]);
        printf("  + Press SPACE to select SECPxxxR1 key length (%d or %d or %d "
               "or %d), press ENTER to next option.\n", secpr1_key_size[0],
               secpr1_key_size[1], secpr1_key_size[2], secpr1_key_size[3]);
        app_state = SELECT_SECPR1_SIZE;
      }
      break;
#endif

    case SELECT_SECPR1_SIZE:
      if (space_press) {
        space_press = false;
        secpr1_key_size_select++;
        if (secpr1_key_size_select > SECPR1_SIZE_MAX) {
          secpr1_key_size_select = 0;
        }
        printf("  + Current SECPxxxR1 key length is %d-bit (%s).\n",
               secpr1_key_size[secpr1_key_size_select],
               secpr1_key_size_string[secpr1_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key_size = secpr1_key_size[secpr1_key_size_select];
        selected_key_string = secpr1_key_size_string[secpr1_key_size_select];
        printf("\n  . Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
        printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or %s "
               "or %s or %s), press ENTER to next option or run if Hash "
               "algorithm is NONE.\n", hash_algo_string[0], hash_algo_string[1],
               hash_algo_string[2], hash_algo_string[3],
               hash_algo_string[4], hash_algo_string[5]);
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
        printf("  + Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        if (hash_algo_select == 0) {
          app_state = CREATE_KEY;
        } else {
          printf("\n  . Current data length is %lu bytes.\n",
                 msg_size[msg_size_select]);
          printf("  + Press SPACE to select data length (%lu or %lu or %lu), "
                 "press ENTER to run.\n", msg_size[0], msg_size[1],
                 msg_size[2]);
          app_state = SELECT_DATA_SIZE;
        }
      }
      break;

    case SELECT_DATA_SIZE:
      if (space_press) {
        space_press = false;
        msg_size_select++;
        if (msg_size_select > MSG_SIZE_MAX) {
          msg_size_select = 0;
        }
        printf("  + Current data length is %lu bytes.\n",
               msg_size[msg_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        app_state = CREATE_KEY;
      }
      break;

    case CREATE_KEY:
      dsa_state = DSA_SIGN_HASH;
      if (hash_algo[hash_algo_select] == 0
          && asymmetric_key_curve_select == 0) {
        // Already calculated hash for ECDSA without hashing
        app_state = SIGN_HASH;
        key_algo = PSA_ALG_ECDSA_ANY;
        printf("\n  . Sign an already-calculated SHA256 hash (ECDSA).\n");
      } else {
        // Any hash algorithm for ECDSA or EdDSA with hashing (hash-and-sign)
        app_state = HASH_AND_SIGN;
        if (asymmetric_key_curve_select == 0) {
          key_algo = PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);
          printf("\n  . Hash (%s) and sign a message (ECDSA).\n",
                 hash_algo_string[hash_algo_select]);
        } else {
          key_algo = PSA_ALG_PURE_EDDSA;
          printf("\n  . Hash and sign a message (EdDSA).\n");
        }
      }

      printf("  + Creating a %s (%d-bit) %s key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);

      if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            PERSISTENT_KEY_ID,
                            DEFAULT_KEY_USAGE,
                            key_algo) == PSA_SUCCESS) {
        // Free resources
        reset_key_attr();
      } else {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case SIGN_HASH:
      app_state = PSA_CRYPTO_EXIT;
      // Setup for sign and verification
      set_dsa_algo(PSA_ALG_ECDSA_ANY);
      set_msg_hash_buf_ptr((uint8_t *)sha256_hash);
      set_msg_hash_len(sizeof(sha256_hash));
      printf("  + Signing a hash with a %s (%d-bit) %s "
             "private key... ", secpr1_key_size_string[secpr1_key_size_select],
             secpr1_key_size[secpr1_key_size_select],
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      if (sign_hash() == PSA_SUCCESS) {
        app_state = EXPORT_PUBLIC_KEY;
      }
      break;

    case HASH_AND_SIGN:
      app_state = PSA_CRYPTO_EXIT;
      // Setup for sign and verification
      set_msg_hash_buf_ptr(msg_buf);
      set_msg_hash_len(msg_size[msg_size_select]);
      if (asymmetric_key_curve_select == 0) {
        set_dsa_algo(PSA_ALG_ECDSA(hash_algo[hash_algo_select]));
      } else {
        // No hash algorithm for EdDSA
        set_dsa_algo(PSA_ALG_PURE_EDDSA);
      }
      printf("  + Hashing %lu bytes message and signing with a %s (%d-bit) "
             "%s private key... ",
             msg_size[msg_size_select],
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      if (sign_message() == PSA_SUCCESS) {
        app_state = EXPORT_PUBLIC_KEY;
      }
      break;

    case EXPORT_PUBLIC_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Exporting a public key of a %s (%d-bit) %s key... ",
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      // Clear key buffer to export public key
      memset(get_key_buf_ptr(), 0, KEY_BUF_SIZE);
      if (export_public_key() == PSA_SUCCESS) {
        app_state = DESTROY_KEY;
      }
      break;

    case IMPORT_PUBLIC_KEY:
      dsa_state = DSA_VERIFY_HASH;
      // Setup length to import SECPR1 public key
      set_key_len(PSA_EXPORT_KEY_OUTPUT_SIZE(PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1),
                                             secpr1_key_size[secpr1_key_size_select]));
      if (hash_algo[hash_algo_select] == 0
          && asymmetric_key_curve_select == 0) {
        // Already calculated hash for ECDSA without hashing
        app_state = VERIFY_HASH_SIGNATURE;
        key_algo = PSA_ALG_ECDSA_ANY;
        printf("\n  . Verify the signature of an already-calculated SHA256 "
               "hash.\n");
      } else {
        // Any hash algorithm for ECDSA or EdDSA with hashing (hash-and-verify)
        app_state = HASH_AND_VERIFY_SIGNATURE;
        if (asymmetric_key_curve_select == 0) {
          key_algo = PSA_ALG_ECDSA(PSA_ALG_ANY_HASH);
          printf("\n  . Hash (%s) a message and verify the signature "
                 "(ECDSA).\n", hash_algo_string[hash_algo_select]);
        } else {
          set_key_len(32);
          key_algo = PSA_ALG_PURE_EDDSA;
          printf("\n  . Hash a message and verify the signature (EdDSA).\n");
        }
      }

      // Public key cannot be wrapped
      if (asymmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
        key_storage_public -= VOLATILE_WRAP_KEY;
      }

      printf("  + Importing a %s (%d-bit) %s public key... ",
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[key_storage_public]);

      if (create_import_key(IMPORT_KEY_OFFSET + key_storage_public,
                            PSA_KEY_TYPE_ECC_PUBLIC_KEY(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            PERSISTENT_KEY_ID,
                            DEFAULT_KEY_USAGE,
                            key_algo) == PSA_SUCCESS) {
        // Free resources
        reset_key_attr();
      } else {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case VERIFY_HASH_SIGNATURE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Verifying the signature of a hash with a %s (%d-bit) %s "
             "public key... ",
             secpr1_key_size_string[secpr1_key_size_select],
             secpr1_key_size[secpr1_key_size_select],
             asymmetric_key_storage_string[key_storage_public]);
      if (verify_hash() == PSA_SUCCESS) {
        app_state = DESTROY_KEY;
      }
      break;

    case HASH_AND_VERIFY_SIGNATURE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Hashing %lu bytes message and verifying the signature with a "
             "%s (%d-bit) %s public key... ",
             msg_size[msg_size_select],
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[key_storage_public]);
      if (verify_message() == PSA_SUCCESS) {
        app_state = DESTROY_KEY;
      }
      break;

    case DESTROY_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Destroying a %s (%d-bit) %s key... ",
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[key_storage_public]);
      if (destroy_key() == PSA_SUCCESS) {
        if (dsa_state == DSA_SIGN_HASH) {
          // From ECDSA or EdDSA sign, import public key to verify signature
          app_state = IMPORT_PUBLIC_KEY;
        } else {
          // From ECDSA or EdDSA verify
          print_key_storage();
        }
      }
      break;

    case IMPORT_PRIVATE_KEY:
      set_key_len(32);
      if (dsa_state == ECDSA_SIGN_KEY_SIGN_HASH) {
        memcpy(get_key_buf_ptr(), private_sign_key, sizeof(private_sign_key));
        printf("\n  . Sign an already-calculated hash with private sign "
               "key.\n");
        printf("  + Importing a SECP256R1 (256-bit) private sign key... ");
      } else {
        memcpy(get_key_buf_ptr(), private_command_key,
               sizeof(private_command_key));
        printf("\n  . Sign an already-calculated hash with private command "
               "key.\n");
        printf("  + Importing a SECP256R1 (256-bit) private command key... ");
      }
      // Import private sign key or private command key
      app_state = PSA_CRYPTO_EXIT;
      if (create_import_key(IMPORT_KEY_OFFSET + VOLATILE_PLAIN_KEY,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
                            256,
                            PERSISTENT_KEY_ID,
                            DEFAULT_KEY_USAGE,
                            PSA_ALG_ECDSA_ANY) == PSA_SUCCESS) {
        app_state = SIGN_HASH_BUILTIN;
      }
      break;

    case SIGN_HASH_BUILTIN:
      // Setup for sign and verification
      set_dsa_algo(PSA_ALG_ECDSA_ANY);
      set_msg_hash_buf_ptr((uint8_t *)sha256_hash);
      set_msg_hash_len(sizeof(sha256_hash));
      if (dsa_state == ECDSA_SIGN_KEY_SIGN_HASH) {
        printf("  + Signing a hash with private sign key... ");
      } else if (dsa_state == ECDSA_SIGN_KEY_VERIFY_HASH) {
        printf("  + Signing a hash with private command key... ");
        dsa_state = ECDSA_COMMAND_KEY_SIGN_HASH;
      } else {
        printf("  + Signing a hash with built-in private device key... ");
      }
      app_state = PSA_CRYPTO_EXIT;
      if (sign_hash() == PSA_SUCCESS) {
        if (dsa_state == ECDSA_DEVICE_KEY_SIGN_HASH) {
          // No key import if using device key
          app_state = EXPORT_PUBLIC_BUILTIN_KEY;
        } else {
          app_state = CLOSE_BUILTIN_KEY;
        }
      }
      break;

#if defined(SEMAILBOX_PRESENT)
    case OPEN_BUILTIN_KEY:
      if (dsa_state == ECDSA_SIGN_KEY_SIGN_HASH) {
        printf("\n  . Verify the signature of an already-calculated hash "
               "with public sign key.\n");
        set_key_id(SL_SE_BUILTIN_KEY_SECUREBOOT_ID);
      } else if (dsa_state == ECDSA_COMMAND_KEY_SIGN_HASH) {
        printf("\n  . Verify the signature of an already-calculated hash "
               "with public command key.\n");
        set_key_id(SL_SE_BUILTIN_KEY_SECUREDEBUG_ID);
      } else {
        printf("\n  . Sign an already-calculated hash with private device "
               "key.\n");
        dsa_state = ECDSA_DEVICE_KEY_SIGN_HASH;
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        set_key_id(SL_SE_BUILTIN_KEY_APPLICATION_ATTESTATION_ID);
#endif
      }
      app_state = EXPORT_PUBLIC_BUILTIN_KEY;
      if (dsa_state == ECDSA_DEVICE_KEY_SIGN_HASH) {
        app_state = SIGN_HASH_BUILTIN;
      }
      break;
#endif

    case EXPORT_PUBLIC_BUILTIN_KEY:
      app_state = CLOSE_BUILTIN_KEY;
      if (dsa_state == ECDSA_SIGN_KEY_SIGN_HASH) {
        printf("  + Exporting a built-in SECP256R1 public sign key... ");
        dsa_state = ECDSA_SIGN_KEY_VERIFY_HASH;
        ret = export_public_key();
        if (ret == PSA_ERROR_INVALID_HANDLE || ret == PSA_ERROR_DOES_NOT_EXIST) {
          printf("  + The public sign key has not been provisioned.\n");
          break;
        }
      } else if (dsa_state == ECDSA_COMMAND_KEY_SIGN_HASH) {
        printf("  + Exporting a built-in SECP256R1 public command key... ");
        dsa_state = ECDSA_COMMAND_KEY_VERIFY_HASH;
        ret = export_public_key();
        if (ret == PSA_ERROR_INVALID_HANDLE || ret == PSA_ERROR_DOES_NOT_EXIST) {
          printf("  + The public command key has not been provisioned.\n");
          break;
        }
      } else {
        printf("\n  . Verify the signature of an already-calculated hash "
               "with public device key.\n");
        printf("  + Exporting a built-in SECP256R1 public device key... ");
        dsa_state = ECDSA_DEVICE_KEY_VERIFY_HASH;
        ret = export_public_key();
      }
      app_state = PSA_CRYPTO_EXIT;
      if (ret == PSA_SUCCESS) {
        printf("  + Public key:");
        // Skip the first byte in uncompressed point format
        print_buf(get_key_buf_ptr() + 1, get_key_len() - 1);
        app_state = VERIFY_HASH_SIGNATURE_BUILTIN;
      }
      break;

    case VERIFY_HASH_SIGNATURE_BUILTIN:
      app_state = CLOSE_BUILTIN_KEY;
      if (dsa_state == ECDSA_SIGN_KEY_VERIFY_HASH) {
        // Skip the first byte in uncompressed point format
        if (memcmp(get_key_buf_ptr() + 1, public_sign_key, get_key_len() - 1)
            != 0) {
          printf("  + The private and public sign key is not a valid key "
                 "pair.\n");
          break;
        }
        printf("  + Verifying the signature of a hash with public sign "
               "key... ");
      } else if (dsa_state == ECDSA_COMMAND_KEY_VERIFY_HASH) {
        // Skip the first byte in uncompressed point format
        if (memcmp(get_key_buf_ptr() + 1, public_command_key, get_key_len() - 1)
            != 0) {
          printf("  + The private and public command key is not a valid key "
                 "pair.\n");
          break;
        }
        printf("  + Verifying the signature of a hash with public command "
               "key... ");
      } else {
        printf("  + Verifying the signature of a hash with public device "
               "key... ");
      }
      if (verify_hash() != PSA_SUCCESS) {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case CLOSE_BUILTIN_KEY:
      if (dsa_state == ECDSA_SIGN_KEY_SIGN_HASH) {
        printf("  + Destroying a private sign key... ");
        app_state = OPEN_BUILTIN_KEY;
      } else if (dsa_state == ECDSA_SIGN_KEY_VERIFY_HASH) {
        // Public sign key
        set_key_id(0);
        app_state = IMPORT_PRIVATE_KEY;
        break;
      } else if (dsa_state == ECDSA_COMMAND_KEY_SIGN_HASH) {
        printf("  + Destroying a private command key... ");
        app_state = OPEN_BUILTIN_KEY;
      } else if (dsa_state == ECDSA_COMMAND_KEY_VERIFY_HASH) {
        // Public command key
        set_key_id(0);
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        // Open a device key if Secure Vault device
        app_state = OPEN_BUILTIN_KEY;
#else
        app_state = SELECT_KEY_STORAGE;
        print_key_storage();
#endif
        break;
      } else {
        // Public device key
        set_key_id(0);
        app_state = SELECT_KEY_STORAGE;
        print_key_storage();
        break;
      }
      // Destroy private sign or command key
      if (destroy_key() != PSA_SUCCESS) {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy a key regardless it is valid or not
      printf("  + Destroying an asymmetric key... ");
      destroy_key();
      mbedtls_psa_crypto_free();
      app_state = PSA_CRYPTO_IDLE;
      break;

    case PSA_CRYPTO_IDLE:
    default:
      break;
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

  if (asymmetric_key_storage_select == KEY_STORAGE_MAX) {
#if defined(SEMAILBOX_PRESENT)
    printf("\n  + Current asymmetric key is a %s key.\n",
           asymmetric_key_storage_string[4]);
#else
    printf("\n  . Current asymmetric key is a %s key.\n",
           asymmetric_key_storage_string[asymmetric_key_storage_select]);
#endif
  } else {
    printf("\n  . Current asymmetric key is a %s key.\n",
           asymmetric_key_storage_string[asymmetric_key_storage_select]);
  }

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s or %s key, press "
         "ENTER to next option or run if %s key is selected.\n",
         asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1],
         asymmetric_key_storage_string[2],
         asymmetric_key_storage_string[3],
         asymmetric_key_storage_string[4],
         asymmetric_key_storage_string[4]);
#else
#if defined(SEMAILBOX_PRESENT)
  printf("  + Press SPACE to select a %s or %s or %s key, press ENTER to next "
         "option or run if %s key is selected.\n",
         asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1],
         asymmetric_key_storage_string[4],
         asymmetric_key_storage_string[4]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1]);
#endif
#endif
  app_state = SELECT_KEY_STORAGE;
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
