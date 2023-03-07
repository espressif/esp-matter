/***************************************************************************//**
 * @file
 * @brief KEYSCAN driver API definition.
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

/**************************************************************************//**
 * @addtogroup driver
 * @{
 *
 * @addtogroup KEYSCAN
 *
 * @brief Keyscan driver.
 *
 * @{
 *
 *****************************************************************************/

#ifndef KEYSCAN_DRIVER_H
#define KEYSCAN_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_device.h"
#include "sl_slist.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *********************************   ENUM   ************************************
 ******************************************************************************/

/// KEYSCAN status values.
typedef enum {
  SL_KEYSCAN_STATUS_KEYPRESS_VALID = 0,
  SL_KEYSCAN_STATUS_KEYPRESS_INVALID,
  SL_KEYSCAN_STATUS_KEYPRESS_RELEASED
} sl_keyscan_driver_status_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/// A Keyscan event handler
typedef void (*sl_keyscan_driver_process_keyscan_event)(uint8_t  *p_keyscan_matrix,
                                                        sl_keyscan_driver_status_t status);

/// An Event Handle
typedef struct {
  sl_slist_node_t node;                              ///< List node.
  sl_keyscan_driver_process_keyscan_event on_event;  ///< Function that must be called when the event occurs.
} sl_keyscan_driver_process_keyscan_event_handle_t;

/***************************************************************************//**
 * @brief
 *   Initializes the keyscan driver.
 *
 * @return
 *   0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_init(void);

/***************************************************************************//**
 * @brief
 *   Registers an event callback to be called on Keyscan event.
 *
 * @param[in]
 *   event_handle  Event handle to register
 *
 * @note
 *   An EFM_ASSERT is thrown if the handle is NULL.
 * @note
 *   Must be called before init function.
 ******************************************************************************/
void sl_keyscan_driver_subscribe_event(sl_keyscan_driver_process_keyscan_event_handle_t  *event_handle);

/***************************************************************************//**
 * @brief
 *   Unregisters an event callback to be called on Keyscan event.
 *
 * @param[in]
 *   event_handle  Event handle which must be unregistered (must have been
 *                      registered previously).
 *
 * @note
 *   An EFM_ASSERT is thrown if the handle is not found or is NULL.
 ******************************************************************************/
void sl_keyscan_driver_unsubscribe_event(sl_keyscan_driver_process_keyscan_event_handle_t  *event_handle);

/***************************************************************************//**
 * @brief
 *   Starts the keyscan scan.
 *
 * @return
 *   0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_start_scan(void);

/***************************************************************************//**
 * @brief
 *   Stops the keyscan scan.
 *
 * @return
 *   0 if successful. Error code otherwise.
 ******************************************************************************/
sl_status_t sl_keyscan_driver_stop_scan(void);

/** @} end KEYSCAN */
/** @} end driver */

#ifdef __cplusplus
}
#endif

#endif /* KEYSCAN_DRIVER_H */
