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
static uint8_t example_string[] = "PSA Crypto X.509 Example";

/// Asymmetric key storage selection
static uint8_t asymmetric_key_storage_select;

static const char *asymmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED",
  "BUILT-IN ECC + VOLATILE PLAIN",
  "BUILT-IN ECC + PERSISTENT PLAIN",
  "BUILT-IN ECC + VOLATILE WRAPPED",
  "BUILT-IN ECC + PERSISTENT WRAPPED",
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
  "SHA224",
  "SHA256",
  "SHA384",
  "SHA512"
};

static const mbedtls_md_type_t hash_algo[] = {
  MBEDTLS_MD_SHA224,
  MBEDTLS_MD_SHA256,
  MBEDTLS_MD_SHA384,
  MBEDTLS_MD_SHA512
};

static const psa_algorithm_t hash_algo_psa[] = {
  PSA_ALG_SHA_224,
  PSA_ALG_SHA_256,
  PSA_ALG_SHA_384,
  PSA_ALG_SHA_512
};

/// Root certificate key ID
static psa_key_id_t root_cert_key_id;

/// Device certificate key ID
static psa_key_id_t device_cert_key_id;

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
        asymmetric_key_storage_select++;
        if (asymmetric_key_storage_select > KEY_STORAGE_MAX) {
          asymmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
        printf("  + Current asymmetric key is a %s key.\n",
               asymmetric_key_storage_string[asymmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        if (asymmetric_key_storage_select > PERSISTENT_WRAP_KEY) {
          // Use built-in ECC key for root certificate
          printf("\n  . Current Hash algorithm is %s.\n",
                 hash_algo_string[hash_algo_select]);
          printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or "
                 "%s), press ENTER to run.\n", hash_algo_string[0],
                 hash_algo_string[1], hash_algo_string[2], hash_algo_string[3]);
          app_state = SELECT_HASH_ALGO;
          break;
        }

        printf("\n  . Current SECPxxxR1 key length is %d-bit (%s).\n",
               secpr1_key_size[secpr1_key_size_select],
               secpr1_key_size_string[secpr1_key_size_select]);
        printf("  + Press SPACE to select SECPxxxR1 key length (%d or %d or %d "
               "or %d), press ENTER to next option.\n", secpr1_key_size[0],
               secpr1_key_size[1], secpr1_key_size[2], secpr1_key_size[3]);
        app_state = SELECT_SECPR1_SIZE;
      }
      break;

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
        printf("\n  . Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
        printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or "
               "%s), press ENTER to run.\n", hash_algo_string[0],
               hash_algo_string[1], hash_algo_string[2], hash_algo_string[3]);
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
        app_state = INIT_ROOT_CSR;
      }
      break;

    case INIT_ROOT_CSR:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Create a root CSR\n");
      printf("  + Initializing a root CSR write context... ");
      if (init_csr_write_ctx(true, hash_algo[hash_algo_select])
          == PSA_SUCCESS) {
        app_state = CREATE_ROOT_KEY;
      }
      break;

    case CREATE_ROOT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      if (asymmetric_key_storage_select > PERSISTENT_WRAP_KEY) {
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        // Use built-in ECC key for root certificate
        set_key_id(SL_SE_BUILTIN_KEY_APPLICATION_ATTESTATION_ID);
#endif
        root_cert_key_id = get_key_id();
        app_state = WRAP_ROOT_PSA_KEY;
      } else {
        printf("  + Creating a %s (%d-bit) %s root certificate key... ",
               secpr1_key_size_string[secpr1_key_size_select],
               secpr1_key_size[secpr1_key_size_select],
               asymmetric_key_storage_string[asymmetric_key_storage_select]);

        if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                              PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
                              secpr1_key_size[secpr1_key_size_select],
                              ROOT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_ECDSA(hash_algo_psa[hash_algo_select])) == PSA_SUCCESS) {
          // Free resources
          reset_key_attr();
          root_cert_key_id = get_key_id();
          app_state = WRAP_ROOT_PSA_KEY;
        }
      }
      break;

    case WRAP_ROOT_PSA_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Initializing a root PK context to wrap a PSA key... ");
      if (init_pk_ctx(true, root_cert_key_id) == PSA_SUCCESS) {
        app_state = WRITE_ROOT_CSR_PEM;
      }
      break;

    case WRITE_ROOT_CSR_PEM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Writing a root CSR to a PEM string... ");
      if (write_csr_pem(true) == PSA_SUCCESS) {
        print_csr();
        app_state = LOAD_ROOT_CSR;
      }
      break;

    case LOAD_ROOT_CSR:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Create a root certificate\n");
      printf("  + Loading a root CSR... ");
      if (load_csr() == PSA_SUCCESS) {
        app_state = INIT_ROOT_CERT;
      }
      break;

    case INIT_ROOT_CERT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Storing the root CSR DN and initializing a root certificate "
             "write context... ");
      if (store_csr_dn(true) == PSA_SUCCESS) {
        printf("PSA_SUCCESS\n");
        init_crt_write_ctx(true);
        app_state = SET_ROOT_ISSUER;
      } else {
        printf("PSA_ERROR\n");
      }
      break;

    case SET_ROOT_ISSUER:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the issuer name for the root certificate... ");
      if (set_issuer_name() == PSA_SUCCESS) {
        app_state = SET_ROOT_SUBJECT;
      }
      break;

    case SET_ROOT_SUBJECT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the subject name for the root certificate... ");
      if (set_subject_name(true) == PSA_SUCCESS) {
        app_state = SET_ROOT_PARAMETER;
      }
      break;

    case SET_ROOT_PARAMETER:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the parameters for the root certificate... ");
      if (set_parameters(true, hash_algo[hash_algo_select]) == PSA_SUCCESS) {
        app_state = SET_ROOT_SERIAL;
      }
      break;

    case SET_ROOT_SERIAL:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the serial number for the root certificate... ");
      if (set_serial() == PSA_SUCCESS) {
        app_state = SET_ROOT_VALIDITY;
      }
      break;

    case SET_ROOT_VALIDITY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the validity period for the root certificate... ");
      if (set_validity(true) == PSA_SUCCESS) {
#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
        app_state = SET_ROOT_BASIC_CONSTRAINT;
#else
        app_state = WRITE_ROOT_CRT_PEM;
#endif
      }
      break;

