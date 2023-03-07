/***************************************************************************//**
 * @file
 * @brief Estimate angle data from IQ samples.
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

#ifndef AOA_ANGLE_H
#define AOA_ANGLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "aoa_types.h"
#include "sl_rtl_clib_api.h"
#include "sl_status.h"
#include "aoa_util.h"
#include "antenna_array.h"

// Forward declaration
typedef struct aoa_mask_node_s aoa_mask_node_t;

/// AoA angle estimation handler type, one instance for each asset tag.
typedef struct aoa_state_s {
  sl_rtl_aox_libitem libitem;
  sl_rtl_util_libitem util_libitem;
  uint8_t correction_timeout;
} aoa_state_t;

/// Elevation or azimuth mask min/max values.
struct aoa_mask_node_s {
  float min;
  float max;
  aoa_mask_node_t *next;
};

/// Locator specific configuration settings for AoA angle estimation.
typedef struct aoa_angle_config_s {
  enum sl_rtl_aox_mode aox_mode;
  bool angle_filtering;
  float angle_filtering_weight;
  uint16_t angle_correction_timeout;
  uint16_t angle_correction_delay;
  uint8_t num_snapshots;
  uint16_t cte_min_length;
  uint16_t cte_slot_duration;
  aoa_mask_node_t *azimuth_mask_head;
  aoa_mask_node_t *elevation_mask_head;
  antenna_array_t antenna_array;
} aoa_angle_config_t;

/**************************************************************************//**
 * Add a config to the config list.
 *
 * @param[in] id id of the config
 * @param[out] config created config entry
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation error.
 * @retval SL_STATUS_OK - Entry creation done.
 *****************************************************************************/
sl_status_t aoa_angle_add_config(aoa_id_t id,
                                 aoa_angle_config_t **config);

/**************************************************************************//**
 * Add an azimuth mask to the list.
 *
 * @param[in] id id of the config
 * @param[in] min min value
 * @param[in] max max value
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation error.
 * @retval SL_STATUS_OK - Entry creation done.
 *****************************************************************************/
sl_status_t aoa_angle_add_azimuth_mask(aoa_id_t id,
                                       float min,
                                       float max);

/**************************************************************************//**
 * Add an elevation mask to the list.
 *
 * @param[in] id id of the config
 * @param[in] min min value
 * @param[in] max max value
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation error.
 * @retval SL_STATUS_OK - Entry creation done.
 *****************************************************************************/
sl_status_t aoa_angle_add_elevation_mask(aoa_id_t id,
                                         float min,
                                         float max);

/**************************************************************************//**
 * Reset the azimuth mask database.
 *
 * @param[in] id id of the config
 *
 * @retval SL_STATUS_NOT_FOUND - Config not found.
 * @retval SL_STATUS_OK - Mask destroy done.
 *****************************************************************************/
sl_status_t aoa_angle_reset_azimuth_masks(aoa_id_t id);

/**************************************************************************//**
 * Reset the elevation mask database.
 *
 * @param[in] id id of the config
 *
 * @retval SL_STATUS_NOT_FOUND - Config not found.
 * @retval SL_STATUS_OK - Mask destroy done.
 *****************************************************************************/
sl_status_t aoa_angle_reset_elevation_masks(aoa_id_t id);

/**************************************************************************//**
 * Reset the config database.
 *****************************************************************************/
void aoa_angle_reset_configs(void);

/**************************************************************************//**
 * Return a config from the config list by its ID.
 *
 * @param[in] id id of the config
 * @param[out] config config entry
 *
 * @retval SL_STATUS_NOT_FOUND - Entry not found.
 * @retval SL_STATUS_OK - Entry found.
 *****************************************************************************/
sl_status_t aoa_angle_get_config(aoa_id_t id,
                                 aoa_angle_config_t **config);

/**************************************************************************//**
 * Finalize configuration settings.
 *
 * This function shall be called on each configuration change.
 *
 * @param[in] id id of the config
 *****************************************************************************/
sl_status_t aoa_angle_finalize_config(aoa_id_t id);

/***************************************************************************//**
 * Initialize angle calculation libraries.
 *
 * @param[in] aoa_state Angle calculation handler
 * @param[in] config config entry id
 *
 * @return Status returned by the RTL library
 ******************************************************************************/
enum sl_rtl_error_code aoa_init_rtl(aoa_state_t *aoa_state,
                                    aoa_id_t config_id);

/***************************************************************************//**
 * Estimate angle data from IQ samples.
 *
 * @param[in] aoa_state Angle calculation handler
 * @param[in] iq_report IQ report to convert
 * @param[out] angle Estimated angle data
 *
 * @return Status returned by the RTL library
 ******************************************************************************/
enum sl_rtl_error_code aoa_calculate(aoa_state_t *aoa_state,
                                     aoa_iq_report_t *iq_report,
                                     aoa_angle_t *angle,
                                     aoa_id_t config_id);

/***************************************************************************//**
 * Set correction data for the estimator
 *
 * @param[in] aoa_state Angle calculation handler
 * @param[in] correction Correction data
 * @param[in] config config entry id
 *
 * @return Status returned by the RTL library
 ******************************************************************************/
enum sl_rtl_error_code aoa_set_correction(aoa_state_t *aoa_state,
                                          aoa_angle_t *correction,
                                          aoa_id_t config_id);

/***************************************************************************//**
 * Deinitialize angle calculation libraries
 *
 * @param[in] aoa_state Angle calculation handler
 * @param[in] config config entry id
 *
 * @return Status returned by the RTL library
 ******************************************************************************/
enum sl_rtl_error_code aoa_deinit_rtl(aoa_state_t *aoa_state,
                                      aoa_id_t config_id);

#ifdef __cplusplus
};
#endif

#endif // AOA_ANGLE_H
