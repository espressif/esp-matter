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
 * Print host firmware upgrade and debug lock selection.
 ******************************************************************************/
static void print_selection(void);

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
static uint8_t example_string[] = "SE Manager Host Firmware Upgrade and Debug Lock Example";

/// Firmware upgrade and debug lock selection
static uint8_t upgrade_lock_select;

#if defined(CRYPTOACC_PRESENT)
/// Response buffer of executed command
static uint8_t resp_buf[RESP_BUF_SIZE];
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
#if defined(CRYPTOACC_PRESENT)
        app_state = CHECK_EXECUTED_COMMAND;
#else
        print_selection();
#endif
      } else {
        app_state = SE_MANAGER_IDLE;
      }
      break;

    case SELECT_UPGRADE_LOCK:
      if (space_press) {
        space_press = false;
        if (++upgrade_lock_select > 1) {
          upgrade_lock_select = 0;
        }
        if (upgrade_lock_select == 0) {
          printf("  + Current selection is HOST FIRMWARE UPGRADE.\n");
        } else {
          printf("  + Current selection is DEBUG LOCK.\n");
        }
      }
      if (enter_press) {
        enter_press = false;
        if (upgrade_lock_select == 0) {
          printf("\n  . The start address and size of host firmware image are "
                 "0x%08lX and %lu bytes.\n", (uint32_t)get_host_firmware_addr(),
                 get_host_firmware_size());
          app_state = GET_HOST_UPGRADE_STATUS;
        } else {
#if defined(CRYPTOACC_PRESENT)
          app_state = READ_VSE_VERSION;
#else
          app_state = READ_LOCK_STATUS;
#endif
        }
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case CHECK_EXECUTED_COMMAND:
      printf("\n  . Check executed command... ");
      if (check_executed_command() != SL_STATUS_OK) {
        print_selection();
      } else {
        switch (get_executed_command()) {
          case SLI_SE_COMMAND_STATUS_HOST_IMAGE:
            app_state = POST_HOST_UPGRADE_STATUS;
            break;

          case SLI_SE_COMMAND_CHECK_HOST_IMAGE:
            app_state = POST_VALIDATE_HOST_IMAGE;
            break;

          case SLI_SE_COMMAND_DBG_LOCK_APPLY:
            app_state = POST_APPLY_DEBUG_LOCK;
            break;

          default:
            app_state = SE_MANAGER_EXIT;
            break;
        }
      }
      break;
#endif

    case GET_HOST_UPGRADE_STATUS:
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to get previous host firmware upgrade status...  ");
      get_host_upgrade_status();
      app_state = SE_MANAGER_EXIT;
#else
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get previous host firmware upgrade status... ");
      if (get_host_upgrade_status() == SL_STATUS_OK) {
        printf("  + The status of the last host upgrade attempt (MSB..LSB): "
               "0x%08lX\n", get_status());
        printf("  + Previous host firmware upgrade version (MSB..LSB): %08lX\n",
               get_version());
        app_state = VALIDATE_HOST_IMAGE;
      }
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_HOST_UPGRADE_STATUS:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get previous host firmware upgrade status response after "
             "reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf))
          == SL_STATUS_OK) {
        printf("  + The status of the last host upgrade attempt (MSB..LSB): "
               "0x%08lX\n", *((uint32_t *)resp_buf));
        printf("  + Previous host firmware upgrade version (MSB..LSB): %08lX\n",
               *((uint32_t *)resp_buf + 1));
        app_state = VALIDATE_HOST_IMAGE;
      }
      break;
#endif

    case VALIDATE_HOST_IMAGE:
#if defined(CRYPTOACC_PRESENT)
      printf("\n  . Reset to validate the host firmware image...  ");
      validate_host_image();
      app_state = SE_MANAGER_EXIT;
#else
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Validate the host firmware image... ");
      if (validate_host_image() == SL_STATUS_OK) {
        printf("  + Press ENTER to upgrade the host firmware or press SPACE to "
               "exit.\n");
        app_state = UPGRADE_HOST_IMAGE;
      }
