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
 * Print SE status.
 ******************************************************************************/
static void print_se_status(void);

/***************************************************************************//**
 * Check SE status.
 ******************************************************************************/
static void check_se_status(void);

/***************************************************************************//**
 * Print buffer data in ASCII hex.
 *
 * @param buf Pointer to the binary buffer.
 * @param len Number of bytes to print.
 ******************************************************************************/
static void print_buf(uint8_t *buf, size_t len);

/***************************************************************************//**
 * Issue a secure debug unlock to the device.
 ******************************************************************************/
static void issue_secure_debug_unlock(void);

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
static uint8_t example_string[] = "SE Manager Secure Debug Example (TrustZone)";

/// Secure debug is active
static bool secure_debug_active;

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
        app_state = GET_SE_STATUS;
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case GET_SE_STATUS:
      printf("\n  . Get SE status... ");
      if (get_se_status() == SL_STATUS_OK) {
        print_se_status();
      } else {
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case CHECK_SE_STATUS:
      check_se_status();
      break;

    case STANDARD_DEBUG_UNLOCK:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Performing a device mass erase and debug unlock... ");
        disable_debug_lock();
        // Cannot reach here if debug unlock was successfully executed.
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case NORMAL_SECURE_DEBUG_ENABLE:
      app_state = SE_MANAGER_EXIT;
      printf("  + Press ENTER to lock the device or press SPACE to exit.\n");
      app_state = ENABLE_DEBUG_LOCK;
      break;

    case NORMAL_SECURE_DEBUG_DISABLE:
      app_state = SE_MANAGER_EXIT;
      printf("  + Reading the public command key from SE OTP... ");
      if (read_public_command_key() != SL_STATUS_OK) {
        break;
      }
      printf("  + The current public command key (64 bytes): ");
      print_buf(get_pubcmdkey_buf_ptr(), 64);

      printf("  + Request current challenge from the SE... ");
      if (request_challenge() != SL_STATUS_OK) {
        break;
      }
      printf("  + The current challenge (16 bytes): ");
      print_buf(get_challenge_buf_ptr(), 16);

      printf("  + Press ENTER to enable secure debug or press SPACE to "
             "exit.\n");
      app_state = ENABLE_SECURE_DEBUG_LOCK;
      break;

    case SECURE_DEBUG_LOCK:
      if (enter_press) {
        enter_press = false;
        app_state = ISSUE_SECURE_DEBUG_UNLOCK;
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case SECURE_DEBUG_UNLOCK:
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
        }
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case ENABLE_SECURE_DEBUG_LOCK:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Enable the secure debug... ");
        if (enable_secure_debug() != SL_STATUS_OK) {
          break;
        }
        printf("  + Press ENTER to lock the device or press SPACE to disable "
               "the secure debug and exit.\n");
        secure_debug_active = true;
        app_state = ENABLE_DEBUG_LOCK;
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
      }
      break;

    case ENABLE_DEBUG_LOCK:
      if (enter_press) {
        enter_press = false;
        app_state = SE_MANAGER_EXIT;
        printf("  + Locking the device... ");
        if (enable_debug_lock() == SL_STATUS_OK) {
          if (get_se_status_buf_ptr()->debug_status.device_erase_enabled) {
            printf("  + Device erase is enabled, press ENTER to disable device "
                   "erase (optional if just for testing) or press SPACE to "
                   "skip.\n");
            app_state = DISABLE_DEVICE_ERASE;
          } else {
            // Device erase had already disabled
            app_state = GET_SE_STATUS;
          }
        }
      }
      if (space_press) {
        space_press = false;
        app_state = SE_MANAGER_EXIT;
        if (secure_debug_active) {
          secure_debug_active = false;
          printf("  + Disable the secure debug... ");
          disable_secure_debug();
        }
      }
      break;

    case DISABLE_DEVICE_ERASE:
      if (enter_press) {
        enter_press = false;
        // Option to disable device erase
        printf("  + Warning: This is a ONE-TIME command which PERMANETLY "
               "disables device erase!\n");
        printf("  + Press ENTER to confirm or press SPACE to skip if you are "
               "not sure.\n");
        app_state = CONFIRM_DISABLE_DEVICE_ERASE;
      }
      if (space_press) {
        space_press = false;
        app_state = GET_SE_STATUS;
      }
      break;

    case CONFIRM_DISABLE_DEVICE_ERASE:
      if (enter_press) {
        enter_press = false;
        printf("  + Disable device erase... ");
        if (disable_device_erase() == SL_STATUS_OK) {
          app_state = GET_SE_STATUS;
        } else {
          app_state = SE_MANAGER_EXIT;
          break;
        }
      }
      if (space_press) {
        space_press = false;
        app_state = GET_SE_STATUS;
      }
      break;

    case ISSUE_SECURE_DEBUG_UNLOCK:
      issue_secure_debug_unlock();
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
 * Print SE status.
 ******************************************************************************/
