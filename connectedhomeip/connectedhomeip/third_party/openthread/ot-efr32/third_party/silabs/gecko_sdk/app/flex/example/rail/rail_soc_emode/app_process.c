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
#include "em_emu.h"
#include "sl_component_catalog.h"
#include "rail.h"
#include "sl_power_manager.h"
#include "app_process.h"
#include "app_assert.h"
#include "app_log.h"

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
/*******************************************************************************
 * Print to CLI the current value of the TX power
 * @param rail_handle: where to get the TX power levels
 ******************************************************************************/
static void print_current_power_levels(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Print out the newly selected sleep_mode
 ******************************************************************************/
static void print_new_sleep_mode(void);

/*******************************************************************************
 * Configure and starts RX to make it periodical
 * @param rail_handle: which rail_handle to use for receiving
 ******************************************************************************/
static void handle_periodic_rx(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Configure and starts TX to make it periodical
 * @param rail_handle: which rail_handle to use for sending
 ******************************************************************************/
static void handle_periodic_tx(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * The API selects the proper sleep level and set it for power_manager
 ******************************************************************************/
static void manage_sleep_levels(void);

/*******************************************************************************
 * The API set the rail back to idle state
 ******************************************************************************/
static void set_radio_to_idle_state(RAIL_Handle_t rail_handle);

/*******************************************************************************
 * Clear flag and allow power manager to go lower then EM1
 ******************************************************************************/
static void clear_em1_mode(void);

/*******************************************************************************
 * Set flag and power manager to EM1 maximum sleep mode
 ******************************************************************************/
static void set_em1_mode(void);

/*******************************************************************************
 * Clear flag and allow power manager to go lower then EM2
 ******************************************************************************/
static void clear_em2_mode(void);

/*******************************************************************************
 * Set flag and power manager to EM2 maximum sleep mode
 ******************************************************************************/
static void set_em2_mode(void);

/*******************************************************************************
 * The API forwards the received rx packet on CLI
 ******************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer);

/*******************************************************************************
 * API to handle received packets
 *
 * @param rail_handle: the rail instance that the packet will be read from
 ******************************************************************************/
static void handle_received_packet(RAIL_Handle_t rail_handle);
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Sleep level variable
volatile uint8_t sleep_mode = 1;

/// TX power settings
volatile RAIL_TxPowerLevel_t power_raw = 0;
volatile RAIL_TxPower_t power_deci_dbm = 0;
volatile bool is_raw = false;

/// Scheduled TX/RX variables
volatile uint32_t sleep_period = 0; //sleep period in us for PERIODIX_TX/_RX
volatile uint32_t rx_on_period = 0; //rx period for periodic _RX

/// Schedule state first run
volatile bool init_needed = false;

/// Schedule TX/RX flags to not to run again accidently
volatile bool packet_sending = false;
volatile bool rx_ended = true;
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
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

/// RAIL Rx packet handle
static volatile RAIL_RxPacketHandle_t rx_packet_handle;

/// Flags to manage rail sleep levels
static bool em1_is_enabled = false;
static bool em2_is_enabled = false;
static bool allow_to_sleep = true;

/// State machine state variable and buffer
static state_t app_state = S_IDLE;
static bool periodic_receive = false;

/// config for the TX schedule option
static RAIL_ScheduleTxConfig_t shedule_tx_config = {
  .mode = RAIL_TIME_DELAY,
  .txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_ABORT_TX,
  .when = 500000
};

/// config for the RX schedule option
static RAIL_ScheduleRxConfig_t shedule_rx_config = {
  .startMode = RAIL_TIME_DELAY,
  .start =  500000,
  .endMode = RAIL_TIME_DELAY,
  .end = 500000,
  .rxTransitionEndSchedule = 0,
  .hardWindowEnd = 0
};

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;

  switch (app_state) {
    case S_IDLE:
      break;
    case S_SET_IDLE:
      print_new_sleep_mode();
      set_radio_to_idle_state(rail_handle);
      app_state = S_IDLE;
      break;
    case S_CW:
      app_log_info("Tx CW mode; EM%d\n", sleep_mode);
      set_radio_to_idle_state(rail_handle);
      rail_status = RAIL_StartTxStream(rail_handle, CHANNEL, RAIL_STREAM_CARRIER_WAVE);
      if (rail_status != RAIL_STATUS_NO_ERROR) {
        app_log_warning("RAIL_StartRx() result:%d", rail_status);
      }
      app_state = S_IDLE;
      break;
    case S_RX:
      app_log_info("Rx mode; EM%d\n", sleep_mode);
      set_radio_to_idle_state(rail_handle);
      rail_status = RAIL_StartRx(rail_handle, 0, NULL);
      if (rail_status != RAIL_STATUS_NO_ERROR) {
        app_log_warning("RAIL_StartRx() result:%d", rail_status);
      }
      app_state = S_IDLE;
      break;
    case S_PERIODIC_RX:
      handle_periodic_rx(rail_handle);
      break;
    case S_PERIODIC_TX:
      handle_periodic_tx(rail_handle);
      break;
    case S_SET_POWER_LEVEL:
      set_radio_to_idle_state(rail_handle);
      if (is_raw) {
        rail_status = RAIL_SetTxPower(rail_handle, power_raw);
        power_deci_dbm = RAIL_GetTxPowerDbm(rail_handle);
        if (rail_status != RAIL_STATUS_NO_ERROR) {
          app_log_warning("RAIL_SetTxPower() result:%d", rail_status);
        }
      } else {
        rail_status = RAIL_SetTxPowerDbm(rail_handle, power_deci_dbm);
        power_raw = RAIL_GetTxPower(rail_handle);
        if (rail_status != RAIL_STATUS_NO_ERROR) {
          app_log_warning("RAIL_SetTxPowerDbm() result:%d", rail_status);
        }
      }
      print_current_power_levels(rail_handle);
      app_state = S_IDLE;
      break;
    case S_GET_POWER_LEVEL:
      print_current_power_levels(rail_handle);
      app_state = S_IDLE;
      break;
    case S_PACKET_RECEIVED:
      handle_received_packet(rail_handle);
      break;
    default:
      break;
  }
  manage_sleep_levels();
}

/*******************************************************************************
 * Set state machine state out of this file
 *
 * @param next_state: the next state the machine will be in
 * @return None.
 ******************************************************************************/
void set_next_state(state_t next_state)
{
  app_state = next_state;
}

/*******************************************************************************
 * Set that the app can go to sleep
 *
 * @param None.
 * @return bool: if true app can go to sleep mode
 ******************************************************************************/
bool app_is_ok_to_sleep(void)
{
  return allow_to_sleep;
}

/*******************************************************************************
 * Set the flag for proper sleep level.
 * @param None.
 * @return None.
 ******************************************************************************/
void init_em1_mode(void)
{
  set_em1_mode();
}

/******************************************************************************
 * RAIL callback, called if a RAIL event occurs
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      rx_packet_handle = RAIL_HoldRxPacket(rail_handle);
      if (app_state == S_PERIODIC_RX) {
        periodic_receive = true;
      } else {
        periodic_receive = false;
      }
      app_state = S_PACKET_RECEIVED;
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      packet_sending = false;
    }
  }

  if (events & RAIL_EVENT_RX_SCHEDULED_RX_END) {
    rx_ended = true;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Print to CLI the current value of the TX power
 * @param rail_handle: where to get the TX power levels
 ******************************************************************************/
static void print_current_power_levels(RAIL_Handle_t rail_handle)
{
  if (is_raw) {
    app_log_info("Power            %d/%d\n",
                 power_raw,
                 (RAIL_GetTxPower(rail_handle)));
  } else {
    app_log_info("Power:            %.1f/%.1fdBm\n",
                 (float)(power_deci_dbm) / 10.0,
                 (float)(RAIL_GetTxPowerDbm(rail_handle)) / 10.0);
  }
}

/*******************************************************************************
 * Print out the newly selected sleep_mode
 ******************************************************************************/
static void print_new_sleep_mode(void)
{
  app_log_info("Idle mode; EM%d\n", sleep_mode);
}

/*******************************************************************************
 * Configure and starts RX to make it periodical
 * @param rail_handle: which rail_handle to use for receiving
 ******************************************************************************/
static void handle_periodic_rx(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  if (init_needed) {
    init_needed = false;
    set_radio_to_idle_state(rail_handle);
    shedule_rx_config.start = sleep_period;
    shedule_rx_config.end = rx_on_period;
    app_log_info(
      "Periodic Rx mode, sleepPeriod=%lu, rxPeriod=%lu; EM%d (sleep); EM%d (active)\n",
      shedule_rx_config.start, shedule_rx_config.end, sleep_mode,
      0);
  }
  if (rx_ended) {
    rx_ended = false;
    rail_status = RAIL_ScheduleRx(rail_handle, CHANNEL, &shedule_rx_config, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ScheduleRx() result:%d", rail_status);
    }
  }
}

/*******************************************************************************
 * Configure and starts TX to make it periodical
 * @param rail_handle: which rail_handle to use for sending
 ******************************************************************************/
static void handle_periodic_tx(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call
  uint16_t allocated_tx_fifo_size = 0;

  if (init_needed) {
    init_needed = false;
    set_radio_to_idle_state(rail_handle);
    shedule_tx_config.when = sleep_period;
    app_log_info("Periodic Tx mode, period=%lu; EM%d (sleep); EM%d (active)\n",
                 sleep_period, sleep_mode, 0);
  }

  if (!packet_sending) {
    allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, TX_PAYLOAD_LENGTH, RAIL_FIFO_SIZE);
    app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
               "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
               allocated_tx_fifo_size,
               RAIL_FIFO_SIZE);
    rail_status = RAIL_StartScheduledTx(rail_handle, 0, RAIL_TX_OPTIONS_DEFAULT, &shedule_tx_config, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_StartScheduledTx() result:%d", rail_status);
    }
    packet_sending = true;
  }
}

