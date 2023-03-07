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

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "aoa_angle.h"
#include "aoa_angle_config.h"
#include "app_log.h"

// -----------------------------------------------------------------------------
// Defines

#define CHECK_ERROR(x)           if ((x) != SL_RTL_ERROR_SUCCESS) return (x)

#define GUARD_PERIOD_US          4
#define REFERENCE_PERIOD_US      8
#define REFERENCE_PERIOD_SAMPLES 8

#define QUALITY_BUFFER_SIZE      100

// -----------------------------------------------------------------------------
// Type definitions

typedef struct aoa_angle_config_node_s aoa_angle_config_node_t;
struct aoa_angle_config_node_s {
  aoa_id_t id;
  aoa_angle_config_t aoa_angle_config;
  aoa_angle_config_node_t *next;
  float ref_i_samples[REFERENCE_PERIOD_SAMPLES];
  float ref_q_samples[REFERENCE_PERIOD_SAMPLES];
  float **i_samples;
  float **q_samples;
  size_t sample_rows;
  size_t sample_cols;
};

// -----------------------------------------------------------------------------
// Private variables

// Linked list head
static aoa_angle_config_node_t *head_config = NULL;

// -----------------------------------------------------------------------------
// Private function declarations

static sl_status_t allocate_2D_float_buffer(float*** buf,
                                            size_t rows,
                                            size_t cols);
static void free_2D_float_buffer(float** buf, size_t rows);
static sl_status_t allocate_sample_buffers(aoa_angle_config_node_t *node);
static void free_sample_buffers(aoa_angle_config_node_t *node);
static void free_masks(aoa_mask_node_t *mask_head);
static void get_samples(aoa_iq_report_t *iq_report,
                        aoa_angle_config_node_t *node);
static float channel_to_frequency(uint8_t channel);
static sl_status_t aoa_angle_set_default_config(aoa_angle_config_t *aoa_angle_config);
static sl_status_t aoa_angle_finalize_node(aoa_angle_config_node_t *node);
static sl_status_t aoa_angle_find(aoa_id_t id, aoa_angle_config_node_t **node);

// -----------------------------------------------------------------------------
// Public function definitions

/**************************************************************************//**
 * Add a config to the config list.
 *****************************************************************************/
sl_status_t aoa_angle_add_config(aoa_id_t id,
                                 aoa_angle_config_t **config)
{
  aoa_angle_config_node_t *new;
  sl_status_t sc;

  if (SL_STATUS_OK == aoa_angle_find(id, NULL)) {
    return SL_STATUS_ALREADY_EXISTS;
  }

  new = malloc(sizeof(aoa_angle_config_node_t));
  if (NULL == new) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  aoa_id_copy(new->id, id);
  new->next = head_config;
  head_config = new;
  if (NULL != config) {
    *config = &(new->aoa_angle_config);
  }

  // Initialize sample buffers
  new->i_samples = NULL;
  new->q_samples = NULL;
  new->sample_rows = 0;
  new->sample_cols = 0;

  sc = aoa_angle_set_default_config(&new->aoa_angle_config);
  if (sc == SL_STATUS_OK) {
    sc = aoa_angle_finalize_node(new);
  }
  return sc;
}

/**************************************************************************//**
 * Add an azimuth mask to the list.
 *****************************************************************************/
sl_status_t aoa_angle_add_azimuth_mask(aoa_id_t config_id,
                                       float min,
                                       float max)
{
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config;
  aoa_mask_node_t *new;

  sc = aoa_angle_get_config(config_id, &aoa_angle_config);
  if (SL_STATUS_OK == sc) {
    new = malloc(sizeof(aoa_mask_node_t));
    if (NULL == new) {
      sc = SL_STATUS_ALLOCATION_FAILED;
    } else {
      new->max = max;
      new->min = min;

      new->next = aoa_angle_config->azimuth_mask_head;
      aoa_angle_config->azimuth_mask_head = new;
    }
  }
  return sc;
}

