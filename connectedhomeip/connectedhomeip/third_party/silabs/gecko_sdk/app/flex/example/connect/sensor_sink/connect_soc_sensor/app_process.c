/***************************************************************************//**
 * @file
 * @brief app_process.c
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "hal/hal.h"
#include "em_chip.h"
#include "app_log.h"
#include "sl_si70xx.h"
#include "sl_i2cspm_instances.h"
#include "poll.h"
#include "sl_app_common.h"
#include "app_process.h"
#include "app_framework_common.h"
#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_simple_led_instances.h"
#endif
#include "sl_simple_button_instances.h"
#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "sl_component_catalog.h"
#include "sl_power_manager.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
#define MAX_TX_FAILURES     (10U)
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Global flag set by a button push to allow or disallow entering to sleep
bool enable_sleep = false;
/// report timing event control
EmberEventControl *report_control;
/// report timing period
uint16_t sensor_report_period_ms =  (1 * MILLISECOND_TICKS_PER_SECOND);
/// TX options set up for the network
EmberMessageOptions tx_options = EMBER_OPTIONS_ACK_REQUESTED | EMBER_OPTIONS_SECURITY_ENABLED;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Destination of the currently processed sink node
static EmberNodeId sink_node_id = EMBER_COORDINATOR_ADDRESS;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    enable_sleep = !enable_sleep;
#if defined(SL_CATALOG_KERNEL_PRESENT)
    if (enable_sleep) {
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
    } else {
      sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM2);
      sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
    }
#endif
  }
}

/**************************************************************************//**
 * Here we print out the first two bytes reported by the sinks as a little
 * endian 16-bits decimal.
 *****************************************************************************/
void report_handler(void)
{
  if (!emberStackIsUp()) {
    emberEventControlSetInactive(*report_control);
  } else {
    EmberStatus status;
    EmberStatus sensor_status = EMBER_SUCCESS;
    uint8_t buffer[SL_SENSOR_SINK_DATA_LENGTH];
    int32_t temp_data = 0;
    uint32_t rh_data = 0;

    // Sample temperature and humidity from sensors.
    // Temperature is sampled in "millicelsius".
    #ifndef UNIX_HOST
    if (sl_si70xx_measure_rh_and_temp(sl_i2cspm_sensor,
                                      SI7021_ADDR,
                                      &rh_data,
                                      &temp_data)) {
      sensor_status = EMBER_ERR_FATAL;
      app_log_info("Warning! Invalid Si7021 reading: %lu %ld\n", rh_data, temp_data);
    }
    #endif

    if (sensor_status == EMBER_SUCCESS) {
      emberStoreLowHighInt32u(buffer, temp_data);
      emberStoreLowHighInt32u(buffer + 4, rh_data);

      status = emberMessageSend(sink_node_id,
                                SL_SENSOR_SINK_ENDPOINT, // endpoint
                                0, // messageTag
                                SL_SENSOR_SINK_DATA_LENGTH,
                                buffer,
                                tx_options);

      app_log_info("TX: Data to 0x%04X:", sink_node_id);
      for (uint8_t i = 0; i < SL_SENSOR_SINK_DATA_LENGTH; i++) {
        app_log_info(" %02X", buffer[i]);
      }
      app_log_info(": 0x%02X\n", status);
      emberEventControlSetDelayMS(*report_control, sensor_report_period_ms);
    }
  }
}

/**************************************************************************//**
 * Entering sleep is approved or denied in this callback, depending on user
 * demand.
 *****************************************************************************/
bool emberAfCommonOkToEnterLowPowerCallback(bool enter_em2, uint32_t duration_ms)
{
  (void) enter_em2;
  (void) duration_ms;
  return enable_sleep;
}

/**************************************************************************//**
 * This function is called when a message is received.
 *****************************************************************************/
void emberAfIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint == SL_SENSOR_SINK_ENDPOINT) {
    app_log_info("RX: Data from 0x%04X:", message->source);
    for (uint8_t i = SL_SENSOR_SINK_DATA_OFFSET; i < message->length; i++) {
      app_log_info(" %x", message->payload[i]);
    }
    app_log_info("\n");
  }
}

/**************************************************************************//**
 * This function is called to indicate whether an outgoing message was
 * successfully transmitted or to indicate the reason of failure.
 *****************************************************************************/
void emberAfMessageSentCallback(EmberStatus status,
                                EmberOutgoingMessage *message)
{
  (void) message;
  if (status != EMBER_SUCCESS) {
    app_log_info("TX: 0x%02X\n", status);
  }
}

/**************************************************************************//**
 * This function is called when the stack status changes.
 *****************************************************************************/
void emberAfStackStatusCallback(EmberStatus status)
{
  switch (status) {
    case EMBER_NETWORK_UP:
      app_log_info("Network up\n");
      app_log_info("Joined to Sink with node ID: 0x%04X\n", emberGetNodeId());
      // Schedule start of periodic sensor reporting to the Sink
      emberEventControlSetDelayMS(*report_control, sensor_report_period_ms);
      break;
    case EMBER_NETWORK_DOWN:
      app_log_info("Network down\n");
      break;
    case EMBER_JOIN_SCAN_FAILED:
      app_log_error("Scanning during join failed\n");
      break;
    case EMBER_JOIN_DENIED:
      app_log_error("Joining to the network rejected!\n");
      break;
    case EMBER_JOIN_TIMEOUT:
      app_log_info("Join process timed out!\n");
      break;
    default:
      app_log_info("Stack status: 0x%02X\n", status);
      break;
  }
}

/**************************************************************************//**
 * This callback is called in each iteration of the main application loop and
 * can be used to perform periodic functions.
 *****************************************************************************/
void emberAfTickCallback(void)
{
#if defined(SL_CATALOG_LED0_PRESENT)
  if (emberStackIsUp()) {
    sl_led_turn_on(&sl_led_led0);
  } else {
    sl_led_turn_off(&sl_led_led0);
  }
#endif
}

/**************************************************************************//**
 * This function is called when a frequency hopping client completed the start
 * procedure.
 *****************************************************************************/
void emberAfFrequencyHoppingStartClientCompleteCallback(EmberStatus status)
{
  if (status != EMBER_SUCCESS) {
    app_log_error("FH Client sync failed, status=0x%02X\n", status);
  } else {
    app_log_info("FH Client Sync Success\n");
  }
}

/**************************************************************************//**
 * This function is called when a requested energy scan is complete.
 *****************************************************************************/
void emberAfEnergyScanCompleteCallback(int8_t mean,
                                       int8_t min,
                                       int8_t max,
                                       uint16_t variance)
{
  app_log_info("Energy scan complete, mean=%d min=%d max=%d var=%d\n",
               mean, min, max, variance);
}

#if defined(EMBER_AF_PLUGIN_MICRIUM_RTOS) && defined(EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1)

/**************************************************************************//**
 * This function is called from the Micrium RTOS plugin before the
 * Application (1) task is created.
 *****************************************************************************/
void emberAfPluginMicriumRtosAppTask1InitCallback(void)
{
  app_log_info("app task init\n");
}

#include <kernel/include/os.h>
#define TICK_INTERVAL_MS 1000

/**************************************************************************//**
 * This function implements the Application (1) task main loop.
 *****************************************************************************/
void emberAfPluginMicriumRtosAppTask1MainLoopCallback(void *p_arg)
{
  RTOS_ERR err;
  OS_TICK yield_time_ticks = (OSCfg_TickRate_Hz * TICK_INTERVAL_MS) / 1000;

  while (true) {
    app_log_info("app task tick\n");

    OSTimeDly(yield_time_ticks, OS_OPT_TIME_DLY, &err);
  }
}

#endif // EMBER_AF_PLUGIN_MICRIUM_RTOS && EMBER_AF_PLUGIN_MICRIUM_RTOS_APP_TASK1
