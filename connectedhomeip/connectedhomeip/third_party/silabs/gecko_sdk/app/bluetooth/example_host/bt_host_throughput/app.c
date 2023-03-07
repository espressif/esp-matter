/***************************************************************************//**
 * @file
 * @brief Throughput test application.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "app.h"
#include "app_log.h"
#include "app_log_cli.h"
#include "throughput_central.h"
#include "throughput_ui_types.h"
#include "throughput_types.h"
#include "cf_parse.h"
#include "sl_bt_api.h"
#include "app_assert.h"
#include "throughput_central_interface.h"
#include "ncp_host.h"

// Optstring argument for getopt.
#define OPTSTRING NCP_HOST_OPTSTRING APP_LOG_OPTSTRING "T:D:p:i:M:N:c:o:h"

// Usage info.
#define USAGE APP_LOG_NL "%s " NCP_HOST_USAGE APP_LOG_USAGE " [-T <time> | -D <data_length>] [-p <phy>] [-i <interval>] [-M <mtu>] [-N <type>] [-c <config>] [-o <log>] [-h]" APP_LOG_NL

// Options info.
#define OPTIONS                                                                            \
  "\nOPTIONS\n"                                                                            \
  NCP_HOST_OPTIONS                                                                         \
  APP_LOG_OPTIONS                                                                          \
  "    -T  Select fixed time mode.\n"                                                      \
  "        <time>           Measurement duration in seconds\n"                             \
  "    -D  Select fixed data length mode.\n"                                               \
  "        <data_length>    Measurement data length in bytes\n"                            \
  "    -p  Select PHY type.\n"                                                             \
  "        <phy>            1 (1M, default), 2 (2M), 4 (Coded, 125k) or 8 (Coded, 500k)\n" \
  "    -i  Set connection interval.\n"                                                     \
  "        <interval>       Connection interval (in 1.25 ms steps)\n"                      \
  "    -M  Set Maximum Transmission Unit (MTU) size.\n"                                    \
  "        <mtu>            Size of the MTU in bytes\n"                                    \
  "    -N  Select test type.\n"                                                            \
  "        <type>           1 (notification) or 2 (indication)\n"                          \
  "    -c  Configuration file.\n"                                                          \
  "        <config>         Path to the configuration file\n"                              \
  "    -o  Log file.\n"                                                                    \
  "        <log>            Path to the log file\n"                                        \
  "    -h  Print this help message.\n"

#define MAX_LOG_LINE_LEN              1024
#define LOG_INTERVAL                  60.0

// These constants are used for input validation
#define FIXED_DATA_SIZE_MIN 1000
#define FIXED_DATA_SIZE_MAX 10000000
#define FIXED_TIME_MIN 1
#define FIXED_TIME_MAX 600
#define CONNECTION_INTERVAL_MIN 6
#define CONNECTION_INTERVAL_MAX 3200
#define MTU_SIZE_MIN 23
#define MTU_SIZE_MAX 250

typedef struct {
  uint16_t connection_interval;
  sl_bt_gap_phy_coding_t phy;
  uint16_t mtu_size;
  sl_bt_gatt_client_config_flag_t test_type;
  throughput_mode_t mode;
  uint32_t fixed_amount;
} test_parameters_t;

static sl_status_t open_log_file(char *filename);
static void log_step(void);
static sl_status_t write_log_entry(float time, throughput_value_t throughput);

// Set default parameters
static test_parameters_t test_parameters = {
  .connection_interval = THROUGHPUT_CENTRAL_CONNECTION_INTERVAL_MIN,
  .phy = THROUGHPUT_DEFAULT_PHY,
  .mtu_size = THROUGHPUT_CENTRAL_MTU_SIZE,
  .test_type = THROUGHPUT_CENTRAL_TEST_TYPE,
  .mode = THROUGHPUT_CENTRAL_MODE_DEFAULT,
  .fixed_amount = THROUGHPUT_CENTRAL_FIXED_TIME
};

static int logfile_descriptor = -1;
static float last_log_time;
static bool log_enabled = false;

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(int argc, char *argv[])
{
  int opt;
  sl_status_t sc;
  char *configuration = NULL;

  last_log_time = timer_end();

  // Parse command line arguments
  while ((opt = getopt(argc, argv, OPTSTRING)) != -1) {
    switch (opt) {
      // Fixed time
      case 'T':
        test_parameters.mode = THROUGHPUT_MODE_FIXED_TIME;
        test_parameters.fixed_amount = (uint32_t)strtoul(optarg, NULL, 0);
        // Validate input value
        if (test_parameters.fixed_amount < FIXED_TIME_MIN
            || test_parameters.fixed_amount > FIXED_TIME_MAX) {
          app_log_critical("Fixed time must be between %u and %u." APP_LOG_NL,
                           FIXED_TIME_MIN,
                           FIXED_TIME_MAX);
          exit(EXIT_FAILURE);
        }
        break;

      // Fixed data length
      case 'D':
        test_parameters.mode = THROUGHPUT_MODE_FIXED_LENGTH;
        test_parameters.fixed_amount = (uint32_t)strtoul(optarg, NULL, 0);
        // Validate input value
        if (test_parameters.fixed_amount < FIXED_DATA_SIZE_MIN
            || test_parameters.fixed_amount > FIXED_DATA_SIZE_MAX) {
          app_log_critical("Fixed data must be between %u and %u." APP_LOG_NL,
                           FIXED_DATA_SIZE_MIN,
                           FIXED_DATA_SIZE_MAX);
          exit(EXIT_FAILURE);
        }
        break;

      // PHY to use
      case 'p':
        test_parameters.phy = (sl_bt_gap_phy_coding_t)strtoul(optarg,
                                                              NULL,
                                                              0);
        // Validate input value
        if (test_parameters.phy != sl_bt_gap_phy_coding_1m_uncoded
            && test_parameters.phy != sl_bt_gap_phy_coding_2m_uncoded
            && test_parameters.phy != sl_bt_gap_phy_coding_125k_coded
            && test_parameters.phy != sl_bt_gap_phy_coding_500k_coded) {
          app_log_critical("PHY must be one of these: 1 => 1M, 2 => 2M, "
                           "4 => 125k, 8 => 500k" APP_LOG_NL);
          exit(EXIT_FAILURE);
        }
        break;

      // Connection interval
      case 'i':
        test_parameters.connection_interval = (uint16_t)strtoul(optarg,
                                                                NULL,
                                                                0);
        // Validate input value
        if (test_parameters.connection_interval < CONNECTION_INTERVAL_MIN
            || test_parameters.connection_interval > CONNECTION_INTERVAL_MAX) {
          app_log_critical("Connection interval must be between %u and %u."
                           APP_LOG_NL,
                           CONNECTION_INTERVAL_MIN,
                           CONNECTION_INTERVAL_MAX);
          exit(EXIT_FAILURE);
        }
        break;

      // MTU size
      case 'M':
        test_parameters.mtu_size = (uint16_t)strtoul(optarg, NULL, 0);
        // Validate input value
        if (test_parameters.mtu_size < MTU_SIZE_MIN
            || test_parameters.mtu_size > MTU_SIZE_MAX) {
          app_log_critical("MTU should be between %u and %u." APP_LOG_NL,
                           MTU_SIZE_MIN,
                           MTU_SIZE_MAX);
          exit(EXIT_FAILURE);
        }
        break;

      // Test type: notification or indication
      case 'N':
        test_parameters.test_type = (sl_bt_gatt_client_config_flag_t)strtoul(
          optarg,
          NULL,
          0);
        // Validate input value
        if (test_parameters.test_type != sl_bt_gatt_notification
            && test_parameters.test_type != sl_bt_gatt_indication) {
          app_log_critical("Test type must be one of these: 1 => notification,"
                           " 2 => indication" APP_LOG_NL);
          exit(EXIT_FAILURE);
        }
        break;

      // Parse config file
      case 'c':
        app_log_info("Parsing config file %s..." APP_LOG_NL, optarg);
        configuration = cf_parse_load_file(optarg);
        if (configuration == NULL) {
          app_log_critical("Failed to open config file!" APP_LOG_NL);
          exit(EXIT_FAILURE);
        }
        break;

      // Log data to file
      case 'o':
        app_log_info("Opening log file %s..." APP_LOG_NL, optarg);
        sc = open_log_file(optarg);
        if (sc != SL_STATUS_OK) {
          app_log_critical("Failed to open log file!" APP_LOG_NL);
          exit(EXIT_FAILURE);
        }
        break;

      // Help!
      case 'h':
        app_log(USAGE, argv[0]);
        app_log(OPTIONS);
        exit(EXIT_SUCCESS);
        break;

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

  // Initialize NCP connection.
  sc = ncp_host_init();
  if (sc == SL_STATUS_INVALID_PARAMETER) {
    app_log(USAGE, argv[0]);
    exit(EXIT_FAILURE);
  }

  if (configuration != NULL) {
    uint8_t address[6];
    uint8_t address_type;

    cf_parse_init(configuration);
    while (cf_parse_allowlist(address, &address_type) == SL_STATUS_OK) {
      throughput_central_allowlist_add(address);
    }
    cf_parse_deinit();
  } else {
    app_log_info("No configuration." APP_LOG_NL);
  }

  app_log_info("Resetting NCP..." APP_LOG_NL);
  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(sl_bt_system_boot_mode_normal);
}

/**************************************************************************//**
 * Bluetooth event handler
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;
  bd_addr address;
  uint8_t address_type;

  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      // Print boot message.
      app_log_info("Bluetooth stack booted: v%d.%d.%d-b%d" APP_LOG_NL,
                   evt->data.evt_system_boot.major,
                   evt->data.evt_system_boot.minor,
                   evt->data.evt_system_boot.patch,
                   evt->data.evt_system_boot.build);
      // Extract unique ID from BT Address.
      sc = sl_bt_system_get_identity_address(&address, &address_type);
      app_assert_status(sc);
      app_log_info("Bluetooth %s address: %02X:%02X:%02X:%02X:%02X:%02X" APP_LOG_NL,
                   address_type ? "static random" : "public device",
                   address.addr[5],
                   address.addr[4],
                   address.addr[3],
                   address.addr[2],
                   address.addr[1],
                   address.addr[0]);

      // Enable the Central component
      throughput_central_enable();

      // Configure the component
      sc = throughput_central_set_mode(test_parameters.mode, test_parameters.fixed_amount);
      app_assert_status(sc);
      sc = throughput_central_set_mtu_size(test_parameters.mtu_size);
      app_assert_status(sc);
      sc = throughput_central_set_type(test_parameters.test_type);
      app_assert_status(sc);
      break;

    default:
      break;
  }
}

/**************************************************************************//**
 * Application step functions
 *****************************************************************************/