/**************************************************************************//**
 * Add an elevation mask to the list.
 *****************************************************************************/
sl_status_t aoa_angle_add_elevation_mask(aoa_id_t config_id,
                                         float min,
                                         float max)
{
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config;
  aoa_mask_node_t *new;

  sc = aoa_angle_get_config(config_id, &aoa_angle_config);
  if (SL_STATUS_OK == sc) {
    new = malloc(sizeof(aoa_mask_node_t));
    if (NULL == new) {
      sc = SL_STATUS_ALLOCATION_FAILED;
    } else {
      new->max = max;
      new->min = min;

      new->next = aoa_angle_config->elevation_mask_head;
      aoa_angle_config->elevation_mask_head = new;
    }
  }
  return sc;
}

/**************************************************************************//**
 * Reset the azimuth mask database
 *****************************************************************************/
sl_status_t aoa_angle_reset_azimuth_masks(aoa_id_t id)
{
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config;

  sc = aoa_angle_get_config(id, &aoa_angle_config);
  if (SL_STATUS_OK == sc) {
    free_masks(aoa_angle_config->azimuth_mask_head);
    aoa_angle_config->azimuth_mask_head = NULL;
  }

  return sc;
}

/**************************************************************************//**
 * Reset the elevation mask database
 *****************************************************************************/
sl_status_t aoa_angle_reset_elevation_masks(aoa_id_t id)
{
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config;

  sc = aoa_angle_get_config(id, &aoa_angle_config);
  if (SL_STATUS_OK == sc) {
    free_masks(aoa_angle_config->elevation_mask_head);
    aoa_angle_config->elevation_mask_head = NULL;
  }

  return sc;
}

/**************************************************************************//**
 * Reset the config database.
 *****************************************************************************/
void aoa_angle_reset_configs(void)
{
  aoa_angle_config_node_t *current = head_config;

  while (NULL != current) {
    // At the end of the iteration, head will be automatically set to NULL.
    head_config = current->next;
    free_sample_buffers(current);
    free_masks(current->aoa_angle_config.azimuth_mask_head);
    free_masks(current->aoa_angle_config.elevation_mask_head);
    free(current);
    current = head_config;
  }
}

/**************************************************************************//**
 * Return a config from the config list by its ID.
 *****************************************************************************/
sl_status_t aoa_angle_get_config(aoa_id_t id,
                                 aoa_angle_config_t **config)
{
  sl_status_t sc;
  aoa_angle_config_node_t *node;

  sc = aoa_angle_find(id, &node);
  if (sc == SL_STATUS_OK) {
    *config = &node->aoa_angle_config;
  }

  return sc;
}

/**************************************************************************//**
 * Finalize configuration settings.
 *****************************************************************************/
sl_status_t aoa_angle_finalize_config(aoa_id_t id)
{
  sl_status_t sc;
  aoa_angle_config_node_t *node;

  sc = aoa_angle_find(id, &node);
  if (sc == SL_STATUS_OK) {
    sc = aoa_angle_finalize_node(node);
  }

  return sc;
}

/***************************************************************************//**
 * Initialize angle calculation libraries
 ******************************************************************************/
enum sl_rtl_error_code aoa_init_rtl(aoa_state_t *aoa_state, aoa_id_t config_id)
{
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config = NULL;
  aoa_mask_node_t *current_azimuth;
  aoa_mask_node_t *current_elevation;
  uint32_t antenna_switch_pattern[ANTENNA_ARRAY_MAX_PATTERN_SIZE];
  uint32_t antenna_switch_pattern_size = sizeof(antenna_switch_pattern) / sizeof(uint32_t);

  sc = aoa_angle_get_config(config_id, &aoa_angle_config);
  if (SL_STATUS_OK != sc) {
    return SL_RTL_ERROR_ARGUMENT;
  }

