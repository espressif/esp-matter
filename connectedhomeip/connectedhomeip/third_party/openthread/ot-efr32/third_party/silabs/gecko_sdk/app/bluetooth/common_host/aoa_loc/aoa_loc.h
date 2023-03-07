/***************************************************************************//**
 * @file
 * @brief Location calculation engine.
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef AOA_LOC_H
#define AOA_LOC_H

#include "sl_status.h"
#include "aoa_types.h"
#include "sl_rtl_clib_api.h"

#ifdef __cplusplus
extern "C" {
#endif

// -----------------------------------------------------------------------------
// Enums.

/// Represents the considered axes.
enum axis_list{
  AXIS_X,
  AXIS_Y,
  AXIS_Z,
  AXIS_COUNT
};

// -----------------------------------------------------------------------------
// Type definitions.

/// Locator struct
typedef struct {
  aoa_id_t id;
  uint32_t loc_id;                          // assigned by RTL lib
  bool functional;
  struct sl_rtl_loc_locator_item item;
} aoa_locator_t;

/// Asset tag struct
typedef struct {
  aoa_id_t id;
  int32_t tag_id;                          // assigned by RTL lib
  sl_rtl_util_libitem filter[AXIS_COUNT];
  aoa_position_t position;
  void *aoa_state;                         //Only if Angle calculation is needed from IQ report
} aoa_asset_tag_t;

/// Init struct
typedef struct {
  uint32_t locator_count;
  uint32_t max_sequence_diff;
  enum sl_rtl_loc_estimation_mode estimation_mode;
  enum sl_rtl_loc_measurement_validation_method validation_method;
  float estimation_interval_sec;
  bool filtering_enabled;
  float filtering_amount;
  bool is_feedback_enabled;
} aoa_loc_config_t;

// -----------------------------------------------------------------------------
// Public variables.

/// Configuration parameters
extern aoa_loc_config_t aoa_loc_config;

// -----------------------------------------------------------------------------
// Public functions.

/**************************************************************************//**
 * Initialize the locator engine.
 *
 * @retval SL_STATUS_FAIL - RTL lib initialization error.
 * @retval SL_STATUS_OK - Initialization ok.
 *****************************************************************************/
sl_status_t aoa_loc_init(void);

/**************************************************************************//**
 * Finalize the configuration.
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation error.
 * @retval SL_STATUS_FAIL - RTL lib error.
 * @retval SL_STATUS_OK - Initialization ok.
 *****************************************************************************/
sl_status_t aoa_loc_finalize_config(void);

/**************************************************************************//**
 * Add a new locator to the database.
 *
 * @param[in] locator_id ID of the locator.
 * @param[in] item Contains the locator coordinates and orientations.
 * @param[in] locator Pointer to the created entry.
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation failure.
 * @retval SL_STATUS_OK - Entry added.
 *****************************************************************************/
sl_status_t aoa_loc_add_locator(aoa_id_t locator_id,
                                struct sl_rtl_loc_locator_item item,
                                aoa_locator_t **locator);

/**************************************************************************//**
 * Get a locator from the database.
 *
 * @param[in] locator_id The desired locator id.
 * @param[in] locator_idx Locator entry index in the list.
 * @param[in] locator Pointer to the entry.
 *
 * @retval SL_STATUS_NOT_FOUND - Entry not found.
 * @retval SL_STATUS_OK - Entry found.
 *****************************************************************************/
sl_status_t aoa_loc_get_locator_by_id(aoa_id_t locator_id,
                                      uint32_t *locator_idx,
                                      aoa_locator_t **locator);

/**************************************************************************//**
 * Get a locator from the database by index.
 *
 * @param[in] locator_idx Locator entry position in the list.
 * @param[in] locator Pointer to the entry.
 *
 * @retval SL_STATUS_NOT_FOUND - Entry not found.
 * @retval SL_STATUS_OK - Entry added.
 *****************************************************************************/
sl_status_t aoa_loc_get_locator_by_index(uint32_t locator_idx,
                                         aoa_locator_t **locator);

/**************************************************************************//**
 * Return the number of locators on the list.
 *
 * @retval Number of locators
 *****************************************************************************/
uint32_t aoa_loc_get_number_of_locators(void);

/**************************************************************************//**
 * Add a new asset tag to the database.
 *
 * @param[in] tag_id Tag id.
 * @param[in] tag Pointer to the created entry.
 *
 * @retval SL_STATUS_ALLOCATION_FAILED - Memory allocation failure.
 * @retval SL_STATUS_FAIL - RTL lib initialization failed.
 * @retval SL_STATUS_OK - Entry added.
 *****************************************************************************/
