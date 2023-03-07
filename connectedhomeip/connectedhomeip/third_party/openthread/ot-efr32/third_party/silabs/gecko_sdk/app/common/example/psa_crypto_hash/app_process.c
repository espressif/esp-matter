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
 * Print current data length.
 ******************************************************************************/
static void print_data_length(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

/***************************************************************************//**
 * Set pointer to buffer containing the expected hash value.
 ******************************************************************************/
static void set_expect_hash_data(void);

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
static uint8_t example_string[] = "PSA Crypto Hash Example";

/// Message size selection
static uint8_t msg_size_select;

static const uint32_t msg_size[] = {
  MSG_SIZE / 16,
  MSG_SIZE / 4,
  MSG_SIZE
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

/// Test message for streaming
static const char test_msg[] = {
  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
};

/// Expected SHA1 hash value of test message
static const uint8_t expect_sha1_hash[] = {
  0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e,
  0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5,
  0xe5, 0x46, 0x70, 0xf1
};

/// Expected SHA224 hash value of test message
static const uint8_t expect_sha224_hash[] = {
  0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc,
  0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50,
  0xb0, 0xc6, 0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19,
  0x52, 0x52, 0x25, 0x25
};

/// Expected SHA256 hash value of test message
static const uint8_t expect_sha256_hash[] = {
  0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
  0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
  0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
  0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

/// Expected SHA384 hash value of test message
static const uint8_t expect_sha384_hash[] = {
  0x33, 0x91, 0xfd, 0xdd, 0xfc, 0x8d, 0xc7, 0x39,
  0x37, 0x07, 0xa6, 0x5b, 0x1b, 0x47, 0x09, 0x39,
  0x7c, 0xf8, 0xb1, 0xd1, 0x62, 0xaf, 0x05, 0xab,
  0xfe, 0x8f, 0x45, 0x0d, 0xe5, 0xf3, 0x6b, 0xc6,
  0xb0, 0x45, 0x5a, 0x85, 0x20, 0xbc, 0x4e, 0x6f,
  0x5f, 0xe9, 0x5b, 0x1f, 0xe3, 0xc8, 0x45, 0x2b
};

/// Expected SHA512 hash value of test message
static const uint8_t expect_sha512_hash[] = {
  0x20, 0x4a, 0x8f, 0xc6, 0xdd, 0xa8, 0x2f, 0x0a,
  0x0c, 0xed, 0x7b, 0xeb, 0x8e, 0x08, 0xa4, 0x16,
  0x57, 0xc1, 0x6e, 0xf4, 0x68, 0xb2, 0x28, 0xa8,
  0x27, 0x9b, 0xe3, 0x31, 0xa7, 0x03, 0xc3, 0x35,
  0x96, 0xfd, 0x15, 0xc1, 0x3b, 0x1b, 0x07, 0xf9,
  0xaa, 0x1d, 0x3b, 0xea, 0x57, 0x78, 0x9c, 0xa0,
  0x31, 0xad, 0x85, 0xc7, 0xa7, 0x1d, 0xd7, 0x03,
  0x54, 0xec, 0x63, 0x12, 0x38, 0xca, 0x34, 0x45
};

/// Message buffer
static uint8_t msg_buf[MSG_SIZE];

/// Stream message size
static uint32_t stream_msg_size;

/// Stream block counter
static uint32_t stream_block_count;

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
        if (generate_random_number(msg_buf, MSG_SIZE) != PSA_SUCCESS) {
          app_state = PSA_CRYPTO_EXIT;
          break;
        }
        print_data_length();
      } else {
        app_state = PSA_CRYPTO_IDLE;
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
        printf("\n  . Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
        printf("  + Press SPACE to select Hash algorithm (%s or %s or %s or %s "
               "or %s), press ENTER to run.\n", hash_algo_string[0],
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
        printf("  + Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        set_hash_algo(hash_algo[hash_algo_select]);
        app_state = COMPUTE_HASH;
      }
      break;

    case COMPUTE_HASH:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . %s test\n", hash_algo_string[hash_algo_select]);
      printf("  + Generating a %s hash value of %lu bytes random message... ",
             hash_algo_string[hash_algo_select],
             msg_size[msg_size_select]);

      set_msg_buf_ptr(msg_buf);
      if (compute_msg_hash() == PSA_SUCCESS) {
        printf("  + The %s hash value (%d bytes): ",
               hash_algo_string[hash_algo_select], get_hash_size());
        print_buf(get_hash_buf_ptr(), get_hash_size());
        app_state = COMPARE_HASH;
      }
      break;

    case COMPARE_HASH:
      // Use the setting and hash value from COMPUTE_HASH
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Comparing a %s hash value of %lu bytes random message... ",
             hash_algo_string[hash_algo_select],
             msg_size[msg_size_select]);
      if (compare_msg_hash() == PSA_SUCCESS) {
        app_state = START_HASH_STREAM;
      }
      break;

    case START_HASH_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("\n  . %s streaming test\n", hash_algo_string[hash_algo_select]);
      printf("  + Test message (%d bytes) for %s streaming: %s\n",
             strlen(test_msg), hash_algo_string[hash_algo_select],
             test_msg);
      printf("  + Setting a %s stream... ",
             hash_algo_string[hash_algo_select]);
      if (start_hash_stream() == PSA_SUCCESS) {
        stream_block_count = 0;
        stream_msg_size = strlen(test_msg);
        set_msg_buf_ptr((uint8_t *)test_msg);
        set_msg_len(STREAM_BLOCK_SIZE);
        app_state = UPDATE_HASH_STREAM;
      }
      break;

    case UPDATE_HASH_STREAM:
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (update_hash_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        stream_block_count++;
        stream_msg_size -= STREAM_BLOCK_SIZE;
        if (stream_msg_size <= STREAM_BLOCK_SIZE) {
          app_state = UPDATE_LAST_HASH_STREAM;
        }
      } else {
        app_state = PSA_CRYPTO_EXIT;
      }
      break;

    case UPDATE_LAST_HASH_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_msg_len(stream_msg_size);
      if (update_hash_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == PSA_SUCCESS) {
        app_state = FINISH_HASH_STREAM;
      }
      break;

    case FINISH_HASH_STREAM:
      // The hash operation becomes inactive after finishing the hash stream
      app_state = PSA_CRYPTO_EXIT;
      printf("  + Cloning a %s hash operation... ",
             hash_algo_string[hash_algo_select]);
      // Clone the hash operation for hash verification
      if (clone_hash_operation() != PSA_SUCCESS) {
        break;
      }
      printf("  + Finishing a %s stream... ",
             hash_algo_string[hash_algo_select]);
      if (finish_hash_stream() == PSA_SUCCESS) {
        printf("  + The %s hash value (%d bytes): ",
               hash_algo_string[hash_algo_select], get_hash_size());
        print_buf(get_hash_buf_ptr(), get_hash_size());
        app_state = VERIFY_HASH_STREAM;
      }
      break;

    case VERIFY_HASH_STREAM:
      app_state = PSA_CRYPTO_EXIT;
      set_expect_hash_data();
      printf("  + Verifying a %s hash stream... ",
             hash_algo_string[hash_algo_select]);
      if (verify_hash_stream() == PSA_SUCCESS) {
        print_data_length();
      }
      break;

    case PSA_CRYPTO_EXIT:
      printf("\n  . PSA Crypto deinitialization\n");
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
 * Print current data length.
 ******************************************************************************/
static void print_data_length(void)
{
  printf("\n  . Current data length is %lu bytes.\n",
         msg_size[msg_size_select]);
  printf("  + Press SPACE to select data length (%lu or %lu or %lu), press "
         "ENTER to next option.\n", msg_size[0], msg_size[1], msg_size[2]);
  app_state = SELECT_DATA_SIZE;
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
 * Set pointer to buffer containing the expected hash value.
 ******************************************************************************/
static void set_expect_hash_data(void)
{
  switch (hash_algo[hash_algo_select]) {
    case PSA_ALG_SHA_1:
      memcpy(get_hash_buf_ptr(), (uint8_t *)expect_sha1_hash,
             sizeof(expect_sha1_hash));
      break;

    case PSA_ALG_SHA_224:
      memcpy(get_hash_buf_ptr(), (uint8_t *)expect_sha224_hash,
             sizeof(expect_sha224_hash));
      break;

    case PSA_ALG_SHA_256:
      memcpy(get_hash_buf_ptr(), (uint8_t *)expect_sha256_hash,
             sizeof(expect_sha256_hash));
      break;

    case PSA_ALG_SHA_384:
      memcpy(get_hash_buf_ptr(), (uint8_t *)expect_sha384_hash,
             sizeof(expect_sha384_hash));
      break;

    case PSA_ALG_SHA_512:
      memcpy(get_hash_buf_ptr(), (uint8_t *)expect_sha512_hash,
             sizeof(expect_sha512_hash));
      break;

    default:
      break;
  }
}
