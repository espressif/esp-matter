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
 * Print OTP configuration.
 ******************************************************************************/
static void print_otp_conf(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Print tamper configuration.
 ******************************************************************************/
static void print_tamper_conf(void);
#endif

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
static uint8_t example_string[] = "SE Manager Key Provisioning Example";

/// Key buffer and response buffer of executed command
SL_ALIGN(4) static uint8_t resp_buf[RESP_BUF_SIZE];

/// Public sign key
SL_ALIGN(4) static const uint8_t public_sign_key[PUB_KEY_SIZE] = {
  0xc4, 0xaf, 0x4a, 0xc6, 0x9a, 0xab, 0x95, 0x12,
  0xdb, 0x50, 0xf7, 0xa2, 0x6a, 0xe5, 0xb4, 0x80,
  0x11, 0x83, 0xd8, 0x54, 0x17, 0xe7, 0x29, 0xa5,
  0x6d, 0xa9, 0x74, 0xf4, 0xe0, 0x8a, 0x56, 0x2c,
  0xde, 0x60, 0x19, 0xde, 0xa9, 0x41, 0x13, 0x32,
  0xdc, 0x1a, 0x74, 0x33, 0x72, 0xd1, 0x70, 0xb4,
  0x36, 0x23, 0x8a, 0x34, 0x59, 0x7c, 0x41, 0x0e,
  0xa1, 0x77, 0x02, 0x4d, 0xe2, 0x0f, 0xc8, 0x19
};

/// Public command key
SL_ALIGN(4) static const uint8_t public_command_key[PUB_KEY_SIZE] = {
  0xb1, 0xbc, 0x6f, 0x6f, 0xa5, 0x66, 0x40, 0xed,
  0x52, 0x2b, 0x2e, 0xe0, 0xf5, 0xb3, 0xcf, 0x7e,
  0x5d, 0x48, 0xf6, 0x0b, 0xe8, 0x14, 0x8f, 0x0d,
  0xc0, 0x84, 0x40, 0xf0, 0xa4, 0xe1, 0xdc, 0xa4,
  0x7c, 0x04, 0x11, 0x9e, 0xd6, 0xa1, 0xbe, 0x31,
  0xb7, 0x70, 0x7e, 0x5f, 0x9d, 0x00, 0x1a, 0x65,
  0x9a, 0x05, 0x10, 0x03, 0xe9, 0x5e, 0x1b, 0x93,
  0x6f, 0x05, 0xc3, 0x7e, 0xa7, 0x93, 0xad, 0x63
};

#if !defined(CRYPTOACC_PRESENT)
/// 128-bit AES key
SL_ALIGN(4) static const uint8_t aes_key[16] = {
  0x81, 0xa5, 0xe2, 0x1f, 0xa1, 0x52, 0x86, 0xf1,
  0xdf, 0x44, 0x5c, 0x2c, 0xc1, 0x20, 0xfa, 0x3f
};

/// Ciphertext to verify 128-bit AES key
static const uint8_t ciphertext[16] = {
  0x66, 0xd2, 0x0f, 0x99, 0x65, 0x3e, 0xa8, 0xd0,
  0x83, 0x05, 0xa6, 0x39, 0xd4, 0x4e, 0x98, 0xa6
};
#endif

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/// Strings for tamper sources
static const char *tamper_source[SL_SE_TAMPER_SIGNAL_NUM_SIGNALS] = {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
  NULL,
  "Filter counter         : ",
  "SE watchdog            : ",
  NULL,
  "SE RAM CRC             : ",
  "SE hard fault          : ",
  NULL,
  "SE software assertion  : ",
  "SE secure boot         : ",
  "User secure boot       : ",
  "Mailbox authorization  : ",
  "DCI authorization      : ",
  "OTP read               : ",
  NULL,
  "Self test              : ",
  "TRNG monitor           : ",
  "PRS0                   : ",
  "PRS1                   : ",
  "PRS2                   : ",
  "PRS3                   : ",
  "PRS4                   : ",
  "PRS5                   : ",
  "PRS6                   : ",
  "PRS7                   : ",
  "Decouple BOD           : ",
  "Temperature sensor     : ",
  "Voltage glitch falling : ",
  "Voltage glitch rising  : ",
  "Secure lock            : ",
  "SE debug               : ",
  "Digital glitch         : ",
  "SE ICACHE              : "
#else
  NULL,
  "Filter counter        : ",
  "SE watchdog           : ",
  NULL,
  "SE RAM ECC 2          : ",
  "SE hard fault         : ",
  NULL,
  "SE software assertion : ",
  "SE secure boot        : ",
  "User secure boot      : ",
  "Mailbox authorization : ",
  "DCI authorization     : ",
  "OTP Read              : ",
  NULL,
  "Self test             : ",
  "TRNG monitor          : ",
  "Secure lock           : ",
  "Digital glitch        : ",
  "Voltage glitch        : ",
  "SE ICACHE             : ",
  "SE RAM ECC 1          : ",
  "BOD                   : ",
  "Temperature sensor    : ",
  "DPLL lock fail low    : ",
  "DPLL lock fial high   : ",
  "PRS0                  : ",
  "PRS1                  : ",
  "PRS2                  : ",
  "PRS3                  : ",
  "PRS4                  : ",
  "PRS5                  : ",
  "PRS6                  : "
#endif
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
      printf("\n\n%s - Core running at %" PRIu32 " kHz.\n", example_string,
             CMU_ClockFreqGet(cmuClock_CORE) / 1000);
      printf("  . SE manager initialization... ");
      if (init_se_manager() == SL_STATUS_OK) {
        app_state = READ_SE_VERSION;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case READ_SE_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get current SE firmware version... ");
      if (get_se_version() == SL_STATUS_OK) {
        printf("  + Current SE firmware version (MSB..LSB): %08lX\n",
               get_version());
        app_state = GET_OTP_CONF;
      }
      break;

    case GET_OTP_CONF:
      printf("\n  . Read SE OTP configuration... ");
      if (get_se_otp_conf() == SL_STATUS_OK) {
        print_otp_conf();
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
        print_tamper_conf();
#endif
      }
#if defined(CRYPTOACC_PRESENT)
      app_state = CHECK_EXECUTED_COMMAND;
#else
      printf("\n  . Press ENTER to program 128-bit AES key in SE OTP or press "
             "SPACE to skip.\n");
      app_state = PRE_INIT_AES_KEY;
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case CHECK_EXECUTED_COMMAND:
      printf("\n  . Check executed command... ");
      if (check_executed_command() != SL_STATUS_OK) {
        printf("  + No executed command before, press ENTER to program public "
               "sign key in SE OTP or press SPACE to skip.\n");
        app_state = PRE_INIT_PUB_SIGN_KEY;
      } else {
        switch (get_executed_command()) {
          case (SLI_SE_COMMAND_INIT_PUBKEY | SLI_SE_KEY_TYPE_BOOT):
            app_state = POST_INIT_PUB_SIGN_KEY;
            break;

          case (SLI_SE_COMMAND_READ_PUBKEY | SLI_SE_KEY_TYPE_BOOT):
            app_state = POST_GET_PUB_SIGN_KEY;
            break;

          case (SLI_SE_COMMAND_INIT_PUBKEY | SLI_SE_KEY_TYPE_AUTH):
            app_state = POST_INIT_PUB_CMD_KEY;
            break;

          case (SLI_SE_COMMAND_READ_PUBKEY | SLI_SE_KEY_TYPE_AUTH):
            app_state = POST_GET_PUB_CMD_KEY;
            break;

          case SLI_SE_COMMAND_INIT_OTP:
            app_state = POST_INIT_OTP;
            break;

          default:
            app_state = SE_MANAGER_EXIT;
            break;
        }
      }
      break;
#endif

#if !defined(CRYPTOACC_PRESENT)
    case PRE_INIT_AES_KEY:
      if (enter_press) {
        enter_press = false;
        printf("  + Warning: The 128-bit AES key in SE OTP cannot be changed "
               "once written!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = INIT_AES_KEY;
      }
      if (space_press) {
        space_press = false;
        app_state = CHECK_AES_KEY;
      }
      break;

    case INIT_AES_KEY:
      if (enter_press) {
        enter_press = false;
        set_key_buf_ptr((uint8_t *)aes_key);
        printf("\n  . Initialize 128-bit AES key... ");
        program_aes_key();
        app_state = CHECK_AES_KEY;
      }
      if (space_press) {
        space_press = false;
        app_state = CHECK_AES_KEY;
      }
      break;

    case CHECK_AES_KEY:
      printf("\n  . Encrypt 16 bytes plaintext with 128-bit AES OTP key... ");
      set_key_buf_ptr(resp_buf);
      if (encrypt_aes_ecb() == SL_STATUS_OK) {
        printf("  + Compare encrypted message with expected ciphertext... ");
        if (memcmp(resp_buf, ciphertext, sizeof(ciphertext)) == 0) {
          printf("OK\n");
        } else {
          printf("Failed\n");
        }
      }
      printf("\n  . Press ENTER to program public sign key in SE OTP or press "
             "SPACE to skip.\n");
      app_state = PRE_INIT_PUB_SIGN_KEY;
      break;
#endif

    case PRE_INIT_PUB_SIGN_KEY:
      if (enter_press) {
        enter_press = false;
        printf("  + Warning: The public sign key in SE OTP cannot be changed "
               "once written!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = INIT_PUB_SIGN_KEY;
      }
      if (space_press) {
        space_press = false;
        app_state = GET_PUB_SIGN_KEY;
      }
      break;

    case INIT_PUB_SIGN_KEY:
      if (enter_press) {
        enter_press = false;
        app_state = GET_PUB_SIGN_KEY;
        set_key_buf_ptr((uint8_t *)public_sign_key);
#if defined(CRYPTOACC_PRESENT)
        printf("\n  . Reset to initialize public sign key...  ");
#else
        printf("\n  . Initialize public sign key... ");
#endif
        program_public_key(SL_SE_KEY_TYPE_IMMUTABLE_BOOT);
      }
      if (space_press) {
        space_press = false;
        app_state = GET_PUB_SIGN_KEY;
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_INIT_PUB_SIGN_KEY:
      printf("\n  . Get public sign key initialization response after "
             "reset... ");
      get_executed_command_response(resp_buf, sizeof(resp_buf));
      app_state = GET_PUB_SIGN_KEY;
      break;
#endif

    case GET_PUB_SIGN_KEY:
      app_state = PRE_INIT_PUB_CMD_KEY;
      set_key_buf_ptr(resp_buf);
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to get public sign key...  ");
      read_public_key(SL_SE_KEY_TYPE_IMMUTABLE_BOOT);
#else
      printf("\n  . Get public sign key... ");
      if (read_public_key(SL_SE_KEY_TYPE_IMMUTABLE_BOOT) == SL_STATUS_OK) {
        printf("  + The public sign key (%d bytes): ", PUB_KEY_SIZE);
        print_buf(resp_buf, PUB_KEY_SIZE);
      }
#endif
      printf("\n  . Press ENTER to program public command key in SE OTP or "
             "press SPACE to skip.\n");
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_GET_PUB_SIGN_KEY:
      printf("\n  . Get public sign key response after reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf))
          == SL_STATUS_OK) {
        printf("  + The public sign key (%d bytes): ", PUB_KEY_SIZE);
        print_buf(resp_buf, PUB_KEY_SIZE);
      }
      printf("\n  . Press ENTER to program public command key in SE OTP or "
             "press SPACE to skip.\n");
      app_state = PRE_INIT_PUB_CMD_KEY;
      break;
#endif

    case PRE_INIT_PUB_CMD_KEY:
      if (enter_press) {
        enter_press = false;
        printf("  + Warning: The public command key in SE OTP cannot be changed"
               " once written!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = INIT_PUB_CMD_KEY;
      }
      if (space_press) {
        space_press = false;
        app_state = GET_PUB_CMD_KEY;
      }
      break;

    case INIT_PUB_CMD_KEY:
      if (enter_press) {
        enter_press = false;
        app_state = GET_PUB_CMD_KEY;
        set_key_buf_ptr((uint8_t *)public_command_key);
#if defined(CRYPTOACC_PRESENT)
        printf("\n  . Reset to initialize public command key...  ");
#else
        printf("\n  . Initialize public command key... ");
#endif
        program_public_key(SL_SE_KEY_TYPE_IMMUTABLE_AUTH);
      }
      if (space_press) {
        space_press = false;
        app_state = GET_PUB_CMD_KEY;
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_INIT_PUB_CMD_KEY:
      printf("\n  . Get public command key initialization response after "
             "reset... ");
      get_executed_command_response(resp_buf, sizeof(resp_buf));
      app_state = GET_PUB_CMD_KEY;
      break;
#endif

    case GET_PUB_CMD_KEY:
      app_state = PRE_INIT_OTP;
      set_key_buf_ptr(resp_buf);
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to get public command key...  ");
      read_public_key(SL_SE_KEY_TYPE_IMMUTABLE_AUTH);
#else
      printf("\n  . Get public command key... ");
      if (read_public_key(SL_SE_KEY_TYPE_IMMUTABLE_AUTH) == SL_STATUS_OK) {
        printf("  + The public command key (%d bytes): ", PUB_KEY_SIZE);
        print_buf(resp_buf, PUB_KEY_SIZE);
      }
#endif
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
      printf("\n  . Press ENTER to initialize SE OTP for secure boot and "
             "tamper configuration or press SPACE to skip.\n");
#else
      printf("\n  . Press ENTER to initialize SE OTP for secure boot "
             "configuration or press SPACE to skip.\n");
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_GET_PUB_CMD_KEY:
      printf("\n  . Get public command key response after reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf))
          == SL_STATUS_OK) {
        printf("  + The public command key (%d bytes): ", PUB_KEY_SIZE);
        print_buf(resp_buf, PUB_KEY_SIZE);
      }
      printf("\n  . Press ENTER to initialize SE OTP for secure boot "
             "configuration or press SPACE to skip.\n");
      app_state = PRE_INIT_OTP;
      break;
#endif

    case PRE_INIT_OTP:
      if (enter_press) {
        enter_press = false;
        printf("  + Warning: The SE OTP configuration cannot be changed once "
               "written!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = INIT_OTP;
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case INIT_OTP:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
#if defined(CRYPTOACC_PRESENT)
        printf("\n  . Reset to initialize SE OTP...  ");
        init_se_otp_conf();
#else
        printf("\n  . Initialize SE OTP... ");
        if (init_se_otp_conf() == SL_STATUS_OK) {
          printf("  + Read SE OTP configuration... ");
          if (get_se_otp_conf() == SL_STATUS_OK) {
            print_otp_conf();
#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
            print_tamper_conf();
#endif
          }
        }
#endif
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_INIT_OTP:
      printf("\n  . Get SE OTP initialization response after reset... ");
      get_executed_command_response(resp_buf, sizeof(resp_buf));
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
 * Print OTP configuration.
 ******************************************************************************/
static void print_otp_conf(void)
{
  printf("  + Secure boot                    : ");
  if (get_se_otp_conf_buf_ptr()->enable_secure_boot) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Secure boot verify certificate : ");
  if (get_se_otp_conf_buf_ptr()->verify_secure_boot_certificate) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Secure boot anti-rollback      : ");
  if (get_se_otp_conf_buf_ptr()->enable_anti_rollback) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Secure boot page lock narrow   : ");
  if (get_se_otp_conf_buf_ptr()->secure_boot_page_lock_narrow) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Secure boot page lock full     : ");
  if (get_se_otp_conf_buf_ptr()->secure_boot_page_lock_full) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
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

#if (_SILICON_LABS_SECURITY_FEATURE == _SILICON_LABS_SECURITY_FEATURE_VAULT)
/***************************************************************************//**
 * Print tamper configuration.
 ******************************************************************************/
static void print_tamper_conf(void)
{
  uint32_t i;

  printf("  + Tamper source level\n");
  for (i = 0; i < SL_SE_TAMPER_SIGNAL_NUM_SIGNALS; i++) {
    if (tamper_source[i] != NULL) {
      printf("    %s %d\n", tamper_source[i],
             get_se_otp_conf_buf_ptr()->tamper_levels[i]);
    }
  }

  printf("  + Reset period for the tamper filter counter: ~32 ms x %u\n",
         1 << get_se_otp_conf_buf_ptr()->tamper_filter_period);
  printf("  + Activation threshold for the tamper filter: %d\n",
         256 / (1 << get_se_otp_conf_buf_ptr()->tamper_filter_threshold));
  if (get_se_otp_conf_buf_ptr()->tamper_flags
      & SL_SE_TAMPER_FLAG_DGLITCH_ALWAYS_ON) {
    printf("  + Digital glitch detector always on: Enabled\n");
  } else {
    printf("  + Digital glitch detector always on: Disabled\n");
  }
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
  if (get_se_otp_conf_buf_ptr()->tamper_flags & (1UL << 2)) {
    printf("  + Keep tamper alive during sleep: Enabled\n");
  } else {
    printf("  + Keep tamper alive during sleep: Disabled\n");
  }
#endif
  printf("  + Tamper reset threshold: %d\n",
         get_se_otp_conf_buf_ptr()->tamper_reset_threshold);
}
#endif
