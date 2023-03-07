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
#include "em_system.h"
#include "sl_component_catalog.h"
#include "app_process.h"
#include "app_assert.h"
#include "app_log.h"
#include "rail.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "sl_flex_packet_asm.h"
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  #include "sl_flex_util_802154_protocol_types.h"
  #include "sl_flex_util_802154_init_config.h"
  #include "sl_flex_rail_ieee802154_config.h"
  #include "sl_flex_ieee802154_support.h"
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  #include "sl_flex_util_ble_protocol_config.h"
  #include "sl_flex_util_ble_init_config.h"
#else
#endif

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// Size of the FIFO buffers
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  #define RAIL_FIFO_SIZE                    (SL_FLEX_RAIL_FRAME_MAX_SIZE)
#else
  #define RAIL_FIFO_SIZE                    (128)
#endif
/// Size of sending data
#define RAIL_PAYLOAD_DATA_SIZE            (17U)
#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/// BLE channel number
  #define BLE_CHANNEL ((uint8_t) 0)
#endif

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Starts listening for radio packets.
 *
 * @param[in] rail_handle     Handle to the RAIL context
 * @return None
 *****************************************************************************/
static void start_receiving(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Transmits the data packet.
 *
 * @param[in] rail_handle Handle to the RAIL context
 * @param[in] packet      The packet that is desired to send
 * @param[in] rail_handle The size of the packet
 * @return None
 *****************************************************************************/
static void handle_transmit(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Checks the received packet (data or ACK).
 *
 * @param rail_handle     Handle to the RAIL context
 * @returns RAIL_RxPacketHandle_t
 *****************************************************************************/
static void handle_receive(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Handle errors detected in RAIL events.
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void handle_error_state(void);

#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/**************************************************************************//**
 * Print BLE packet
 *
 * @param[in] sl_flex_ble_advertising_packet_t  BLE packet pointer
 * @returns void
 *****************************************************************************/
static void printf_ble_packet(const sl_flex_ble_advertising_packet_t *packet);

/**************************************************************************//**
 * Print BLE received data
 *
 * @param[in] rx_buffer  BLE received data pointer
 * @param[in] length  length of BLE received data
 * @returns void
 *****************************************************************************/
static void printf_ble_recv_payload(const uint8_t * const rx_buffer, uint16_t length);
#endif

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
/// Flag, indicating transmit request (button was pressed
/// / CLI transmit request has occurred)
volatile bool tx_requested = false;
/// Flag, indicating received packet is forwarded on CLI or not
volatile bool rx_requested = true;
/// CLI requests comes via CLI
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
volatile sl_flex_ieee802154_cli_requests cli_requests = { 0 };
/// CLI requests' desired settings/values for changing the application
volatile sl_flex_ieee802154_cli_data cli_desired_settings;
/// Status of the application
sl_flex_ieee802154_status_t comm_status = {
  .ack = false,
  .auto_ack = false,
  .crc_length = 0,
  .channel = 0,
  .data_whitening = false,
  .destination_address = 0,
  .destination_pan_id = 0,
  .source_address = 0,
  .std = SL_FLEX_IEEE802154_STD_IEEE802154_2P4GHZ
};
#endif
// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
/// IEEE 802.15.4 frame structure for RX direction
static sl_flex_ieee802154_frame_t rx_frame = { 0 };
/// IEEE 802.15.4 frame structure for TX direction
static sl_flex_ieee802154_frame_t tx_frame = {
  .mhr_config = {
    .frame_control          = DEFAULT_FCF_FIELD,
    .sequence_number        = 0U,
    .destination_pan_id     = DEFAULT_BROADCAST_PAN_ID,
    .destination_address    = DEFAULT_BROADCAST_DEST_ADDR,
    .source_address         = DEFAULT_BROADCAST_SRC_ADDR
  },
  .phr_config = SL_FLEX_IEEE802154G_PHR_MODE_SWITCH_OFF
                | SL_FLEX_IEEE802154G_PHR_CRC_2_BYTE
                | APP_WHITENING_ON_OFF,
};
#endif

/// The variable shows the actual state of the state machine
static volatile state_t state = S_IDLE;
/// Contains the status of RAIL Calibration
static volatile RAIL_Status_t calibration_status = 0;
/// RAIL Rx packet handle
// static volatile RAIL_RxPacketHandle_t rx_packet_handle;
/// Receive app buffer
static uint8_t rx_app_buff[RAIL_FIFO_SIZE];
/// Transmit FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];
/// Transmit FIFO
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_FIFO_SIZE];
} tx_fifo;
/// Transmit app buffer
static uint8_t tx_app_buff[RAIL_FIFO_SIZE];
/// Sending data (payload)
static const uint8_t tx_payload_data[RAIL_PAYLOAD_DATA_SIZE] = {
  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
  0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
};
/// Notify RAIL Tx, Rx calibration error
static bool rail_error = false;
/// Copy of last RAIL events to process
static RAIL_Events_t rail_last_state = RAIL_EVENTS_NONE;
/// Notify end of packet transmission
static bool rail_packet_sent = false;
/// Notify reception of packet
static bool rail_packet_received = false;
/// Request start receiving
static bool start_rx = true;

// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Initialization of RAIL TX FIFO
 *****************************************************************************/
int16_t app_set_rail_tx_fifo(RAIL_Handle_t rail_handle)
{
  // RAIL FIFO size allocated by RAIL_SetTxFifo() call
  uint16_t allocated_tx_fifo_size = 0;

  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo,
                                          0U, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough fifo"
             " (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size, RAIL_FIFO_SIZE);
  if (allocated_tx_fifo_size != RAIL_FIFO_SIZE) {
    return -1;
  }

  return 0;
}

/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  // Handle errors if pending
  if (rail_error) {
    rail_error = false;
    state = S_ERROR;
  }

  switch (state) {
    case S_INIT:
      /* IEEE 802.15.4 Application init*/
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
      // initializes the IEEE 802154 according to desired changes;
      // updates the status
      sl_flex_ieee802154_request_manager(rail_handle, &tx_frame, &cli_requests,
                                         &cli_desired_settings);
      // updates the status for changes
      sl_flex_ieee802154_update_status(&comm_status, &tx_frame);
#endif
      state = S_IDLE;
      start_rx = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;

    case S_IDLE:
      if (start_rx) {
        start_receiving(rail_handle);
        start_rx = false;
      }

      if (tx_requested) {
        state = S_TRANSMIT;
        tx_requested = false;
#if defined(SL_CATALOG_KERNEL_PRESENT)
        app_task_notify();
#endif
      }

      if (rail_packet_sent) {
        rail_packet_sent = false;
        app_log_info("Packet has been sent\n");
        // toggle when the TX packet sent
#if defined(SL_CATALOG_LED1_PRESENT)
        sl_led_toggle(&sl_led_led1);
#endif
      }

      if (rail_packet_received) {
        rail_packet_received = false;
        state = S_RECEIVE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
        app_task_notify();
#endif
      }

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
      // if any change comes, it goes into S_INIT state and performs the changes
      if (sl_flex_ieee802154_is_change_requested(&cli_requests)) {
        RAIL_Idle(rail_handle, RAIL_IDLE_ABORT, true);
        state = S_INIT;
#if defined(SL_CATALOG_KERNEL_PRESENT)
        app_task_notify();
#endif
      }
#endif
      break;

    case S_TRANSMIT:
      // sends the packet for IEEE 802.15.4 and BLE standards
      handle_transmit(rail_handle);
      tx_requested = false;
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;

    case S_RECEIVE:
      // receive the packet for IEEE 802.15.4 and BLE standards
      handle_receive(rail_handle);
#if defined(SL_CATALOG_LED0_PRESENT)
      sl_led_toggle(&sl_led_led0);
#endif
      start_rx = true;
      state = S_IDLE;
#if defined(SL_CATALOG_KERNEL_PRESENT)
      app_task_notify();
#endif
      break;

    case S_ERROR:
      // calls the error handle to print the reason and id of the error.
      handle_error_state();
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

void app_process_init(RAIL_Handle_t rail_handle)
{
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  sl_flex_ieee802154_protocol_init(rail_handle, SL_FLEX_UTIL_INIT_PROTOCOL_INSTANCE_DEFAULT);
  // updates the status for changes
  sl_flex_ieee802154_update_status(&comm_status, &tx_frame);
#else
  (void)rail_handle;
#endif
}

/******************************************************************************
 * RAIL callback, called while the RAIL is initializing.
 *****************************************************************************/
RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle)
{
  uint16_t rxFifoSize = RAIL_FIFO_SIZE;
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, &rx_fifo[0], &rxFifoSize);
  if (rxFifoSize != RAIL_FIFO_SIZE) {
    // We set up an incorrect FIFO size
    return RAIL_STATUS_INVALID_PARAMETER;
  }
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    return RAIL_STATUS_NO_ERROR;
  }
  return status;
}

