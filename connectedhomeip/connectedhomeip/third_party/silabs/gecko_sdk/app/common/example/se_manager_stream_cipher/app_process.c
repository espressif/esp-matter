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

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state variable
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Stream Cipher Example";

/// Stream message size
static uint32_t stream_msg_size;

/// Stream block counter
static uint32_t stream_block_count;

/// Stream output index
static uint32_t stream_output_index;

/// Test vector for CMAC
/// CMAC key
static const uint8_t cmac_key[]  = {
  0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe,
  0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
  0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7,
  0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4
};

/// CMAC plain text
static const uint8_t cmac_plain_text[] = {
  0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
  0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
  0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c,
  0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
  0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11
};

/// CMAC tag
static const uint8_t cmac_tag[] = {
  0xaa, 0xf3, 0xd8, 0xf1, 0xde, 0x56, 0x40, 0xc2,
  0x32, 0xf5, 0xb1, 0x69, 0xb9, 0xc9, 0x11, 0xe6
};

/// Test vector for GCM
/// GCM key
static const uint8_t gcm_key[]  = {
  0x5f, 0xe0, 0x1c, 0x4b, 0xaf, 0x01, 0xcb, 0xe0,
  0x77, 0x96, 0xd5, 0xaa, 0xef, 0x6e, 0xc1, 0xf4,
  0x51, 0x93, 0xa9, 0x8a, 0x22, 0x35, 0x94, 0xae,
  0x4f, 0x0e, 0xf4, 0x95, 0x2e, 0x82, 0xe3, 0x30
};

/// GCM IV
static const uint8_t gcm_iv[] = {
  0xbd, 0x58, 0x73, 0x21, 0x56, 0x6c, 0x7f, 0x1a,
  0x5d, 0xd8, 0x65, 0x2d
};

/// GCM plain text
static const uint8_t gcm_plain_text[] = {
  0x88, 0x1d, 0xc6, 0xc7, 0xa5, 0xd4, 0x50, 0x9f,
  0x3c, 0x4b, 0xd2, 0xda, 0xab, 0x08, 0xf1, 0x65,
  0xdd, 0xc2, 0x04, 0x48, 0x9a, 0xa8, 0x13, 0x45,
  0x62, 0xa4, 0xea, 0xc3, 0xd0, 0xbc, 0xad, 0x79,
  0x65, 0x84, 0x7b, 0x10, 0x27, 0x33, 0xbb, 0x63,
  0xd1, 0xe5, 0xc5, 0x98, 0xec, 0xe0, 0xc3, 0xe5,
  0xda, 0xdd, 0xdd
};

/// GCM associated data
static const uint8_t gcm_ad[] = {
  0x90, 0x13, 0x61, 0x78, 0x17, 0xdd, 0xa9, 0x47,
  0xe1, 0x35, 0xee, 0x6d, 0xd3, 0x65, 0x33, 0x82
};

/// GCM cipher text
static const uint8_t gcm_cipher_text[] = {
  0x16, 0xe3, 0x75, 0xb4, 0x97, 0x3b, 0x33, 0x9d,
  0x3f, 0x74, 0x6c, 0x1c, 0x5a, 0x56, 0x8b, 0xc7,
  0x52, 0x6e, 0x90, 0x9d, 0xdf, 0xf1, 0xe1, 0x9c,
  0x95, 0xc9, 0x4a, 0x6c, 0xcf, 0xf2, 0x10, 0xc9,
  0xa4, 0xa4, 0x06, 0x79, 0xde, 0x57, 0x60, 0xc3,
  0x96, 0xac, 0x0e, 0x2c, 0xeb, 0x12, 0x34, 0xf9,
  0xf5, 0xfe, 0x26
};

/// GCM tag
static const uint8_t gcm_tag[] = {
  0xab, 0xd3, 0xd2, 0x6d, 0x65, 0xa6, 0x27, 0x5f,
  0x7a, 0x4f, 0x56, 0xb4, 0x22, 0xac, 0xab, 0x49
};

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Initial counter for ChaCha20
static uint32_t initial_counter;

/// Test vector for ChaCha20
/// ChaCha20 key
static const uint8_t chacha20_key[] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
  0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
};

