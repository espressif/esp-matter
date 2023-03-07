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
 * Fill buffers for cipher operations.
 *
 * @returns Returns PSA error code, @ref crypto_values.h.
 ******************************************************************************/
static psa_status_t fill_buf_with_random_number(void);

/***************************************************************************//**
 * Print key storage.
 ******************************************************************************/
static void print_key_storage(void);

/***************************************************************************//**
 * Single part AEAD encryption.
 *
 * @param algo The AEAD algorithm.
 ******************************************************************************/
static void encrypt_aead_single(psa_algorithm_t algo);

/***************************************************************************//**
 * Single part AEAD decryption.
 ******************************************************************************/
static void decrypt_aead_single(void);

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
static uint8_t example_string[] = "PSA Crypto AEAD Example";

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

/// Plain message size selection
static uint8_t plain_msg_size_select;

static const uint32_t plain_msg_size[] = {
  PLAIN_MSG_SIZE / 16,
  PLAIN_MSG_SIZE / 4,
  PLAIN_MSG_SIZE
};

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
        if (fill_buf_with_random_number() != PSA_SUCCESS) {
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
        symmetric_key_storage_select++;
        if (symmetric_key_storage_select > KEY_STORAGE_MAX) {
          symmetric_key_storage_select = VOLATILE_PLAIN_KEY;
        }
        printf("  + Current symmetric key is a %s key.\n",
               symmetric_key_storage_string[symmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
        printf("\n  . Current symmetric key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
        printf("  + Press SPACE to select symmetric key length (%d or %d or "
               "%d), press ENTER to next option.\n", symmetric_key_size[0],
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
        printf("  + Current symmetric key length is %d-bit.\n",
               symmetric_key_size[symmetric_key_size_select]);
      }
      if (enter_press) {
        enter_press = false;
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
        if (symmetric_key_size_select == (KEY_SIZE_MAX - 1)) {
          printf("  + Series 1 devices do not support 192-bit key, press SPACE "
                 "to select other key length.\n");
          break;
        }
#endif
        printf("\n  . Current data length is %lu bytes.\n",
               plain_msg_size[plain_msg_size_select]);
        printf("  + Press SPACE to select data length (%lu or %lu or %lu), "
               "press ENTER to run.\n", plain_msg_size[0], plain_msg_size[1],
               plain_msg_size[2]);
        app_state = SELECT_DATA_SIZE;
      }
      break;

    case SELECT_DATA_SIZE:
      if (space_press) {
        space_press = false;
        plain_msg_size_select++;
        if (plain_msg_size_select > MSG_SIZE_MAX) {
          plain_msg_size_select = 0;
        }
        printf("  + Current data length is %lu bytes.\n",
               plain_msg_size[plain_msg_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        set_plain_msg_len(plain_msg_size[plain_msg_size_select]);
        // Plain message hash for decryption check
        printf("  + Generating a SHA256 hash value of %lu bytes random "
               "message... ", plain_msg_size[plain_msg_size_select]);
        app_state = PSA_CRYPTO_EXIT;
        if (compute_msg_hash() == PSA_SUCCESS) {
          app_state = AES_CCM_TEST;
        }
      }
      break;

    case AES_CCM_TEST:
      printf("\n  . AES CCM encryption -");
      encrypt_aead_single(PSA_ALG_CCM);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES CCM decryption -");
      decrypt_aead_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = AES_GCM_TEST;
      break;

    case AES_GCM_TEST:
      printf("\n  . AES GCM encryption -");
      encrypt_aead_single(PSA_ALG_GCM);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES GCM decryption -");
      decrypt_aead_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = CHACHA20_POLY1305_TEST;
      break;

    case CHACHA20_POLY1305_TEST:
      if (symmetric_key_size_select != KEY_SIZE_MAX) {
        printf("\n  . ChaCha20-Poly1305 algorithm can only use 256-bit key.\n");
      } else {
        printf("\n  . ChaCha20-Poly1305 encryption -");
        encrypt_aead_single(PSA_ALG_CHACHA20_POLY1305);
        if (app_state == PSA_CRYPTO_EXIT) {
          break;
        }
        printf("\n  . ChaCha20-Poly1305 decryption -");
        decrypt_aead_single();
        if (app_state == PSA_CRYPTO_EXIT) {
          break;
        }
      }
      print_key_storage();
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
      // Destroy a key regardless it is valid or not
      printf("  + Destroying a key... ");
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
 * Fill buffers for cipher operations.
 ******************************************************************************/
static psa_status_t fill_buf_with_random_number(void)
{
  // Fill nonce buffer with random number
  printf("\n  . Fill buffers for AEAD operations.\n");
  printf("  + Filling %d bytes nonce buffer with random number... ",
         NONCE_SIZE);
  if (generate_random_number(get_nonce_buf_ptr(),
                             NONCE_SIZE) != PSA_SUCCESS) {
    return PSA_ERROR_GENERIC_ERROR;
  }

  // Fill associated data buffer with random number
  printf("  + Filling %d bytes additional data buffer with random number... ",
         AD_SIZE);
  if (generate_random_number(get_ad_buf_ptr(), AD_SIZE) != PSA_SUCCESS) {
    return PSA_ERROR_GENERIC_ERROR;
  }

  // Fill plain message buffer with random number
  printf("  + Filling %d bytes plain message buffer with random number... ",
         PLAIN_MSG_SIZE);
  return(generate_random_number(get_plain_msg_buf_ptr(), PLAIN_MSG_SIZE));
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

  printf("\n  . Current symmetric key is a %s key.\n",
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
 * Single part AEAD encryption.
 ******************************************************************************/
static void encrypt_aead_single(psa_algorithm_t algo)
{
  app_state = PSA_CRYPTO_EXIT;
  memset(get_cipher_msg_buf_ptr(), 0, plain_msg_size[plain_msg_size_select]);
  printf(" %lu bytes random plaintext with %d bit key\n",
         plain_msg_size[plain_msg_size_select],
         symmetric_key_size[symmetric_key_size_select]);

  // Create key for AEAD algorithm
  printf("  + Creating a %d-bit %s key... ",
         symmetric_key_size[symmetric_key_size_select],
         symmetric_key_storage_string[symmetric_key_storage_select]);
  if (create_import_key(CREATE_KEY_OFFSET + symmetric_key_storage_select,
                        PSA_KEY_TYPE_NONE,
                        symmetric_key_size[symmetric_key_size_select],
                        PERSISTENT_KEY_ID,
                        DEFAULT_KEY_USAGE,
                        algo) != PSA_SUCCESS) {
    return;
  }
  // Free resources
  reset_key_attr();

  // Use default AEAD tag length
  set_nonce_len(NONCE_LEN);
  set_ad_len(AD_LEN);
  printf("  + Encrypting message... ");
  if (encrypt_aead() != PSA_SUCCESS) {
    return;
  }
  app_state = PSA_CRYPTO_INIT;
}

/***************************************************************************//**
 * Single part AEAD decryption.
 ******************************************************************************/
static void decrypt_aead_single(void)
{
  app_state = PSA_CRYPTO_EXIT;
  // Use the key in encryption for decryption
  memset(get_plain_msg_buf_ptr(), 0, plain_msg_size[plain_msg_size_select]);
  printf(" %lu bytes ciphertext with %d bit key\n",
         plain_msg_size[plain_msg_size_select],
         symmetric_key_size[symmetric_key_size_select]);

  // Setup length is not necessary if decryption after encryption
  set_nonce_len(NONCE_LEN);
  set_ad_len(AD_LEN);
  printf("  + Decrypting message... ");
  if (decrypt_aead() != PSA_SUCCESS) {
    return;
  }
  printf("  + Comparing the SHA256 hash of decrypted message and plain "
         "message... ");
  if (compare_msg_hash() != PSA_SUCCESS) {
    return;
  }
  printf("  + Destroying a %d-bit %s key... ",
         symmetric_key_size[symmetric_key_size_select],
         symmetric_key_storage_string[symmetric_key_storage_select]);
  if (destroy_key() != PSA_SUCCESS) {
    return;
  }
  app_state = PSA_CRYPTO_INIT;
}