  sc = antenna_array_get_continuous_pattern(&aoa_angle_config->antenna_array,
                                            antenna_switch_pattern,
                                            &antenna_switch_pattern_size);
  if (SL_STATUS_OK != sc) {
    return SL_RTL_ERROR_ARGUMENT;
  }

  current_azimuth = aoa_angle_config->azimuth_mask_head;
  current_elevation = aoa_angle_config->elevation_mask_head;

  enum sl_rtl_error_code ec;
  // Initialize AoX library
  ec = sl_rtl_aox_init(&aoa_state->libitem);
  CHECK_ERROR(ec);
  // Set the number of snapshots, i.e. how many times the antennas are scanned
  // during one measurement
  ec = sl_rtl_aox_set_num_snapshots(&aoa_state->libitem,
                                    aoa_angle_config->num_snapshots);
  CHECK_ERROR(ec);
  // Set the antenna array type
  ec = sl_rtl_aox_set_array_type(&aoa_state->libitem,
                                 aoa_angle_config->antenna_array.array_type);
  CHECK_ERROR(ec);
  // Select mode (high speed/high accuracy/etc.)
  ec = sl_rtl_aox_set_mode(&aoa_state->libitem,
                           aoa_angle_config->aox_mode);
  CHECK_ERROR(ec);
  // Enable IQ sample quality analysis processing
  ec = sl_rtl_aox_iq_sample_qa_configure(&aoa_state->libitem);
  CHECK_ERROR(ec);

  // Add azimuth constraints
  while (current_azimuth != NULL) {
    ec = sl_rtl_aox_add_constraint(&aoa_state->libitem,
                                   SL_RTL_AOX_CONSTRAINT_TYPE_AZIMUTH,
                                   current_azimuth->min,
                                   current_azimuth->max);
    CHECK_ERROR(ec);
    current_azimuth = current_azimuth->next;
  }

  // Add elevation constraints
  while (current_elevation != NULL) {
    ec = sl_rtl_aox_add_constraint(&aoa_state->libitem,
                                   SL_RTL_AOX_CONSTRAINT_TYPE_ELEVATION,
                                   current_elevation->min,
                                   current_elevation->max);
    CHECK_ERROR(ec);
    current_elevation = current_elevation->next;
  }

  // Create AoX estimator
  ec = sl_rtl_aox_create_estimator(&aoa_state->libitem);
  CHECK_ERROR(ec);
  // Set the switching pattern mode
  ec = sl_rtl_aox_set_switch_pattern_mode(&aoa_state->libitem, SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTERNAL);
  CHECK_ERROR(ec);
  if (antenna_array_type_is_dp(aoa_angle_config->antenna_array.array_type)) {
    // Skip samples from the reference antenna.
    ec = sl_rtl_aox_set_switch_pattern_mode(&aoa_state->libitem, SL_RTL_AOX_SWITCH_PATTERN_MODE_EXTRA_REFERENCE);
    CHECK_ERROR(ec);
  }
  // Set the switching pattern
  ec = sl_rtl_aox_update_switch_pattern(&aoa_state->libitem, antenna_switch_pattern, NULL);
  CHECK_ERROR(ec);
  if (aoa_angle_config->angle_filtering == true) {
    // Initialize an util item
    ec = sl_rtl_util_init(&aoa_state->util_libitem);
    CHECK_ERROR(ec);
    ec = sl_rtl_util_set_parameter(&aoa_state->util_libitem,
                                   SL_RTL_UTIL_PARAMETER_AMOUNT_OF_FILTERING,
                                   aoa_angle_config->angle_filtering_weight);
    CHECK_ERROR(ec);
  }
  // Initialize correction timeout counter
  aoa_state->correction_timeout = 0;

  return ec;
}

/***************************************************************************//**
 * Estimate angle data from IQ samples
 ******************************************************************************/