/// ChaCha20 nonce
static const uint8_t chacha20_nonce[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4a,
  0x00, 0x00, 0x00, 0x00
};

/// ChaCha20 plain text
static const uint8_t chacha20_plain_text[] = {
  0x4c, 0x61, 0x64, 0x69, 0x65, 0x73, 0x20, 0x61,
  0x6e, 0x64, 0x20, 0x47, 0x65, 0x6e, 0x74, 0x6c,
  0x65, 0x6d, 0x65, 0x6e, 0x20, 0x6f, 0x66, 0x20,
  0x74, 0x68, 0x65, 0x20, 0x63, 0x6c, 0x61, 0x73,
  0x73, 0x20, 0x6f, 0x66, 0x20, 0x27, 0x39, 0x39,
  0x3a, 0x20, 0x49, 0x66, 0x20, 0x49, 0x20, 0x63,
  0x6f, 0x75, 0x6c, 0x64, 0x20, 0x6f, 0x66, 0x66,
  0x65, 0x72, 0x20, 0x79, 0x6f, 0x75, 0x20, 0x6f,
  0x6e, 0x6c, 0x79, 0x20, 0x6f, 0x6e, 0x65, 0x20,
  0x74, 0x69, 0x70, 0x20, 0x66, 0x6f, 0x72, 0x20,
  0x74, 0x68, 0x65, 0x20, 0x66, 0x75, 0x74, 0x75,
  0x72, 0x65, 0x2c, 0x20, 0x73, 0x75, 0x6e, 0x73,
  0x63, 0x72, 0x65, 0x65, 0x6e, 0x20, 0x77, 0x6f,
  0x75, 0x6c, 0x64, 0x20, 0x62, 0x65, 0x20, 0x69,
  0x74, 0x2e
};

/// ChaCha20 cipher text
static const uint8_t chacha20_cipher_text[] = {
  0x6e, 0x2e, 0x35, 0x9a, 0x25, 0x68, 0xf9, 0x80,
  0x41, 0xba, 0x07, 0x28, 0xdd, 0x0d, 0x69, 0x81,
  0xe9, 0x7e, 0x7a, 0xec, 0x1d, 0x43, 0x60, 0xc2,
  0x0a, 0x27, 0xaf, 0xcc, 0xfd, 0x9f, 0xae, 0x0b,
  0xf9, 0x1b, 0x65, 0xc5, 0x52, 0x47, 0x33, 0xab,
  0x8f, 0x59, 0x3d, 0xab, 0xcd, 0x62, 0xb3, 0x57,
  0x16, 0x39, 0xd6, 0x24, 0xe6, 0x51, 0x52, 0xab,
  0x8f, 0x53, 0x0c, 0x35, 0x9f, 0x08, 0x61, 0xd8,
  0x07, 0xca, 0x0d, 0xbf, 0x50, 0x0d, 0x6a, 0x61,
  0x56, 0xa3, 0x8e, 0x08, 0x8a, 0x22, 0xb6, 0x5e,
  0x52, 0xbc, 0x51, 0x4d, 0x16, 0xcc, 0xf8, 0x06,
  0x81, 0x8c, 0xe9, 0x1a, 0xb7, 0x79, 0x37, 0x36,
  0x5a, 0xf9, 0x0b, 0xbf, 0x74, 0xa3, 0x5b, 0xe6,
  0xb4, 0x0b, 0x8e, 0xed, 0xf2, 0x78, 0x5e, 0x42,
  0x87, 0x4d
};
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------

/***************************************************************************//**
 * Application state machine, called infinitely.
 ******************************************************************************/
