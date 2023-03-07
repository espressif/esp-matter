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
 * Print tamper test.
 ******************************************************************************/
static void print_tamper_test(void);

/***************************************************************************//**
 * Print and verify tamper configuration.
 ******************************************************************************/
static void print_tamper_conf(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

/***************************************************************************//**
 * Issue a tamper disable token to the device.
 ******************************************************************************/
static void issue_tamper_disable(void);

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
static uint8_t example_string[] = "SE Manager Tamper Example";

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
  "DPLL lock fail high   : ",
  "PRS0                  : ",
  "PRS1                  : ",
  "PRS2                  : ",
  "PRS3                  : ",
  "PRS4                  : ",
  "PRS5                  : ",
  "PRS6                  : "
#endif
};

/// Tamper test selection
static uint8_t tamper_test_select;

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
        app_state = READ_RST_CAUSE;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case READ_RST_CAUSE:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Read EMU RSTCAUSE register... ");
      if (get_reset_cause() == SL_STATUS_OK) {
#if defined(_SILICON_LABS_32B_SERIES_2_CONFIG_1)
        printf("  + The EMU RSTCAUSE register (MSB..LSB): ");
        printf("%08lX\n", *get_rst_cause_buf_ptr());
        if (*get_rst_cause_buf_ptr() & EMU_RSTCAUSE_SETAMPER) {
#else
        printf("OK\n");
        printf("  + The EMU RSTCAUSE register (MSB..LSB): ");
        printf("%08lX\n", *get_rst_cause_buf_ptr());
        if (*get_rst_cause_buf_ptr() & 0x00002000) {
#endif
          printf("  + The tamper reset is observed\n");
        }
        app_state = GET_OTP_CONFIG;
      }
      break;

    case GET_OTP_CONFIG:
      printf("\n  . Read SE OTP configuration... ");
      if (get_se_otp_conf() == SL_STATUS_OK) {
        print_tamper_conf();
      } else {
        printf("  + Cannot read SE OTP configuration.\n");
        printf("  + Press ENTER to initialize SE OTP for tamper configuration "
               "or press SPACE to abort.\n");
        app_state = WAIT_KEY_SELECT;
      }
      break;

    case WAIT_KEY_SELECT:
      if (enter_press) {
        enter_press = false;
        printf("  + Warning: The OTP configuration cannot be changed once "
               "written!\n");
        printf("  + Press ENTER to confirm or press SPACE to abort if you are "
               "not sure.\n");
        app_state = WAIT_KEY_CONFIRM;
        break;
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case WAIT_KEY_CONFIRM:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Initialize SE OTP for tamper configuration... ");
        if (init_se_otp_conf() == SL_STATUS_OK) {
          printf("  + Issue a power-on or pin reset to activate the new tamper "
                 "configuration.\n");
          break;
        }
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case SELECT_TAMPER_TEST:
      if (space_press) {
        space_press = false;
        tamper_test_select++;
        if (tamper_test_select > 1) {
          tamper_test_select = 0;
        }
        if (tamper_test_select == 0) {
          printf("  + Current tamper test is NORMAL.\n");
        } else {
          printf("  + Current tamper test is TAMPER DISABLE.\n");
        }
      }
      if (enter_press) {
        enter_press = false;
        if (tamper_test_select == 0) {
          // Print normal tamper test instructions
          printf("\n  . Normal tamper test instructions:\n");
          printf("  + Press PB0 to increase filter counter and tamper status "
                 "is displayed.\n");
          printf("  + PRS will issue a tamper reset if filter counter reaches "
                 "%d within ~32 ms x %u.\n",
                 256 / (1 << get_se_otp_conf_buf_ptr()->tamper_filter_threshold),
                 1 << get_se_otp_conf_buf_ptr()->tamper_filter_period);
          printf("  + Press PB1 to issue a tamper reset.\n");
          if (get_se_otp_conf_buf_ptr()->tamper_reset_threshold == 0) {
            printf("  + Device will never enter diagnostic mode\n");
          } else {
            printf("  + Device will enter diagnostic mode if tamper reset "
                   "reaches %d.\n",
                   get_se_otp_conf_buf_ptr()->tamper_reset_threshold);
          }
          enable_tamper_int();
          init_tamper_prs();
          app_state = WAIT_TAMPER_SIGNAL;
        } else {
          app_state = CHECK_COMMAND_KEY;
        }
      }
      break;

    case WAIT_TAMPER_SIGNAL:
      if (get_tamper_int_status()) {
        printf("\n  . Get tamper status... ");
        if (get_se_status() == SL_STATUS_OK) {
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
          // SE manager disables the SE mailbox clock for SETAMPERHOST interrupt
          CMU_ClockEnable(cmuClock_SEMAILBOX, true);
#endif
          printf("  + Recorded tamper status (MSB..LSB): ");
          printf("%08lX\n", get_se_status_buf_ptr()->tamper_status);
          printf("  + Currently active tamper sources (MSB..LSB): ");
          printf("%08lX\n", get_se_status_buf_ptr()->tamper_status_raw);

          // Check if tamper filter interrupt is triggered
          if (get_se_status_buf_ptr()->tamper_status & 0x00000002) {
            printf("  + Tamper filter threshold is reached, issue a reset "
                   "through PRS\n");
            printf("  ");
            // Issue a tamper reset through PRS
            PRS_PulseTrigger(0x01 << SW_RST_TAMPER_PRS_CH);
          }
        } else {
          app_state = SE_MANAGER_EXIT;
          break;
        }
      }
      if (tamper_test_select == 1) {
        // Tamper disable test
        app_state = ROLL_CHALLENGE;
      }
      break;

    case CHECK_COMMAND_KEY:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Verify the device public command key in SE OTP.\n");
      printf("  + Exporting a public command key from a hard-coded private "
             "command key... ");
      if (export_public_command_key() != SL_STATUS_OK) {
        break;
      }
      printf("  + Reading the public command key from SE OTP... ");
      if (read_public_command_key() != SL_STATUS_OK) {
        printf("  + Press ENTER to program public command key in SE OTP or "
               "press SPACE to abort.\n");
        app_state = PROGRAM_COMMAND_KEY;
      } else {
        printf("  + Comparing exported public command key with SE OTP public "
               "command key... ");
        if (compare_public_command_key() != 0) {
          printf("Failed\n");
          print_tamper_test();
        } else {
          printf("OK\n");
          printf("  + Press ENTER to disable tamper signals or press SPACE to "
                 "exit.\n");
          app_state = DISABLE_TAMPER_SIGNAL;
        }
      }
      break;

    case PROGRAM_COMMAND_KEY:
      if (enter_press) {
        enter_press = false;
        // Option to provision public command key
        printf("  + Warning: The public command key in SE OTP cannot be "
               "changed once written!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = CONFIRM_PROGRAM_COMMAND_KEY;
        break;
      }
      if (space_press) {
        space_press = false;
        print_tamper_test();
      }
      break;

    case CONFIRM_PROGRAM_COMMAND_KEY:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Programming a public command key to SE OTP... ");
        if (program_public_command_key() == SL_STATUS_OK) {
          printf("  + Press ENTER to disable tamper signals or press SPACE to "
                 "exit.\n");
          app_state = DISABLE_TAMPER_SIGNAL;
        }
        break;
      }
      if (space_press) {
        space_press = false;
        print_tamper_test();
      }
      break;

    case DISABLE_TAMPER_SIGNAL:
      if (enter_press) {
        enter_press = false;
        issue_tamper_disable();
        break;
      }
      if (space_press) {
        space_press = false;
        print_tamper_test();
      }
      break;

    case ROLL_CHALLENGE:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("\n  . Check and roll the challenge.\n");
        printf("  + Request current challenge from the SE... ");
        if (request_challenge() != SL_STATUS_OK) {
          break;
        }
        printf("  + The current challenge (16 bytes): ");
        print_buf(get_challenge_buf_ptr(), 16);

        printf("  + Rolling the challenge... ");
        if (roll_challenge() == SL_STATUS_OK) {
          printf("  + Request rolled challenge from the SE... ");
          if (request_challenge() != SL_STATUS_OK) {
            break;
          }
          printf("  + The rolled challenge (16 bytes): ");
          print_buf(get_challenge_buf_ptr(), 16);
          printf("  + Issue a power-on or pin reset to activate the rolled "
                 "challenge.\n");
          break;
        }
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
        break;
      }
      // Wait tamper signal if no key press
      app_state = WAIT_TAMPER_SIGNAL;
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
 * Print tamper test.
 ******************************************************************************/
static void print_tamper_test(void)
{
  if (tamper_test_select == 0) {
    printf("\n  . Current tamper test is NORMAL.\n");
  } else {
    printf("\n  . Current tamper test is TAMPER DISABLE.\n");
  }
  printf("  + Press SPACE to select NORMAL or TAMPER DISABLE, press ENTER to "
         "run.\n");
  app_state = SELECT_TAMPER_TEST;
}

/***************************************************************************//**
 * Print and verify tamper configuration.
 ******************************************************************************/
static void print_tamper_conf(void)
{
  uint32_t i;
  sl_se_otp_init_t *conf = get_se_otp_conf_buf_ptr();

  // Print tamper configuration
  printf("  + Secure boot: ");
  if (conf->enable_secure_boot) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }

  printf("  + Tamper source level\n");
  for (i = 0; i < SL_SE_TAMPER_SIGNAL_NUM_SIGNALS; i++) {
    if (tamper_source[i] != NULL) {
      printf("    %s %d\n", tamper_source[i], conf->tamper_levels[i]);
    }
  }

  printf("  + Reset period for the tamper filter counter: ~32 ms x %u\n",
         1 << conf->tamper_filter_period);
  printf("  + Activation threshold for the tamper filter: %d\n",
         256 / (1 << conf->tamper_filter_threshold));
  if (conf->tamper_flags & SL_SE_TAMPER_FLAG_DGLITCH_ALWAYS_ON) {
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
  printf("  + Tamper reset threshold: %d\n", conf->tamper_reset_threshold);

  // Check tamper configuration can run on this example or not
  if (conf->tamper_levels[SL_SE_TAMPER_SIGNAL_FILTER_COUNTER] == SL_SE_TAMPER_LEVEL_INTERRUPT
      && conf->tamper_levels[SL_SE_TAMPER_SIGNAL_PRS0] == SL_SE_TAMPER_LEVEL_INTERRUPT
      && conf->tamper_levels[SL_SE_TAMPER_SIGNAL_PRS1] == SL_SE_TAMPER_LEVEL_INTERRUPT
      && conf->tamper_levels[SL_SE_TAMPER_SIGNAL_PRS2] == SL_SE_TAMPER_LEVEL_FILTER
      && conf->tamper_levels[SL_SE_TAMPER_SIGNAL_PRS4] == SL_SE_TAMPER_LEVEL_RESET
      && conf->tamper_levels[SL_SE_TAMPER_SIGNAL_PRS5] == SL_SE_TAMPER_LEVEL_RESET
      && conf->tamper_filter_period == SL_SE_TAMPER_FILTER_PERIOD_33S
      && conf->tamper_filter_threshold >= SL_SE_TAMPER_FILTER_THRESHOLD_8
      && conf->tamper_reset_threshold <= 8) {
    print_tamper_test();
  } else {
    printf("  + The tamper configuration does not match with this example.\n");
    app_state = SE_MANAGER_EXIT;
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

/***************************************************************************//**
 * Issue a tamper disable token to the device.
 ******************************************************************************/
static void issue_tamper_disable(void)
{
  app_state = SE_MANAGER_EXIT;

  printf("\n  . Start the tamper disable processes.\n");
  printf("  + Creating a private certificate key in a buffer... ");
  if (create_private_certificate_key() != SL_STATUS_OK) {
    return;
  }

  printf("  + Exporting a public certificate key from a private certificate "
         "key... ");
  if (export_public_certificate_key() != SL_STATUS_OK) {
    return;
  }

  printf("  + Read the serial number of the SE and save it to access "
         "certificate... ");
  if (read_serial_number() != SL_STATUS_OK) {
    return;
  }

  printf("  + Signing the access certificate with private command key... ");
  if (sign_access_certificate() != SL_STATUS_OK) {
    return;
  }

  printf("  + Request challenge from the SE and save it to challenge "
         "response... ");
  if (request_challenge() != SL_STATUS_OK) {
    return;
  }

  printf("  + Signing the challenge response with private certificate key... ");
  if (sign_challenge_response() != SL_STATUS_OK) {
    return;
  }

  printf("  + Creating a tamper disable token to disable tamper signals... ");
  if (create_tamper_disable_token() == SL_STATUS_OK) {
    printf("  + Success to disable the tamper signals!\n");
    // Print tamper disable test instructions
    printf("\n  . Tamper disable test instructions:\n");
    printf("  + Press PB0 to increase filter counter and tamper status "
           "is displayed.\n");
    printf("  + PRS will NOT issue a tamper reset even filter counter reaches "
           "%d within ~32 ms x %u.\n",
           256 / (1 << get_se_otp_conf_buf_ptr()->tamper_filter_threshold),
           1 << get_se_otp_conf_buf_ptr()->tamper_filter_period);
    printf("  + Press PB1 will NOT issue a tamper reset.\n");
    printf("  + Issue a power-on or pin reset to re-enable the tamper "
           "signals.\n");
    printf("  + Press ENTER to roll the challenge to invalidate the current "
           "tamper disable token or press SPACE to exit.\n");
    enable_tamper_int();
    init_tamper_prs();
    app_state = WAIT_TAMPER_SIGNAL;
  } else {
    printf("  + Fail to disable the tamper signals!\n");
  }
}