enum sl_rtl_error_code aoa_calculate(aoa_state_t *aoa_state,
                                     aoa_iq_report_t *iq_report,
                                     aoa_angle_t *angle,
                                     aoa_id_t config_id)
{
  enum sl_rtl_error_code ec;
  float phase_rotation;
  sl_status_t sc;
  aoa_angle_config_node_t *node;
  aoa_angle_config_t *aoa_angle_config;
  uint32_t quality;
  char quality_buffer[QUALITY_BUFFER_SIZE];
  char* quality_string;

  sc = aoa_angle_find(config_id, &node);
  if (SL_STATUS_OK != sc) {
    return SL_RTL_ERROR_ARGUMENT;
  }

  aoa_angle_config = &node->aoa_angle_config;

  // Copy IQ samples into preallocated buffers.
  get_samples(iq_report, node);

  // Calculate phase rotation from reference IQ samples.
  ec = sl_rtl_aox_calculate_iq_sample_phase_rotation(&aoa_state->libitem,
                                                     2.0f,
                                                     node->ref_i_samples,
                                                     node->ref_q_samples,
                                                     REFERENCE_PERIOD_SAMPLES,
                                                     &phase_rotation);
  CHECK_ERROR(ec);

  // Provide calculated phase rotation to the estimator.
  ec = sl_rtl_aox_set_iq_sample_phase_rotation(&aoa_state->libitem,
                                               phase_rotation);
  CHECK_ERROR(ec);

  // Estimate Angle of Arrival from IQ samples.
  // sl_rtl_aox_process will return SL_RTL_ERROR_ESTIMATION_IN_PROGRESS
  // until it has received enough packets for angle estimation.
  ec = sl_rtl_aox_process(&aoa_state->libitem,
                          node->i_samples,
                          node->q_samples,
                          channel_to_frequency(iq_report->channel),
                          &angle->azimuth,
                          &angle->elevation);
  CHECK_ERROR(ec);

  ec = sl_rtl_aox_get_latest_aoa_standard_deviation(&aoa_state->libitem,
                                                    &angle->azimuth_stdev,
                                                    &angle->elevation_stdev);
  CHECK_ERROR(ec);

  // Calculate distance from RSSI.
  ec = sl_rtl_util_rssi2distance(AOA_ANGLE_TAG_TX_POWER,
                                 (float)iq_report->rssi,
                                 &angle->distance);
  CHECK_ERROR(ec);

  // Distance standard deviation is not supported, use 0.
  angle->distance_stdev = 0;

  if (aoa_angle_config->angle_filtering == true) {
    ec = sl_rtl_util_filter(&aoa_state->util_libitem,
                            angle->distance,
                            &angle->distance);
    CHECK_ERROR(ec);
  }

  // Copy sequence counter.
  angle->sequence = iq_report->event_counter;

  // Fetch the quality result.
  quality = sl_rtl_aox_iq_sample_qa_get_results(&aoa_state->libitem);
  if (quality != 0) {
    quality_string = sl_rtl_util_iq_sample_qa_code2string(quality_buffer,
                                                          sizeof(quality_buffer),
                                                          quality);
    app_log_debug("%s [%d] quality: %s" APP_LOG_NL,
                  config_id,
                  angle->sequence,
                  quality_string);
  }

  if (aoa_state->correction_timeout > 0) {
    // Decrement timeout counter.
    --aoa_state->correction_timeout;
    if (aoa_state->correction_timeout == 0) {
      // Timer expired, clear correction values.
      ec = sl_rtl_aox_clear_expected_direction(&aoa_state->libitem);
    }
  }
  return ec;
}

/***************************************************************************//**
 * Set correction data for the estimator
 ******************************************************************************/
enum sl_rtl_error_code aoa_set_correction(aoa_state_t *aoa_state,
                                          aoa_angle_t *correction,
                                          aoa_id_t config_id)
{
  enum sl_rtl_error_code ec;
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config = NULL;

  sc = aoa_angle_get_config(config_id, &aoa_angle_config);
  if (SL_STATUS_OK != sc) {
    return SL_RTL_ERROR_ARGUMENT;
  }

  ec = sl_rtl_aox_set_expected_direction(&aoa_state->libitem,
                                         correction->azimuth,
                                         correction->elevation);
  CHECK_ERROR(ec);
  ec = sl_rtl_aox_set_expected_deviation(&aoa_state->libitem,
                                         correction->azimuth_stdev,
                                         correction->elevation_stdev);
  CHECK_ERROR(ec);

  aoa_state->correction_timeout = aoa_angle_config->angle_correction_timeout;
  return ec;
}

