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
#include "printf.h"
#include "sl_component_catalog.h"
#include "app_assert.h"
#include "app_log.h"
#include "rail.h"
#include "app_process.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_led_instances.h"
#include "demo-ui.h"
#include "em_system.h"
#include "em_core.h"
#include "sl_light_switch_support.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "app_task_init.h"
#endif

// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/// DEVICE_TYPE used in LCD functions
#define DEVICE_TYPE "Switch"

// Variable for keeping the states consistent in both devices
/// this structure contains the Light module's details
typedef struct {
  uint8_t addr[8];
  int8_t rssi;
  light_state_t light_state;
  demo_control_command_type_t light_mode;
  light_mode_t communication_state;
} light_t;

/// This structure contains the Switch module's details
typedef struct {
  switch_mode_t mode;
  char *switch_text[2];
  char switch_text_buffer[10];
  bool is_paired;
} switch_t;

// -----------------------------------------------------------------------------
//                          Static Function Declarations
// -----------------------------------------------------------------------------

/**************************************************************************//**
 * Prepare the communication state and the Light's ID in order to display it
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void write_ID_to_buffer();

/**************************************************************************//**
 * Copy the Light's address to the TX FIFO
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void copy_light_address_to_tx_Fifo();

/**************************************************************************//**
 * Check if an advertise message come from the Light device
 *
 * @param None
 * @returns the advertisement type came from the Light device
 *****************************************************************************/
static demo_control_command_type_t get_light_response_type();

/**************************************************************************//**
 * Get light mode from the rx_fifo
 *
 * @param FIFO with the received message
 * @returns the light's state: READY or ADVERT
 *****************************************************************************/
static light_state_t get_light_mode(uint8_t* rx_fifo);

/**************************************************************************//**
 * Update the RSSI an ID values of the Light module
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void update_light_RSSI(void);

/**************************************************************************//**
 * The SCAN state's function in the state machine
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void  handle_scan_state(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * The LINKED state's function in the state machine
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void  handle_linked_state(RAIL_Handle_t rail_handle);

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
static void set_switch_state_in_tx_fifo();

/**************************************************************************//**
 * Display the app name, light state, and the ID of the connected Light node
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void display_all_information();

/**************************************************************************//**
 * Send a wireless pocket
 *
 * @param rail_handle
 * @returns None
 *****************************************************************************/
static void transmit_packet(RAIL_Handle_t rail_handle);

/**************************************************************************//**
 * Get the communication state of the Light node, if possible (if it is a new Light)
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void get_light_state_from_rx_fifo();

/**************************************************************************//**
 * Check if the Light node is "soc-light_rail-dmp"
 *
 * @param return FALSE if it is a "soc-light_rail-dmp", TRUE anyway
 * @returns None
 *****************************************************************************/
static bool is_that_a_new_light();

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
 * Write to CLI the change of the Switch state
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void cli_switch_side_light_bulb_toggle();

/**************************************************************************//**
 * Check if the Switch and Light nodes are connected
 *
 * @param None
 * @returns None
 *****************************************************************************/
static void check_paired_state();

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------
// Light bulb toggle required from from CLI command
bool cli_toggle_light_required = false;
//State change in the State machine required from CLI command
bool cli_change_state_required = false;

// -----------------------------------------------------------------------------
//                                Static Variables
// -----------------------------------------------------------------------------
static light_t light_module = {
  .addr = { 0 },
  .rssi = -128,
  .light_state = LIGHT_STATE_OFF,
};

static switch_t switch_module = {
  .mode = SWITCH_MODE_SCAN,
  .switch_text = { "SCAN:", "LINK:" },
  .is_paired = false,
};

/// The variable shows the actual state of the state machine
state_t state = S_SCAN_STATE;

/// Contains the last RAIL Rx/Tx error events
static volatile uint64_t current_rail_err = 0;

/// Contains the received packet's details
static RAIL_RxPacketDetails_t rxPacketDetails;

/// Receive FIFO
static uint8_t rx_fifo[RAIL_FIFO_SIZE];

/// Transmit FIFO
static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[RAIL_FIFO_SIZE];
} tx_fifo = { .fifo = {
                0x0F, 0x16, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
                0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0x00, 0x00,
              } };