#endif
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_VALIDATE_HOST_IMAGE:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get the host firmware image validation response after "
             "reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf))
          == SL_STATUS_OK) {
        printf("  + Press ENTER to upgrade the host firmware or press SPACE to "
               "exit.\n");
        app_state = UPGRADE_HOST_IMAGE;
      }
      break;
#endif

    case UPGRADE_HOST_IMAGE:
      if (enter_press) {
        enter_press = false;
        printf("\n  . The updated host firmware will run if host firmware "
               "upgrade succeeds.\n");
        printf("  + Upgrade the host firmware image...  ");
        upgrade_host_image();
        // Cannot reach here if host firmware was successfully upgraded.
        app_state = SE_MANAGER_EXIT;
      }
      if (space_press) {
        space_press = false;
#if defined(CRYPTOACC_PRESENT)
        printf("  + Reset the device to restart.\n");
        app_state = SE_MANAGER_EXIT;
#else
        print_selection();
#endif
      }
      break;

    case READ_VSE_VERSION:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get current VSE firmware version... ");
      if (get_se_version() == SL_STATUS_OK) {
        printf("  + Current VSE firmware version (MSB..LSB): %08lX\n",
               get_version());
        if (get_version() < VSE_DEBUG_LCOK_VER) {
          printf("  + Current VSE firmware cannot enable debug lock through "
                 "application.\n");
          print_selection();
        } else {
          app_state = READ_LOCK_STATUS;
        }
      }
      break;

    case READ_LOCK_STATUS:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get debug lock status... ");
      if (get_debug_lock_status() == SL_STATUS_OK) {
        if (get_debug_lock_status_ptr()->debug_port_lock_applied) {
          printf("  + Debug lock was enabled.\n");
          print_selection();
        } else {
          printf("  + Debug lock: Disabled\n");
          printf("  + Press ENTER to apply debug lock or press SPACE to "
                 "exit.\n");
          app_state = APPLY_DEBUG_LOCK;
        }
      }
      break;

    case APPLY_DEBUG_LOCK:
      if (enter_press) {
        enter_press = false;
#if defined(CRYPTOACC_PRESENT)
        printf("\n  . Reset to apply the debug lock...  ");
        apply_debug_lock();
        // Cannot reach here if debug lock is successfully applied.
        app_state = SE_MANAGER_EXIT;
#else
        app_state = SE_MANAGER_EXIT;
        printf("\n  . Apply the debug lock...  ");
        if (apply_debug_lock() == SL_STATUS_OK) {
          printf("  + Get debug lock status... ");
          if (get_debug_lock_status() == SL_STATUS_OK) {
            if (get_debug_lock_status_ptr()->debug_port_lock_applied) {
              printf("  + Debug lock: Enabled\n");
            } else {
              printf("  + Debug lock: Disabled\n");
            }
            print_selection();
          }
        }
#endif
      }
      if (space_press) {
        space_press = false;
        print_selection();
      }
      break;

#if defined(CRYPTOACC_PRESENT)
    case POST_APPLY_DEBUG_LOCK:
      app_state = SE_MANAGER_EXIT;
      printf("\n  . Get the apply debug lock response after reset... ");
      if (get_executed_command_response(resp_buf, sizeof(resp_buf))
          == SL_STATUS_OK) {
        printf("  + Apply debug lock is succeeded.\n");
        printf("  + Reset the device to restart.\n");
      } else {
        printf("  + Apply debug lock is failed.\n");
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
 * Print host firmware upgrade and debug lock selection.
 ******************************************************************************/
static void print_selection(void)
{
  if (upgrade_lock_select == 0) {
    printf("\n  . Current selection is HOST FIRMWARE UPGRADE.\n");
  } else {
    printf("\n  . Current selection is DEBUG LOCK.\n");
  }
  printf("  + Press SPACE to select HOST FIRMWARE UPGRADE or DEBUG LOCK, "
         "press ENTER to run.\n");
  app_state = SELECT_UPGRADE_LOCK;
}
