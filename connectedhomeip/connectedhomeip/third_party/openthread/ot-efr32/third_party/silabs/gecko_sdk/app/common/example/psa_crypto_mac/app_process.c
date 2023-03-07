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

/***************************************************************************//**
 * Calculate the MAC of a message.
 *
 * @param algo The MAC algorithm.
 ******************************************************************************/
static void compute_mac_single(psa_algorithm_t algo);

/***************************************************************************//**
 * Calculate the MAC of a message and compare it with an expected value.
 ******************************************************************************/
static void verify_mac_single(void);

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
static uint8_t example_string[] = "PSA Crypto MAC Example";

/// Symmetric key type
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

/// Message size selection
static uint8_t msg_size_select;

static const uint32_t msg_size[] = {
  MSG_SIZE / 16,
  MSG_SIZE / 4,
  MSG_SIZE
};

/// Hash algorithm selection for HMAC
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

/// Test vector for SHA256 HMAC
/// The 128-bit key
static const uint8_t hmac_key[] = {
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
};

/// Message to calculate HMAC MAC
static const uint8_t hmac_msg[] = {
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
  0x38, 0x39, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66
};

/// HMAC MAC
static const uint8_t hmac_mac[] = {
  0xfb, 0x5b, 0x26, 0x22, 0x9c, 0x20, 0xb7, 0xed,
  0x86, 0x67, 0x06, 0xa2, 0xfb, 0xfa, 0xe6, 0x7e,
  0x3f, 0x40, 0x4b, 0xb6, 0xab, 0xe7, 0x7f, 0xf4,
  0x50, 0x63, 0xa4, 0x59, 0xa4, 0x29, 0x24, 0xa4
};

/// Test vector for CMAC
/// The 256-bit key
static const uint8_t cmac_key[] = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

/// Message to calculate CMAC MAC
static const uint8_t cmac_msg[] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
};

/// CMAC MAC
static const uint8_t cmac_mac[] = {
  0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
  0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6
};

/// Message buffer
static uint8_t msg_buf[MSG_SIZE];

/// Stream message size
static uint32_t stream_msg_size;

/// Stream block counter
static uint32_t stream_block_count;