/***************************************************************************//**
 * Deinitialize angle calculation libraries
 ******************************************************************************/
enum sl_rtl_error_code aoa_deinit_rtl(aoa_state_t *aoa_state,
                                      aoa_id_t config_id)
{
  enum sl_rtl_error_code ec;
  sl_status_t sc;
  aoa_angle_config_t *aoa_angle_config = NULL;

  sc = aoa_angle_get_config(config_id, &aoa_angle_config);
  if (SL_STATUS_OK != sc) {
    return SL_RTL_ERROR_ARGUMENT;
  }

  ec = sl_rtl_aox_deinit(&aoa_state->libitem);
  CHECK_ERROR(ec);
  if (aoa_angle_config->angle_filtering == true) {
    ec = sl_rtl_util_deinit(&aoa_state->util_libitem);
    CHECK_ERROR(ec);
  }

  return ec;
}

// -----------------------------------------------------------------------------
// Private function declarations

static sl_status_t allocate_2D_float_buffer(float*** buf, size_t rows, size_t cols)
{
  *buf = malloc(sizeof(float*) * rows);
  if (*buf == NULL) {
    return SL_STATUS_ALLOCATION_FAILED;
  }

  for (size_t i = 0; i < rows; i++) {
    (*buf)[i] = malloc(sizeof(float) * cols);
    if ((*buf)[i] == NULL) {
      return SL_STATUS_ALLOCATION_FAILED;
    }
  }

  return SL_STATUS_OK;
}

static void free_2D_float_buffer(float** buf, size_t rows)
{
  for (size_t i = 0; i < rows; i++) {
    free(buf[i]);
  }
  free(buf);
}

static float channel_to_frequency(uint8_t channel)
{
  static const uint8_t logical_to_physical_channel[40] = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21,
    22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    0, 12, 39
  };

  // Return the center frequency of the given channel.
  return 2402000000 + 2000000 * logical_to_physical_channel[channel];
}

static void get_samples(aoa_iq_report_t *iq_report, aoa_angle_config_node_t *node)
{
  size_t index = 0;
  // Write reference IQ samples into the IQ sample buffer (sampled on one antenna)
  for (size_t sample = 0; sample < REFERENCE_PERIOD_SAMPLES; ++sample) {
    node->ref_i_samples[sample] = iq_report->samples[index++] / 127.0;
    if (index == iq_report->length) {
      break;
    }
    node->ref_q_samples[sample] = iq_report->samples[index++] / 127.0;
    if (index == iq_report->length) {
      break;
    }
  }
  // The last reference sample is the first measurement sample too.
  index = (REFERENCE_PERIOD_SAMPLES - 1) * 2;
  // Write antenna IQ samples into the IQ sample buffer (sampled on all antennas)
  for (size_t snapshot = 0; snapshot < node->sample_rows; ++snapshot) {
    for (size_t antenna = 0; antenna < node->sample_cols; ++antenna) {
      node->i_samples[snapshot][antenna] = iq_report->samples[index++] / 127.0;
      if (index == iq_report->length) {
        break;
      }
      node->q_samples[snapshot][antenna] = iq_report->samples[index++] / 127.0;
      if (index == iq_report->length) {
        break;
      }
    }
    if (index == iq_report->length) {
      break;
    }
  }
}

/**************************************************************************//**
 * Sets the default config.
 *****************************************************************************/
