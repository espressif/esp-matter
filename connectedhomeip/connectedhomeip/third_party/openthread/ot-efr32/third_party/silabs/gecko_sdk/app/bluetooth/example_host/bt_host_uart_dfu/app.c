/***************************************************************************//**
 * @file
 * @brief UART Device Firmware Update example.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include <stdlib.h>
#include <unistd.h>
#include "app.h"
#include "ncp_host.h"
#include "app_log.h"
#include "app_log_cli.h"
#include "app_assert.h"
#include "sl_bt_api.h"
#include "uart_dfu.h"

// Wait for the system boot event after a successful update.
#define WAIT_FOR_BOOT  0

// Optstring argument for getopt.
#define OPTSTRING      NCP_HOST_OPTSTRING APP_LOG_OPTSTRING "h"

// Usage info.
#define USAGE          APP_LOG_NL "%s " NCP_HOST_USAGE APP_LOG_USAGE " [-h] <gbl_file_path>" APP_LOG_NL

// Options info.
#define OPTIONS    \
  "\nOPTIONS\n"    \
  NCP_HOST_OPTIONS \
  APP_LOG_OPTIONS  \
  "    -h  Print this help message.\n"

// The advertising set handle allocated from Bluetooth stack.
static char *gbl_file = NULL;

// Flag indicating that the DFU has finished successfully.
static bool dfu_done = false;

static void dfu_progress(size_t uploaded_size, size_t total_size);

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  sl_status_t sc;
  int opt;

  // Process command line options.
  while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
    switch (opt) {
      // Print help.
      case 'h':
        app_log(USAGE, argv[0]);
        app_log(OPTIONS);
        exit(EXIT_SUCCESS);

      // Process options for other modules.
      default:
        sc = ncp_host_set_option((char)opt, optarg);
        if (sc == SL_STATUS_NOT_FOUND) {
          sc = app_log_set_option((char)opt, optarg);
        }
        if (sc != SL_STATUS_OK) {
          app_log(USAGE, argv[0]);
          exit(EXIT_FAILURE);
        }
        break;
    }
  }

  // The GBL file path is a positional argument.
  if (optind < argc) {
    gbl_file = argv[optind];
  } else {
    app_log_error("GBL file path missing." APP_LOG_NL);
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  // Initialize NCP connection.
  sc = ncp_host_init();
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }
  app_assert_status(sc);
  app_log_info("NCP host initialised." APP_LOG_NL);
  app_log_info("Reset NCP target in bootloader mode..." APP_LOG_NL);
  sl_bt_user_reset_to_dfu();
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void)
{
}

/**************************************************************************//**
 * Application Deinit.
 *****************************************************************************/
void app_deinit(void)
{
  ncp_host_deinit();
}

/**************************************************************************//**
 * Application User Interrupt.
 *****************************************************************************/
void app_interrupt(void)
{
  uart_dfu_abort();
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started in DFU mode and is ready for
    // the update.
    case sl_bt_evt_dfu_boot_id:
      app_log_info("DFU booted: v0x%08x" APP_LOG_NL,
                   evt->data.evt_dfu_boot.version);

      if (dfu_done) {
        // Bootloader started instead of the application. Prevent permanent boot
        // loop by exiting.
        app_log_error("Failed to start new application." APP_LOG_NL);
        app_deinit();
        exit(EXIT_FAILURE);
      }

      app_log_info("Pressing Crtl+C aborts the update process." APP_LOG_NL);
      app_log_info("WARNING! If the update process is aborted, the device will"
                   " stay in bootloader mode." APP_LOG_NL APP_LOG_NL);

      sc = uart_dfu_run(gbl_file, dfu_progress);
      app_log(APP_LOG_NL);
      if (sc != SL_STATUS_OK) {
        app_log_error("DFU failed." APP_LOG_NL);
      }
      // Print readable error messages for the most common error codes.
      switch (sc) {
        case SL_STATUS_IO:
          app_log_error("Please check if the file exists: %s" APP_LOG_NL,
                        gbl_file);
          break;
        case SL_STATUS_SECURITY_IMAGE_CHECKSUM_ERROR:
          app_log_error("Please check if the file has valid GBL format: %s"
                        APP_LOG_NL, gbl_file);
          break;
        case SL_STATUS_ABORT:
          app_log_error("Aborted by user.");
          break;
      }
      app_assert_status(sc);
      dfu_done = true;
      app_log_info("DFU finished successfully. Resetting the device."
                   APP_LOG_NL);
      sl_bt_system_reset(sl_bt_system_boot_mode_normal);
#if !WAIT_FOR_BOOT
      app_deinit();
      exit(EXIT_SUCCESS);
#endif
      break;

    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d" APP_LOG_NL,
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      app_deinit();
      if (dfu_done) {
        // The new application has started.
        exit(EXIT_SUCCESS);
      } else {
        app_log_error("Failed to start bootloader. Please make sure to flash"
                      " BGAPI UART DFU Bootloader on the target device."
                      APP_LOG_NL);
        exit(EXIT_FAILURE);
      }
      break;

    // -------------------------------
    // Default event handler.
    default:
      app_log_debug("Unhandled event: 0x%08x" APP_LOG_NL,
                    SL_BT_MSG_ID(evt->header));
      break;
  }
}

/***************************************************************************//**
 * Callback function to indicate the update progress.
 ******************************************************************************/
static void dfu_progress(size_t uploaded_size, size_t total_size)
{
  uint32_t percentage = 100 * uploaded_size / total_size;
  app_log("\r%zu/%zu (%d%%)", uploaded_size, total_size, percentage);
}
