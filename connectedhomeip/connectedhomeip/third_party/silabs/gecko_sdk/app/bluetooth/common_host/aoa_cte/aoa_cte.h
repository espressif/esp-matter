/***************************************************************************//**
 * @file
 * @brief CTE Handle module for AoA locator
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef AOA_CTE_H
#define AOA_CTE_H

#include "aoa_db.h"
#include "aoa_types.h"
#include "antenna_array.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Type definitions.

/// Config structure.
typedef struct {
  uint16_t cte_sampling_interval;
  uint16_t cte_min_length;
  uint16_t cte_count;
  uint16_t cte_slot_duration;
  antenna_array_t *antenna_array;
} aoa_cte_config_t;

/// Enum for CTE type selection.
typedef enum {
  AOA_CTE_TYPE_SILABS = 0,
  AOA_CTE_TYPE_CONN,
  AOA_CTE_TYPE_CONN_LESS,
  AOA_CTE_TYPE_COUNT
} aoa_cte_type_t;

// -----------------------------------------------------------------------------
// Public variables.

/// Configuration parameters
extern aoa_cte_config_t aoa_cte_config;

// -----------------------------------------------------------------------------
// Public function declarations

/**************************************************************************//**
 * Handles the bluetooth event.
 *
 * @param[in] evt Bluetooth event.
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t aoa_cte_bt_on_event(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Sets the CTE Mode.
 *
 * @param[in] mode CTE mode.
 *
 * @retval SL_STATUS_OK - CTE mode is set.
 * @retval SL_STATUS_INVALID_PARAMETER - Invalid CTE mode.
 *****************************************************************************/
sl_status_t aoa_cte_set_mode(aoa_cte_type_t mode);

/**************************************************************************//**
 * Returns the CTE Mode.
 *
 * @return The currently used CTE Mode.
 *****************************************************************************/
aoa_cte_type_t aoa_cte_get_mode(void);

/**************************************************************************//**
 * Bluetooth event handle for connectionless CTE.
 *
 * @param[in] evt Bluetooth event
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t cte_bt_on_event_conn_less(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Bluetooth event handle for connection CTE.
 *
 * @param[in] evt Bluetooth event
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t cte_bt_on_event_conn(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Bluetooth event handle for Silabs CTE.
 *
 * @param[in] evt Bluetooth event
 *
 * @return Status of the operation.
 *****************************************************************************/
sl_status_t cte_bt_on_event_silabs(sl_bt_msg_t *evt);

/**************************************************************************//**
 * Callback to notify the application on new iq report.
 *
 * @param[in] tag Pointer to the tag.
 * @param[in] iq_report IQ report from the tag.
 *****************************************************************************/
void aoa_cte_on_iq_report(aoa_db_entry_t *tag,
                          aoa_iq_report_t *iq_report);

#ifdef __cplusplus
};
#endif

#endif /* AOA_CTE_H */