/// App_name used in LCD functions
static const uint8_t app_name[7] = "Switch";
// Increase value if packet has received, decrease after process it
static uint8_t packet_received = 0;
// It shows if there was a transition between the state machine states
static bool state_changed = true;
// Light bulb toggle required from PB0 button push
static bool light_bulb_toggle_required = false;
// State change in the State machine required from PB1 button push
static bool state_change_required = false;
// Indicates a button push on the board
static bool button_was_pushed = false;
// Hold information about the incoming message
static RAIL_RxPacketInfo_t packet_info;
// Status indicator of the RAIL API calls
static RAIL_Status_t rail_status;
// RAIL FIFO size allocated by RAIL_SetTxFifo() call
static uint16_t allocated_tx_fifo_size = 0;
// -----------------------------------------------------------------------------
//                          Public Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Application state machine, called infinitely
 *****************************************************************************/
void app_process_action(RAIL_Handle_t rail_handle)
{
  if (current_rail_err != 0) {
    app_log_error("RAIL Error occurred\nEvents: %lld\n", current_rail_err);
    current_rail_err = 0;
  }

  switch (state) {
    case S_SCAN_STATE:
      handle_scan_state(rail_handle);
      break;
    case S_LINK_STATE:
      handle_linked_state(rail_handle);
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

/******************************************************************************
 * Initialize the display at the beginning of the application
 *****************************************************************************/
void init_display()
{
  demoUIInit();
  display_all_information();
  set_EUI(&tx_fifo.fifo[PACKET_HEADER_LEN]);
}

// -----------------------------------------------------------------------------
//                          Static Function Definitions
// -----------------------------------------------------------------------------
/******************************************************************************
 * Handle SCAN-state related tasks
 *****************************************************************************/
static void handle_scan_state(RAIL_Handle_t rail_handle)
{
  // Enter actual state, code just runs once
  if (state_changed) {
    // Store the actual state
    switch_module.mode = SWITCH_MODE_SCAN;
    display_all_information();
    check_paired_state();
    state_changed = false;
  }

  if (packet_received != 0) {
    CORE_ATOMIC_SECTION(
      packet_received--;
      )
    save_received_packet(rail_handle);
    // Get the highest Light node based on the  Received Signal Strength Indicator
    update_light_RSSI();
    // Display the data
    display_all_information();
    // Check if is there a new version of light
    if (is_that_a_new_light()) {
      get_light_state_from_rx_fifo();
    }
  }

  // If CLI action occurred, move to next state
  if (cli_change_state_required) {
    cli_change_state_required = false;
    cli_state_machine_change();
    state = S_LINK_STATE;
    state_changed = true;
    state_change_required = false;
  }
  // If button was pushed, move to next state
  if (state_change_required) {
    state = S_LINK_STATE;
    state_changed = true;
    state_change_required = false;
    cli_state_machine_change();
  }
}

/******************************************************************************
 * Handle LINKED-state related tasks
 *****************************************************************************/
static void handle_linked_state(RAIL_Handle_t rail_handle)
{
  // Enter actual state, code just runs once
  if (state_changed) {
    // Store the actual state
    switch_module.mode = SWITCH_MODE_LINKED;
    display_all_information();
    check_paired_state();
    state_changed = false;
  }
  // If there is any received message, process it
  if (packet_received != 0) {
    CORE_ATOMIC_SECTION(
      packet_received--;
      )
    save_received_packet(rail_handle);
    get_light_state_from_rx_fifo();
    check_paired_state();
    // If Switch side button push happened
    if (button_was_pushed) {
      cli_switch_side_light_bulb_toggle();
      // If not, but according to the incoming message,
      // light bulb should toggle
    } else if (light_module.light_state != get_light_mode(rx_fifo)) {
      cli_light_side_light_bulb_toggle();
    }
    button_was_pushed = false;
    light_module.light_state = get_light_mode(rx_fifo);
    display_all_information();
  }

  // If CLI action occurred, move to next state
  if (cli_change_state_required) {
    cli_change_state_required = false;
    cli_state_machine_change();
    state = S_SCAN_STATE;
    state_changed = true;
    state_change_required = false;
  }

  // If CLI action occurred, toggle the light bulb
  if (cli_toggle_light_required) {
    cli_toggle_light_required = false;
    button_was_pushed = true;
    transmit_packet(rail_handle);
    display_all_information();
  }

  // If PB1 button pushed, move to next state
  if (state_change_required) {
    state = S_SCAN_STATE;
    state_changed = true;
    state_change_required = false;
    cli_state_machine_change();
  }
  // If PB0 button pushed, toggle the light bulb
  if (light_bulb_toggle_required) {
    transmit_packet(rail_handle);
    button_was_pushed = true;
    light_bulb_toggle_required = false;
  }
}

/******************************************************************************
 * Check if the Switch and Light nodes are connected
 *****************************************************************************/
static void check_paired_state()
{
  // If Light node is in READY mode and Switch node is in LINKED mode
  bool is_paired = ((rx_fifo[DEVICE_STATUS_PAYLOAD_BYTE] & 0x01) && (switch_module.mode == SWITCH_MODE_LINKED));
  if (switch_module.is_paired != is_paired) {
    switch_module.is_paired = is_paired;
    char text_tmp[32];
    snprintf(text_tmp, sizeof(text_tmp), "%s%s%s",
             "NODES ARE IN ",
             ((is_paired == true) ? "PAIRED " : "NOT PAIRED "),
             "STATE\n");
    app_log_info(text_tmp);
  }
}
/******************************************************************************
 * Write to CLI the change of the Light state
 *****************************************************************************/
static void cli_state_machine_change()
{
  char text_tmp[64];
  snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
           "State changing event at Switch Node [",
           ((uint16_t)(SYSTEM_GetUnique() & 0x0000FFFF)),
           "]. ",
           ((switch_module.mode == SWITCH_MODE_SCAN) ? "Mode: LINK\n" : "Mode: SCAN\n"));
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
  snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
           "Led Toggle event at Switch Node [",
           ((uint16_t)(SYSTEM_GetUnique() & 0x0000FFFF)),
           "]. ",
           ((light_module.light_state == LIGHT_STATE_OFF) ? "Light Bulb is ON\n" : "Light Bulb is OFF\n"));
  app_log_info(text_tmp);
  button_was_pushed = true;
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Write to CLI the change of the Light state
 *****************************************************************************/
static void cli_light_side_light_bulb_toggle()
{
  char text_tmp[64];
  snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
           "Led Toggle event at Light Node [",
           *((uint16_t*)light_module.addr),
           "]. ",
           ((light_module.light_state == LIGHT_STATE_OFF) ? "Light Bulb is ON\n" : "Light Bulb is OFF\n"));
  app_log_info(text_tmp);
#if defined(SL_CATALOG_KERNEL_PRESENT)
  app_task_notify();
#endif
}

