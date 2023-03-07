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
#include "app_assert.h"
#include "app_log.h"
#include "rail.h"
#include "app_process.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "rail_config.h"
#include "sl_flex_packet_asm.h"

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

/// State machine of simple_trx
typedef enum {
  S_PACKET_RECEIVED,
  S_PACKET_SENT,
  S_RX_PACKET_ERROR,
  S_TX_PACKET_ERROR,
  S_CALIBRATION_ERROR,
  S_IDLE,
} state_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * The function printfs the received rx message.
 *
 * @param rx_buffer Msg buffer
 * @param length How many bytes should be printed out
 * @returns None
 *****************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer, uint16_t length);

/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *
 * @param rx_destination Where should the full packet be unpacked
 * @param packet_information Where should all the information of the packet stored
 * @param start_of_payload Pointer where the payload starts
 * @return The length of the received payload
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload);

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *
 * @param rail_handle Which rail handlers should be used for the TX FIFO writing
 * @param out_data The payload buffer
 * @param length The length of the payload
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length);

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
static volatile uint64_t error_code = 0;

/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;

/// Receive and Send FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];

static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_FIFO_SIZE];
} tx_fifo;

/// Transmit packet
static uint8_t out_packet[TX_PAYLOAD_LENGTH] = {
  0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
  0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
};

/// Flags to update state machine from interrupt
static volatile bool packet_recieved = false;
static volatile bool packet_sent = false;
static volatile bool rx_error = false;
static volatile bool tx_error = false;
static volatile bool cal_error = false;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // RAIL Rx packet handles
  RAIL_RxPacketHandle_t rx_packet_handle;
  RAIL_RxPacketInfo_t packet_info;
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;
  RAIL_Status_t calibration_status_buff = RAIL_STATUS_NO_ERROR;

  if (packet_recieved) {
    packet_recieved = false;
    state = S_PACKET_RECEIVED;
  } else if (packet_sent) {
    packet_sent = false;
    state = S_PACKET_SENT;
  } else if (rx_error) {
    rx_error = false;
    state = S_RX_PACKET_ERROR;
  } else if (tx_error) {
    tx_error = false;
    state = S_TX_PACKET_ERROR;
  } else if (cal_error) {
    cal_error = false;
    state = S_CALIBRATION_ERROR;
  }

  switch (state) {
    case S_PACKET_RECEIVED:
      // Packet received:
      //  - Check whether RAIL_HoldRxPacket() was successful, i.e. packet handle is valid
      //  - Copy it to the application FIFO
      //  - Free up the radio FIFO
      //  - Return to app IDLE state (RAIL will automatically switch back to Rx radio state)
      rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID) {
        uint8_t *start_of_packet = 0;
        uint16_t packet_size = unpack_packet(rx_fifo, &packet_info, &start_of_packet);
        rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
        if (rail_status != RAIL_STATUS_NO_ERROR) {
          app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
        }
        if (rx_requested) {
          printf_rx_packet(start_of_packet, packet_size);
        }
        sl_led_toggle(&sl_led_led0);
        rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
      }
      state = S_IDLE;
      break;
    case S_PACKET_SENT:
      app_log_info("Packet has been sent\n");
#if defined(SL_CATALOG_LED1_PRESENT)
      sl_led_toggle(&sl_led_led1);
#else
      sl_led_toggle(&sl_led_led0);