void app_process_action(void)
{
  throughput_central_step();
  timer_step_rssi();
  log_step();
}

/**************************************************************************//**
 * State change callback implementation
 *****************************************************************************/
void throughput_central_on_state_change(throughput_state_t state)
{
  sl_status_t sc;
  static bool have_work = true;
  log_enabled = false;

  #ifdef SL_CATALOG_THROUGHPUT_UI_PRESENT
  throughput_ui_set_state(state);
  throughput_ui_update();
  #else
  switch (state) {
    case THROUGHPUT_STATE_CONNECTED:
      app_log_info(THROUGHPUT_UI_STATE_CONNECTED_TEXT);
      sc = throughput_central_set_connection_parameters(
        test_parameters.connection_interval,
        test_parameters.connection_interval,
        THROUGHPUT_CENTRAL_CONNECTION_RESPONDER_LATENCY,
        THROUGHPUT_CENTRAL_CONNECTION_TIMEOUT);
      app_assert_status(sc);
      sc = throughput_central_set_connection_phy(test_parameters.phy);
      app_assert(sc == SL_STATUS_OK || sc == SL_STATUS_INVALID_PARAMETER,
                 "Unexpected error while setting connection PHY!");
      if (sc == SL_STATUS_INVALID_PARAMETER) {
        app_log_nl();
        app_log_critical("PHY not supported: %u." APP_LOG_NL,
                         (unsigned int)test_parameters.phy);
        exit(EXIT_FAILURE);
      }
      break;
    case THROUGHPUT_STATE_DISCONNECTED:
      app_log_info(THROUGHPUT_UI_STATE_DISCONNECTED_TEXT);
      break;
    case THROUGHPUT_STATE_SUBSCRIBED:
      app_log_info(THROUGHPUT_UI_STATE_SUBSCRIBED_TEXT);
      if (test_parameters.mode == THROUGHPUT_MODE_FIXED_TIME
          || test_parameters.mode == THROUGHPUT_MODE_FIXED_LENGTH) {
        if (have_work) {
          app_log_nl();
          throughput_central_start();
          have_work = false;
        } else {
          //This is the normal exit point of the program
          exit(EXIT_SUCCESS);
        }
      }
      break;
    case THROUGHPUT_STATE_TEST:
      app_log_info(THROUGHPUT_UI_STATE_TEST_TEXT);
      if (logfile_descriptor > 0) {
        log_enabled = true;
      }
      break;
    default:
      app_log_info(THROUGHPUT_UI_STATE_UNKNOWN_TEXT);
      break;
  }
  app_log_nl();
  #endif
}