/*******************************************************************************
 * API to handle received packets
 *
 * @param rail_handle: the rail instance that the packet will be read from
 ******************************************************************************/
static void handle_received_packet(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status;
  // for received packets
  RAIL_RxPacketInfo_t packet_info;

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
  if (periodic_receive) {
    rx_ended = false;
    rail_status = RAIL_ScheduleRx(rail_handle, CHANNEL, &shedule_rx_config, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ScheduleRx() result:%d", rail_status);
    }
    app_state = S_PERIODIC_RX;
  } else {
    rail_status = RAIL_StartRx(rail_handle, CHANNEL, NULL);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_StartRx() result:%d", rail_status);
    }
    app_state = S_IDLE;
  }
}

/*******************************************************************************
 * The API forwards the received rx packet on CLI
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
 * The API selects the proper sleep level and set it for power_manager
 ******************************************************************************/
static void manage_sleep_levels(void)
{
  allow_to_sleep = true;
  switch (sleep_mode) {
    case 0:
      allow_to_sleep = false;
      break;
    case 1:
      set_em1_mode();
      break;
    case 2:
      clear_em1_mode();
      set_em2_mode();
      break;
    case 3:
      clear_em1_mode();
      clear_em2_mode();
      break;
    default:
      break;
  }
}

/*******************************************************************************
 * The API set the rail back to idle state
 ******************************************************************************/
static void set_radio_to_idle_state(RAIL_Handle_t rail_handle)
{
  RAIL_StopTxStream(rail_handle);
  RAIL_Idle(rail_handle, RAIL_IDLE, true);
}

/*******************************************************************************
 * Set flag and power manager to EM1 maximum sleep mode
 ******************************************************************************/
static void set_em1_mode(void)
{
  if (!em1_is_enabled) {
    em1_is_enabled = true;
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
  }
}

/*******************************************************************************
 * Clear flag and allow power manager to go lower then EM1
 ******************************************************************************/
static void clear_em1_mode(void)
{
  if (em1_is_enabled) {
    em1_is_enabled = false;
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
  }
}

/*******************************************************************************
 * Set flag and power manager to EM2 maximum sleep mode
 ******************************************************************************/
static void set_em2_mode(void)
{
  if (!em2_is_enabled) {
    em2_is_enabled = true;
    sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM2);
  }
}

/*******************************************************************************
 * Clear flag and allow power manager to go lower then EM2
 ******************************************************************************/
static void clear_em2_mode(void)
{
  if (em2_is_enabled) {
    em2_is_enabled = false;
    sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM2);
  }
}