#endif
      state = S_IDLE;
      break;
    case S_RX_PACKET_ERROR:
      // Handle Rx error
      app_log_error("Radio RX Error occurred\nEvents: %llX\n", error_code);
      state = S_IDLE;
      break;
    case S_TX_PACKET_ERROR:
      // Handle Tx error
      app_log_error("Radio TX Error occurred\nEvents: %llX\n", error_code);
      state = S_IDLE;
      break;
    case S_IDLE:
      if (tx_requested) {
        prepare_package(rail_handle, out_packet, sizeof(out_packet));
        rail_status = RAIL_StartTx(rail_handle, CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
        if (rail_status != RAIL_STATUS_NO_ERROR) {
          app_log_warning("RAIL_StartTx() result:%d ", rail_status);
        }
        tx_requested = false;
      }
      break;
    case S_CALIBRATION_ERROR:
      calibration_status_buff = calibration_status;
      app_log_error("Radio Calibration Error occurred\nEvents: %llX\nRAIL_Calibrate() result:%d\n",
                    error_code,
                    calibration_status_buff);
      state = S_IDLE;
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
  error_code = events;
  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer, download it later at the state machine
      RAIL_HoldRxPacket(rail_handle);
      packet_recieved = true;
    } else {
      // Handle Rx error
      rx_error = true;
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      packet_sent = true;
    } else {
      // Handle Tx error
      tx_error = true;
    }
  }

  // Perform all calibrations when needed
  if ( events & RAIL_EVENT_CAL_NEEDED ) {
    calibration_status = RAIL_Calibrate(rail_handle, NULL, RAIL_CAL_ALL_PENDING);
    if (calibration_status != RAIL_STATUS_NO_ERROR) {
      cal_error = true;
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
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    tx_requested = true;
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Set up the rail TX fifo for later usage
 * @param rail_handle Which rail handler should be updated
 *****************************************************************************/
void set_up_tx_fifo(RAIL_Handle_t rail_handle)
{
  uint16_t allocated_tx_fifo_size = 0;
  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, 0, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             RAIL_FIFO_SIZE);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * The API forwards the received rx packet on CLI
 *****************************************************************************/
static void printf_rx_packet(const uint8_t * const rx_buffer, uint16_t length)
{
  app_log_info("Packet has been received: ");
  for (uint16_t i = 0; i < length; i++) {
    app_log_info("0x%02X, ", rx_buffer[i]);
  }
  app_log_info("\n");
}

#if defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_4FSK_9P6KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_169MHZ_2FSK_2P4KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_2FSK_4P8KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_450MHZ_4FSK_9P6KBPS) \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_896MHZ_2FSK_40KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_915MHZ_2FSK_10KBPS)  \
  || defined(RAIL0_CHANNEL_GROUP_1_PHY_IEEE802154_SUN_FSK_920MHZ_4FSK_400KBPS)
#undef RAIL0_CHANNEL_GROUP_1_PROFILE_BASE
#define RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK
#endif

#if defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FSK) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_FAN_1_0) || defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_HAN)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  sl_flex_802154_packet_mhr_frame_t rx_mhr = { 0 };
  uint16_t payload_size = 0;
  uint8_t rx_phr_config = 0U;
  RAIL_CopyRxPacket(rx_destination, packet_information);

  *start_of_payload = sl_flex_802154_packet_unpack_g_opt_data_frame(&rx_phr_config,
                                                                    &rx_mhr,
                                                                    &payload_size,
                                                                    rx_destination);
  return ((payload_size > (RAIL_FIFO_SIZE - SL_FLEX_IEEE802154_MHR_LENGTH)) ? (RAIL_FIFO_SIZE - SL_FLEX_IEEE802154_MHR_LENGTH) : payload_size);
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_phr_config = SL_FLEX_IEEE802154G_PHR_MODE_SWITCH_OFF
                          | SL_FLEX_IEEE802154G_PHR_CRC_4_BYTE
                          | SL_FLEX_IEEE802154G_PHR_DATA_WHITENING_ON;
  sl_flex_802154_packet_mhr_frame_t tx_mhr = {
    .frame_control          = MAC_FRAME_TYPE_DATA                \
                              | MAC_FRAME_FLAG_PANID_COMPRESSION \
                              | MAC_FRAME_DESTINATION_MODE_SHORT \
                              | MAC_FRAME_VERSION_2006           \
                              | MAC_FRAME_SOURCE_MODE_SHORT,
    .sequence_number        = 0U,
    .destination_pan_id     = (0xFFFF),
    .destination_address    = (0xFFFF),
    .source_address         = (0x0000)
  };
  uint8_t tx_frame_buffer[256];
  sl_flex_802154_packet_pack_g_opt_data_frame(tx_phr_config,
                                              &tx_mhr,
                                              length,
                                              out_data,
                                              &packet_size,
                                              tx_frame_buffer);
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}
#elif defined(RAIL0_CHANNEL_GROUP_1_PROFILE_WISUN_OFDM)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination,
                              const RAIL_RxPacketInfo_t *packet_information,
                              uint8_t **start_of_payload)
{
  uint16_t payload_size = 0U;
  uint8_t rate = 0U;
  uint8_t scrambler = 0U;

  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = sl_flex_802154_packet_unpack_ofdm_data_frame(packet_information,
                                                                   &rate,
                                                                   &scrambler,
                                                                   &payload_size,
                                                                   rx_destination);
  return payload_size;
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_frame_buffer[256];
  uint8_t rate = 0x06;     // rate: 5 bits wide, The Rate field (RA4-RA0) specifies the data rate of the payload and is equal to the numerical value of the MCS
                           // 0x0 BPSK, coding rate 1/2, 4 x frequency repetition
                           // 0x1 BPSK, coding rate 1/2, 2 x frequency repetition
                           // 0x2 QPSK, coding rate 1/2, 2 x frequency repetition
                           // 0x3 QPSK, coding rate 1/2
                           // 0x4 QPSK, coding rate 3/4
                           // 0x5 16-QAM, coding rate 1/2
                           // 0x6 16-QAM, coding rate 3/4
  uint8_t scrambler = 0; // scrambler: 2 bits wide, The Scrambler field (S1-S0) specifies the scrambling seed

  sl_flex_802154_packet_pack_ofdm_data_frame(rate,
                                             scrambler,
                                             length,
                                             out_data,
                                             &packet_size,
                                             tx_frame_buffer);
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}