sl_status_t aoa_loc_add_asset_tag(aoa_id_t tag_id,
                                  aoa_asset_tag_t **tag);

/**************************************************************************//**
 * Get a tag from the database by its id.
 *
 * @param[in] tag_id The desired tag id.
 * @param[in] tag Pointer to the entry.
 *
 * @retval SL_STATUS_NOT_FOUND - Entry not found.
 * @retval SL_STATUS_OK - Entry found.
 *****************************************************************************/
sl_status_t aoa_loc_get_tag_by_id(aoa_id_t tag_id,
                                  aoa_asset_tag_t **tag);

/**************************************************************************//**
 * Get a tag from the database by its index.
 *
 * @param[in] index The desired tag index.
 * @param[in] tag Pointer to the entry.
 *
 * @retval SL_STATUS_NOT_FOUND - Entry not found.
 * @retval SL_STATUS_OK - Entry found.
 *****************************************************************************/
sl_status_t aoa_loc_get_tag_by_index(uint32_t index,
                                     aoa_asset_tag_t **tag);

/**************************************************************************//**
 * Calculate the asset tag position and notify the app.
 *
 * @param[in] tag_id Tag for calculate the location.
 * @param[in] angle_count Number of angles.
 * @param[in] angle_list Angle list.
 * @param[in] locator_list Locator list.
 *
 * @retval SL_STATUS_FAIL - Position calculation failed in the RTL lib.
 * @retval SL_STATUS_OK - Calculation was successful.
 *****************************************************************************/
sl_status_t aoa_loc_calc_position(aoa_id_t tag_id,
                                  uint32_t angle_count,
                                  aoa_angle_t *angle_list,
                                  aoa_id_t *locator_list);

/**************************************************************************//**
 * Destroy the locator database.
 *****************************************************************************/
void aoa_loc_destroy_locators(void);

/**************************************************************************//**
 * Destroy the tags database
 *****************************************************************************/
void aoa_loc_destroy_tags(void);

/**************************************************************************//**
 * Remove a locator from the list.
 *
 * @retval SL_STATUS_NOT_FOUND - Locator is not on the list.
 * @retval SL_STATUS_EMPTY - Locator list is empty.
 * @retval SL_STATUS_OK - Locator removed.
 *****************************************************************************/
sl_status_t aoa_loc_remove_locator(aoa_id_t locator_id);

/**************************************************************************//**
 * Reinitialize the estimator.
 *
 * @deprecated Use the combination of aoa_loc_deinit and aoa_loc_init instead.
 *
 * @retval SL_STATUS_FAIL - Reinitialization failed.
 * @retval SL_STATUS_OK - Reinitialization was successful.
 *****************************************************************************/
sl_status_t aoa_loc_reinit(void);

/**************************************************************************//**
 * Deinitialize the estimator.
 *
 * @retval SL_STATUS_FAIL - Deinitialization failed.
 * @retval SL_STATUS_OK - Deinitialization was successful.
 *****************************************************************************/
sl_status_t aoa_loc_deinit(void);

/**************************************************************************//**
 * Position ready callback.
 *
 * @param[in] tag Pointer to the tag entry.
 *****************************************************************************/
void aoa_loc_on_position_ready(aoa_asset_tag_t *tag);

/**************************************************************************//**
 * Correction ready callback.
 *
 * @param[in] tag Pointer to the tag entry.
 * @param[in] sequence Sequence number of the correlated angle.
 * @param[in] locator_id Locator id.
 * @param[in] loc_idx Locator index.
 * @param[in] correction Pointer to the correction structure.
 *****************************************************************************/
void aoa_loc_on_correction_ready(aoa_asset_tag_t *tag,
                                 int32_t sequence,
                                 aoa_id_t locator_id,
                                 uint32_t loc_idx,
                                 aoa_angle_t *correction);

/**************************************************************************//**
 * Angle init callback. Used when angle calculation is needed from IQ report.
 *
 * @param[in] tag Pointer to the tag entry.
 * @param[in] locator_count Number of locators
 *****************************************************************************/
void aoa_loc_angle_init(aoa_asset_tag_t *tag,
                        uint32_t locator_count);

/**************************************************************************//**
 * Angle deinit callback. Used when angle calculation is needed from IQ report.
 *
 * @param[in] tag Pointer to the tag entry.
 * @param[in] locator_count Number of locators
 *****************************************************************************/
void aoa_loc_angle_deinit(aoa_asset_tag_t *tag,
                          uint32_t locator_count);

#ifdef __cplusplus
};
#endif

#endif /* AOA_LOC_H */
