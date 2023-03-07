/***************************************************************************//**
 * @file
 * @brief KEYSCAN API implementation.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "keyscan_driver.h"
#include "keyscan_driver_hal.h"
#include "keyscan_driver_config.h"
#include "em_core.h"
#include "em_gpio.h"

/*******************************************************************************
 **************************   LOCAL VARIABLES   ********************************
 ******************************************************************************/

/// Array Variable used to store the read keyscan data. Each column has its index
/// and the rows information is saved in the column index.
uint8_t keyscan_matrix[SL_KEYSCAN_DRIVER_COLUMN_NUMBER] = { 0 };

// Events subscribers lists
static sl_slist_node_t *keyscan_driver_process_keyscan_event_list = NULL;

/*******************************************************************************
 **************************   LOCAL PROTOTYPES   *******************************
 ******************************************************************************/

static void keyscan_set_keypress(void);

/***************************************************************************//**
 * Initializes the keyscan driver.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_init(void)
{
  uint8_t local_flags;
  // GPIO setup
  sli_keyscan_driver_hal_init_gpio();
  // Hardware initialization
  sli_keyscan_driver_hal_init();
  //init callbacks list
  sl_slist_init(&keyscan_driver_process_keyscan_event_list);
  // Enable interrupts
  local_flags = KEYSCAN_DRIVER_EVENT_WAKEUP
                | KEYSCAN_DRIVER_EVENT_SCANNED
                | KEYSCAN_DRIVER_EVENT_KEY
                | KEYSCAN_DRIVER_EVENT_NOKEY;
  sli_keyscan_driver_hal_enable_interrupts(local_flags);
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Registers an event callback on Keyscan event.
 ******************************************************************************/
void sl_keyscan_driver_subscribe_event(sl_keyscan_driver_process_keyscan_event_handle_t  *event_handle)
{
  EFM_ASSERT(event_handle != NULL);
  EFM_ASSERT(event_handle->on_event != NULL);

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sl_slist_push(&keyscan_driver_process_keyscan_event_list, &event_handle->node);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Unregisters an event callback on Keyscan event.
 ******************************************************************************/
void sl_keyscan_driver_unsubscribe_event(sl_keyscan_driver_process_keyscan_event_handle_t  *event_handle)
{
  EFM_ASSERT(event_handle != NULL);

  CORE_DECLARE_IRQ_STATE;

  CORE_ENTER_CRITICAL();
  sl_slist_remove(&keyscan_driver_process_keyscan_event_list, &event_handle->node);
  CORE_EXIT_CRITICAL();
}

/***************************************************************************//**
 * Starts the keyscan scan.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_start_scan(void)
{
  bool is_running;

  // Check if scan is already in progress, return if already running
  is_running = sli_keyscan_driver_hal_is_scan_running();
  if (is_running) {
    return SL_STATUS_OK;
  }

  // Start the scan if not already started
  sli_keyscan_driver_hal_start_scan(true);

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Stops the keyscan scan.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_stop_scan(void)
{
  bool is_running;
  uint8_t i;

  // Check if scan is in progress, return if already not running
  is_running = sli_keyscan_driver_hal_is_scan_running();
  if (!is_running) {
    return SL_STATUS_OK;
  }

  // Stop the scan
  sli_keyscan_driver_hal_stop_scan();

  // Clear keyscan matrix
  for (i = 0; i < SL_KEYSCAN_DRIVER_COLUMN_NUMBER; i++) {
    keyscan_matrix[i] = 0;
  }

  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Processes the events from the keyscan interrupt.
 ******************************************************************************/
void sli_keyscan_process_irq(uint8_t local_flags)
{
  sl_keyscan_driver_process_keyscan_event_handle_t *handle;
  uint8_t i;

  // Event "wake-up from sleep" handling.
  if (local_flags & KEYSCAN_DRIVER_EVENT_WAKEUP) {
    // Start the scan
    sli_keyscan_driver_hal_start_scan(false);
  }

  // Event "key pressed detected" handling.
  if (local_flags & KEYSCAN_DRIVER_EVENT_KEY) {
    // Retrieve which keypad matrix with key pressed.
    keyscan_set_keypress();
    if (sli_keyscan_driver_hal_is_singlepress_enabled()) {
      // If in single-press mode, call hook to immediately process data
      SL_SLIST_FOR_EACH_ENTRY(keyscan_driver_process_keyscan_event_list, handle, sl_keyscan_driver_process_keyscan_event_handle_t, node) {
        handle->on_event(keyscan_matrix, SL_KEYSCAN_STATUS_KEYPRESS_VALID);
      }
    }
  }

  // Event "all keys pressed now released" handling.
  if (local_flags & KEYSCAN_DRIVER_EVENT_NOKEY) {
    if (!sli_keyscan_driver_hal_is_singlepress_enabled()) {
      // If in multi-press mode, now is the time to call hook to process data
      SL_SLIST_FOR_EACH_ENTRY(keyscan_driver_process_keyscan_event_list, handle, sl_keyscan_driver_process_keyscan_event_handle_t, node) {
        handle->on_event(keyscan_matrix, SL_KEYSCAN_STATUS_KEYPRESS_RELEASED);
      }
    }

    // Clear keyscan matrix
    for (i = 0; i < SL_KEYSCAN_DRIVER_COLUMN_NUMBER; i++) {
      keyscan_matrix[i] = 0;
    }
  }

  // Event "scanned complete with no key pressed" handling.
  if (local_flags & KEYSCAN_DRIVER_EVENT_SCANNED) {
    // Clear keyscan matrix
    for (i = 0; i < SL_KEYSCAN_DRIVER_COLUMN_NUMBER; i++) {
      keyscan_matrix[i] = 0;
    }

    if (sli_keyscan_driver_hal_is_singlepress_enabled()) {
      // If in single-press mode, keypress released is after event "all columns scanned" is triggered.
      // Scanned interrupt is active when scan logic completes an entire column-by-column scan without detecting
      // any key presses.
      SL_SLIST_FOR_EACH_ENTRY(keyscan_driver_process_keyscan_event_list, handle, sl_keyscan_driver_process_keyscan_event_handle_t, node) {
        handle->on_event(keyscan_matrix, SL_KEYSCAN_STATUS_KEYPRESS_RELEASED);
      }
    }
  }
}

/***************************************************************************//**
 * Analyzes the keypad data and sets the key(s) press.
 ******************************************************************************/
static void keyscan_set_keypress(void)
{
  uint8_t column;
  uint8_t row;

  sli_keyscan_driver_hal_get_column_row(&column, &row);

  keyscan_matrix[column] = row;
}