#elif defined(RAIL0_CHANNEL_GROUP_1_PROFILE_SUN_OQPSK)
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  uint16_t payload_size = 0U;
  bool spreadingMode = false;
  uint8_t rateMode = 0U;

  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = sl_flex_802154_packet_unpack_oqpsk_data_frame(packet_information,
                                                                    &spreadingMode,
                                                                    &rateMode,
                                                                    &payload_size,
                                                                    rx_destination);
  return payload_size;
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  uint16_t packet_size = 0U;
  uint8_t tx_frame_buffer[256];
  bool spreadingMode = false;
  uint8_t rateMode = 0; // rateMode: 2 bits wide

  sl_flex_802154_packet_pack_oqpsk_data_frame(spreadingMode,
                                              rateMode,
                                              length,
                                              out_data,
                                              &packet_size,
                                              tx_frame_buffer);

  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, tx_frame_buffer, packet_size, true);
  app_assert(bytes_writen_in_fifo == packet_size,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             packet_size);
}
#else
/******************************************************************************
 * The API helps to unpack the received packet, point to the payload and returns the length.
 *****************************************************************************/
static uint16_t unpack_packet(uint8_t *rx_destination, const RAIL_RxPacketInfo_t *packet_information, uint8_t **start_of_payload)
{
  RAIL_CopyRxPacket(rx_destination, packet_information);
  *start_of_payload = rx_destination;
  return ((packet_information->packetBytes > RAIL_FIFO_SIZE) ? RAIL_FIFO_SIZE : packet_information->packetBytes);
}

/******************************************************************************
 * The API prepares the packet for sending and load it in the RAIL TX FIFO
 *****************************************************************************/
static void prepare_package(RAIL_Handle_t rail_handle, uint8_t *out_data, uint16_t length)
{
  // Check if write fifo has written all bytes
  uint16_t bytes_writen_in_fifo = 0;
  bytes_writen_in_fifo = RAIL_WriteTxFifo(rail_handle, out_data, length, true);
  app_assert(bytes_writen_in_fifo == TX_PAYLOAD_LENGTH,
             "RAIL_WriteTxFifo() failed to write in fifo (%d bytes instead of %d bytes)\n",
             bytes_writen_in_fifo,
             TX_PAYLOAD_LENGTH);
}
#endif