#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
    case SET_ROOT_BASIC_CONSTRAINT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the basic constraints extension for the root "
             "certificate... ");
      if (set_basic_constraints(true) == PSA_SUCCESS) {
        app_state = SET_ROOT_KEY_USAGE;
      }
      break;

    case SET_ROOT_KEY_USAGE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the key usage extension for the root "
             "certificate... ");
      if (set_key_usage(true) == PSA_SUCCESS) {
        app_state = SET_ROOT_CERT_TYPE;
      }
      break;

    case SET_ROOT_CERT_TYPE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the certificate type extension for the root "
             "certificate... ");
      if (set_cert_type(true) == PSA_SUCCESS) {
#if defined(MBEDTLS_SHA1_C)
        app_state = SET_ROOT_SUBJECT_KEY_EXT;
#else
        app_state = WRITE_ROOT_CRT_PEM;
#endif
      }
      break;

#if defined(MBEDTLS_SHA1_C)
    case SET_ROOT_SUBJECT_KEY_EXT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the subject key identifier extension for the root "
             "certificate... ");
      if (set_subject_key_identifier() == PSA_SUCCESS) {
        app_state = SET_ROOT_AUTH_KEY_EXT;
      }
      break;

    case SET_ROOT_AUTH_KEY_EXT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the authority key identifier extension for the root "
             "certificate... ");
      if (set_authority_key_identifier() == PSA_SUCCESS) {
        app_state = WRITE_ROOT_CRT_PEM;
      }
      break;