static void print_se_status(void)
{
  printf("  + The SE firmware version (MSB..LSB): ");
  printf("%08lX\n", get_se_status_buf_ptr()->se_fw_version);
  printf("  + Debug lock: ");
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Debug lock state: ");
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_state) {
    printf("True\n");
  } else {
    printf("False\n");
  }
  printf("  + Device Erase: ");
  if (get_se_status_buf_ptr()->debug_status.device_erase_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Secure debug: ");
  if (get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  printf("  + Secure boot: ");
  if (get_se_status_buf_ptr()->secure_boot_enabled) {
    printf("Enabled\n");
  } else {
    printf("Disabled\n");
  }
  app_state = CHECK_SE_STATUS;
}

/***************************************************************************//**
 * Check SE status.
 ******************************************************************************/
static void check_se_status(void)
{
  // Standard debug lock
  if (get_se_status_buf_ptr()->debug_status.device_erase_enabled
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in standard debug lock state, the secure "
           "debug unlock cannot apply to this part.\n");
    printf("  + Press ENTER to unlock the device or press SPACE to exit.\n");
    app_state = STANDARD_DEBUG_UNLOCK;
    return;
  }

  // Permanent debug lock
  if (!get_se_status_buf_ptr()->debug_status.device_erase_enabled
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in permanent debug lock state, this part "
           "cannot be unlocked.\n");
    app_state = SE_MANAGER_EXIT;
    return;
  }

  // Normal state with secure debug enable
  if (!get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in normal state and secure debug is "
           "enabled.\n");
    app_state = NORMAL_SECURE_DEBUG_ENABLE;
    return;
  }

  // Normal state with secure debug disable
  if (!get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && !get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in normal state and secure debug is "
           "disabled.\n");
    app_state = NORMAL_SECURE_DEBUG_DISABLE;
    return;
  }

  // Secure debug lock
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in secure debug lock state.\n");
    printf("  + Press ENTER to issue a secure debug unlock or press SPACE to "
           "exit.\n");
    app_state = SECURE_DEBUG_LOCK;
    return;
  }

  // Secure debug unlock
  if (get_se_status_buf_ptr()->debug_status.debug_port_lock_applied
      && !get_se_status_buf_ptr()->debug_status.debug_port_lock_state
      && get_se_status_buf_ptr()->debug_status.secure_debug_enabled) {
    printf("\n  . The device is in secure debug unlock state.\n");
    printf("  + Issue a power-on or pin reset to re-enable the secure debug "
           "lock.\n");
    printf("  + Press ENTER to roll the challenge to invalidate the current "
           "unlock token or press SPACE to exit.\n");
    app_state = SECURE_DEBUG_UNLOCK;
    return;
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
 * Issue a secure debug unlock to the device.
 ******************************************************************************/
static void issue_secure_debug_unlock(void)
{
  app_state = SE_MANAGER_EXIT;

  printf("  + Read the serial number from the SE... ");
  if (read_serial_number() != SL_STATUS_OK) {
    return;
  }

  printf("  + The device serial number (16 bytes): ");
  print_buf(get_serialno_buf_ptr(), 16);

  printf("  + Setting the debug options... ");
  if (set_debug_option() != SL_STATUS_OK) {
    return;
  }

  printf("  + Use an unlock token to unlock the device... ");
  if (use_unlock_token() != SL_STATUS_OK) {
    return;
  }

  printf("  + Get debug status to verify the device is unlocked... ");
  if (get_debug_lock_status() != SL_STATUS_OK) {
    return;
  }

  if (!get_debug_lock_status_buf_ptr()->debug_port_lock_state) {
    printf("  + Success to unlock the device!\n");
    app_state = GET_SE_STATUS;
  } else {
    printf("  + Fail to unlock the device!\n");
  }
}
