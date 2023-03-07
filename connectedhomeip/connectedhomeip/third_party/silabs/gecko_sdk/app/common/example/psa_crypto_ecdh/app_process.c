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
 * Print asymmetric key size option.
 ******************************************************************************/
static void print_key_size_option(void);

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
static uint8_t example_string[] = "PSA Crypto ECDH Example";

/// Asymmetric key storage selection
static uint8_t asymmetric_key_storage_select;

static const char *asymmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED"
};

/// Asymmetric key ECC curve selection
static uint8_t asymmetric_key_curve_select;

static const char *asymmetric_key_curve_string[] = {
  "SECPxxxR1",
  "MONTGOMERY"
};

static const psa_ecc_family_t asymmetric_key_curve[] = {
  PSA_ECC_FAMILY_SECP_R1,
  PSA_ECC_FAMILY_MONTGOMERY
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

/// ECC Montgomery key size selection
static uint8_t montgomery_key_size_select;

static const char *montgomery_key_size_string[] = {
  "CURVE25519 (X25519)",
  "CURVE448 (X448)"
};

static const size_t montgomery_key_size[] = {
  255,
  448
};

/// Selected key size
static size_t selected_key_size;

/// Selected key string
static const char *selected_key_string;

/// Client key identifier
static psa_key_id_t client_key_id;

/// Server key identifier
static psa_key_id_t server_key_id;

/// Shared secret copy
static uint8_t shared_secret_copy[SHARED_SECRET_SIZE];

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
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("\n  . Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
        printf("  + Press SPACE to select ECC curve (%s or %s), press ENTER to "
               "next option.\n",
               asymmetric_key_curve_string[0],
               asymmetric_key_curve_string[1]);
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 0) {
          printf("\n  . Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        } else {
          printf("\n  . Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        }
        printf("  + Press SPACE to select ECC curve (%s or %s), press ENTER to "
               "next option or run if %s is selected.\n",
               asymmetric_key_curve_string[0],
               montgomery_key_size_string[0], montgomery_key_size_string[0]);
#endif
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
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Current ECC curve is %s.\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select]);
#else
        // No software fallback on X448
        if (asymmetric_key_curve_select == 0) {
          printf("  + Current ECC curve is %s.\n",
                 asymmetric_key_curve_string[asymmetric_key_curve_select]);
        } else {
          printf("  + Current ECC curve is %s.\n",
                 montgomery_key_size_string[0]);
        }
#endif
      }
      if (enter_press) {
        enter_press = false;
        print_key_size_option();
      }
      break;

    case SELECT_SECPR1_SIZE:
      if (space_press) {
        space_press = false;
        secpr1_key_size_select++;
        if (secpr1_key_size_select > SECPR1_SIZE_MAX) {
          secpr1_key_size_select = 0;
        }
        printf("  + Current %s key length is %d-bit (%s).\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select],
               secpr1_key_size[secpr1_key_size_select],
               secpr1_key_size_string[secpr1_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key_size = secpr1_key_size[secpr1_key_size_select];
        selected_key_string = secpr1_key_size_string[secpr1_key_size_select];
        app_state = CREATE_CLIENT_KEY;
      }
      break;

    case SELECT_MONTGOMERY_SIZE:
      if (space_press) {
        space_press = false;
        montgomery_key_size_select++;
        if (montgomery_key_size_select > MONTGOMERY_SIZE_MAX) {
          montgomery_key_size_select = 0;
        }
        printf("  + Current %s key length is %d-bit (%s).\n",
               asymmetric_key_curve_string[asymmetric_key_curve_select],
               montgomery_key_size[montgomery_key_size_select],
               montgomery_key_size_string[montgomery_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        selected_key_size = montgomery_key_size[montgomery_key_size_select];
        selected_key_string = montgomery_key_size_string[montgomery_key_size_select];
        app_state = CREATE_CLIENT_KEY;
      }
      break;

    case CREATE_CLIENT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . ECDH Client\n");
      printf("  + Creating a %s (%d-bit) %s client key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            CLIENT_KEY_ID,
                            DEFAULT_KEY_USAGE,
                            PSA_ALG_ECDH) == PSA_SUCCESS) {
        // Free resources
        reset_key_attr();
        // Save client key identifier
        client_key_id = get_key_id();
        app_state = CREATE_SERVER_KEY;
      }
      break;

    case CREATE_SERVER_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Creating a %s (%d-bit) %s server key... ",
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);

      if (create_import_key(CREATE_KEY_OFFSET + asymmetric_key_storage_select,
                            PSA_KEY_TYPE_ECC_KEY_PAIR(asymmetric_key_curve[asymmetric_key_curve_select]),
                            selected_key_size,
                            SERVER_KEY_ID,
                            DEFAULT_KEY_USAGE,
                            PSA_ALG_ECDH) == PSA_SUCCESS) {
        // Free resources
        reset_key_attr();
        // Save server key identifier
        server_key_id = get_key_id();
        app_state = EXPORT_SERVER_PUBLIC_KEY;
      }
      break;

    case EXPORT_SERVER_PUBLIC_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Exporting a public key of a %s (%d-bit) %s server key... ",
             selected_key_string,
             selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      // Export server public key
      set_key_id(server_key_id);
      if (export_public_key() == PSA_SUCCESS) {
        app_state = COMPUTE_CLIENT_SHARED_SECRET;
      }
      break;

    case COMPUTE_CLIENT_SHARED_SECRET:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Computing client shared secret with a %s (%d-bit) server "
             "public key... ", selected_key_string, selected_key_size);
      // Use server public key to compute shared secret
      memset(get_shared_secret_buf_ptr(), 0, SHARED_SECRET_SIZE);
      set_key_id(client_key_id);
      set_peer_public_key_buf_ptr(get_key_buf_ptr());
      set_peer_public_key_len(get_key_len());
      if (compute_ecdh_shared_secret() == PSA_SUCCESS) {
        // Save client shared secret
        memcpy(shared_secret_copy, get_shared_secret_buf_ptr(),
               get_shared_secret_length());
        app_state = EXPORT_CLIENT_PUBLIC_KEY;
      }
      break;

    case EXPORT_CLIENT_PUBLIC_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . ECDH Server\n");
      printf("  + Exporting a public key of a %s (%d-bit) %s client key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      // Export client public key
      set_key_id(client_key_id);
      if (export_public_key() == PSA_SUCCESS) {
        app_state = COMPUTE_SERVER_SHARED_SECRET;
      }
      break;

    case COMPUTE_SERVER_SHARED_SECRET:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Computing server shared secret with a %s (%d-bit) client "
             "public key... ", selected_key_string, selected_key_size);
      // Use client public key to compute shared secret
      memset(get_shared_secret_buf_ptr(), 0, SHARED_SECRET_SIZE);
      set_key_id(server_key_id);
      set_peer_public_key_buf_ptr(get_key_buf_ptr());
      set_peer_public_key_len(get_key_len());
      if (compute_ecdh_shared_secret() == PSA_SUCCESS) {
        printf("  + Comparing shared secret from client and server... ");
        if (memcmp(shared_secret_copy, get_shared_secret_buf_ptr(),
                   get_shared_secret_length()) == 0) {
          printf("OK\n");
          app_state = DESTROY_CLIENT_KEY;
        } else {
          printf("Failed\n");
        }
      }
      break;

    case DESTROY_CLIENT_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Destroying a %s (%d-bit) %s client key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      set_key_id(client_key_id);
      if (destroy_key() == PSA_SUCCESS) {
        client_key_id = 0;      // Reset client key identifier
        app_state = DESTROY_SERVER_KEY;
      }
      break;

    case DESTROY_SERVER_KEY:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Destroying a %s (%d-bit) %s server key... ",
             selected_key_string, selected_key_size,
             asymmetric_key_storage_string[asymmetric_key_storage_select]);
      set_key_id(server_key_id);
      if (destroy_key() == PSA_SUCCESS) {
        server_key_id = 0;      // Reset server key identifier
        print_key_storage();
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy the keys regardless they are valid or not
      set_key_id(client_key_id);
      printf("  + Destroying a client key... ");
      destroy_key();
      set_key_id(server_key_id);
      printf("  + Destroying a server key... ");
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
  printf("  + Press SPACE to select a %s or %s or %s or %s key, press ENTER to "
         "next option.\n",
         asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1],
         asymmetric_key_storage_string[2],
         asymmetric_key_storage_string[3]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", asymmetric_key_storage_string[0],
         asymmetric_key_storage_string[1]);
#endif
  app_state = SELECT_KEY_STORAGE;
}

