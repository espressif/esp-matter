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
/// Size of RAIL RX/TX FIFO
#define RAIL_FIFO_SIZE (256U)
/// Transmit data length
#define TX_PAYLOAD_LENGTH (16U)
/// Application type string
#define APP_TYPE_STRING   "Long Preamble"

/// State machine of Duty Cycle
typedef enum {
  S_IDLE,             //!< Idle or sleep state
  S_PACKET_SENDING,   //!< Sending a packet
  S_PACKET_SENT,      //!< Packet sending ended with success
  S_PACKET_RECEIVED,  //!< Packet received successful
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
 * Send a prepared TX packet on selecetd rail handler
 *
 * @param rail_handle: which rail handler to use for tx function
 * @return rail_status: error code from rail
 ******************************************************************************/
static RAIL_Status_t send_tx_packet(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * The API print out the received errors from RAIL to CLI
 *
 * @param  None.
 * @return None.
 ******************************************************************************/
static void process_rail_errors(void);

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
/// Flag, indicating packet was sent
static volatile bool packet_is_sent = false;

/// Flag, indicating packet sending had errors
static volatile bool rail_tx_error = false;

/// Flag, indicating packet was received
static volatile bool packet_is_received = false;

/// Flag, indicating packet receiving had errors
static volatile bool rail_rx_error = false;

/// Flag, indicating duty cycle cycle was completed
static volatile bool duty_cycle_end = false;

/// Flag, indicating radio calibration had errors
static volatile bool rail_cal_error = false;

/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

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
static bool first_run = false;

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
  if (!(button_interrupt || radio_interrupt || first_run || tx_requested)) {
    return;
  } else {
    button_interrupt = radio_interrupt = first_run = false;
  }

  // Received packet information
  RAIL_RxPacketInfo_t packet_info;

  // Received packet handle
  RAIL_RxPacketHandle_t rx_packet_handle;

  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  //Handle the errors before everything else
  process_rail_errors();

  //Select which state comes
  if (packet_is_received) {
    packet_is_received = false;
    state = S_PACKET_RECEIVED;
  } else if (packet_is_sent) {
    packet_is_sent = false;
    state = S_PACKET_SENT;
  }

  switch (state) {
    case S_IDLE:
      if (tx_requested) {
        send_tx_packet(rail_handle);
        state = S_PACKET_SENDING;
        tx_requested = false;
#if defined(SL_CATALOG_KERNEL_PRESENT)
        app_task_notify();
#endif
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
    case S_PACKET_RECEIVED:
      rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
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

        rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      }
      state = S_IDLE;
      radio_interrupt = true;
      refresh_display = true;
      duty_cycle_end = true;
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
}

/*******************************************************************************
 * RAIL callback, called if a RAIL event occurs.
 ******************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  radio_interrupt = true;
  current_rail_err = events;

  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      packet_is_sent = true;
    } else {
      // Handle Tx error
      rail_tx_error = true;
    }
  }

  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      RAIL_HoldRxPacket(rail_handle);
      packet_is_received = true;
    } else {
      // Handle Rx error
      rail_rx_error = true;
    }
  }

  if ( events & RAIL_EVENT_RX_DUTY_CYCLE_RX_END) {
#if DUTY_CYCLE_ALLOW_EM2 == 1
    duty_cycle_end = true;
#endif
  }

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      rail_cal_error = true;
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
 * The API print out the received errors from RAIL to CLI
 *
 * @param  None.
 * @return None.
 ******************************************************************************/
static void process_rail_errors(void)
{
  if (rail_tx_error) {
    rail_tx_error = false;
    app_log_warning("Radio TX Error occurred\nEvents: %lld\n", (current_rail_err & RAIL_EVENTS_TX_COMPLETION));
    duty_cycle_end = true;
  }
  if (rail_rx_error) {
    rail_rx_error = false;
    app_log_warning("Radio RX Error occurred\nEvents: %lld\n", (current_rail_err & RAIL_EVENTS_RX_COMPLETION));
    duty_cycle_end = true;
  }
  if (rail_cal_error) {
    rail_cal_error = false;
    app_log_error("Radio Calibration Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                  (current_rail_err & RAIL_EVENT_CAL_NEEDED),
                  calibration_status);
  }
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
  rail_status = RAIL_StartTx(rail_handle, CHANNEL, RAIL_TX_OPTION_ALT_PREAMBLE_LEN, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_StartTx() result:%d ", rail_status);
  }

  return rail_status;
}
