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
#include "sl_duty_cycle_utility.h"
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
/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE (256U)
/// Transmit data length
#define TX_PAYLOAD_LENGTH (16U)
/// Application type string
#define APP_TYPE_STRING   "Legacy Long Pr."
/// Times for Duty Cycle timer
#define DUTY_CYCLE_RX_PRINT_TIME    ((uint32_t)  40000) // time for the cli to print out rx packet (us)

/// State machine of Duty Cycle
typedef enum {
  S_IDLE,
  S_PACKET_SENDING,
  S_PACKET_SENT,
  S_PACKET_RECEIVED,
  S_RX_WAITING,
  S_PREAMBLE_RECEIVED,
  S_SYNC_RECEIVED,
  S_RX_PACKET_ERROR,
  S_TX_PACKET_ERROR,
  S_CALIBRATION_ERROR,
  S_INVALID_APP_STATE
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
 * Callback function for timer, used for scheduling the rail states
 *
 * @param railHandle
 * @return None.
 ******************************************************************************/
static void RAILCb_timer_expired(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Timer start/stop for the off part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_idle_timer(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Timer start/stop for the on part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_rx_timer(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Timer start/stop for preamble found part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_preable_timer(RAIL_Handle_t rail_handle);

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
/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Contains the status of RAIL Calibration
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
static uint16_t packet_transmitted = 0;              // TX packets count
static uint16_t packet_received = 0;                 // RX packets count

/// Flag to refresh LCD with new values
static bool refresh_display = true;

/// Flags to know is the interrupt was useful
static bool button_interrupt = false;
static bool radio_interrupt = false;
static bool timer_interrupt = false;
static bool first_run = false;

/// Flag to know in preable received state that rx timer was running
static bool rx_timer_is_running = false;

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
  if (!(button_interrupt || radio_interrupt || timer_interrupt || first_run)) {
    return;
  } else {
    button_interrupt = radio_interrupt = timer_interrupt = first_run = false;
  }

  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  // Radio state variable to know which state the radio is in
  RAIL_RadioState_t radio_actual_state = RAIL_RF_STATE_INACTIVE;

  switch (state) {
    case S_IDLE:
      if (tx_requested) {
        send_tx_packet(rail_handle);
        tx_requested = false;
      } else {
        RAIL_Idle(rail_handle, RAIL_IDLE, true);
        manage_idle_timer(rail_handle);
      }
      break;
    case S_PACKET_SENDING:
      // wait out sending
      break;
    case S_PACKET_SENT:
      sl_led_toggle(&sl_led_led1);
      packet_transmitted++;
      state = S_IDLE;
      refresh_display = true;
      radio_interrupt = true;
      app_log_info("Packet has been sent\n");
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RX_WAITING:
      radio_actual_state = RAIL_GetRadioState(rail_handle);
      if (!(radio_actual_state & RAIL_RF_STATE_RX)) {
        RAIL_Idle(rail_handle, RAIL_IDLE, true);
        RAIL_StartRx(rail_handle, CHANNEL, NULL);
      }
      manage_rx_timer(rail_handle);
      break;
    case S_PREAMBLE_RECEIVED:
      manage_preable_timer(rail_handle);
      break;
    case S_SYNC_RECEIVED:
      // Wait for hole package from now
      if (RAIL_IsTimerRunning(rail_handle)) {
        RAIL_CancelTimer(rail_handle);
      }
      break;
    case S_PACKET_RECEIVED:
      if (RAIL_IsTimerRunning(rail_handle)) {
        RAIL_CancelTimer(rail_handle);
      }
      // Packet received:
      //  - Check whether RAIL_HoldRxPacket() was successful, i.e. packet handle is valid
      //  - Copy it to the application FIFO
      //  - Free up the radio FIFO
      //  - Return to IDLE state i.e. RAIL Rx
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

      if (rx_requested) {
        printf_rx_packet(&rx_fifo[0]);
      }
      sl_led_toggle(&sl_led_led0);
      packet_received++;
      state = S_IDLE;
      radio_interrupt = true;
      refresh_display = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      app_log_error("Radio RX Error occurred\nEvents: %lld\n", current_rail_err);
      state = S_IDLE;
      radio_interrupt = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_TX_PACKET_ERROR:
      // Handle Tx error
      app_log_error("Radio TX Error occurred\nEvents: %lld\n", current_rail_err);
      state = S_IDLE;
      radio_interrupt = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_CALIBRATION_ERROR:
      app_log_error("Radio Calibration Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                    current_rail_err,
                    calibration_status);
      state = S_IDLE;
      radio_interrupt = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    default:
      // Unexpected state
      app_log_error("Unexpected Simple TRX state occurred:%d\n", state);
#if DUTY_CYCLE_USE_LCD_BUTTON == 1
      display_error_on_lcd(INVALID_APP_STATE);
      refresh_display = false;
#endif
      break;
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
  radio_interrupt = true;
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket(rail_handle);
      state = S_PACKET_RECEIVED;
    } else {
      // Handle Rx error
      current_rail_err = (events & RAIL_EVENTS_RX_COMPLETION);
      state = S_RX_PACKET_ERROR;
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      state = S_PACKET_SENT;
    } else {
      // Handle Tx error
      current_rail_err = (events & RAIL_EVENTS_TX_COMPLETION);
      state = S_TX_PACKET_ERROR;
    }
  }
  // if preamble detected, continue listening for sync
  if (events & RAIL_EVENT_RX_PREAMBLE_DETECT) {
    state = S_PREAMBLE_RECEIVED;
  }
  // if Sync received, continue listening for packet
  if (events & RAIL_EVENT_RX_SYNC1_DETECT) {
    state = S_SYNC_RECEIVED;
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

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
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
 * Callback function for timer, used for scheduling the rail states
 *
 * @param railHandle
 * @return None.
 ******************************************************************************/
static void RAILCb_timer_expired(RAIL_Handle_t rail_handle)
{
  (void) rail_handle; //unused param
  timer_interrupt = true;
  switch (state) {
    case S_IDLE:    // set to RX mode, if we were in IDLE
      state = S_RX_WAITING;
      break;
    case S_RX_WAITING:     // set to IDLE, if we were in RX mode
      state = S_IDLE;
      rx_timer_is_running = false;
      break;
    case S_PREAMBLE_RECEIVED:   // set to IDLE, if we received a preamble but not sync
      state = S_IDLE;
      break;
    case S_SYNC_RECEIVED:
      state = S_SYNC_RECEIVED;
      break;
    case S_PACKET_RECEIVED:
      state = S_PACKET_RECEIVED;
      break;
    default:
    {
      app_log_info("Invalid state is : %d", state);
      display_error_on_lcd(INVALID_APP_STATE);
      while (1) ;   //should never get here
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/*******************************************************************************
 * Timer start/stop for the off part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_idle_timer(RAIL_Handle_t rail_handle)
{
  if (!RAIL_IsTimerRunning(rail_handle)) {
    RAIL_SetTimer(rail_handle,
                  (rx_requested ? (duty_cycle_config.delay - DUTY_CYCLE_RX_PRINT_TIME) : duty_cycle_config.delay),
                  RAIL_TIME_DELAY,
                  &RAILCb_timer_expired);
  }
}

/*******************************************************************************
 * Timer start/stop for the on part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_rx_timer(RAIL_Handle_t rail_handle)
{
  if (!RAIL_IsTimerRunning(rail_handle)) {
    RAIL_SetTimer(rail_handle,
                  duty_cycle_config.parameter,
                  RAIL_TIME_DELAY,
                  &RAILCb_timer_expired);
    rx_timer_is_running = true;
  }
}

/*******************************************************************************
 * Timer start/stop for preamble found part of the application
 *
 * @param rail_handle: which rail handler to use for timer function
 * @return None.
 ******************************************************************************/
static void manage_preable_timer(RAIL_Handle_t rail_handle)
{
  if (rx_timer_is_running) {
    rx_timer_is_running = false;
    RAIL_CancelTimer(rail_handle);
  }
  if (!RAIL_IsTimerRunning(rail_handle)) {
    RAIL_SetTimer(rail_handle,
                  ((2 * duty_cycle_config.parameter) + duty_cycle_config.delay),
                  RAIL_TIME_DELAY,
                  &RAILCb_timer_expired);
  }
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

  state = S_PACKET_SENDING;

  // Stop timer to not to interfier with sending
  if (RAIL_IsTimerRunning(rail_handle)) {
    RAIL_CancelTimer(rail_handle);
  }

  RAIL_Idle(rail_handle, RAIL_IDLE, true);
  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, TX_PAYLOAD_LENGTH, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             RAIL_FIFO_SIZE);
  rail_status = RAIL_StartTx(rail_handle, CHANNEL, RAIL_TX_OPTION_ALT_PREAMBLE_LEN, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_StartTx() result:%d ", rail_status);
  }

  return rail_status;
}
