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
#include "em_system.h"
#include "sl_component_catalog.h"
#include "app_process.h"
#include "app_assert.h"
#include "app_log.h"
#include "rail.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
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
#include "sl_flex_packet_asm.h"
#include "em_system.h"
#include "em_core.h"
#include "demo-ui.h"
#include "sl_light_switch_support.h"
#include "printf.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// DEVICE_TYPE used in LCD functions
#define DEVICE_TYPE "Light"
/// Send broadcast message in every second
#define DEMO_LIGHT_STATUS_BROADCAST_INTERVAL    (1000000UL)
/// This bit indicates that the Light is in the advertise state, and send
/// Broadcast messages periodically
#define DEMO_CONTROL_CMD_ADVERTISE (0U)

/// this structure contains the Light module's details
typedef struct {
  uint8_t addr[8];
  light_mode_t mode;
  char* modeText[2];
  char modeTextBuf[10];
  bool state;
} light_t;
// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------
/**************************************************************************//**
 * Set the User LEDs according to the light state
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void set_LEDs();

/**************************************************************************//**
 * Used for display the module's unique ID
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void put_unique_ID_to_buffer();

/**************************************************************************//**
 * Put the actual light state in the payload, in order to send it wireless
 *
 * @param payload of the transmitting message
 * @param state of the light
 * @returns None
 *****************************************************************************/
static inline void set_light_state(uint8_t * payload, bool state);

/**************************************************************************//**
 * Display the protocol, light state, and unique ID
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void display_all_information();

/**************************************************************************//**
 * Callback function for the RAIL_SetTimer API, restart the RAIL timer
 *
 * @param None
 * @returns None
 *****************************************************************************/
static inline void broadcast_timer_expired();