/******************************************************************************
 * Send a wireless pocket
 *****************************************************************************/
static void transmit_packet(RAIL_Handle_t rail_handle)
{
  // Encode the control role in the message
  set_role(&tx_fifo.fifo[DEMO_CONTROL_PAYLOAD_BYTE], DEMO_CONTROL_ROLE_SWITCH);
  copy_light_address_to_tx_Fifo();
  // Send out a light bulb toggle command
  set_command_type(&tx_fifo.fifo[DEMO_CONTROL_PAYLOAD_BYTE], LIGHT_TOGGLE);
  set_switch_state_in_tx_fifo();
  allocated_tx_fifo_size = RAIL_SetTxFifo(rail_handle, tx_fifo.fifo, TX_PAYLOAD_LENGTH, RAIL_FIFO_SIZE);
  app_assert(allocated_tx_fifo_size == RAIL_FIFO_SIZE,
             "RAIL_SetTxFifo() failed to allocate a large enough FIFO (%d bytes instead of %d bytes)\n",
             allocated_tx_fifo_size,
             RAIL_FIFO_SIZE);
  rail_status = RAIL_StartTx(rail_handle, CHANNEL, RAIL_TX_OPTIONS_DEFAULT, NULL);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_StartTx() result:%d ", rail_status);
  }
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
  RAIL_GetRxPacketDetails(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &rxPacketDetails);
  rail_status = RAIL_ReleaseRxPacket(rail_handle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE);
  if (rail_status != RAIL_STATUS_NO_ERROR) {
    app_log_warning("RAIL_ReleaseRxPacket() result:%d", rail_status);
  }
  light_module.light_mode = get_light_response_type();
}

/******************************************************************************
 * Set the actual state in the transmit buffer
 *****************************************************************************/
static void set_switch_state_in_tx_fifo()
{
  // Encode the actual state in the outgoing message
  switch (switch_module.mode) {
    case SWITCH_MODE_SCAN:
      tx_fifo.fifo[DEVICE_STATUS_PAYLOAD_BYTE] &= ~0x0C;
      break;
    case SWITCH_MODE_LINKED:
      tx_fifo.fifo[DEVICE_STATUS_PAYLOAD_BYTE] |=  0x04;
      tx_fifo.fifo[DEVICE_STATUS_PAYLOAD_BYTE] &= ~0x08;
      break;
  }
}

