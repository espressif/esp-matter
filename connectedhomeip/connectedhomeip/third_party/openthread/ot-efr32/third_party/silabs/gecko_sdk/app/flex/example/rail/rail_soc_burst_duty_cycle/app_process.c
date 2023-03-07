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
#include <stdint.h>
#include "sl_component_catalog.h"
#include "app_assert.h"
#include "app_log.h"
#include "rail.h"
#include "app_init.h"
#include "app_process.h"
#include "sl_simple_led_instances.h"
#include "sl_duty_cycle_config.h"

#if DUTY_CYCLE_USE_LCD_BUTTON
#include "app_graphics.h"
#include "sl_simple_button_instances.h"
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------

/// Application type string
#define APP_TYPE_STRING   "Burst Mode"
/// Required transmission time for a guaranteed reception in Slave Rx mode
#define BURST_TIME        (DUTY_CYCLE_OFF_TIME + (2UL * DUTY_CYCLE_ON_TIME))
/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE    (256U)
/// Transmit data length
#define TX_PAYLOAD_LENGTH (16U)

/// State machine of Duty Cycle
typedef enum {
  S_IDLE,             //!< Idling in default Slave Mode
  S_BURST_RECEIVE,    //!< Burst recevied in Slave mode
  S_BURST_SENDING,    //!< Burst TX in progress in Master Mode
  S_ERROR             //!< An error occurred
} state_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/*******************************************************************************
 * The function printfs the received rx message.
 *
 * @param rx_buffer Msg buffer
 * @returns None
 ******************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer);

/*******************************************************************************
 * This helper function handles the S_IDLE state of the state machine.
 *
 * @returns None
 ******************************************************************************/