void app_process_action(void)
{
  switch (app_state) {
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        printf("  + Setting a 256-bit symmetric plaintext key... ");
        if (create_plain_symmetric_key(SL_SE_KEY_TYPE_AES_256) == SL_STATUS_OK) {
          app_state = START_CMAC_STREAM;
        } else {
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case START_CMAC_STREAM:
      printf("\n  . AES CMAC streaming test\n");
      printf("  + Preparing an AES CMAC streaming command context... ");
      memcpy(get_symmetric_key_buf_ptr(), cmac_key, sizeof(cmac_key));

      if (start_cmac_stream() == SL_STATUS_OK) {
        stream_block_count = 0;
        stream_msg_size = sizeof(cmac_plain_text);
        set_plain_msg_len(CMAC_BLCOK_SIZE);
        memcpy(get_plain_msg_buf_ptr(), cmac_plain_text,
               sizeof(cmac_plain_text));
        app_state = UPDATE_CMAC_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_CMAC_STREAM:
      printf("  + Ongoing %d bytes CMAC computation with %d-bit key... ",
             CMAC_BLCOK_SIZE, get_symmetric_key_len());
      if (update_cmac_stream(CMAC_BLCOK_SIZE * stream_block_count)
          == SL_STATUS_OK) {
        stream_block_count++;
        stream_msg_size -= CMAC_BLCOK_SIZE;
        if (stream_msg_size <= CMAC_BLCOK_SIZE) {
          app_state = UPDATE_LAST_CMAC_STREAM;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_LAST_CMAC_STREAM:
      printf("  + Last %lu byte(s) CMAC computation with %d-bit key... ",
             stream_msg_size, get_symmetric_key_len());
      set_plain_msg_len(stream_msg_size);
      if (update_cmac_stream(CMAC_BLCOK_SIZE * stream_block_count)
          == SL_STATUS_OK) {
        app_state = FINISH_CMAC_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case FINISH_CMAC_STREAM:
      printf("  + Generating 16 bytes CMAC tag... ");
      if (finish_cmac_stream() == SL_STATUS_OK) {
        printf("  + Comparing CMAC tag with expected data... ");
        if (memcmp(cmac_tag, get_cipher_msg_buf_ptr(), sizeof(cmac_tag))
            == 0) {
          printf("OK\n");
          app_state = START_GCM_ENCRYPT_STREAM;
        } else {
          printf("Failed\n");
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case START_GCM_ENCRYPT_STREAM:
      printf("\n  . AES GCM encryption streaming test\n");
      printf("  + Preparing an AES GCM encryption streaming command "
             "context... ");
      memcpy(get_symmetric_key_buf_ptr(), gcm_key, sizeof(gcm_key));
      memcpy(get_iv_nonce_buf_ptr(), gcm_iv, sizeof(gcm_iv));
      memcpy(get_ad_buf_ptr(), gcm_ad, sizeof(gcm_ad));
      set_iv_len(sizeof(gcm_iv));
      set_ad_len(sizeof(gcm_ad));

      if (start_gcm_encrypt_stream() == SL_STATUS_OK) {
        stream_block_count = 0;
        stream_output_index = 0;
        stream_msg_size = sizeof(gcm_plain_text);
        set_plain_msg_len(AES_BLOCK_SIZE);
        memcpy(get_plain_msg_buf_ptr(), gcm_plain_text, sizeof(gcm_plain_text));
        app_state = UPDATE_GCM_ENCRYPT_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_GCM_ENCRYPT_STREAM:
      printf("  + Encrypting %d bytes plaintext with %d-bit key... ",
             AES_BLOCK_SIZE, get_symmetric_key_len());
      if (update_gcm_encrypt_stream(AES_BLOCK_SIZE * stream_block_count, stream_output_index)
          == SL_STATUS_OK) {
        stream_block_count++;
        stream_output_index += get_output_len();
        stream_msg_size -= AES_BLOCK_SIZE;
        if (stream_msg_size <= AES_BLOCK_SIZE) {
          app_state = UPDATE_LAST_GCM_ENCRYPT_STREAM;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_LAST_GCM_ENCRYPT_STREAM:
      printf("  + Encrypting last %lu byte(s) plaintext with %d-bit key... ",
             stream_msg_size, get_symmetric_key_len());
      set_plain_msg_len(stream_msg_size);
      if (update_gcm_encrypt_stream(AES_BLOCK_SIZE * stream_block_count, stream_output_index)
          == SL_STATUS_OK) {
        app_state = FINISH_GCM_ENCRYPT_STREAM;
        stream_output_index += get_output_len();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case FINISH_GCM_ENCRYPT_STREAM:
      printf("  + Generating %d bytes AES GCM tag... ", sizeof(gcm_tag));
      set_tag_len(sizeof(gcm_tag));
      set_plain_msg_len(sizeof(gcm_plain_text));
      if (finish_gcm_encrypt_stream(stream_output_index) == SL_STATUS_OK) {
        printf("  + Comparing cipher message and tag with expected data... ");
        if (memcmp(gcm_cipher_text, get_cipher_msg_buf_ptr(),
                   sizeof(gcm_cipher_text)) == 0
            && memcmp(gcm_tag, get_tag_buf_ptr(),
                      sizeof(gcm_tag)) == 0) {
          printf("OK\n");
          app_state = START_GCM_DECRYPT_STREAM;
        } else {
          printf("Failed\n");
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case START_GCM_DECRYPT_STREAM:
      printf("\n  . AES GCM decryption streaming test\n");
      printf("  + Preparing an AES GCM decryption streaming command "
             "context... ");
      if (start_gcm_decrypt_stream() == SL_STATUS_OK) {
        stream_block_count = 0;
        stream_output_index = 0;
        stream_msg_size = sizeof(gcm_cipher_text);
        set_plain_msg_len(AES_BLOCK_SIZE);
        // Clear plain message buffer and using buffers from encryption
        memset(get_plain_msg_buf_ptr(), 0, PLAIN_MSG_SIZE);
        app_state = UPDATE_GCM_DECRYPT_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_GCM_DECRYPT_STREAM:
      printf("  + Decrypting %d bytes ciphertext with %d-bit key... ",
             AES_BLOCK_SIZE, get_symmetric_key_len());
      if (update_gcm_decrypt_stream(AES_BLOCK_SIZE * stream_block_count, stream_output_index)
          == SL_STATUS_OK) {
        stream_block_count++;
        stream_output_index += get_output_len();
        stream_msg_size -= AES_BLOCK_SIZE;
        if (stream_msg_size <= AES_BLOCK_SIZE) {
          app_state = UPDATE_LAST_GCM_DECRYPT_STREAM;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_LAST_GCM_DECRYPT_STREAM:
      printf("  + Decrypting last %lu byte(s) ciphertext with %d-bit key... ",
             stream_msg_size, get_symmetric_key_len());
      set_plain_msg_len(stream_msg_size);
      if (update_gcm_decrypt_stream(AES_BLOCK_SIZE * stream_block_count, stream_output_index)
          == SL_STATUS_OK) {
        app_state = FINISH_GCM_DECRYPT_STREAM;
        stream_output_index += get_output_len();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case FINISH_GCM_DECRYPT_STREAM:
      printf("  + Generating %d bytes AES GCM tag... ", sizeof(gcm_tag));
      set_tag_len(sizeof(gcm_tag));
      set_plain_msg_len(sizeof(gcm_plain_text));
      memcpy(get_tag_buf_ptr(), gcm_tag, sizeof(gcm_tag));
      if (finish_gcm_decrypt_stream(stream_output_index) == SL_STATUS_OK) {
        printf("  + Comparing plain message and tag with expected data... ");
        if (memcmp(gcm_plain_text, get_plain_msg_buf_ptr(),
                   sizeof(gcm_plain_text)) == 0) {
          printf("OK\n");
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
          app_state = START_CHACHA20_ENCRYPT_STREAM;
#else
          app_state = SE_MANAGER_EXIT;
#endif
        } else {
          printf("Failed\n");
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case START_CHACHA20_ENCRYPT_STREAM:
      printf("\n  . ChaCha20 encryption streaming test\n");
      printf("  + Encrypting %d bytes plaintext with %d-bit key... ",
             CHACHA20_BLOCK_SIZE, get_symmetric_key_len());
      memcpy(get_plain_msg_buf_ptr(), chacha20_plain_text,
             sizeof(chacha20_plain_text));
      memcpy(get_symmetric_key_buf_ptr(), chacha20_key, sizeof(chacha20_key));
      memcpy(get_iv_nonce_buf_ptr(), chacha20_nonce, sizeof(chacha20_nonce));
      set_plain_msg_len(CHACHA20_BLOCK_SIZE);
      initial_counter = 0x01;
      stream_block_count = 0;
      stream_msg_size = sizeof(chacha20_plain_text);

      if (update_chacha20_encrypt_stream(initial_counter,
                                         stream_block_count * CHACHA20_BLOCK_SIZE)
          == SL_STATUS_OK) {
        app_state = UPDATE_CHACHA20_ENCRYPT_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_CHACHA20_ENCRYPT_STREAM:
      stream_block_count++;
      stream_msg_size -= CHACHA20_BLOCK_SIZE;
      initial_counter++;
      if (stream_msg_size <= CHACHA20_BLOCK_SIZE) {
        printf("  + Encrypting last %lu byte(s) plaintext with %d-bit key... ",
               stream_msg_size, get_symmetric_key_len());
        set_plain_msg_len(stream_msg_size);
        if (update_chacha20_encrypt_stream(initial_counter,
                                           stream_block_count * CHACHA20_BLOCK_SIZE)
            == SL_STATUS_OK) {
          app_state = FINISH_CHACHA20_ENCRYPT_STREAM;
        } else {
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        printf("  + Encrypting %d bytes plaintext with %d-bit key... ",
               CHACHA20_BLOCK_SIZE, get_symmetric_key_len());
        if (update_chacha20_encrypt_stream(initial_counter,
                                           stream_block_count * CHACHA20_BLOCK_SIZE)
            != SL_STATUS_OK) {
          app_state = SE_MANAGER_EXIT;
        }
      }
      break;

    case FINISH_CHACHA20_ENCRYPT_STREAM:
      printf("  + Comparing cipher message with expected data... ");
      if (memcmp(chacha20_cipher_text, get_cipher_msg_buf_ptr(),
                 sizeof(chacha20_cipher_text)) == 0) {
        printf("OK\n");
        app_state = START_CHACHA20_DECRYPT_STREAM;
      } else {
        printf("Failed\n");
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case START_CHACHA20_DECRYPT_STREAM:
      printf("\n  . ChaCha20 decryption streaming test\n");
      printf("  + Decrypting %d bytes ciphertext with %d-bit key... ",
             CHACHA20_BLOCK_SIZE, get_symmetric_key_len());
      initial_counter = 0x01;
      stream_block_count = 0;
      stream_msg_size = sizeof(chacha20_cipher_text);
      set_plain_msg_len(CHACHA20_BLOCK_SIZE);
      // Clear plain message buffer and using buffers from encryption
      memset(get_plain_msg_buf_ptr(), 0, PLAIN_MSG_SIZE);

      if (update_chacha20_decrypt_stream(initial_counter,
                                         stream_block_count * CHACHA20_BLOCK_SIZE)
          == SL_STATUS_OK) {
        app_state = UPDATE_CHACHA20_DECRYPT_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_CHACHA20_DECRYPT_STREAM:
      stream_block_count++;
      stream_msg_size -= CHACHA20_BLOCK_SIZE;
      if (stream_msg_size <= CHACHA20_BLOCK_SIZE) {
        printf("  + Decrypting last %lu byte(s) ciphertext with %d-bit key... ",
               stream_msg_size, get_symmetric_key_len());
        set_plain_msg_len(stream_msg_size);
        initial_counter++;
        if (update_chacha20_decrypt_stream(initial_counter,
                                           stream_block_count * CHACHA20_BLOCK_SIZE)
            == SL_STATUS_OK) {
          app_state = FINISH_CHACHA20_DECRYPT_STREAM;
        } else {
          app_state = SE_MANAGER_EXIT;
        }
      } else {
        printf("  + Decrypting %d bytes ciphertext with %d-bit key... ",
               CHACHA20_BLOCK_SIZE, get_symmetric_key_len());
        initial_counter++;
        if (update_chacha20_decrypt_stream(initial_counter,
                                           stream_block_count * CHACHA20_BLOCK_SIZE)
            != SL_STATUS_OK) {
          app_state = SE_MANAGER_EXIT;
        }
      }
      break;

    case FINISH_CHACHA20_DECRYPT_STREAM:
      printf("  + Comparing plain message with expected data... ");
      if (memcmp(chacha20_plain_text, get_plain_msg_buf_ptr(),
                 sizeof(chacha20_plain_text)) == 0) {
        printf("OK\n");
      } else {
        printf("Failed\n");
      }
      app_state = SE_MANAGER_EXIT;
      break;
#endif

    case SE_MANAGER_EXIT:
      printf("\n  . SE manager deinitialization... ");
      deinit_se_manager();
      app_state = SE_MANAGER_IDLE;
      break;

    case SE_MANAGER_IDLE:
    default:
      break;
  }
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