#endif
#endif

    case WRITE_ROOT_CRT_PEM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Writing the root certificate to a X509 PEM string... ");
      if (write_crt_pem() == PSA_SUCCESS) {
        print_cert();
        free_create_ctx();
        app_state = PARSE_ROOT_CERT;
      }
      break;

    case PARSE_ROOT_CERT:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Create a device CSR\n");
      printf("  + Parsing the root certificate in PEM format... ");
      if (parse_cert(true) == PSA_SUCCESS) {
        app_state = STORE_ROOT_DN;
      }
      break;

    case STORE_ROOT_DN:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Storing the root certificate DN... ");
      if (store_root_dn() == PSA_SUCCESS) {
        printf("PSA_SUCCESS\n");
        app_state = INIT_DEVICE_CSR;
      } else {
        printf("PSA_ERROR\n");
      }
      break;

    case INIT_DEVICE_CSR:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Initializing a device CSR write context... ");
      if (init_csr_write_ctx(false, hash_algo[hash_algo_select])
          == PSA_SUCCESS) {
        app_state = CREATE_DEVICE_KEY;
      }
      break;

    case CREATE_DEVICE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      if (asymmetric_key_storage_select > PERSISTENT_WRAP_KEY) {
        // Built-in ECC key is SECP256R1
        printf("  + Creating a %s (%d-bit) %s device certificate key... ",
               secpr1_key_size_string[1],
               secpr1_key_size[1],
               asymmetric_key_storage_string[asymmetric_key_storage_select - 4]);

        if (create_import_key(CREATE_KEY_OFFSET + (asymmetric_key_storage_select - 4),
                              PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
                              secpr1_key_size[1],
                              DEVICE_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_ECDSA(hash_algo_psa[hash_algo_select])) == PSA_SUCCESS) {
          // Free resources
          reset_key_attr();
          device_cert_key_id = get_key_id();
          app_state = WRAP_DEVICE_PSA_KEY;
        }
      } else {
        printf("  + Creating a %s (%d-bit) %s device certificate key... ",
               secpr1_key_size_string[secpr1_key_size_select],
               secpr1_key_size[secpr1_key_size_select],
               asymmetric_key_storage_string[asymmetric_key_storage_select]);

        if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                              PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1),
                              secpr1_key_size[secpr1_key_size_select],
                              DEVICE_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_ECDSA(hash_algo_psa[hash_algo_select])) == PSA_SUCCESS) {
          // Free resources
          reset_key_attr();
          device_cert_key_id = get_key_id();
          app_state = WRAP_DEVICE_PSA_KEY;
        }
      }
      break;

    case WRAP_DEVICE_PSA_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Initializing a device PK context to wrap a PSA key... ");
      if (init_pk_ctx(false, device_cert_key_id) == PSA_SUCCESS) {
        app_state = WRITE_DEVICE_CSR_PEM;
      }
      break;

    case WRITE_DEVICE_CSR_PEM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Writing a device CSR to a PEM string... ");
      if (write_csr_pem(false) == PSA_SUCCESS) {
        print_csr();
        app_state = LOAD_DEVICE_CSR;
      }
      break;

    case LOAD_DEVICE_CSR:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Create a device certificate\n");
      printf("  + Loading a device CSR... ");
      if (load_csr() == PSA_SUCCESS) {
        app_state = INIT_DEVICE_CERT;
      }
      break;

    case INIT_DEVICE_CERT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Storing the device CSR DN and initializing a device "
             "certificate write context... ");
      if (store_csr_dn(false) == PSA_SUCCESS) {
        printf("PSA_SUCCESS\n");
        init_crt_write_ctx(false);
        app_state = SET_DEVICE_ISSUER;
      } else {
        printf("PSA_ERROR\n");
      }
      break;

    case SET_DEVICE_ISSUER:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the issuer name for the device certificate... ");
      if (set_issuer_name() == PSA_SUCCESS) {
        app_state = SET_DEVICE_SUBJECT;
      }
      break;

    case SET_DEVICE_SUBJECT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the subject name for the device certificate... ");
      if (set_subject_name(false) == PSA_SUCCESS) {
        app_state = SET_DEVICE_PARAMETER;
      }
      break;

    case SET_DEVICE_PARAMETER:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the parameters for the device certificate... ");
      if (set_parameters(false, hash_algo[hash_algo_select]) == PSA_SUCCESS) {
        app_state = SET_DEVICE_SERIAL;
      }
      break;

    case SET_DEVICE_SERIAL:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the serial number for the device certificate... ");
      if (set_serial() == PSA_SUCCESS) {
        app_state = SET_DEVICE_VALIDITY;
      }
      break;

    case SET_DEVICE_VALIDITY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the validity period for the device certificate... ");
      if (set_validity(false) == PSA_SUCCESS) {
#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
        app_state = SET_DEVICE_BASIC_CONSTRAINT;
#else
        app_state = WRITE_DEVICE_CRT_PEM;
#endif
      }
      break;

