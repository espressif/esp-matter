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
#include "rail.h"
#include "sl_component_catalog.h"
#include "app_init.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "sl_wmbus_support.h"
#include "wmbus_sample_frame.h"
#include "em_emu.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// TX buffer length
#define BUFFER_LENGTH  (512U)

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Prepare and send the WMBus packet
 *
 * @param rail_handle: which rail handle to use for sending
 * @param length: length of the packet
 * @param send_at: absolute time when to send it
 ******************************************************************************/
void send_packet_at(RAIL_Handle_t rail_handle, uint16_t length, uint32_t send_at);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Variables from app_init.c
extern uint16_t rx_channel;
extern uint8_t access_number;

/// Time for calculation for the proper sending timing
RAIL_Time_t last_tx_start_time = 0;
RAIL_Time_t last_tx_end_time = 0;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// State machine state
static volatile state_t state = S_SCHEDULE_TX;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0U;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = RAIL_STATUS_NO_ERROR;

/// Variable to allow to go to sleep
static volatile bool ok_to_sleep = true;

/// Last sent packet length, need for calculating the next one
static uint16_t last_tx_length = 0U;

/// Buffer to store in packets before sending
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[BUFFER_LENGTH / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[BUFFER_LENGTH];
} tx_buffer;

/// WMBus specific parameters
static uint32_t wmbus_app_period_acc = 500e3;
static const uint32_t wmbus_app_period_nom = 10e6; //10s

/// Start the rx window earlier and close it later by this amount
static const uint32_t response_delay_safety_margin = 200U;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Set the next state for the process state machine
 * @param next_state: Which state will the sate machine enter next time
 ******************************************************************************/
void set_next_state(state_t next_state)
{
  state = next_state;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;
  uint16_t length = 0;

  switch (state) {
    case S_SCHEDULE_TX:
      length = WMBUS_SAMPLE_setupFrame(tx_buffer.fifo, access_number, get_wmbus_accessibility(), 12345, 123, true, true);
      send_packet_at(rail_handle, length, last_tx_start_time + wmbus_app_period_acc);
      // TX is scheduled, sleep can be enabled as RAIL deals with power manager
      ok_to_sleep = true;
      state = S_IDLE_OR_RX;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_TX_DONE:
      sl_led_toggle(&sl_led_led0);
      //let's setup the time for the next tx.
      /*
       * from EN13757-4:
       * tACC=(1+(|nACC-128|-64)/2048*tNOM
       * a bit easier to calculate:
       * tACC=(2048+|nACC-128|-64)*tNOM/2048
       */
      access_number++;
      wmbus_app_period_acc = access_number > 128 ? access_number - 128 : 128 - access_number;       //abs(gAccessNumber-128)
      wmbus_app_period_acc = (2048 + wmbus_app_period_acc - 64) * (wmbus_app_period_nom / 2048);
      switch (get_wmbus_accessibility()) {
        case WMBUS_ACCESSIBILITY_LIMITED_ACCESS:
          state = S_RESPONSE_DELAY;
          break;
        case WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS:
          state = S_UNLIMITED_ACCESS;
          break;
        // no RX between TXes, schedule the next TX immediately
        default:
          state = S_SCHEDULE_TX;
          break;
      }
      // don't sleep until an RX or TX is scheduled
      ok_to_sleep = false;

#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RESPONSE_DELAY:
    {
      RAIL_ScheduleRxConfig_t schedule = {
        .start = last_tx_end_time + WMBUS_getMeterLimitedAccRxStart(false) - response_delay_safety_margin,
        .startMode = RAIL_TIME_ABSOLUTE,
        .end = last_tx_end_time + WMBUS_getMeterLimitedAccRxStop(false) + response_delay_safety_margin,
        .endMode = RAIL_TIME_ABSOLUTE,
        .hardWindowEnd = 0,            // extend RX window if we're receiving
      };
      RAIL_Idle(rail_handle, RAIL_IDLE, true);
      RAIL_ScheduleRx(rail_handle, rx_channel, &schedule, NULL);
      // RX is scheduled, sleep can be enabled as RAIL deals with power manager
      ok_to_sleep = true;
      state = S_IDLE_OR_RX;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
    }
    break;
    case S_UNLIMITED_ACCESS:
    {
      RAIL_ScheduleRxConfig_t schedule = {
        .start = last_tx_end_time + WMBUS_getMeterLimitedAccRxStart(false) - response_delay_safety_margin,
        .startMode = RAIL_TIME_ABSOLUTE,
        // end RX just before the next TX, 2ms safety gap
        .end = last_tx_start_time + wmbus_app_period_acc - 2e3,
        .endMode = RAIL_TIME_ABSOLUTE,
        .rxTransitionEndSchedule = 0,  // stay in scheduled RX on reception
        .hardWindowEnd = 0,            // extend RX window if we're receiving
      };
      RAIL_Idle(rail_handle, RAIL_IDLE, true);
      RAIL_ScheduleRx(rail_handle, rx_channel, &schedule, NULL);
      // RX is scheduled, sleep can be enabled as RAIL deals with power manager
      // although, if the gap between RX and TX is short enough, the device will
      // not have time to go to sleep
      ok_to_sleep = true;
      state = S_IDLE_OR_RX;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
    }
    break;
    //in idle states (like S_RX, we need the main oscillator, but waiting for an interrupt. EM1 is safe)
    case S_IDLE_OR_RX:
      // go to sleep or back to rx listening
      break;
    default:
      // unkown state
      break;
  }
}

