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
 * @returns Returns true if successful and false otherwise.
 ******************************************************************************/
static bool fill_buf_with_random_number(void);

/***************************************************************************//**
 * Create symmetric key for cipher operations.
 ******************************************************************************/
static void create_symmetric_key(void);

/***************************************************************************//**
 * Print key type.
 ******************************************************************************/
static void print_key_type(void);

/***************************************************************************//**
 * Print encryption text.
 ******************************************************************************/
static void print_encrypt_text(void);

/***************************************************************************//**
 * Print decryption text.
 ******************************************************************************/
static void print_decrypt_text(void);

/***************************************************************************//**
 * Compare decrypted message with plaintext
 * @returns Returns true if successful and false otherwise
 ******************************************************************************/
static bool compare_decrypt_with_plain(void);

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
static uint8_t example_string[] = "SE Manager Block Cipher Example";

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Symmetric key type (plain or wrap or volatile)
static uint8_t symmetric_key_type_select;

static const char *symmetric_key_type_string[] = {
  "plaintext",
  "wrapped",
  "volatile"
};

/// Symmetric key size when creating volatile key
static uint8_t create_volatile_key_size = 0xff;
#endif

/// Symmetric key size selection
static uint8_t symmetric_key_size_select;

static const sl_se_key_type_t symmetric_key_size[] = {
  SL_SE_KEY_TYPE_AES_128,
  SL_SE_KEY_TYPE_AES_192,
  SL_SE_KEY_TYPE_AES_256
};

/// Plain message size selection
static uint8_t plain_msg_size_select;

static const uint32_t plain_msg_size[] = {
  PLAIN_MSG_SIZE / 16,
  PLAIN_MSG_SIZE / 4,
  PLAIN_MSG_SIZE
};

/// Hash algorithm selection for HMAC
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

/// Initialization vector (IV) or nonce buffer copy
static uint8_t iv_nonce_buf_copy[IV_NONCE_SIZE];

/// Plain message buffer copy
static uint8_t plain_msg_buf_copy[PLAIN_MSG_SIZE];

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
        if (!fill_buf_with_random_number()) {
          app_state = SE_MANAGER_EXIT;
          break;
        }
        print_key_type();
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case SELECT_KEY_TYPE:
      if (space_press) {
        space_press = false;
        symmetric_key_type_select++;
        if (symmetric_key_type_select == 3) {
          symmetric_key_type_select = 0;
        }
        printf("  + Current symmetric key is a %s key.\n",
               symmetric_key_type_string[symmetric_key_type_select]);
      }
      if (enter_press) {
        enter_press = false;
        printf("\n  . Current symmetric key length is %lu-bit.\n",
               symmetric_key_size[symmetric_key_size_select] * 8);
        printf("  + Press SPACE to select symmetric key length (128 or 192 or "
               "256), press ENTER to next option.\n");
        app_state = SELECT_KEY_LENGTH;
      }
      break;