/**************************************************************************//**
 * Write to CLI the change of the Light state
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void cli_state_machine_change();

/**************************************************************************//**
 * Write to CLI the change of the Light state
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void cli_light_side_light_bulb_toggle();

/**************************************************************************//**
 * The ADVERTISE state's function in the state machine
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void handle_advertise_state(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * The READY state's function in the state machine
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void handle_ready_state(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Receive the wireless packet, and save it in a buffer
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void save_received_packet(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Set the actual state in the transmit buffer
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void set_light_state_in_tx_fifo();

/**************************************************************************//**
 * Send a wireless pocket
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void transmit_packet(RAIL_Handle_t rail_handle);
/**************************************************************************//**
 * Copy the Light's address to the RX FIFO
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void copy_light_addr_to_tx_fifo();
/**************************************************************************//**
 * Write to CLI the change of the Switch state
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void cli_switch_side_light_bulb_toggle();
// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
//app_name used in LCD functions
const uint8_t app_name[6] = "Light";
// The variable shows the actual state of the state machine
volatile state_t state = S_ADVERTISE_STATE;

// Light bulb toggle required from from CLI command
bool cli_toggle_light_required = false;
// State change in the State machine required from CLI command
bool cli_change_state_required = false;

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
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

/// IEEE 802.15.4 frame structure for RX direction
sl_flex_ieee802154_frame_t rx_frame = { 0 };
/// IEEE 802.15.4 frame structure for TX direction
sl_flex_ieee802154_frame_t tx_frame = {
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

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
/// Go into transfer mode
static volatile bool light_state_broadcast = false;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Receive FIFO
static uint8_t rx_frame_buff[TX_PAYLOAD_LENGTH];

/// Receive FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];

/// Transmit FIFO
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[TX_PAYLOAD_LENGTH / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[TX_PAYLOAD_LENGTH];
} tx_payload = { .fifo = {
                   0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                   0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xFF, 0x00,
                 } };

static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_FIFO_SIZE];
} tx_fifo;

/// Transmit app buffer
static uint8_t tx_frame_buff[RAIL_FIFO_SIZE];

static light_t light = {
  .addr = { 0 },
  .mode = LIGHT_MODE_ADVERTISE,
  .modeText = { "ADVERT", "READY" },
  .modeTextBuf = { 0 },
  .state = false
};

//Send broadcast message periodically
static bool schedule_broadcast = true;
// Increase value if packet has received, decrease after process it
static uint8_t packet_received = 0;
// It shows if there was a transition between the state machine states
static bool state_changed = true;
// Light bulb toggle required from PB0 button push
static bool light_bulb_toggle_required = false;
// State change in the State machine required from PB1 button push
static bool state_change_required = false;
// Hold information about the incoming message
static RAIL_RxPacketInfo_t packet_info;
// Status indicator of the RAIL API calls
static RAIL_Status_t rail_status;
// Received message payload
static uint8_t payload = 0;

static volatile bool re_start_rx = false;
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
  if (current_rail_err != 0) {
    app_log_error("RAIL Error occurred\nEvents: %lld\n", current_rail_err);
    current_rail_err = 0;
  }

  if (re_start_rx) {
    re_start_rx = false;
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

  switch (state) {
    case S_ADVERTISE_STATE:
      handle_advertise_state(rail_handle);
      break;
    case S_READY_STATE:
      handle_ready_state(rail_handle);
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
      RAIL_HoldRxPacket(rail_handle);
      CORE_ATOMIC_SECTION(
        packet_received++;
        )
    } else {
      // Handle Rx error
      current_rail_err |= (events & RAIL_EVENTS_RX_COMPLETION);
    }
  }
  // Handle Tx events
  if ( events & RAIL_EVENTS_TX_COMPLETION) {
    if (!(events & RAIL_EVENT_TX_PACKET_SENT)) {
      // Handle Tx error
      current_rail_err |= (events & RAIL_EVENTS_TX_COMPLETION);
    } else {
      re_start_rx = true;
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
  // Check if any button was pressed
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (&sl_button_btn0 == handle) {
      light_bulb_toggle_required = true;
    }
    if (&sl_button_btn1 == handle) {
      state_change_required = true;
    }
  }
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

void handle_advertise_state(RAIL_Handle_t rail_handle)
{
  // Enter actual state, code just run once
  if (state_changed) {
    // Store the actual state
    light.mode = LIGHT_MODE_ADVERTISE;
    display_all_information();
    state_changed = false;
  }

  if (packet_received != 0) {
    CORE_ATOMIC_SECTION(
      packet_received--;
      )
    save_received_packet(rail_handle);
  }

  // Send broadcast message
  if (schedule_broadcast) {
    schedule_broadcast = false;
    RAIL_SetTimer(rail_handle,
                  DEMO_LIGHT_STATUS_BROADCAST_INTERVAL,
                  RAIL_TIME_DELAY,
                  &broadcast_timer_expired);
    // Send broadcast message
    transmit_packet(rail_handle);
  }

  // If CLI action occurred, move to next state
  if (cli_change_state_required) {
    cli_change_state_required = false;
    state = S_READY_STATE;
    state_changed = true;
    cli_state_machine_change();
  }

  // If button was pushed, move to next state
  if (state_change_required) {
    state = S_READY_STATE;
    state_changed = true;
    state_change_required = false;
    cli_state_machine_change();
  }
}

void handle_ready_state(RAIL_Handle_t rail_handle)
{
  // Enter actual state, code just runs once
  if (state_changed) {
    light.mode = LIGHT_MODE_READY;
    display_all_information();
    state_changed = false;
  }

  if (packet_received != 0) {
    CORE_ATOMIC_SECTION(
      packet_received--;
      )
    save_received_packet(rail_handle);
    // Save the control byte from the incoming message
    payload = rx_frame_buff[DEMO_CONTROL_PAYLOAD_BYTE];
    light.state = !light.state;
    display_all_information();
    set_LEDs();
    // Restart the timer with the 1s period
    schedule_broadcast = true;
    cli_switch_side_light_bulb_toggle();
  }
  // Send broadcast message
  if (schedule_broadcast) {
    schedule_broadcast = false;
    RAIL_SetTimer(rail_handle,
                  DEMO_LIGHT_STATUS_BROADCAST_INTERVAL,
                  RAIL_TIME_DELAY,
                  &broadcast_timer_expired);
    // Send broadcast message
    transmit_packet(rail_handle);
  }

  // If CLI action occurred, move to next state
  if (cli_change_state_required) {
    cli_change_state_required = false;
    state = S_ADVERTISE_STATE;
    state_changed = true;
    cli_state_machine_change();
  }

  // If CLI action occurred, toggle the light bulb
  if (cli_toggle_light_required) {
    cli_toggle_light_required = false;
    // Send out the light bulb's actual state
    transmit_packet(rail_handle);
    light.state = !light.state;
    display_all_information();
    set_LEDs();
    schedule_broadcast = true;
    cli_light_side_light_bulb_toggle();
  }

  // If PB1 button pushed, move to next state
  if (state_change_required) {
    state_change_required = false;
    state = S_ADVERTISE_STATE;
    state_changed = true;
    cli_state_machine_change();
  }

  // If PB0 button pushed, toggle the light bulb
  if (light_bulb_toggle_required) {
    light_bulb_toggle_required = false;
    light.state = !light.state;
    display_all_information();
    set_LEDs();
    // Send out the light bulb's actual state
    transmit_packet(rail_handle);
    cli_light_side_light_bulb_toggle();
  }
}

/******************************************************************************
 * Copy the Light's address to the TX FIFO
 *****************************************************************************/
