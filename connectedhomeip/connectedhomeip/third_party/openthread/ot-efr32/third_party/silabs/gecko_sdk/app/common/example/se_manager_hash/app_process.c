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
 * Compare hash value with expected data.
 ******************************************************************************/
static void compare_hash_with_expect_data(void);

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
static state_t app_state = SE_MANAGER_INIT;

/// String for example
static uint8_t example_string[] = "SE Manager Hash Example";

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
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  "SHA384",
  "SHA512"
#endif
};

static const sl_se_hash_type_t hash_algo[] = {
  SL_SE_HASH_SHA1,
  SL_SE_HASH_SHA224,
  SL_SE_HASH_SHA256,
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  SL_SE_HASH_SHA384,
  SL_SE_HASH_SHA512
#endif
};

/// Message buffer
static uint8_t msg_buf[MSG_SIZE];

/// Stream message size
static uint32_t stream_msg_size;

/// Stream block counter
static uint32_t stream_block_count;

/// Test vector for streaming
static const char test_vector[] = {
  "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
};

/// Expected SHA1 hash value of test vector
static const uint8_t expect_sha1_hash[] = {
  0x84, 0x98, 0x3e, 0x44, 0x1c, 0x3b, 0xd2, 0x6e,
  0xba, 0xae, 0x4a, 0xa1, 0xf9, 0x51, 0x29, 0xe5,
  0xe5, 0x46, 0x70, 0xf1
};

/// Expected SHA224 hash value of test vector
static const uint8_t expect_sha224_hash[] = {
  0x75, 0x38, 0x8b, 0x16, 0x51, 0x27, 0x76, 0xcc,
  0x5d, 0xba, 0x5d, 0xa1, 0xfd, 0x89, 0x01, 0x50,
  0xb0, 0xc6, 0x45, 0x5c, 0xb4, 0xf5, 0x8b, 0x19,
  0x52, 0x52, 0x25, 0x25
};

