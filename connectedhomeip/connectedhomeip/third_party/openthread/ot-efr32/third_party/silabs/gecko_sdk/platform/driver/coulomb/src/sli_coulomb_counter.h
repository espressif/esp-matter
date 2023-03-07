/***************************************************************************//**
 * @file
 * @brief Coulomb Counter Driver Internal Header
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

#ifndef SLI_COULOMB_COUNTER_DRIVER_H
#define SLI_COULOMB_COUNTER_DRIVER_H

#include "sl_coulomb_counter.h"
#include "sl_slist.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define EMU_VSCALE0         0
#define EMU_VSCALE1         1
#define EMU_VSCALE2         2
#define NANOCOULOMB_TO_COULOMB  1000000000
/// @endcond

/// @brief Output's calibration state machine enum.
typedef enum {
  SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED,
  SLI_COULOMB_COUNTER_CALIBRATION_EM0_SETTINGS_REQUESTED,
  SLI_COULOMB_COUNTER_CALIBRATION_EM2_SETTINGS_REQUESTED,
  SLI_COULOMB_COUNTER_CALIBRATION_NREQ_LOW_STARTED,
  SLI_COULOMB_COUNTER_CALIBRATION_CAL_LOW_STARTED,
  SLI_COULOMB_COUNTER_CALIBRATION_CAL_HIGH_STARTED,
  SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE,
} sli_coulomb_counter_calibration_state_t;

/// @brief Structure for storing output state.
typedef struct sli_coulomb_counter_output_result {
  sli_coulomb_counter_calibration_state_t   calibration_state;  ///< Calibration state machine.
  uint8_t calibration_mode;                                     ///< Current mode when output was calibrated.
  float cpp;                                                    ///< Charge per pulse computed during calibration.
  float total_charge;                                           ///< Accumulated charge.
  sl_coulomb_counter_output_mask_t mask;                        ///< Output mask.
  sl_slist_node_t node;                                         ///< Node for inclusion in linked list.
} sli_coulomb_counter_output_t;

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
#define SLI_COULOMB_COUNTER_OUTPUT_INIT(_mask)                           \
  {                                                                      \
    .calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED, \
    .calibration_mode = 0,                                               \
    .cpp = 0,                                                            \
    .total_charge = 0,                                                   \
    .mask = _mask,                                                       \
  }
/// @endcond

/// @brief Structure for the Coulomb counter driver.
typedef struct sli_coulomb_counter_handle {
  uint8_t                             prescaler;        ///< Prescaler value.
  uint8_t                             threshold;        ///< Threshold value.
  bool                                running;          ///< Flag to track if counters are started.
  sl_coulomb_counter_output_mask_t    selected_outputs; ///< Outputs selected in config header.
  sl_slist_node_t                     *output_head;     ///< Head of outputs linked list.

  unsigned int cal_nreq_low;   ///< nreq value for calibration with low current.
  unsigned int cal_nreq_high;  ///< nreq value for calibration with high current.
  unsigned int cal_count_low;  ///< Number of pulses for calibration with low current.
  unsigned int cal_count_high; ///< Number of pulses for calibration with high current.
  bool em2_requested;          ///< True if last mode requested to user is EM2.
  int cal_em0_vscale;          ///< Voltage scaling programmed for EM0.
  int cal_em2_vscale;          ///< Voltage scaling programmed for EM2.
} sli_coulomb_counter_handle_t;

/***************************************************************************//**
 * @brief
 *   HAL implementation to enable interrupt flag.
 *
 * @param[in] flag
 *   Interrupt flag to be enabled.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_int_enable(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   HAL implementation to disable interrupt.
 *
 * @param[in] flags
 *   Interrupt flag to be disabled.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_int_disable(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   HAL implementation for checking if interrupt flag is set.
 *
 * @param[in] flag
 *   Interrupt flag to be checked.
 *
 * @param[out] is_set
 *   Boolean with result.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_int_is_set(uint8_t flag, bool *is_set);

/***************************************************************************//**
 * @brief
 *   HAL implementation for clearing interrupt flag.
 *
 * @param[in] flag
 *   Interrupt flag to be checked.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_int_clear(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   HAL-specific initialization. This is called by
 *   @ref sl_coulomb_counter_init().
 *
 * @param[in] handle
 *   Driver handle.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_init(sli_coulomb_counter_handle_t *handle);

/***************************************************************************//**
 * @brief
 *   Check if output is available in EM2 or not.
 *
 * param[in] output
 *   Output to be checked
 *
 * @return
 *   True if output is available, false otherwise.
 ******************************************************************************/