/******************************************************************************
 * RAIL callback, called if any RAIL event occurs.
 *****************************************************************************/
void sl_rail_util_on_event(RAIL_Handle_t rail_handle, RAIL_Events_t events)
{
  // Make a copy of the events
  rail_last_state = events;

  // Handle Rx events
  if ( events & RAIL_EVENTS_RX_COMPLETION ) {
    if (events & RAIL_EVENT_RX_PACKET_RECEIVED) {
      // Keep the packet in the radio buffer,
      // download it later at the state machine
      (void)RAIL_HoldRxPacket(rail_handle);
      rail_packet_received = true;
    } else {
      rail_error = true;
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (events & RAIL_EVENT_TX_PACKET_SENT) {
      rail_packet_sent = true;
    } else {
      rail_error = true;
    }
  }

  if (events & RAIL_EVENTS_TXACK_COMPLETION ) {
    if (events & RAIL_EVENT_TXACK_PACKET_SENT) {
    } else {
      rail_error = true;
    }
  }

  // Perform all calibrations when needed
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

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/*******************************************************************************
 * Use RAIL to transmit a data packet
 ******************************************************************************/
static void handle_transmit(RAIL_Handle_t rail_handle)
{
  RAIL_Status_t status;
#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  sl_flex_ble_advertising_packet_t *ble_send_packet;
#endif
  uint16_t packet_size;

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  // Prepare packet
  sl_flex_ieee802154_prepare_sending(&tx_frame,
                                     (uint8_t *)tx_payload_data,
                                     sizeof(tx_payload_data));
  // packs the data frame using the parameter information and the packed
  // frame is copied into the fifo
  (void)sl_flex_ieee802154_pack_data_frame(sl_flex_ieee802154_get_std(),
                                           &tx_frame,
                                           &packet_size,
                                           tx_app_buff);

  // Send packet
  // sets the tx options based on the current ACK settings (auto-ACK enabled?)
  status = sl_flex_ieee802154_transmission(rail_handle, tx_app_buff, packet_size);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("sl_flex_ieee802154_transmission() status: %d", status);
  }

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  // Prepare packet
  // set ble_send_packet pointer to tx app buff
  ble_send_packet = sl_flex_ble_get_packet(tx_app_buff);
  sl_flex_ble_prepare_packet(ble_send_packet, tx_payload_data, sizeof(tx_payload_data));
  packet_size = sl_flex_ble_get_packet_size(ble_send_packet);
  printf_ble_packet(ble_send_packet);

  // Send Packet
  status = RAIL_WriteTxFifo(rail_handle, tx_app_buff, packet_size, true);
  if (status != packet_size) {
    app_log_warning("BLE RAIL_WriteTxFifo status: %d", status);
  }
  status = RAIL_StartTx(rail_handle, BLE_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if (status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("BLE RAIL_StartTx status: %d", status);
  }
  #else
#endif
}

/*******************************************************************************
 * Use RAIL to start listening for radio packets
 ******************************************************************************/
static void start_receiving(RAIL_Handle_t rail_handle)
{
  // Status indicator of the RAIL API calls
  RAIL_Status_t rail_status = RAIL_STATUS_NO_ERROR;

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  rail_status = RAIL_StartRx(rail_handle, sl_flex_ieee802154_get_channel(), NULL);
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  rail_status = RAIL_StartRx(rail_handle, BLE_CHANNEL, NULL);
#else
#endif
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_StartRx() result:%d",
                    rail_status);
  }
}

