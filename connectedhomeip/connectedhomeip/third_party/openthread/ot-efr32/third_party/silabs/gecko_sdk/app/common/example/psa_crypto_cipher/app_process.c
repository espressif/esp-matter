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
 * Single part cipher encryption.
 *
 * @param algo The cipher algorithm.
 ******************************************************************************/
static void encrypt_cipher_single(psa_algorithm_t algo);

/***************************************************************************//**
 * Single part cipher decryption.
 ******************************************************************************/
static void decrypt_cipher_single(void);

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
static uint8_t example_string[] = "PSA Crypto Cipher Example";

/// Symmetric key storage selection
static uint8_t symmetric_key_storage_select;

static const char *symmetric_key_storage_string[] = {
  "VOLATILE PLAIN",
  "PERSISTENT PLAIN",
  "VOLATILE WRAPPED",
  "PERSISTENT WRAPPED",
  "BUILT-IN AES-128"
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

/// Test vector (AES CTR) for built-in AES-128 key
/// Plaintext
static const uint8_t aes128_plain[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// IV
static const uint8_t aes128_iv[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// Ciphertext
static const uint8_t aes128_cipher[] = {
  0x66, 0xd2, 0x0f, 0x99, 0x65, 0x3e, 0xa8, 0xd0,
  0x83, 0x05, 0xa6, 0x39, 0xd4, 0x4e, 0x98, 0xa6
};

/// Test vector for AES CFB
/// The 128-bit key
static const uint8_t cfb_key[] = {
  0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// Plaintext
static const uint8_t cfb_plain[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// IV
static const uint8_t cfb_iv[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// Ciphertext
static const uint8_t cfb_cipher[] = {
  0x97, 0x00, 0x14, 0xd6, 0x34, 0xe2, 0xb7, 0x65,
  0x07, 0x77, 0xe8, 0xe8, 0x4d, 0x03, 0xcc, 0xd8
};

/// Test vector for AES CTR
/// The 128-bit key
static const uint8_t ctr_key[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
};

/// Plaintext
static const uint8_t ctr_plain[] = {
  0xd8, 0x65, 0xc9, 0xcd, 0xea, 0x33, 0x56, 0xc5,
  0x48, 0x8e, 0x7b, 0xa1, 0x5e, 0x84, 0xf4, 0xeb,
  0xa3, 0xb8, 0x25, 0x9c, 0x05, 0x3f, 0x24, 0xce,
  0x29, 0x67, 0x22, 0x1c, 0x00, 0x38, 0x84, 0xd7,
  0x9d, 0x4c, 0xa4, 0x87, 0x7f, 0xfa, 0x4b, 0xc6,
  0x87, 0xc6, 0x67, 0xe5, 0x49, 0x5b, 0xcf, 0xec,
  0x12, 0xf4, 0x87, 0x17, 0x32, 0xaa, 0xe4, 0x5a,
  0x11, 0x06, 0x76, 0x11, 0x3d, 0xf9, 0xe7, 0xda
};

/// IV
static const uint8_t ctr_iv[] = {
  0x22, 0x22, 0x1a, 0x70, 0x22, 0x22, 0x1a, 0x70,
  0x22, 0x22, 0x1a, 0x70, 0x22, 0x22, 0x1a, 0x70
};

/// Ciphertext
static const uint8_t ctr_cipher[] = {
  0xb6, 0x72, 0xf2, 0xaf, 0x6a, 0xcc, 0x20, 0xae,
  0xee, 0x1a, 0xd8, 0x14, 0x12, 0x8c, 0x31, 0x8b,
  0x95, 0x5b, 0xbe, 0x80, 0x5b, 0x38, 0x92, 0x49,
  0x89, 0x76, 0x00, 0xf5, 0x20, 0x74, 0x54, 0x32,
  0x7d, 0x6d, 0x0f, 0xb4, 0xac, 0x0a, 0x94, 0xf3,
  0x7c, 0xa0, 0x9e, 0x45, 0x05, 0x33, 0x98, 0xfe,
  0xa8, 0x9c, 0x20, 0x0a, 0xd3, 0x58, 0x12, 0x6d,
  0x9e, 0x89, 0xa4, 0x05, 0x26, 0x5c, 0x96, 0xe7
};

/// Test vector for ChaCha20
/// The 256-bit key
static const uint8_t chacha20_key[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// Plaintext
static const uint8_t chacha20_plain[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/// Nonce
static const uint8_t chacha20_nonce[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00
};

/// Ciphertext
static const uint8_t chacha20_cipher[] = {
  0x76, 0xb8, 0xe0, 0xad, 0xa0, 0xf1, 0x3d, 0x90,
  0x40, 0x5d, 0x6a, 0xe5, 0x53, 0x86, 0xbd, 0x28,
  0xbd, 0xd2, 0x19, 0xb8, 0xa0, 0x8d, 0xed, 0x1a,
  0xa8, 0x36, 0xef, 0xcc, 0x8b, 0x77, 0x0d, 0xc7,
  0xda, 0x41, 0x59, 0x7c, 0x51, 0x57, 0x48, 0x8d,
  0x77, 0x24, 0xe0, 0x3f, 0xb8, 0xd8, 0x4a, 0x37,
  0x6a, 0x43, 0xb8, 0xf4, 0x15, 0x18, 0xa1, 0x1c,
  0xc3, 0x87, 0xb6, 0x69, 0xb2, 0xee, 0x65, 0x86
};

/// Stream message size
static uint32_t stream_msg_size;

/// Stream block counter
static uint32_t stream_block_count;

/// Stream encrypt or decrypt
static bool encrypt_decrypt;

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
               PLAIN_MSG_SIZE);
        if (generate_random_number(get_plain_msg_buf_ptr(), PLAIN_MSG_SIZE)
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
#if defined(SEMAILBOX_PRESENT)
        if (symmetric_key_storage_select == KEY_STORAGE_MAX) {
          printf("  + Current symmetric key is a %s key.\n",
                 symmetric_key_storage_string[4]);
          break;
        }
#endif
        printf("  + Current symmetric key is a %s key.\n",
               symmetric_key_storage_string[symmetric_key_storage_select]);
      }
      if (enter_press) {
        enter_press = false;
#if defined(SEMAILBOX_PRESENT)
        if (symmetric_key_storage_select == KEY_STORAGE_MAX) {
          printf("\n  . Built-in AES-128 key encryption\n");
          set_key_id(SL_SE_BUILTIN_KEY_AES128_ID);
          app_state = BUILTIN_KEY_ENCRYPTION;
          break;
        }
#endif
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
          app_state = AES_ECB_TEST;
        }
      }
      break;

    case AES_ECB_TEST:
      printf("\n  . AES ECB encryption -");
      encrypt_cipher_single(PSA_ALG_ECB_NO_PADDING);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES ECB decryption -");
      decrypt_cipher_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = AES_CBC_TEST;
      break;

    case AES_CBC_TEST:
      printf("\n  . AES CBC encryption -");
      encrypt_cipher_single(PSA_ALG_CBC_NO_PADDING);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES CBC decryption -");
      decrypt_cipher_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = AES_CFB_TEST;
      break;

    case AES_CFB_TEST:
      printf("\n  . AES CFB encryption -");
      encrypt_cipher_single(PSA_ALG_CFB);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES CFB decryption -");
      decrypt_cipher_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = AES_CTR_TEST;
      break;

    case AES_CTR_TEST:
      printf("\n  . AES CTR encryption -");
      encrypt_cipher_single(PSA_ALG_CTR);
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      printf("\n  . AES CTR decryption -");
      decrypt_cipher_single();
      if (app_state == PSA_CRYPTO_EXIT) {
        break;
      }
      app_state = CHACHA20_TEST;
      break;

    case CHACHA20_TEST:
      if (symmetric_key_size_select != KEY_SIZE_MAX) {
        printf("\n  . ChaCha20 algorithm can only use 256-bit key.\n");
      } else {
        printf("\n  . CHACHA20 encryption -");
        encrypt_cipher_single(PSA_ALG_STREAM_CIPHER);
        if (app_state == PSA_CRYPTO_EXIT) {
          break;
        }
        printf("\n  . CHACHA20 decryption -");
        decrypt_cipher_single();
        if (app_state == PSA_CRYPTO_EXIT) {
          break;
        }
      }
      app_state = START_CFB_STREAM;
      break;

    case BUILTIN_KEY_ENCRYPTION:
      app_state = PSA_CRYPTO_EXIT;
      memcpy(get_plain_msg_buf_ptr(), aes128_plain, sizeof(aes128_plain));
      memcpy(get_iv_buf_ptr(), aes128_iv, sizeof(aes128_iv));
      set_plain_msg_len(sizeof(aes128_plain));
      set_iv_len(sizeof(aes128_iv));
      printf("  + Starting an encryption... ");
      if (start_cipher_encryption() != PSA_SUCCESS) {
        break;
      }
      if (get_key_algo() != PSA_ALG_ECB_NO_PADDING) {
        printf("  + Setting an IV... ");
        if (set_cipher_iv() != PSA_SUCCESS) {
          break;
        }
      }
      printf("  + Updating an encryption... ");
      ret = update_cipher_encryption(0);
      if (ret == PSA_ERROR_INVALID_HANDLE || ret == PSA_ERROR_DOES_NOT_EXIST) {
        printf("  + The AES-128 key has not been provisioned.\n");
        set_key_id(0);
        print_key_storage();
        break;
      }
      printf("  + Finishing an encryption... ");
      if (finish_cipher_encryption() != PSA_SUCCESS) {
        break;
      }
      printf("  + Comparing the encrypted message with expected cipher "
             "message... ");
      if (memcmp(get_cipher_msg_buf_ptr(), aes128_cipher,
                 sizeof(aes128_cipher)) == 0) {
        printf("OK\n");
        app_state = BUILTIN_KEY_DECRYPTION;
      } else {
        printf("Failed\n");
        set_key_id(0);
        print_key_storage();
      }
      break;

    case BUILTIN_KEY_DECRYPTION:
      app_state = PSA_CRYPTO_EXIT;
      memset(get_plain_msg_buf_ptr(), 0xff, sizeof(aes128_plain));
      printf("\n  . Built-in AES-128 key decryption\n");
      printf("  + Starting a decryption... ");
      if (start_cipher_decryption() != PSA_SUCCESS) {
        break;
      }
      if (get_key_algo() != PSA_ALG_ECB_NO_PADDING) {
        printf("  + Setting an IV... ");
        if (set_cipher_iv() != PSA_SUCCESS) {
          break;
        }
      }
      printf("  + Updating a decryption... ");
      if (update_cipher_decryption(0) != PSA_SUCCESS) {
        break;
      }
      printf("  + Finishing a decryption... ");
      if (finish_cipher_decryption() != PSA_SUCCESS) {
        break;
      }
      printf("  + Comparing the decrypted message with expected plain "
             "message... ");
      if (memcmp(get_plain_msg_buf_ptr(), aes128_plain,
                 sizeof(aes128_plain)) == 0) {
        printf("OK\n");
      } else {
        printf("Failed\n");
      }
      set_key_id(0);
      print_key_storage();
      break;

    case START_CFB_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("\n  . AES CFB stream encryption\n");
      } else {
        printf("\n  . AES CFB stream decryption\n");
      }

      if (!encrypt_decrypt) {
        printf("  + Importing a %d-bit %s key... ", sizeof(cfb_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        memset(get_cipher_msg_buf_ptr(), 0, sizeof(cfb_cipher));
        // Setup buffer and length to import key
        memcpy(get_key_buf_ptr(), cfb_key, sizeof(cfb_key));
        set_key_len(sizeof(cfb_key));
        if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                              PSA_KEY_TYPE_NONE,
                              sizeof(cfb_key) * 8,
                              PERSISTENT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_CFB) != PSA_SUCCESS) {
          break;
        }
        // Free resources
        reset_key_attr();
        memcpy(get_plain_msg_buf_ptr(), cfb_plain, sizeof(cfb_plain));
        printf("  + Setting a CFB encrypt stream... ");
        if (start_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
      } else {
        // Use encryption key for decryption
        memset(get_plain_msg_buf_ptr(), 0, sizeof(cfb_plain));
        memcpy(get_cipher_msg_buf_ptr(), cfb_cipher, sizeof(cfb_cipher));
        printf("  + Setting a CFB decrypt stream... ");
        if (start_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
      }

      // Not necessary (IV doesn't change) if decryption after encryption
      set_iv_len(sizeof(cfb_iv));
      memcpy(get_iv_buf_ptr(), cfb_iv, sizeof(cfb_iv));
      printf("  + Setting the IV... ");
      if (set_cipher_iv() != PSA_SUCCESS) {
        break;
      }

      // Same size for encryption and decryption
      stream_msg_size = sizeof(cfb_plain);
      set_plain_msg_len(STREAM_BLOCK_SIZE);
      stream_block_count = 0;
      app_state = UPDATE_CFB_STREAM;
      break;

    case UPDATE_CFB_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      }
      stream_block_count++;
      stream_msg_size -= STREAM_BLOCK_SIZE;
      app_state = UPDATE_CFB_STREAM;
      if (stream_msg_size <= STREAM_BLOCK_SIZE) {
        app_state = UPDATE_LAST_CFB_STREAM;
        if (stream_msg_size == 0) {
          app_state = FINISH_CFB_STREAM;
        }
      }
      break;

    case UPDATE_LAST_CFB_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_plain_msg_len(stream_msg_size);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CFB_STREAM;
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CFB_STREAM;
      }
      break;

    case FINISH_CFB_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("  + Finishing a CFB encrypt stream... ");
        if (finish_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the encrypted message with cipher message... ");
        if (memcmp(cfb_cipher, get_cipher_msg_buf_ptr(), sizeof(cfb_cipher))
            != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        // Start decryption
        encrypt_decrypt = true;
        app_state = START_CFB_STREAM;
      } else {
        printf("  + Finishing a CFB decrypt stream... ");
        if (finish_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the decrypted message with plain message... ");
        if (memcmp(cfb_plain, get_plain_msg_buf_ptr(), sizeof(cfb_plain))
            != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        printf("  + Destroying a %d-bit %s key... ", sizeof(cfb_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        if (destroy_key() != PSA_SUCCESS) {
          break;
        }
        // Start encryption
        encrypt_decrypt = false;
        app_state = START_CTR_STREAM;
      }
      break;

    case START_CTR_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("\n  . AES CTR stream encryption\n");
      } else {
        printf("\n  . AES CTR stream decryption\n");
      }

      if (!encrypt_decrypt) {
        printf("  + Importing a %d-bit %s key... ", sizeof(ctr_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        memset(get_cipher_msg_buf_ptr(), 0, sizeof(ctr_cipher));
        // Setup buffer and length to import key
        memcpy(get_key_buf_ptr(), ctr_key, sizeof(ctr_key));
        set_key_len(sizeof(ctr_key));
        if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                              PSA_KEY_TYPE_NONE,
                              sizeof(ctr_key) * 8,
                              PERSISTENT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_CTR) != PSA_SUCCESS) {
          break;
        }
        // Free resources
        reset_key_attr();
        memcpy(get_plain_msg_buf_ptr(), ctr_plain, sizeof(ctr_plain));
        printf("  + Setting a CTR encrypt stream... ");
        if (start_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
      } else {
        // Use encryption key for decryption
        memset(get_plain_msg_buf_ptr(), 0, sizeof(ctr_plain));
        memcpy(get_cipher_msg_buf_ptr(), ctr_cipher, sizeof(ctr_cipher));
        printf("  + Setting a CTR decrypt stream... ");
        if (start_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
      }

      // Not necessary (IV doesn't change) if decryption after encryption
      set_iv_len(sizeof(ctr_iv));
      memcpy(get_iv_buf_ptr(), ctr_iv, sizeof(ctr_iv));
      printf("  + Setting the IV... ");
      if (set_cipher_iv() != PSA_SUCCESS) {
        break;
      }

      // Same size for encryption and decryption
      stream_msg_size = sizeof(ctr_plain);
      set_plain_msg_len(STREAM_BLOCK_SIZE);
      stream_block_count = 0;
      app_state = UPDATE_CTR_STREAM;
      break;

    case UPDATE_CTR_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      }
      stream_block_count++;
      stream_msg_size -= STREAM_BLOCK_SIZE;
      app_state = UPDATE_CTR_STREAM;
      if (stream_msg_size <= STREAM_BLOCK_SIZE) {
        app_state = UPDATE_LAST_CTR_STREAM;
        if (stream_msg_size == 0) {
          app_state = FINISH_CTR_STREAM;
        }
      }
      break;

    case UPDATE_LAST_CTR_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_plain_msg_len(stream_msg_size);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CTR_STREAM;
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CTR_STREAM;
      }
      break;

    case FINISH_CTR_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("  + Finishing a CTR encrypt stream... ");
        if (finish_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the encrypted message with cipher message... ");
        if (memcmp(ctr_cipher, get_cipher_msg_buf_ptr(), sizeof(ctr_cipher))
            != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        // Start decryption
        encrypt_decrypt = true;
        app_state = START_CTR_STREAM;
      } else {
        printf("  + Finishing a CTR decrypt stream... ");
        if (finish_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the decrypted message with plain message... ");
        if (memcmp(ctr_plain, get_plain_msg_buf_ptr(), sizeof(ctr_plain))
            != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        printf("  + Destroying a %d-bit %s key... ", sizeof(ctr_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        if (destroy_key() != PSA_SUCCESS) {
          break;
        }
        // Start encryption
        encrypt_decrypt = false;
        app_state = START_CHACHA20_STREAM;
      }
      break;

    case START_CHACHA20_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("\n  . CHACHA20 stream encryption\n");
      } else {
        printf("\n  . CHACHA20 stream decryption\n");
      }

      if (!encrypt_decrypt) {
        printf("  + Importing a %d-bit %s key... ", sizeof(chacha20_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        memset(get_cipher_msg_buf_ptr(), 0, sizeof(chacha20_cipher));
        // Setup buffer and length to import key
        memcpy(get_key_buf_ptr(), chacha20_key, sizeof(chacha20_key));
        set_key_len(sizeof(chacha20_key));
        if (create_import_key(IMPORT_KEY_OFFSET + symmetric_key_storage_select,
                              PSA_KEY_TYPE_NONE,
                              sizeof(chacha20_key) * 8,
                              PERSISTENT_KEY_ID,
                              DEFAULT_KEY_USAGE,
                              PSA_ALG_STREAM_CIPHER) != PSA_SUCCESS) {
          break;
        }
        // Free resources
        reset_key_attr();
        memcpy(get_plain_msg_buf_ptr(), chacha20_plain, sizeof(chacha20_plain));
        printf("  + Setting a CHACHA20 encrypt stream... ");
        if (start_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
      } else {
        // Use encryption key for decryption
        memset(get_plain_msg_buf_ptr(), 0, sizeof(chacha20_plain));
        memcpy(get_cipher_msg_buf_ptr(), chacha20_cipher,
               sizeof(chacha20_cipher));
        printf("  + Setting a CHACHA20 decrypt stream... ");
        if (start_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
      }

      // Not necessary (IV doesn't change) if decryption after encryption
      set_iv_len(sizeof(chacha20_nonce));
      memcpy(get_iv_buf_ptr(), chacha20_nonce, sizeof(chacha20_nonce));
      printf("  + Setting the nonce... ");
      if (set_cipher_iv() != PSA_SUCCESS) {
        break;
      }

      // Same size for encryption and decryption
      stream_msg_size = sizeof(chacha20_plain);
      set_plain_msg_len(STREAM_BLOCK_SIZE);
      stream_block_count = 0;
      app_state = UPDATE_CHACHA20_STREAM;
      break;

    case UPDATE_CHACHA20_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
      }
      stream_block_count++;
      stream_msg_size -= STREAM_BLOCK_SIZE;
      app_state = UPDATE_CHACHA20_STREAM;
      if (stream_msg_size <= STREAM_BLOCK_SIZE) {
        app_state = UPDATE_LAST_CHACHA20_STREAM;
        if (stream_msg_size == 0) {
          app_state = FINISH_CHACHA20_STREAM;
        }
      }
      break;

    case UPDATE_LAST_CHACHA20_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_plain_msg_len(stream_msg_size);
      if (!encrypt_decrypt) {
        if (update_cipher_encryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CHACHA20_STREAM;
      } else {
        if (update_cipher_decryption(STREAM_BLOCK_SIZE * stream_block_count)
            != PSA_SUCCESS) {
          break;
        }
        app_state = FINISH_CHACHA20_STREAM;
      }
      break;

    case FINISH_CHACHA20_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      if (!encrypt_decrypt) {
        printf("  + Finishing a CHACHA20 encrypt stream... ");
        if (finish_cipher_encryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the encrypted message with cipher message... ");
        if (memcmp(chacha20_cipher, get_cipher_msg_buf_ptr(),
                   sizeof(chacha20_cipher)) != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        // Start decryption
        encrypt_decrypt = true;
        app_state = START_CHACHA20_STREAM;
      } else {
        printf("  + Finishing a CHACHA20 decrypt stream... ");
        if (finish_cipher_decryption() != PSA_SUCCESS) {
          break;
        }
        printf("  + Comparing the decrypted message with plain message... ");
        if (memcmp(chacha20_plain, get_plain_msg_buf_ptr(),
                   sizeof(chacha20_plain)) != 0) {
          printf("Failed\n");
          break;
        } else {
          printf("OK\n");
        }
        printf("  + Destroying a %d-bit %s key... ", sizeof(chacha20_key) * 8,
               symmetric_key_storage_string[symmetric_key_storage_select]);
        if (destroy_key() != PSA_SUCCESS) {
          break;
        }
        encrypt_decrypt = false;
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

  if (symmetric_key_storage_select == KEY_STORAGE_MAX) {
#if defined(SEMAILBOX_PRESENT)
    printf("\n  + Current symmetric key is a %s key.\n",
           symmetric_key_storage_string[4]);
#else
    printf("\n  . Current symmetric key is a %s key.\n",
           symmetric_key_storage_string[symmetric_key_storage_select]);
#endif
  } else {
    printf("\n  . Current symmetric key is a %s key.\n",
           symmetric_key_storage_string[symmetric_key_storage_select]);
  }

#if defined(SEMAILBOX_PRESENT) && (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("  + Press SPACE to select a %s or %s or %s or %s or %s key, press "
         "ENTER to next option or run if %s key is selected.\n",
         symmetric_key_storage_string[0],
         symmetric_key_storage_string[1],
         symmetric_key_storage_string[2],
         symmetric_key_storage_string[3],
         symmetric_key_storage_string[4],
         symmetric_key_storage_string[4]);
#else
#if defined(SEMAILBOX_PRESENT)
  printf("  + Press SPACE to select a %s or %s or %s key, press ENTER to next "
         "option or run if %s key is selected.\n",
         symmetric_key_storage_string[0],
         symmetric_key_storage_string[1],
         symmetric_key_storage_string[4],
         symmetric_key_storage_string[4]);
#else
  printf("  + Press SPACE to select a %s or %s key, press ENTER to next "
         "option.\n", symmetric_key_storage_string[0],
         symmetric_key_storage_string[1]);
#endif
#endif
  app_state = SELECT_KEY_STORAGE;
}

/***************************************************************************//**
 * Single part cipher encryption.
 ******************************************************************************/
static void encrypt_cipher_single(psa_algorithm_t algo)
{
  app_state = PSA_CRYPTO_EXIT;
  memset(get_cipher_msg_buf_ptr(), 0,
         PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES) + plain_msg_size[plain_msg_size_select]);
  printf(" %lu bytes random plaintext with %d bit key\n",
         plain_msg_size[plain_msg_size_select],
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

  printf("  + Encrypting message... ");
  if (encrypt_cipher() != PSA_SUCCESS) {
    return;
  }
  app_state = PSA_CRYPTO_INIT;
}

/***************************************************************************//**
 * Single part cipher decryption.
 ******************************************************************************/
static void decrypt_cipher_single(void)
{
  app_state = PSA_CRYPTO_EXIT;
  memset(get_plain_msg_buf_ptr(), 0, plain_msg_size[plain_msg_size_select]);
  printf(" %lu bytes ciphertext with %d bit key\n",
         plain_msg_size[plain_msg_size_select],
         symmetric_key_size[symmetric_key_size_select]);

  // Use the key in encryption for decryption
  printf("  + Decrypting message... ");
  if (decrypt_cipher() != PSA_SUCCESS) {
    return;
  }
  printf("  + Comparing the hash of decrypted message and plain message... ");
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