#if (CERT_VERSION == MBEDTLS_X509_CRT_VERSION_3)
    case SET_DEVICE_BASIC_CONSTRAINT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the basic constraints extension for the device "
             "certificate... ");
      if (set_basic_constraints(false) == PSA_SUCCESS) {
        app_state = SET_DEVICE_KEY_USAGE;
      }
      break;

    case SET_DEVICE_KEY_USAGE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the key usage extension for the device "
             "certificate... ");
      if (set_key_usage(false) == PSA_SUCCESS) {
        app_state = SET_DEVICE_CERT_TYPE;
      }
      break;

    case SET_DEVICE_CERT_TYPE:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the certificate type extension for the device "
             "certificate... ");
      if (set_cert_type(false) == PSA_SUCCESS) {
#if defined(MBEDTLS_SHA1_C)
        app_state = SET_DEVICE_SUBJECT_KEY_EXT;
#else
        app_state = WRITE_DEVICE_CRT_PEM;
#endif
      }
      break;

#if defined(MBEDTLS_SHA1_C)
    case SET_DEVICE_SUBJECT_KEY_EXT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the subject key identifier extension for the device "
             "certificate... ");
      if (set_subject_key_identifier() == PSA_SUCCESS) {
        app_state = SET_DEVICE_AUTH_KEY_EXT;
      }
      break;

    case SET_DEVICE_AUTH_KEY_EXT:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Setting the authority key identifier extension for the "
             "device certificate... ");
      if (set_authority_key_identifier() == PSA_SUCCESS) {
        app_state = WRITE_DEVICE_CRT_PEM;
      }
      break;
#endif
#endif

    case WRITE_DEVICE_CRT_PEM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Writing the device certificate to a X509 PEM string... ");
      if (write_crt_pem() == PSA_SUCCESS) {
        print_cert();
        free_create_ctx();
        app_state = VERIFY_CERT_CHAIN;
      }
      break;

    case VERIFY_CERT_CHAIN:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . Verify the certificate chain\n");
      printf("  + Parsing the device certificate in PEM format... ");
      if (parse_cert(false) == PSA_SUCCESS) {
#if (PSA_CRYPTO_PRINT_CERT == 0)
        printf("  + Verify the certificate chain with root certificate... ");
#endif
        if (verify_cert_chain() == PSA_SUCCESS) {
          free_cert_ctx();
          app_state = DESTROY_ROOT_KEY;
        }
      }
      break;

    case DESTROY_ROOT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      if (asymmetric_key_storage_select > PERSISTENT_WRAP_KEY) {
        // Use built-in ECC key for root certificate
        root_cert_key_id = 0;
        app_state = DESTROY_DEVICE_KEY;
      } else {
        printf("  + Destroying a %s (%d-bit) %s root certificate key... ",
               secpr1_key_size_string[secpr1_key_size_select],
               secpr1_key_size[secpr1_key_size_select],
               asymmetric_key_storage_string[asymmetric_key_storage_select]);
        set_key_id(root_cert_key_id);
        if (destroy_key() == PSA_SUCCESS) {
          app_state = DESTROY_DEVICE_KEY;
        }
      }
      break;

    case DESTROY_DEVICE_KEY:
      app_state = PSA_CRYPTO_EXIT;
      if (asymmetric_key_storage_select > PERSISTENT_WRAP_KEY) {
        // Built-in ECC key is SECP256R1
        printf("  + Destroying a %s (%d-bit) %s device certificate key... ",
               secpr1_key_size_string[1],
               secpr1_key_size[1],
               asymmetric_key_storage_string[asymmetric_key_storage_select - 4]);
      } else {
        printf("  + Destroying a %s (%d-bit) %s device certificate key... ",
               secpr1_key_size_string[secpr1_key_size_select],
               secpr1_key_size[secpr1_key_size_select],
               asymmetric_key_storage_string[asymmetric_key_storage_select]);
      }
      set_key_id(device_cert_key_id);
      if (destroy_key() == PSA_SUCCESS) {
        print_key_storage();
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy the keys regardless they are valid or not
      set_key_id(root_cert_key_id);
      printf("  + Destroying a root certificate key... ");
      destroy_key();
      set_key_id(device_cert_key_id);
      printf("  + Destroying a device certificate key... ");
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

  printf("\n  . Current asymmetric key is a %s key.\n",
         asymmetric_key_storage_string[asymmetric_key_storage_select]);

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s or \n"
         "    %s or %s or %s or %s key, press ENTER to next option.\n",
         asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1],
         asymmetric_key_storage_string[2],
         asymmetric_key_storage_string[3],
         asymmetric_key_storage_string[4],
         asymmetric_key_storage_string[5],
         asymmetric_key_storage_string[6],
         asymmetric_key_storage_string[7]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1]);
#endif
  app_state = SELECT_KEY_STORAGE;
}