/*******************************************************************************
 * Handle received packets in loop
 ******************************************************************************/
static void handle_receive(RAIL_Handle_t rail_handle)
{
  RAIL_RxPacketHandle_t rx_packet_handle;
  RAIL_RxPacketDetails_t packet_details;
  RAIL_RxPacketInfo_t packet_info;
  RAIL_Status_t rail_status;
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  int16_t res = SL_FLEX_IEEE802154_ERROR;
#endif
#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  sl_flex_ble_advertising_packet_t *ble_recv_packet = NULL;
#endif

  do {
    rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);

    if (rx_packet_handle == RAIL_RX_PACKET_HANDLE_INVALID) {
      break;
    }

    (void)RAIL_GetRxPacketDetails(rail_handle, rx_packet_handle, &packet_details);
    // copies the data in the RX Buffer
    RAIL_CopyRxPacket(rx_app_buff, &packet_info);
    // after the copy of the packet, the RX packet can be release for RAIL
    rail_status = RAIL_ReleaseRxPacket(rail_handle, rx_packet_handle);
    if (rail_status != RAIL_STATUS_NO_ERROR) {
      app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
    }

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
    // Is it a ACK or a data frame
    if (packet_details.isAck) {
      app_log_info("ACK received\n");
      // prints the received ACK frame
      sl_flex_ieee802154_print_ack(sl_flex_ieee802154_get_std(), rx_app_buff);
    } else {
      // unpack the IEEE802154 frame
      res = sl_flex_ieee802154_unpack_data_frame(sl_flex_ieee802154_get_std(),
                                                 &rx_frame,
                                                 rx_app_buff,
                                                 RAIL_FIFO_SIZE);
      // prints the received data based on the standard
      if (rx_requested && (res == SL_FLEX_IEEE802154_OK)) {
        sl_flex_ieee802154_print_frame(sl_flex_ieee802154_get_std(), &rx_frame);
        app_log_info("\n");
      }
    }
#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT

    ble_recv_packet = sl_flex_ble_get_packet(rx_app_buff);
    if (ble_recv_packet == NULL) {
      if (ble_recv_packet == NULL) {
        app_log_warning("BLE received packet is NULL pointer");
      }
    } else {
      if (rx_requested) {
        printf_ble_packet(ble_recv_packet);
      }
    }

    if (rx_requested) {
      // Getting payload and print it
      printf_ble_recv_payload(sl_flex_ble_get_payload(ble_recv_packet),
                              sl_flex_ble_get_payload_len(ble_recv_packet));
    }
#endif
  } while (rx_packet_handle != RAIL_RX_PACKET_HANDLE_INVALID);
}