#endif

    case SELECT_KEY_LENGTH:
      if (space_press) {
        space_press = false;
        symmetric_key_size_select++;
        if (symmetric_key_size_select == 3) {
          symmetric_key_size_select = 0;
        }
        printf("  + Current symmetric key length is %lu-bit.\n",
               symmetric_key_size[symmetric_key_size_select] * 8);
      }
      if (enter_press) {
        enter_press = false;
        create_symmetric_key();
      }
      break;

    case SELECT_DATA_SIZE:
      if (space_press) {
        space_press = false;
        plain_msg_size_select++;
        if (plain_msg_size_select == 3) {
          plain_msg_size_select = 0;
        }
        printf("  + Current data length is %lu bytes.\n",
               plain_msg_size[plain_msg_size_select]);
      }
      if (enter_press) {
        enter_press = false;
        set_plain_msg_len(plain_msg_size[plain_msg_size_select]);
        printf("\n  . Current Hash algorithm for HMAC is %s.\n",
               hash_algo_string[hash_algo_select]);
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        printf("  + Press SPACE to select Hash algorithm (SHA1/224/256/384/512) "
               "for HMAC, press ENTER to run.\n");
#else
        printf("  + Press SPACE to select Hash algorithm (SHA1/224/256) for "
               "HMAC, press ENTER to run.\n");
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
        printf("  + Current Hash algorithm for HMAC is %s.\n",
               hash_algo_string[hash_algo_select]);
      }
      if (enter_press) {
        enter_press = false;
        app_state = AES_ECB_TEST;
      }
      break;

    case AES_ECB_TEST:
      printf("\n  . AES ECB test\n");
      print_encrypt_text();
      if (encrypt_aes_ecb() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      print_decrypt_text();
      if (decrypt_aes_ecb() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CTR_TEST;
      }
      break;

    case AES_CTR_TEST:
      printf("\n  . AES CTR test\n");
      set_stream_offset(0);
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      memset(get_iv_nonce_buf_ptr() + 12, 0, 4);        // Reset counter to 0
      print_encrypt_text();
      if (encrypt_aes_ctr() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      set_stream_offset(0);
      memset(get_iv_nonce_buf_ptr() + 12, 0, 4);        // Reset counter to 0
      print_decrypt_text();
      if (decrypt_aes_ctr() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CCM_TEST;
      }
      break;

    case AES_CCM_TEST:
      printf("\n  . AES CCM test\n");
      set_tag_len(TAG_LEN);
      set_nonce_len(NONCE_LEN);
      set_ad_len(AD_SIZE);
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);

      print_encrypt_text();
      if (encrypt_aes_ccm() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      print_decrypt_text();
      if (decrypt_aes_ccm() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_GCM_TEST;
      }
      break;

    case AES_GCM_TEST:
      printf("\n  . AES GCM test\n");
      set_tag_len(TAG_LEN);
      set_nonce_len(NONCE_LEN);
      set_ad_len(AD_SIZE);
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);

      print_encrypt_text();
      if (encrypt_aes_gcm() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      print_decrypt_text();
      if (decrypt_aes_gcm() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CBC_TEST;
      }
      break;

    case AES_CBC_TEST:
      printf("\n  . AES CBC test\n");
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_encrypt_text();
      if (encrypt_aes_cbc() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_decrypt_text();
      if (decrypt_aes_cbc() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CFB8_TEST;
      }
      break;

    case AES_CFB8_TEST:
      printf("\n  . AES CFB8 test\n");
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_encrypt_text();
      if (encrypt_aes_cfb8() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_decrypt_text();
      if (decrypt_aes_cfb8() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CFB128_TEST;
      }
      break;

    case AES_CFB128_TEST:
      printf("\n  . AES CFB128 test\n");
      set_stream_offset(0);
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_encrypt_text();
      if (encrypt_aes_cfb128() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      set_stream_offset(0);
      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      print_decrypt_text();
      if (decrypt_aes_cfb128() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      if (compare_decrypt_with_plain()) {
        app_state = AES_CMAC_TEST;
      }
      break;

    case AES_CMAC_TEST:
      printf("\n  . AES CMAC test\n");
      memset(get_cipher_msg_buf_ptr(), 0,
             plain_msg_size[plain_msg_size_select]);
      printf(
        "  + Generating 16 bytes CMAC on %lu bytes message with %d bit key... ",
        plain_msg_size[plain_msg_size_select],
        get_symmetric_key_len());
      if (generate_aes_cmac() == SL_STATUS_OK) {
        app_state = HMAC_TEST;
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case HMAC_TEST:
      printf("\n  . HMAC test\n");
      memset(get_cipher_msg_buf_ptr(), 0,
             plain_msg_size[plain_msg_size_select]);
      printf("  + Generating %s HMAC on %lu bytes message with %d bit key... ",
             hash_algo_string[hash_algo_select],
             plain_msg_size[plain_msg_size_select], get_symmetric_key_len());
      if (generate_hmac(hash_algo[hash_algo_select]) == SL_STATUS_OK) {
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        app_state = CHACHA20_POLY1305_TEST;
#else
        print_key_type();
#endif
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
    case CHACHA20_POLY1305_TEST:
      printf("\n  . ChaCha20-Poly1305 test\n");

      // Must use 256-bit key
      uint8_t temp;
      temp = symmetric_key_size_select;
      if (symmetric_key_size_select != 2) {
        symmetric_key_size_select = 2;
        create_symmetric_key();
        if (app_state == SE_MANAGER_EXIT) {
          break;
        }
      }

      memcpy(get_iv_nonce_buf_ptr(), iv_nonce_buf_copy, IV_NONCE_SIZE);
      printf("  + Generating Poly1305 MAC on %lu bytes message with %d bit "
             "key... ", plain_msg_size[plain_msg_size_select],
             get_symmetric_key_len());

      if (generate_poly1305_mac() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      set_ad_len(AD_SIZE);
      print_encrypt_text();
      if (encrypt_chacha20_poly1305() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      print_decrypt_text();
      if (decrypt_chacha20_poly1305() != SL_STATUS_OK) {
        app_state = SE_MANAGER_EXIT;
        break;
      }

      symmetric_key_size_select = temp;
      if (compare_decrypt_with_plain()) {
        print_key_type();
      }
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
static bool fill_buf_with_random_number(void)
{
  // Fill IV buffer with random number
  printf("\n  . Fill buffers for block cipher operations.\n");
  printf("  + Filling %d bytes IV buffer with random number... ",
         IV_NONCE_SIZE);
  if (generate_random_number(get_iv_nonce_buf_ptr(),
                             IV_NONCE_SIZE) != SL_STATUS_OK) {
    return false;
  }

  // Fill associated data buffer with random number
  printf("  + Filling %d bytes associated data buffer with random number... ",
         AD_SIZE);
  if (generate_random_number(get_ad_buf_ptr(), AD_SIZE) != SL_STATUS_OK) {
    return false;
  }

  // Fill plain message buffer with random number
  printf("  + Filling %d bytes plain message buffer with random number... ",
         PLAIN_MSG_SIZE);
  if (generate_random_number(get_plain_msg_buf_ptr(),
                             PLAIN_MSG_SIZE) != SL_STATUS_OK) {
    return false;
  }

  // Plain message buffer copy for decryption check
  memcpy(plain_msg_buf_copy, get_plain_msg_buf_ptr(), PLAIN_MSG_SIZE);

  // Copy to restore IV buffer (modified by CTR, CBC and CFB)
  memcpy(iv_nonce_buf_copy, get_iv_nonce_buf_ptr(), IV_NONCE_SIZE);
  return true;
}

/***************************************************************************//**
 * Create symmetric key for cipher operations.
 ******************************************************************************/
static void create_symmetric_key(void)
{
  sl_status_t ret;

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  switch (symmetric_key_type_select) {
    case 0:
      printf("  + Generating a %lu-bit symmetric plaintext key... ",
             symmetric_key_size[symmetric_key_size_select] * 8);
      ret = create_plain_symmetric_key(symmetric_key_size[symmetric_key_size_select]);
      break;

    case 1:
      printf("  + Generating a %lu-bit non-exportable symmetric wrapped key... ",
             symmetric_key_size[symmetric_key_size_select] * 8);
      ret = create_wrap_symmetric_key(symmetric_key_size[symmetric_key_size_select]);
      break;

    case 2:
      if (create_volatile_key_size != 0xff) {
        printf("  + Delete a non-exportable symmetric key in a volatile SE "
               "key slot... ");
        ret = delete_volatile_symmetric_key(symmetric_key_size[create_volatile_key_size]);
        if (ret != SL_STATUS_OK) {
          break;
        }
      }
      create_volatile_key_size = symmetric_key_size_select;
      printf("  + Generating a %lu-bit non-exportable symmetric volatile key... ",
             symmetric_key_size[symmetric_key_size_select] * 8);
      ret = create_volatile_symmetric_key(symmetric_key_size[symmetric_key_size_select]);
      break;

    default:
      ret = SL_STATUS_FAIL;
      break;
  }
#else
  printf("  + Generating a %lu-bit symmetric plaintext key... ",
         symmetric_key_size[symmetric_key_size_select] * 8);
  ret = create_plain_symmetric_key(symmetric_key_size[symmetric_key_size_select]);
#endif
  if (ret == SL_STATUS_OK) {
    if (app_state != CHACHA20_POLY1305_TEST) {
      printf("\n  . Current data length is %lu bytes.\n",
             plain_msg_size[plain_msg_size_select]);
      printf("  + Press SPACE to select data length (%lu or %lu or %lu), press "
             "ENTER to next option.\n", plain_msg_size[0], plain_msg_size[1],
             plain_msg_size[2]);
      app_state = SELECT_DATA_SIZE;
    }
  } else {
    app_state = SE_MANAGER_EXIT;
  }
}

/***************************************************************************//**
 * Print key type.
 ******************************************************************************/
static void print_key_type(void)
{
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
  printf("\n  . Current symmetric key is a %s key.\n",
         symmetric_key_type_string[symmetric_key_type_select]);
  printf("  + Press SPACE to select a %s or %s or %s key, press ENTER to next "
         "option.\n",
         symmetric_key_type_string[0],
         symmetric_key_type_string[1],
         symmetric_key_type_string[2]);
  app_state = SELECT_KEY_TYPE;
#else
  printf("\n  . Current symmetric key length is %lu-bit.\n",
         symmetric_key_size[symmetric_key_size_select] * 8);
  printf("  + Press SPACE to select symmetric key length (128 or 192 or 256), "
         "press ENTER to next option.\n");
  app_state = SELECT_KEY_LENGTH;
#endif
}

/***************************************************************************//**
 * Print encryption text.
 ******************************************************************************/
static void print_encrypt_text(void)
{
  memset(get_cipher_msg_buf_ptr(), 0,
         plain_msg_size[plain_msg_size_select] + TAG_LEN);
  printf("  + Encrypting %lu bytes plaintext with %d bit key... ",
         plain_msg_size[plain_msg_size_select], get_symmetric_key_len());
}

/***************************************************************************//**
 * Print decryption text.
 ******************************************************************************/
static void print_decrypt_text(void)
{
  memset(get_plain_msg_buf_ptr(), 0, plain_msg_size[plain_msg_size_select]);
  printf("  + Decrypting %lu bytes ciphertext with %d bit key... ",
         plain_msg_size[plain_msg_size_select], get_symmetric_key_len());
}

/***************************************************************************//**
 * Compare decrypted message with plaintext.
 ******************************************************************************/
static bool compare_decrypt_with_plain(void)
{
  printf("  + Comparing decrypted message with plain message... ");
  if (memcmp(plain_msg_buf_copy, get_plain_msg_buf_ptr(),
             plain_msg_size[plain_msg_size_select]) == 0) {
    printf("OK\n");
    return true;
  } else {
    printf("Failed\n");
    app_state = SE_MANAGER_EXIT;
    return false;
  }
}