static sl_status_t aoa_angle_set_default_config(aoa_angle_config_t *aoa_angle_config)
{
  aoa_angle_config->aox_mode = AOA_ANGLE_AOX_MODE;
  aoa_angle_config->angle_filtering = true;
  aoa_angle_config->angle_filtering_weight = AOA_ANGLE_FILTERING_AMOUNT;
  aoa_angle_config->angle_correction_timeout = AOA_ANGLE_CORRECTION_TIMEOUT;
  aoa_angle_config->angle_correction_delay = AOA_ANGLE_MAX_CORRECTION_DELAY;
  aoa_angle_config->cte_min_length = AOA_ANGLE_CTE_MIN_LENGTH;
  aoa_angle_config->cte_slot_duration = AOA_ANGLE_CTE_SLOT_DURATION;
  aoa_angle_config->azimuth_mask_head = NULL;
  aoa_angle_config->elevation_mask_head = NULL;
  return antenna_array_init(&aoa_angle_config->antenna_array,
                            AOA_ANGLE_ANTENNA_ARRAY_TYPE);
}

static sl_status_t aoa_angle_finalize_node(aoa_angle_config_node_t *node)
{
  aoa_angle_config_t *cfg = &node->aoa_angle_config;
  uint8_t antenna_switch_pattern_size;
  sl_status_t sc;

  sc = antenna_array_get_pin_pattern(&cfg->antenna_array,
                                     NULL,
                                     &antenna_switch_pattern_size);
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  cfg->num_snapshots = (uint8_t)(((cfg->cte_min_length * 8)
                                  - GUARD_PERIOD_US - REFERENCE_PERIOD_US)
                                 / (cfg->cte_slot_duration * 2)
                                 / antenna_switch_pattern_size);

  return allocate_sample_buffers(node);
}

static sl_status_t allocate_sample_buffers(aoa_angle_config_node_t *node)
{
  sl_status_t sc;
  aoa_angle_config_t *cfg = &node->aoa_angle_config;
  uint8_t antenna_switch_pattern_size;

  sc = antenna_array_get_pin_pattern(&cfg->antenna_array,
                                     NULL,
                                     &antenna_switch_pattern_size);
  if (sc != SL_STATUS_OK) {
    return sc;
  }

  // Reallocate sample buffers
  if ((node->sample_rows != cfg->num_snapshots)
      || (node->sample_cols != antenna_switch_pattern_size)) {
    free_2D_float_buffer(node->i_samples, node->sample_rows);
    free_2D_float_buffer(node->q_samples, node->sample_rows);
    sc = allocate_2D_float_buffer(&node->i_samples,
                                  cfg->num_snapshots,
                                  antenna_switch_pattern_size);
    if (SL_STATUS_OK != sc) {
      return sc;
    }
    sc = allocate_2D_float_buffer(&node->q_samples,
                                  cfg->num_snapshots,
                                  antenna_switch_pattern_size);
    if (SL_STATUS_OK != sc) {
      return sc;
    }
    // Store new sample buffer dimensions
    node->sample_rows = cfg->num_snapshots;
    node->sample_cols = antenna_switch_pattern_size;
  }

  return SL_STATUS_OK;
}

static void free_sample_buffers(aoa_angle_config_node_t *node)
{
  free_2D_float_buffer(node->i_samples, node->sample_rows);
  free_2D_float_buffer(node->q_samples, node->sample_rows);
}

static sl_status_t aoa_angle_find(aoa_id_t id, aoa_angle_config_node_t **node)
{
  aoa_angle_config_node_t *current = head_config;

  while (NULL != current) {
    if (aoa_id_compare(id, current->id) == 0) {
      if (NULL != node) {
        *node = current;
      }
      return SL_STATUS_OK;
    }
    current = current->next;
  }

  return SL_STATUS_NOT_FOUND;
}

static void free_masks(aoa_mask_node_t *mask_head)
{
  aoa_mask_node_t *current;
  aoa_mask_node_t *next;

  for (current = mask_head; current != NULL; current = next) {
    next = current->next;
    free(current);
  }
}