/******************************************************************************
 * Get the communication state of the Light node, if possible (if it is a new Light)
 *****************************************************************************/
static void get_light_state_from_rx_fifo()
{
  // If the light changed its state
  if (light_module.communication_state != (rx_fifo[DEVICE_STATUS_PAYLOAD_BYTE] & 0x03)) {
    char text_tmp[64];
    snprintf(text_tmp, sizeof(text_tmp), "%s%04X%s%s",
             "State changing event at Light Node [",
             *(uint16_t*)light_module.addr,
             "] ",
             ((light_module.communication_state == LIGHT_MODE_ADVERTISE) ? "Mode: ADVERTISE\n" : "Mode: READY\n"));
    light_module.communication_state = rx_fifo[DEVICE_STATUS_PAYLOAD_BYTE] & 0x03;
  }
}

/******************************************************************************
 * Check if the Light node is "soc-light_rail-dmp"
 *****************************************************************************/
static bool is_that_a_new_light()
{
  // In the previous Light versions this byte is 0xDD -> 0b11011101
  // so if the 6th bit is zero, than this is a "soc-light_rail-dmp"
  return (rx_fifo[DEVICE_STATUS_PAYLOAD_BYTE] & 0x20) ? true : false;
}

/******************************************************************************
 * Display the app name, light state, and the ID of the connected Light node
 *****************************************************************************/
static void display_all_information()
{
  demoUIClearMainScreen((uint8_t *)app_name, true, false);
  demoUIDisplayLight((bool)light_module.light_state);
  demoUIDisplayProtocol(DEMO_UI_PROTOCOL1, false);
  write_ID_to_buffer();
  demoUIDisplayId(DEMO_UI_PROTOCOL1, (uint8_t*)switch_module.switch_text_buffer);
}

/******************************************************************************
 * Update the RSSI and ID values
 *****************************************************************************/
static void update_light_RSSI()
{
  // Same Light, update RSSI value
  if (!memcmp((void*)light_module.addr, (void*)&rx_fifo[2], sizeof(light_module.addr))) {
    light_module.rssi = rxPacketDetails.rssi;
  } else {
    // Other Light with stronger signal: save ID and RSSI
    if (rxPacketDetails.rssi > light_module.rssi) {
      memcpy((void*)light_module.addr, (void*)&rx_fifo[2], sizeof(light_module.addr));
      light_module.rssi = rxPacketDetails.rssi;
    }
  }
}

/******************************************************************************
 * Put the communication state and the Light device's ID into a buffer in order to display it
 *****************************************************************************/
static void write_ID_to_buffer()
{
  uint8_t blankAddr[8] = { 0 };
  int ID_not_null = memcmp((void*)light_module.addr, blankAddr, sizeof(light_module.addr));
  if (ID_not_null) {
    snprintf(switch_module.switch_text_buffer, sizeof(switch_module.switch_text_buffer), \
             "%s%04X", switch_module.switch_text[switch_module.mode], *((uint16_t*)light_module.addr));
  } else {
    snprintf(switch_module.switch_text_buffer, sizeof(switch_module.switch_text_buffer), \
             "%s", switch_module.switch_text[switch_module.mode]);
  }
}
/******************************************************************************
 * Copy the Light's address to the RX FIFO
 *****************************************************************************/
static void copy_light_address_to_tx_Fifo()
{
  memcpy((void*)&tx_fifo.fifo[PACKET_HEADER_LEN], (void*)light_module.addr, sizeof(light_module.addr));
}

/******************************************************************************
 * Check if an advertise message come from the Light device
 *****************************************************************************/
static demo_control_command_type_t get_light_response_type()
{
  return ((rx_fifo[DEMO_CONTROL_PAYLOAD_BYTE]) & DEMO_CONTROL_PAYLOAD_CMD_MASK) >> DEMO_CONTROL_PAYLOAD_CMD_MASK_SHIFT;
}

/******************************************************************************
 * Get light mode from the rx_fifo
 *****************************************************************************/
static light_state_t get_light_mode(uint8_t* rx_fifo)
{
  return (light_state_t)(rx_fifo[DEMO_CONTROL_PAYLOAD_BYTE] & DEMO_CONTROL_PAYLOAD_CMD_DATA);
}
