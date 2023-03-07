/***************************************************************************//**
 * @file
 * @brief app_tick.c
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
#include "app_log.h"
#include "sl_rail_util_init.h"
#include "app_process.h"
#include "app_init.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"

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

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * The function printfs the received rx message.
 *
 * @param rx_buffer Msg buffer
 * @returns None
 *****************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer);

/**************************************************************************//**
 * The function printfs the received rx message.
 *
 * @param railHandle
 * @returns None
 *****************************************************************************/
static void sl_timer_callback(RAIL_Handle_t railHandle);

/**************************************************************************//**
 * Set RAIL Tx Fifo and start RAIL transmission
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void send_prepared_packet();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// The variable shows whether only one channel is used for rx, or both (scan mode)
volatile bool scan_mode = true;

/// Flag that indicates that send already happend in scan mode
volatile bool send_requested = false;

/// Which channel is active for receiving
volatile uint16_t rx_active_channel = DEFAULT_CHANNEL;

/// Which channel will be used for sending
volatile uint8_t send_channel = DEFAULT_CHANNEL;
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
  RAIL_FIFO_ALIGNMENT_TYPE align[TX_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[TX_FIFO_SIZE];
} tx_fifo = { .fifo = {
                0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
              } };

static const RAIL_Events_t events = (
  RAIL_EVENTS_RX_COMPLETION | RAIL_EVENTS_TX_COMPLETION | RAIL_EVENT_CAL_NEEDED
  | RAIL_EVENT_RX_PREAMBLE_DETECT | RAIL_EVENT_RX_PREAMBLE_LOST);

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
void stop_timer_wait_for_idle(void)
{
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  RAIL_CancelTimer(rail_handle);
  RAIL_Idle(rail_handle, RAIL_IDLE, true);
}

void set_next_state(state_t next_state)
{
  state = next_state;
}

state_t get_current_state(void)
{
  return state;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // // RAIL received packet
  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  switch (state) {
    case S_START_SENDING:
      send_prepared_packet();
      set_next_state(S_SENDING);
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_SENDING:
      break;
    case S_SENT:
      app_log_info("Packet has been sent on channel: %d\n", send_channel);
      if (send_channel) {
        sl_led_toggle(&sl_led_led1);
      } else {
        sl_led_toggle(&sl_led_led0);
      }
      set_next_state(S_START_RECEIVING);
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_START_RECEIVING:
      RAIL_CancelTimer(rail_handle);
      RAIL_Idle(rail_handle, RAIL_IDLE, true);
      if (send_requested) {
        //handle pending tx events first
        send_requested = false;
        set_next_state(S_START_SENDING);
      } else if (scan_mode) {
        rx_active_channel = (rx_active_channel + 1) % ACTIVE_CHANNEL_CONFIG_NUMS;

        RAIL_ConfigEvents(rail_handle, RAIL_EVENTS_ALL, RAIL_EVENTS_NONE);
        RAIL_ConfigEvents(rail_handle, events, events);

        set_next_state(S_RECEIVING);
        RAIL_StartRx(rail_handle, rx_active_channel, NULL);
        RAIL_SetTimer(rail_handle, RAIL_GetTime() + scan_times[rx_active_channel],
                      RAIL_TIME_ABSOLUTE, sl_timer_callback);
      } else {
        set_next_state(S_RECEIVING);
        RAIL_StartRx(rail_handle, rx_active_channel, NULL);
      }
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RECEIVING:
      break;
    case S_RECEIVED:
      // Packet received:
      //  - Check whether RAIL_HoldRxPacket() was successful, i.e. packet handle is valid
      //  - Copy it to the application FIFO
      //  - Free up the radio FIFO
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
      printf_rx_packet(&rx_fifo[0]);

      if (rx_active_channel) {
        sl_led_toggle(&sl_led_led1);
      } else {
        sl_led_toggle(&sl_led_led0);
      }

      if (scan_mode) {
        set_next_state(S_START_RECEIVING);
      } else {
        set_next_state(S_RECEIVING);
      }
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      app_log_error("Radio RX Error occurred\nEvents: %lld\n", current_rail_err);
      if (scan_mode) {
        set_next_state(S_START_SENDING);
      } else {
        set_next_state(S_RECEIVING);
      }
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_TX_PACKET_ERROR:
      // Handle Tx error
      app_log_error("Radio TX Error occurred\nEvents: %lld\n", current_rail_err);
      set_next_state(S_START_SENDING);
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    case S_IDLE:
      break;
    case S_CALIBRATION_ERROR:
      app_log_error("Radio Calibration Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                    current_rail_err,
                    calibration_status);
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;
    default:
      // Unexpected state
      app_log_error("Unexpected Simple TRX state occurred:%d\n", state);
      break;
  }
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket(rail_handle);
      set_next_state(S_RECEIVED);
    } else {
      // Handle Rx error
      current_rail_err = (events & RAIL_EVENTS_RX_COMPLETION);
      set_next_state(S_RX_PACKET_ERROR);
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      set_next_state(S_SENT);
    } else {
      // Handle Tx error
      current_rail_err = (events & RAIL_EVENTS_TX_COMPLETION);
      set_next_state(S_TX_PACKET_ERROR);
    }
  }

  if (events & RAIL_EVENT_RX_PREAMBLE_DETECT) {
    RAIL_CancelTimer(rail_handle);
    state = S_RECEIVING;
  }

  if (events & RAIL_EVENT_RX_PREAMBLE_LOST) {
    state = S_START_RECEIVING;
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

/******************************************************************************
 * Button callback, called if any button is pressed or released.
 *****************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  uint8_t channel = 0;
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      channel = 0;
    } else if (&sl_button_btn1 == handle) {
      channel = 1;
    }

    send_channel = channel;

    if ( !scan_mode ) {
      set_next_state(S_START_SENDING);
    } else {
      send_requested = true;
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Set RAIL Tx Fifo and start RAIL transmission
 *****************************************************************************/
static inline void send_prepared_packet()
{
  RAIL_Handle_t rail_handle = sl_rail_util_get_handle(SL_RAIL_UTIL_HANDLE_INST0);
  RAIL_Idle(rail_handle, RAIL_IDLE, true);
  RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, 16, TX_FIFO_SIZE);
  RAIL_StartTx(rail_handle, send_channel, RAIL_TX_OPTIONS_DEFAULT, NULL);
}

/******************************************************************************
 * Timer callback used for switching back and forth between the 2 rx channels
 * if scan mode is ON
 *****************************************************************************/
static void sl_timer_callback(RAIL_Handle_t rail_handle)
{
  (void) rail_handle;
  if ( state == S_RECEIVING ) {
    state = S_START_RECEIVING;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * The API forwards the received rx packet on CLI
 *****************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer)
{
  app_log_info("Packet has been received on channel %d: ", rx_active_channel);
  for (uint8_t i = 0; i < TX_PAYLOAD_LENGTH; i++) {
    app_log_info("0x%02X, ", rx_buffer[i]);
  }
  app_log_info("\n");
}