/// Expected SHA256 hash value of test vector
static const uint8_t expect_sha256_hash[] = {
  0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
  0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
  0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
  0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Expected SHA384 hash value of test vector
static const uint8_t expect_sha384_hash[] = {
  0x33, 0x91, 0xfd, 0xdd, 0xfc, 0x8d, 0xc7, 0x39,
  0x37, 0x07, 0xa6, 0x5b, 0x1b, 0x47, 0x09, 0x39,
  0x7c, 0xf8, 0xb1, 0xd1, 0x62, 0xaf, 0x05, 0xab,
  0xfe, 0x8f, 0x45, 0x0d, 0xe5, 0xf3, 0x6b, 0xc6,
  0xb0, 0x45, 0x5a, 0x85, 0x20, 0xbc, 0x4e, 0x6f,
  0x5f, 0xe9, 0x5b, 0x1f, 0xe3, 0xc8, 0x45, 0x2b
};

/// Expected SHA512 hash value of test vector
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
#endif

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
    case SE_MANAGER_INIT:
      printf("\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        printf("  + Filling %d bytes message buffer with random number... ",
               MSG_SIZE);
        if (generate_random_number(msg_buf, MSG_SIZE) != SL_STATUS_OK) {
          app_state = SE_MANAGER_EXIT;
          break;
        }
        print_data_length();
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case SELECT_DATA_SIZE:
      if (space_press) {
        space_press = false;
        msg_size_select++;
        if (msg_size_select == 3) {
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
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Press SPACE to select Hash algorithm (SHA1/224/256/384/512), "
               " press ENTER to run.\n");
#else
        printf("  + Press SPACE to select Hash algorithm (SHA1/224/256), "
               "press ENTER to run.\n");
#endif
        app_state = SELECT_HASH_TYPE;
      }
      break;

    case SELECT_HASH_TYPE:
      if (space_press) {
        space_press = false;
        hash_algo_select++;
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        if (hash_algo_select == 5) {
#else
        if (hash_algo_select == 3) {
#endif
          hash_algo_select = 0;
        }
        printf("  + Current Hash algorithm is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        set_hash_type(hash_algo[hash_algo_select]);
        app_state = HASH_TEST;
      }
      break;

    case HASH_TEST:
      printf("\n  . %s test\n", hash_algo_string[hash_algo_select]);
      printf("  + Generating a %s hash value of %lu bytes message... ",
             hash_algo_string[hash_algo_select],
             msg_size[msg_size_select]);

      set_msg_buf_ptr(msg_buf);
      if (compute_msg_hash() == SL_STATUS_OK) {
        printf("  + The %s hash value (%d bytes): ",
               hash_algo_string[hash_algo_select], get_hash_size());
        print_buf(get_hash_buf_ptr(), get_hash_size());
        app_state = START_HASH_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case START_HASH_STREAM:
      printf("\n  . %s streaming test\n", hash_algo_string[hash_algo_select]);
      printf("  + Test vector (%d bytes) for %s streaming: %s\n",
             strlen(test_vector), hash_algo_string[hash_algo_select],
             test_vector);
      printf("  + Preparing a %s streaming command context... ",
             hash_algo_string[hash_algo_select]);
      // It can also use start_shax_stream()
      if (start_generic_stream() == SL_STATUS_OK) {
        stream_block_count = 0;
        stream_msg_size = strlen(test_vector);
        set_msg_buf_ptr((uint8_t *)test_vector);
        set_msg_len(STREAM_BLOCK_SIZE);
        app_state = UPDATE_HASH_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_HASH_STREAM:
      printf("  + Streaming %d bytes message... ", STREAM_BLOCK_SIZE);
      if (update_hash_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == SL_STATUS_OK) {
        stream_block_count++;
        stream_msg_size -= STREAM_BLOCK_SIZE;
        if (stream_msg_size <= STREAM_BLOCK_SIZE) {
          app_state = UPDATE_LAST_HASH_STREAM;
        }
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case UPDATE_LAST_HASH_STREAM:
      printf("  + Streaming last %lu byte(s) message... ", stream_msg_size);
      set_msg_len(stream_msg_size);
      if (update_hash_stream(STREAM_BLOCK_SIZE * stream_block_count)
          == SL_STATUS_OK) {
        app_state = FINISH_HASH_STREAM;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case FINISH_HASH_STREAM:
      printf("  + Finishing %s streaming... ",
             hash_algo_string[hash_algo_select]);
      if (finish_hash_stream() == SL_STATUS_OK) {
        printf("  + The %s hash value (%d bytes): ",
               hash_algo_string[hash_algo_select], get_hash_size());
        print_buf(get_hash_buf_ptr(), get_hash_size());
        compare_hash_with_expect_data();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

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
void print_buf(uint8_t *buf, size_t len)
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
 * Compare hash value with expected data.
 ******************************************************************************/
static void compare_hash_with_expect_data(void)
{
  int16_t ret;

  printf("  + Comparing %s hash value with expected data... ",
         hash_algo_string[hash_algo_select]);

  switch (hash_algo[hash_algo_select]) {
    case SL_SE_HASH_SHA1:
      ret = memcmp(get_hash_buf_ptr(), expect_sha1_hash, get_hash_size());
      break;

    case SL_SE_HASH_SHA224:
      ret = memcmp(get_hash_buf_ptr(), expect_sha224_hash, get_hash_size());
      break;

    case SL_SE_HASH_SHA256:
      ret = memcmp(get_hash_buf_ptr(), expect_sha256_hash, get_hash_size());
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SL_SE_HASH_SHA384:
      ret = memcmp(get_hash_buf_ptr(), expect_sha384_hash, get_hash_size());
      break;

    case SL_SE_HASH_SHA512:
      ret = memcmp(get_hash_buf_ptr(), expect_sha512_hash, get_hash_size());
      break;
#endif

    default:
      ret = 1;
      break;
  }

  if (ret == 0) {
    printf("OK\n");
    print_data_length();
  } else {
    printf("Failed\n");
    app_state = SE_MANAGER_EXIT;
  }
}