/**************************************************************************//**
 * Open data log file
 *****************************************************************************/
static sl_status_t open_log_file(char *filename)
{
  sl_status_t ret_val = SL_STATUS_OK;

  if (open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644) == -1) {
    ret_val = SL_STATUS_FAIL;
  }

  return ret_val;
}

/**************************************************************************//**
 * Write log record to the log file
 *****************************************************************************/
static sl_status_t write_log_entry(float time, throughput_value_t throughput)
{
  sl_status_t ret_val = SL_STATUS_OK;
  static bool first_call = true;
  char line[MAX_LOG_LINE_LEN];
  static uint64_t line_id = 0;
  int ret = 0;

  if (logfile_descriptor == -1) {
    ret_val = SL_STATUS_NOT_INITIALIZED;
  } else {
    if (first_call == true) {
      ret = snprintf(line, MAX_LOG_LINE_LEN, "ID, Time [s], Throughput [bps]\n\n");
      if (ret > 0) {
        ret = write(logfile_descriptor, line, ret);
        first_call = false;
      } else {
        ret_val = SL_STATUS_NOT_INITIALIZED;
      }
    }
    ret = snprintf(line, MAX_LOG_LINE_LEN, "%llu, %f, %u\n",
                   line_id, time, throughput);
    if (ret > 0) {
      ret = write(logfile_descriptor, line, ret);
      line_id++;
    } else {
      ret_val = SL_STATUS_NOT_INITIALIZED;
    }
  }

  if (ret < 0) {
    ret_val = SL_STATUS_NOT_INITIALIZED;
  }

  return ret_val;
}

/**************************************************************************//**
 * Check if we have to log
 *****************************************************************************/
static void log_step(void)
{
  float now;
  throughput_value_t throughput;

  if (log_enabled) {
    now = throughput_central_calculate(&throughput);

    if (now < last_log_time) {
      last_log_time = now;
    }

    if (now - last_log_time >= LOG_INTERVAL) {
      write_log_entry(now, throughput);
      last_log_time = now;
    }
  }
}

/**************************************************************************//**
 * Application deinit
 *****************************************************************************/
void app_deinit(void)
{
  app_log_info("Shutting down." APP_LOG_NL);
  ncp_host_deinit();
  if (logfile_descriptor > 0) {
    close(logfile_descriptor);
  }
}