bool sli_coulomb_counter_hal_output_supports_em2(sli_coulomb_counter_output_t *output);

/***************************************************************************//**
 * @brief
 *   Return pointer to output structure that matches the mask.
 *
 * @param[in] mask
 *   Output mask
 *
 * @return
 *   A pointer to output structure, or NULL if HAL doesn't support that output.
 ******************************************************************************/
sli_coulomb_counter_output_t* sli_coulomb_counter_hal_get_output(sl_coulomb_counter_output_mask_t mask);

/***************************************************************************//**
 * @brief
 *   HAL implementation to start counting Coulombs.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_start(void);

/***************************************************************************//**
 * @brief
 *   HAL implementation to stop counting Coulombs.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_stop(void);

/***************************************************************************//**
 * @brief
 *   Read counters for that output and increment its total charge.
 *
 * @param[in] output
 *   Output to be updated
 *
 * @param[out] need_recalibration
 *   True if DCDC operating mode has changed since last calibration.
 *
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_read_output(sli_coulomb_counter_output_t *output,
                                                bool *need_recalibration);

/***************************************************************************//**
 * @brief
 *   Reset Coulomb counters for all outputs to zero.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_clear_counters(void);

/***************************************************************************//**
 * @brief
 *   Start calibration for given output.
 *
 * @param[in] output
 *   Output to be calibrated.
 *
 * @param[in] nreq
 *   Number of PFM pulses to count for calibration.
 *
 * @param[in] ccl_level
 *   Calibration current load level.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_cal_start(sli_coulomb_counter_output_t *output,
                                              int8_t nreq,
                                              int8_t ccl_level);

/***************************************************************************//**
 * @brief
 *   Stop the calibration. Remove current load that was applied for calibration.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_cal_stop(void);

/***************************************************************************//**
 * @brief
 *   Read calibration result, i.e. the number of 10MHz clock cycles that were
 *   counted.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_cal_read_result(uint16_t *result);

/***************************************************************************//**
 * @brief
 *   Adjust output's CPP if voltage scaling is different in EM0 and EM2.
 *
 * @param[in] output
 *   Output for which the CPP must be adjusted
 *
 * @param[in] em0_vscale
 *   Voltage scaling value for EM0
 *
 * @param[in] em2_vscale
 *   Voltage scaling value for EM2
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sli_coulomb_counter_hal_cal_adjust_em2_cpp(sli_coulomb_counter_output_t *output,
                                                       int em0_vscale, int em2_vscale);

/***************************************************************************//**
 * @brief
 *   Convert calibration current load enum to current value.
 *
 * @param[in] ccl_level
 *   Calibration current load, as set in CC_CAL.CCL_LVL
 *
 * @return
 *   Current value, in uA.
 ******************************************************************************/
float sli_coulomb_counter_hal_cal_get_load_current(int8_t ccl_level);

/***************************************************************************//**
 * @brief
 *   Get 10MHz oscillator frequency
 *
 * @details
 *   In case a more precise value for the oscillator frequency is available, use
 *   that to make the computation of the charge-per-pulse more accurate.
 *
 * @return
 *   10MHz oscillator frequency, in Hz.
 ******************************************************************************/
float sli_coulomb_counter_hal_get_osc_frequency(void);

#ifdef __cplusplus
}
#endif

#endif /* SLI_COULOMB_COUNTER_DRIVER_H */