/******************************************************************************
 * Manages the power_manager_sleep in main.c
 *****************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return ok_to_sleep;
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  if ( events & RAIL_EVENT_TX_STARTED) {
    RAIL_GetTxTimePreambleStart(rail_handle, RAIL_TX_STARTED_BYTES, &last_tx_start_time);
  }

  if ( events & RAIL_EVENTS_TX_COMPLETION ) {
    if ( events & RAIL_EVENT_TX_PACKET_SENT ) {
      RAIL_GetTxPacketDetailsAlt(rail_handle, false, &last_tx_end_time);
      RAIL_GetTxTimeFrameEnd(rail_handle, last_tx_length, &last_tx_end_time);
    }
    state = S_TX_DONE;
  }

  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      sl_led_toggle(&sl_led_led1);
    }
    switch (get_wmbus_accessibility()) {
      case WMBUS_ACCESSIBILITY_LIMITED_ACCESS:
        state = S_SCHEDULE_TX;
        break;
      case WMBUS_ACCESSIBILITY_UNLIMITED_ACCESS:
        state = S_IDLE_OR_RX;
        break;
      default:
        state = S_IDLE_OR_RX;
    }
  }

  if ( events & RAIL_EVENT_RX_SCHEDULED_RX_END ) {
    state = S_SCHEDULE_TX;
  }

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      current_rail_err = (events & RAIL_EVENT_CAL_NEEDED);
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Prepare and send the WMBus packet
 *
 * @param rail_handle: which rail handle to use for sending
 * @param length: length of the packet
 * @param send_at: absolute time when to send it
 ******************************************************************************/
void send_packet_at(RAIL_Handle_t rail_handle, uint16_t length, uint32_t send_at)
{
  last_tx_length = WMBUS_phy_software(tx_buffer.fifo, (uint8_t) length, BUFFER_LENGTH);
  RAIL_SetTxFifo(rail_handle, tx_buffer.fifo, last_tx_length, BUFFER_LENGTH);
  if ( last_tx_length != length ) {
    //Only for Series 1 Mode T M2O
    RAIL_SetFixedLength(rail_handle, last_tx_length);
  } else {
    RAIL_SetFixedLength(rail_handle, RAIL_SETFIXEDLENGTH_INVALID);
  }
  RAIL_ScheduleTxConfig_t schedule = {
    .mode = RAIL_TIME_ABSOLUTE,
    .when = send_at,
  };
  RAIL_StartScheduledTx(rail_handle, TX_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, &schedule, NULL);
}