void copy_light_addr_to_tx_fifo()
{
  memcpy((void*)&tx_payload.fifo[PACKET_HEADER_LEN], (void*)light.addr, sizeof(light.addr));
}

/******************************************************************************
 * Send a wireless pocket
 *****************************************************************************/
void transmit_packet(RAIL_Handle_t rail_handle)
{
  uint16_t packet_size;
  copy_light_addr_to_tx_fifo();
  // Set current light state.
  set_role(&tx_payload.fifo[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_ROLE_LIGHT);
  // Advertisement packet
  if (LIGHT_MODE_ADVERTISE == light.mode) {
    set_command_type(&tx_payload.fifo[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_CMD_ADVERTISE);
  } else {   // Status packet
    set_command_type(&tx_payload.fifo[DEMO_CONTROL_PAYLOAD_BYTE], LIGHT_STATE_REPORT);
    set_light_state(&tx_payload.fifo[DEMO_CONTROL_PAYLOAD_BYTE], light.state);
  }
  set_light_state_in_tx_fifo(rail_handle);
#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  // Prepare packet
  sl_flex_ieee802154_prepare_sending(&tx_frame,
                                     (uint8_t *)tx_payload.fifo,
                                     sizeof(tx_payload.fifo));
  // packs the data frame using the parameter information and the packed
  // frame is copied into the fifo
  (void)sl_flex_ieee802154_pack_data_frame(sl_flex_ieee802154_get_std(),
                                           &tx_frame,
                                           &packet_size,
                                           tx_frame_buff);

  // Send packet
  // sets the tx options based on the current ACK settings (auto-ACK enabled?)
  rail_status = sl_flex_ieee802154_transmission(rail_handle, tx_frame_buff, packet_size);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("sl_flex_ieee802154_transmission() status: %d \n", rail_status);
  }

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  sl_flex_ble_advertising_packet_t *ble_send_packet;
  // Prepare packet
  // set ble_send_packet pointer to tx app buff
  ble_send_packet = sl_flex_ble_get_packet(tx_frame_buff);
  sl_flex_ble_prepare_packet(ble_send_packet, tx_payload.fifo, sizeof(tx_payload.fifo));
  packet_size = sl_flex_ble_get_packet_size(ble_send_packet);
  // Send Packet
  rail_status = RAIL_WriteTxFifo(rail_handle, tx_frame_buff, packet_size, true);
  if (rail_status != packet_size) {
    app_log_warning("BLE RAIL_WriteTxFifo status: %d", rail_status);
  }
  rail_status = RAIL_StartTx(rail_handle, BLE_CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("BLE RAIL_StartTx status: %d", rail_status);
  }
#endif
}

/******************************************************************************
 * Write to CLI the change of the Light state
 *****************************************************************************/
static void cli_state_machine_change()
{
  char text_tmp[64];
  snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
           "State changing event at Light Node [",
           ((uint16_t)(SYSTEM_GetUnique() & 0x0000FFFF)),
           "]. ",
           ((light.mode == LIGHT_MODE_ADVERTISE) ? "Mode: READY\n" : "Mode: ADVERTISE\n"));
  app_log_info(text_tmp);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Write to CLI the change of the Switch state
 *****************************************************************************/
static void cli_light_side_light_bulb_toggle()
{
  char text_tmp[64];
  snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
           "Led Toggle event at Light Node [",
           ((uint16_t)(SYSTEM_GetUnique() & 0x0000FFFF)),
           "]. ",
           ((light.state == LIGHT_STATE_OFF) ? "Light Bulb is OFF\n" : "Light Bulb is ON\n"));
  app_log_info(text_tmp);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Write to CLI the change of the Switch state
 *****************************************************************************/
static void cli_switch_side_light_bulb_toggle()
{
  char text_tmp[64];
  snprintf(text_tmp, sizeof(text_tmp), "%s%s",
           "Led Toggle event at Switch Node ",
           ((light.state == LIGHT_STATE_OFF) ? "Light Bulb is OFF\n" : "Light Bulb is ON\n"));
  app_log_info(text_tmp);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Receive the wireless packet, and save it in a buffer
 *****************************************************************************/
static void save_received_packet(RAIL_Handle_t rail_handle)
{
  RAIL_RxPacketHandle_t rx_packet_handle;
  rx_packet_handle = RAIL_GetRxPacketInfo(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packet_info);
  if (rx_packet_handle == RAIL_RX_PACKET_HANDLE_INVALID) {
    app_log_error("RAIL_GetRxPacketInfo() error: RAIL_RX_PACKET_HANDLE_INVALID\n");
  }
  RAIL_CopyRxPacket(rx_fifo, &packet_info);
  rail_status = RAIL_ReleaseRxPacket(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_ReleaseRxPacket() result:%d \n", rail_status);
  }

#ifdef SL_CATALOG_FLEX_IEEE802154_SUPPORT_PRESENT
  int16_t res = SL_FLEX_IEEE802154_ERROR;
  res = sl_flex_ieee802154_unpack_data_frame(sl_flex_ieee802154_get_std(),
                                             &rx_frame,
                                             rx_fifo,
                                             RAIL_FIFO_SIZE);
  if (res != SL_FLEX_IEEE802154_OK) {
    app_log_warning("ieee802154 unpack failed with %d", res);
  }
  memcpy(rx_frame_buff, rx_frame.payload, TX_PAYLOAD_LENGTH);

#elif defined SL_CATALOG_FLEX_BLE_SUPPORT_PRESENT
  sl_flex_ble_advertising_packet_t *ble_recv_packet = NULL;
  ble_recv_packet = sl_flex_ble_get_packet(rx_fifo);
  if (ble_recv_packet == NULL) {
    app_log_warning("BLE received packet is NULL pointer");
  } else {
    memcpy(rx_frame_buff, sl_flex_ble_get_payload(ble_recv_packet), TX_PAYLOAD_LENGTH);
  }

#endif

  re_start_rx = true;
}

/******************************************************************************
 * Set the actual state in the transmit buffer
 *****************************************************************************/
static void set_light_state_in_tx_fifo()
{
  // Encode the actual state in the outgoing message
  switch (light.mode) {
    case LIGHT_MODE_ADVERTISE:
      tx_payload.fifo[DEVICE_STATUS_PAYLOAD_BYTE] &= ~0x03;
      break;
    case LIGHT_MODE_READY:
      tx_payload.fifo[DEVICE_STATUS_PAYLOAD_BYTE] |=  0x01;
      tx_payload.fifo[DEVICE_STATUS_PAYLOAD_BYTE] &= ~0x02;
      break;
  }
}

/******************************************************************************
 * Initialize the display at the beginning of the application
 *****************************************************************************/
void init_display()
{
  demoUIInit();
  set_EUI(light.addr);
  display_all_information();
}
// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------

/******************************************************************************
 * Display the app name, light state, and the ID of the connected Light node
 *****************************************************************************/
static void display_all_information()
{
  demoUIClearMainScreen((uint8_t *)app_name, true, false);
  demoUIDisplayLight(light.state);
  demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, false);
  demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t*)light.modeText[light.mode]);
  put_unique_ID_to_buffer();
  demoUIDisplayId(DEMO_UI_PROTOCOL2, (uint8_t*)light.modeTextBuf);
}

/******************************************************************************
 * Callback function for the RAIL_SetTimer API
 *****************************************************************************/
static inline void broadcast_timer_expired()
{
  light_state_broadcast = true;
  schedule_broadcast = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Set the User LEDs according to the light state
 *****************************************************************************/
static inline void set_LEDs()
{
  (light.state == false) \
  ? sl_led_turn_off(&sl_led_led0) : sl_led_turn_on(&sl_led_led0);
  (light.state == false) \
  ? sl_led_turn_off(&sl_led_led1) : sl_led_turn_on(&sl_led_led1);
}

/******************************************************************************
 * Used for display the module's unique ID
 *****************************************************************************/
static inline void put_unique_ID_to_buffer()
{
  snprintf(light.modeTextBuf,
           sizeof(light.modeTextBuf),
           "ID:%04X", *((uint16_t*)light.addr));
}

/******************************************************************************
 * Put the actual light state in the payload, in order to send it wireless
 *****************************************************************************/
static inline void set_light_state(uint8_t * payload, bool state)
{
  *payload &= ~DEMO_CONTROL_PAYLOAD_CMD_DATA;
  *payload |= state;
}