/*******************************************************************************
 * This helper function handles the S_ERROR state of the state machine.
 ******************************************************************************/
static void handle_error_state(void)
{
  // Handle Rx error
  if (rail_last_state & RAIL_EVENTS_RX_COMPLETION) {
    app_log_error("Radio RX Error occurred\nEvents: %lld\n", rail_last_state);
    // Handle Tx error
  } else if (rail_last_state & RAIL_EVENTS_TX_COMPLETION) {
    app_log_error("Radio TX Error occurred\nEvents: %lld\n", rail_last_state);
    // Handle calibration error
  } else if (rail_last_state & RAIL_EVENT_CAL_NEEDED) {
    app_log_warning("Radio Calibr. Error occurred\nEvents: %lld\nRAIL_Calibrate() result:%d\n",
                    rail_last_state,
                    calibration_status);
  } else if (rail_last_state & RAIL_EVENTS_TXACK_COMPLETION) {
    app_log_error("ACK TX Error occurred\nEvents: %lld\n", rail_last_state);
  }
  start_rx = true;
}

#ifdef SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
/*******************************************************************************
 * Print BLE packet
 ******************************************************************************/
static void printf_ble_packet(const sl_flex_ble_advertising_packet_t *packet)
{
  uint32_t packet_size = sl_flex_ble_get_packet_size(packet);
  uint32_t payload_len = sl_flex_ble_get_payload_len(packet);
  uint8_t *packet_byte;
  app_log_info("BLE Packet: (size = %d) (payload_len = %d) {\n", packet_size, payload_len);
  for (uint32_t i = 0; i < packet_size; ++i) {
    packet_byte = (uint8_t *)packet + i;
    app_log_info("[%d] -> 0x%02X\n", i, *packet_byte);
  }
  app_log_info("}\n");
}

/*******************************************************************************
 * Print BLE packet
 ******************************************************************************/
static void printf_ble_recv_payload(const uint8_t * const rx_buffer, uint16_t length)
{
  uint8_t i = 0;
  app_log_info("BLE Packet has been received. Payload (%d): ", length);
  for (i = 0; i < length; i++) {
    app_log_info("0x%02X, ", rx_buffer[i]);
  }
  app_log_info("\n");
}
#endif