/// Stream sign or verify
static bool sign_verify;

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
        printf("  + Filling %d bytes message buffer with random number... ",
               MSG_SIZE);
        if (generate_random_number(msg_buf, MSG_SIZE)
            != PSA_SUCCESS) {
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
        printf("\n  . Current data length is %lu bytes.\n",
               msg_size[msg_size_select]);
        printf("  + Press SPACE to select data length (%lu or %lu or %lu), "
               "press ENTER to next option.\n", msg_size[0], msg_size[1],
               msg_size[2]);
        app_state = SELECT_DATA_SIZE;
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
        set_msg_len(msg_size[msg_size_select]);
        printf("\n  . Current Hash algorithm for HMAC is %s.\n",
               hash_algo_string[hash_algo_select]);
        printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or %s "
               "or %s) for HMAC, press ENTER to run.\n", hash_algo_string[0],
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
        printf("  + Current Hash algorithm for HMAC is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        app_state = COMPUTE_HMAC;
      }
      break;

    case COMPUTE_HMAC:
      printf("\n  . Compute %s HMAC -", hash_algo_string[hash_algo_select]);
      set_msg_buf_ptr(msg_buf);
      compute_mac_single(PSA_ALG_HMAC(hash_algo[hash_algo_select]));
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("  + The %s HMAC MAC (%d bytes): ",
             hash_algo_string[hash_algo_select], get_mac_len());
      print_buf(get_mac_buf_ptr(), get_mac_len());

      printf("\n  . Verify %s HMAC -", hash_algo_string[hash_algo_select]);
      verify_mac_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      sign_verify = false;
      app_state = START_HMAC_STREAM;
      // HMAC streaming with wrapped key is not supported yet
      if (symmetric_key_storage_select > PERSISTENT_PLAIN_KEY) {
        app_state = COMPUTE_CMAC;
      }
      break;

    case START_HMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!sign_verify) {
        printf("\n  . SHA256 HMAC stream sign\n");
      } else {
        printf("\n  . SHA256 HMAC stream verify\n");
      }

      if (!sign_verify) {
        printf("  + Importing a %d-bit %s key... ", sizeof(hmac_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        // Setup buffer and length to import key
        memcpy(get_key_buf_ptr(), hmac_key, sizeof(hmac_key));
        set_key_len(sizeof(hmac_key));
        if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                              PSA_KEY_TYPE_NONE,
                              sizeof(hmac_key) * 8,
                              PERSISTENT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_HMAC(PSA_ALG_SHA_256))
            != PSA_SUCCESS) {
          break;
        }
        // Free resources
        reset_key_attr();
        printf("  + Setting a SHA256 HMAC sign stream... ");
        if (start_mac_sign_stream() != PSA_SUCCESS) {
          break;
        }
      } else {
        printf("  + Setting a SHA256 HMAC verify stream... ");
        if (start_mac_verify_stream() != PSA_SUCCESS) {
          break;
        }
      }
      stream_block_count = 0;
      stream_msg_size = sizeof(hmac_msg);
      set_msg_buf_ptr((uint8_t *)hmac_msg);
      set_msg_len(STREAM_BLOCK_SIZE);
      app_state = UPDATE_HMAC_STREAM;
      break;

    case UPDATE_HMAC_STREAM:
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (update_mac_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        stream_block_count++;
        stream_msg_size -= STREAM_BLOCK_SIZE;
        if (stream_msg_size <= STREAM_BLOCK_SIZE) {
          app_state = UPDATE_LAST_HMAC_STREAM;
        }
      } else {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case UPDATE_LAST_HMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_msg_len(stream_msg_size);
      if (update_mac_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        app_state = VERIFY_HMAC_STREAM;
        if (!sign_verify) {
          app_state = FINISH_HMAC_STREAM;
        }
      }
      break;

    case FINISH_HMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Finishing a SHA256 HMAC stream... ");
      if (finish_mac_sign_stream() != PSA_SUCCESS) {
        break;
      }
      printf("  + The SHA256 HMAC MAC (%d bytes): ", get_mac_len());
      print_buf(get_mac_buf_ptr(), get_mac_len());
      sign_verify = true;
      app_state = START_HMAC_STREAM;
      break;

    case VERIFY_HMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Verifying a SHA256 HMAC stream... ");
      // Setup buffer and length to verify
      memcpy(get_mac_buf_ptr(), hmac_mac, sizeof(hmac_mac));
      set_mac_len(sizeof(hmac_mac));
      if (finish_mac_verify_stream() == PSA_SUCCESS) {
        printf("  + Destroying a %d-bit %s key... ", sizeof(hmac_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        if (destroy_key() != PSA_SUCCESS) {
          break;
        }
        app_state = COMPUTE_CMAC;
      }
      break;

    case COMPUTE_CMAC:
#if defined(CRYPTO_COUNT) && (CRYPTO_COUNT > 0)
      if (symmetric_key_size_select == (KEY_SIZE_MAX - 1)) {
        printf("\n  . Series 1 devices do not support 192-bit key on CMAC\n");
        sign_verify = false;
        app_state = START_CMAC_STREAM;
        break;
      }
#endif
      printf("\n  . Compute CMAC -");
      set_msg_buf_ptr(msg_buf);
      compute_mac_single(PSA_ALG_CMAC);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("  + The CMAC MAC (%d bytes): ", get_mac_len());
      print_buf(get_mac_buf_ptr(), get_mac_len());

      printf("\n  . Verify CMAC -");
      verify_mac_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      sign_verify = false;
      app_state = START_CMAC_STREAM;
      break;

    case START_CMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!sign_verify) {
        printf("\n  . CMAC stream sign\n");
      } else {
        printf("\n  . CMAC stream verify\n");
      }

      if (!sign_verify) {
        printf("  + Importing a %d-bit %s key... ", sizeof(cmac_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        // Setup buffer and length to import key
        memcpy(get_key_buf_ptr(), cmac_key, sizeof(cmac_key));
        set_key_len(sizeof(cmac_key));
        if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                              PSA_KEY_TYPE_NONE,
                              sizeof(cmac_key) * 8,
                              PERSISTENT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_CMAC) != PSA_SUCCESS) {
          break;
        }
        // Free resources
        reset_key_attr();
        printf("  + Setting a CMAC sign stream... ");
        if (start_mac_sign_stream() != PSA_SUCCESS) {
          break;
        }
      } else {
        printf("  + Setting a CMAC verify stream... ");
        if (start_mac_verify_stream() != PSA_SUCCESS) {
          break;
        }
      }
      stream_block_count = 0;
      stream_msg_size = sizeof(cmac_msg);
      set_msg_buf_ptr((uint8_t *)cmac_msg);
      set_msg_len(STREAM_BLOCK_SIZE);
      app_state = UPDATE_CMAC_STREAM;
      break;

    case UPDATE_CMAC_STREAM:
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (update_mac_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        stream_block_count++;
        stream_msg_size -= STREAM_BLOCK_SIZE;
        if (stream_msg_size <= STREAM_BLOCK_SIZE) {
          app_state = UPDATE_LAST_CMAC_STREAM;
        }
      } else {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case UPDATE_LAST_CMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_msg_len(stream_msg_size);
      if (update_mac_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        app_state = VERIFY_CMAC_STREAM;
        if (!sign_verify) {
          app_state = FINISH_CMAC_STREAM;
        }
      }
      break;

    case FINISH_CMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Finishing a CMAC stream... ");
      if (finish_mac_sign_stream() != PSA_SUCCESS) {
        break;
      }
      printf("  + The CMAC MAC (%d bytes): ", get_mac_len());
      print_buf(get_mac_buf_ptr(), get_mac_len());
      sign_verify = true;
      app_state = START_CMAC_STREAM;
      break;

    case VERIFY_CMAC_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Verifying a CMAC stream... ");
      // Setup buffer and length to verify
      memcpy(get_mac_buf_ptr(), cmac_mac, sizeof(cmac_mac));
      set_mac_len(sizeof(cmac_mac));
      if (finish_mac_verify_stream() == PSA_SUCCESS) {
        printf("  + Destroying a %d-bit %s key... ", sizeof(cmac_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        if (destroy_key() != PSA_SUCCESS) {
          break;
        }
        print_key_storage();
      }
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
 * Calculate the MAC of a message.
 ******************************************************************************/
static void compute_mac_single(psa_algorithm_t algo)
{
  app_state = PSA_CRYPTO_EXIT;
  memset(get_mac_buf_ptr(), 0, MAC_BUF_SIZE);
  printf(" %lu bytes random message with %d bit key\n",
         msg_size[msg_size_select],
         symmetric_key_size[symmetric_key_size_select]);

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

  printf("  + Computing a MAC... ");
  if (compute_mac() != PSA_SUCCESS) {
    return;
  }
  app_state = PSA_CRYPTO_INIT;
}

/***************************************************************************//**
 * Calculate the MAC of a message and compare it with an expected value.
 ******************************************************************************/
static void verify_mac_single(void)
{
  app_state = PSA_CRYPTO_EXIT;
  printf(" %lu bytes random message with %d bit key\n",
         msg_size[msg_size_select],
         symmetric_key_size[symmetric_key_size_select]);

  // Use the key to calculate the MAC for verification
  printf("  + Computing a MAC and verify... ");
  if (verify_mac() != PSA_SUCCESS) {
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