static void handle_idle_state(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * This helper function handles the S_BURST_RECEIVE state of the state machine.
 *
 * @returns None
 ******************************************************************************/
static void handle_receive_state(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * This helper function handles the S_BURST_SENDING state of the state machine.
 *
 * @returns None
 ******************************************************************************/
static void handle_send_state(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * This helper function handles the S_ERROR state of the state machine.
 *
 * @returns None
 ******************************************************************************/
static void handle_error_state(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Send a prepared TX packet on selecetd rail handler
 *
 * @param rail_handle: which rail handler to use for tx function
 * @return rail_status: error code from rail
 ******************************************************************************/
static RAIL_Status_t send_tx_packet(RAIL_Handle_t rail_handle);

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating transmit request (button was pressed / CLI transmit request has occurred)
volatile bool tx_requested = false;
/// Flag, indicating received packet is forwarded on CLI or not
volatile bool rx_requested = true;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------

/// Master mode constants and variables
/// ID for a blast
static volatile uint8_t master_burst_id = 0U;
/// count of packets sent in the current burst
static volatile uint32_t master_burst_packets_count = 0UL;
/// burst ID received by the Slave
static volatile uint8_t slave_rx_burst_id = 0U;
/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;
/// Contains the status of RAIL Calibration, useful for error handling
static volatile RAIL_Status_t calibration_status = 0;
/// RAIL Rx packet handle
static volatile RAIL_RxPacketHandle_t rx_packet_handle;
/// Receive FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];

/// Transmit FIFO
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_FIFO_SIZE];
} tx_fifo = { .fifo = {
                0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
              } };

/// Counter to be displayed on LCD
static uint16_t packet_transmitted = 0U;              // TX packets count
static uint16_t packet_received = 0U;                 // RX packets count

/// Flag to refresh LCD with new values
static bool refresh_display = true;

/// Flags to know is the interrupt was useful
static bool button_interrupt = false;
static bool radio_interrupt = false;
static bool first_run = false;

/// Flags to indicate relevant RAIL events
static bool rail_packet_sent = false;
static bool rail_packet_received = false;
static bool rail_error = false;
/// Timeout calculated for the burst
static RAIL_Time_t timeout = 0UL;
static RAIL_Events_t rail_last_state = RAIL_EVENTS_NONE;

/// Flag, indicating duty cycle cycle was completed
static volatile bool duty_cycle_end = false;

#if DUTY_CYCLE_ALLOW_EM2 == 1
static RAIL_ScheduleRxConfig_t rx_schedule_config = {
  .start = DUTY_CYCLE_OFF_TIME,
  .startMode = RAIL_TIME_DELAY,
  .end = 0U,
  .endMode = RAIL_TIME_DISABLED,
  .rxTransitionEndSchedule = 0U,
  .hardWindowEnd = 0U
};
#endif

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Setter function for flag which allows to run state machine without interrupt
 *
 * @param is_first_run: Set the first run flag
 * @return None.
 ******************************************************************************/
void set_first_run(bool is_first_run)
{
  first_run = is_first_run;
}

/*******************************************************************************
 * Application state machine, called infinitely
 *
 * @param rail_handle: which rail handler to use for rx and tx
 * @return None.
 ******************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // To make sure to only enter in state machine if useful interrupt happened
  if (!(button_interrupt || radio_interrupt || first_run || rail_error)) {
    return;
  } else {
    button_interrupt = radio_interrupt = first_run = false;
  }

  // Handle errors if pending
  if (rail_error) {
    rail_error = false;
    state = S_ERROR;
  }

  switch (state) {
    case S_IDLE:
      handle_idle_state(rail_handle);
      break;
    case S_BURST_SENDING:
      handle_send_state(rail_handle);
      break;
    case S_BURST_RECEIVE:
      handle_receive_state(rail_handle);
      break;
    case S_ERROR:
      handle_error_state(rail_handle);
      break;
    default:
      // Unexpected state
      app_log_error("Unexpected state occurred:%d\n", state);
#if DUTY_CYCLE_USE_LCD_BUTTON == 1
      display_error_on_lcd(INVALID_APP_STATE);
      refresh_display = false;
#endif
      break;
  }

  // Reset copy of RAIL events
  rail_last_state = RAIL_EVENTS_NONE;

  // After packet received or DutyCyle end RAIL RX needs to be restarted
  if (duty_cycle_end) {
    duty_cycle_end = false;
#if DUTY_CYCLE_ALLOW_EM2 == 0
    RAIL_StartRx(rail_handle, CHANNEL, NULL);
#else
    rx_schedule_config.start = duty_cycle_config.delay;
    RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, true);
    RAIL_ScheduleRx(rail_handle, CHANNEL, &rx_schedule_config, NULL);
#endif
  }

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
  if (refresh_display) {
    refresh_display = false;
    update_display(APP_TYPE_STRING, packet_transmitted, packet_received);
  }
#endif
}

/*******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 ******************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  // Make a copy of the events
  rail_last_state = events;
  // something RAIL happened
  radio_interrupt = true;

  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      rail_packet_sent = true;
    } else {
      rail_error = true;
    }
  }

  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket(rail_handle);
      rail_packet_received = true;
    } else {
      rail_error = true;
    }
  }

#if DUTY_CYCLE_ALLOW_EM2 == 1
  if ( events & RAIL_EVENT_RX_DUTY_CYCLE_RX_END) {
    duty_cycle_end = true;
  }
#endif

  // Perform all calibrations when needed or indicate error if failed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      rail_error = true;
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

#if DUTY_CYCLE_USE_LCD_BUTTON == 1
/*******************************************************************************
 * Button callback, called if any button is pressed or released.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    button_interrupt = true;
    tx_requested = true;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}
#endif

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * This helper function handles the S_IDLE state of the state machine.
 ******************************************************************************/
static void handle_idle_state(RAIL_Handle_t rail_handle)
{
  // Wait for a received packet
  if (rail_packet_received) {
    // Don't clear the RX flag, keep coming back to state machine
    radio_interrupt = true;
    // Switch to handling of the received packet
    state = S_BURST_RECEIVE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
    app_task_notify();
#endif
  } else if (tx_requested) {
    // Clear request and start a new burst
    tx_requested = false;
    master_burst_id++;
    // Disable duty cycle,then schedule the burst period and start sending
    RAIL_Idle(rail_handle, RAIL_IDLE, true);
    RAIL_EnableRxDutyCycle(rail_handle, false);
    // Time the stop of burst to time when the Rx side is guaranteed to have listened.
    timeout = RAIL_GetTime() + BURST_TIME;
    // Kickstart the burst
    radio_interrupt = true;
    rail_packet_sent = true;
    state = S_BURST_SENDING;
#if defined(SL_CATALOG_KERNEL_PRESENT)
    app_task_notify();
#endif
  }
}

/*******************************************************************************
 * This helper function handles the S_BURST_RECEIVE state of the state machine.
 ******************************************************************************/
static void handle_receive_state(RAIL_Handle_t rail_handle)
{
  // Used for accessing the packet data
  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  // Incoming packet?
  if (rail_packet_received) {
    rail_packet_received = false;
    // Get the RX packet data
    if (rx_packet_handle == RAIL_RX_PACKET_HANDLE_INVALID) {
      app_log_error("RAIL_HoldRxPacket() error: RAIL_RX_PACKET_HANDLE_INVALID\n"
                    "No such RAIL rx packet yet exists or rail_handle is not active");
    }
    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
    if (rx_packet_handle == RAIL_RX_PACKET_HANDLE_INVALID) {
      app_log_error("RAIL_GetRxPacketInfo() error: RAIL_RX_PACKET_HANDLE_INVALID\n");
    }
    RAIL_CopyRxPacket(rx_fifo, &packet_info);
    rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
    }
    // Check if this is a new burst
    if (slave_rx_burst_id != rx_fifo[0]) {
      slave_rx_burst_id = rx_fifo[0];
      // Print packet if requested
      if (rx_requested) {
        printf_rx_packet(&rx_fifo[0]);
      }
      // RX bookkeeping & update LCD
      sl_led_toggle(&sl_led_led0);
      packet_received++;
      refresh_display = true;
      // Rx of one packet done, going back to listen
      duty_cycle_end = true;
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
    }
  }
}

/*******************************************************************************
 * This helper function handles the S_BURST_SENDING state of the state machine.
 ******************************************************************************/
static void handle_send_state(RAIL_Handle_t rail_handle)
{
  // Check if burst time elapsed
  if (RAIL_GetTime() >= timeout) {
    // Burst completed, clear any Tx flags and upadte UI
    rail_packet_sent = false;
    sl_led_toggle(&sl_led_led1);
    packet_transmitted++;
    refresh_display = true;
    app_log_info("Burst of %u packets sent.\n", master_burst_packets_count);
    // Run-time check if the listener had no chance to receive the burst
    if ( (BURST_TIME / master_burst_packets_count) > DUTY_CYCLE_ON_TIME) {
      app_log_info("WARNING! Packet time of %u is longer than the ON time of %u!\n",
                   (BURST_TIME / master_burst_packets_count),
                   (uint32_t)DUTY_CYCLE_ON_TIME);
    }
    master_burst_packets_count = 0UL;
    // Go back to Slave Idle state in Duty Cycle
    RAIL_Idle(rail_handle, RAIL_IDLE, true);
    RAIL_EnableRxDutyCycle(rail_handle, true);
    duty_cycle_end = true;
    state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
    app_task_notify();
#endif
  } else {
    // Otherwise, do the burst
    if (rail_packet_sent) {
      rail_packet_sent = false;
      master_burst_packets_count++;
      tx_fifo.fifo[0] = master_burst_id;
      send_tx_packet(rail_handle);
    }
  }
}

/*******************************************************************************
 * This helper function handles the S_ERROR state of the state machine.
 ******************************************************************************/
static void handle_error_state(RAIL_Handle_t rail_handle)
{
  (void)rail_handle;
  // Handle Rx error
  if (rail_last_state & RAIL_EVENTS_RX_COMPLETION) {
    app_log_error("Radio RX Error occurred\nEvents: %lld\n", rail_last_state);
    // Handle Tx error
  } else if (rail_last_state & RAIL_EVENTS_TX_COMPLETION) {
    app_log_error("Radio TX Error occurred\nEvents: %lld\n", rail_last_state);
    // Handle calibration error
  } else if (rail_last_state & RAIL_EVENT_CAL_NEEDED) {
    app_log_error("Radio Calibration Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                  rail_last_state,
                  calibration_status);
  }
  // Restart duty cylce
  duty_cycle_end = true;

  state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * The API forwards the received rx packet on CLI
 *
 * @param  rx_buffer pointer to the buffer to be written out.
 * @return None.
 ******************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer)
{
  app_log_info("Packet has been received: ");
  for (uint8_t i = 0; i < TX_PAYLOAD_LENGTH; i++) {
    app_log_info("0x%02X, ", rx_buffer[i]);
  }
  app_log_info("\n");
}

/*******************************************************************************
 * Send a prepared TX packet on selecetd rail handler
 *
 * @param rail_handle: which rail handler to use for tx function
 * @return rail_status: error code from rail
 ******************************************************************************/
static RAIL_Status_t send_tx_packet(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call
  uint16_t allocated_tx_fifo_size = 0;

  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, TX_PAYLOAD_LENGTH, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             RAIL_FIFO_SIZE);
  rail_status = RAIL_StartTx(rail_handle, CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_StartTx() result:%d ", rail_status);
  }

  return rail_status;
}