/***************************************************************************//**
 * Print asymmetric key size option.
 ******************************************************************************/
static void print_key_size_option(void)
{
  if (asymmetric_key_curve_select == 0) {
    printf("\n  . Current %s key length is %d-bit (%s).\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           secpr1_key_size[secpr1_key_size_select],
           secpr1_key_size_string[secpr1_key_size_select]);
    printf("  + Press SPACE to select %s key length (%d or %d or %d or %d), "
           "press ENTER to run.\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           secpr1_key_size[0], secpr1_key_size[1], secpr1_key_size[2],
           secpr1_key_size[3]);
    app_state = SELECT_SECPR1_SIZE;
  } else {
#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    printf("\n  . Current %s key length is %d-bit (%s).\n",
           asymmetric_key_curve_string[asymmetric_key_curve_select],
           montgomery_key_size[montgomery_key_size_select],
           montgomery_key_size_string[montgomery_key_size_select]);
    printf("  + Press SPACE to select %s key length (%d or %d), press ENTER to "
           "run.\n", asymmetric_key_curve_string[asymmetric_key_curve_select],
           montgomery_key_size[0], montgomery_key_size[1]);
    app_state = SELECT_MONTGOMERY_SIZE;
#else
    // No software fallback on X448, only X25519 is available
    selected_key_size = 255;
    selected_key_string = asymmetric_key_curve_string[asymmetric_key_curve_select];
    app_state = CREATE_CLIENT_KEY;
#endif
  }
}
