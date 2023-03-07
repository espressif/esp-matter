/***************************************************************************//**
 * @file
 * @brief Coulomb Counter Driver API definition.
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

/**************************************************************************//**
 * @addtogroup coulomb_counter Coulomb Counter API
 * @brief Coulomb Counter Driver API.
 * @{
 *****************************************************************************/

#ifndef SL_COULOMB_COUNTER_DRIVER_H
#define SL_COULOMB_COUNTER_DRIVER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "em_device.h"
#include "sl_efp.h"
#include "sl_slist.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************  DEFINES   ************************************
 ******************************************************************************/

/// EFP's Output VOA.
#define SL_COULOMB_COUNTER_OUTPUT_EFP_VOA         ((sl_coulomb_counter_output_mask_t)0x01)
/// EFP's Output VOB in EM0 mode.
#define SL_COULOMB_COUNTER_OUTPUT_EFP_VOB_EM0     ((sl_coulomb_counter_output_mask_t)0x02)
/// EFP's Output VOB in EM2 mode.
#define SL_COULOMB_COUNTER_OUTPUT_EFP_VOB_EM2     ((sl_coulomb_counter_output_mask_t)0x04)
/// EFP's Output VOC.
#define SL_COULOMB_COUNTER_OUTPUT_EFP_VOC         ((sl_coulomb_counter_output_mask_t)0x08)
/// All Outputs.
#define SL_COULOMB_COUNTER_OUTPUT_ALL             ((sl_coulomb_counter_output_mask_t)0x0F)

/// Calibration done interrupt flag.
#define SL_COULOMB_COUNTER_INT_CALIBRATION_DONE   0x1
/// Counter full interrupt flag.
#define SL_COULOMB_COUNTER_INT_COUNTER_FULL       0x2

/// Bitmask of outputs to operate on.
typedef uint32_t sl_coulomb_counter_output_mask_t;

/**
 * @brief Coulomb counter prescaler enum.
 *
 * Each count in the counter results registers represents
 * 2^(16 - 2 * prescaler).
 */
typedef enum {
  SL_COULOMB_COUNTER_PRESCALER_ZERO,  ///< Set prescaler to 0.
  SL_COULOMB_COUNTER_PRESCALER_ONE,   ///< Set prescaler to 1.
  SL_COULOMB_COUNTER_PRESCALER_TWO,   ///< Set prescaler to 2.
  SL_COULOMB_COUNTER_PRESCALER_THREE, ///< Set prescaler to 3.
} sl_coulomb_counter_prescaler_t;

/**
 * @brief Coulomb counter threshold for counter full interrupt enum.
 *
 * Expressed in percentage, the interrupt full interrupt will be raised (if
 * unmasked) when at least one of the counters reaches that threshold.
 */
typedef enum {
  SL_COULOMB_COUNTER_THRESHOLD_50,  ///< Set threshold to 50%.
  SL_COULOMB_COUNTER_THRESHOLD_62,  ///< Set threshold to 62.5%.
  SL_COULOMB_COUNTER_THRESHOLD_75,  ///< Set threshold to 75%.
  SL_COULOMB_COUNTER_THRESHOLD_87,  ///< Set threshold to 87.5%.
} sl_coulomb_counter_threshold_t;

/// @brief Coulomb counter calibration status enum.
typedef enum {
  SL_COULOMB_COUNTER_CALIBRATION_ERROR,             ///< An error occured during calibration.
  SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS,       ///< Calibration is in progress.
  SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0,  ///< User must set peak current to EM0 value.
  SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2,  ///< User must set peak current to EM2 value.
  SL_COULOMB_COUNTER_CALIBRATION_DONE,              ///< Calibration is done.
} sl_coulomb_counter_calibration_status_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Enable interrupt.
 *
 * @param[in] flag
 *   Interrupt flag to be enabled.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_int_enable(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   Disable interrupt.
 *
 * @param[in] flag
 *   Interrupt flag to be disabled.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_int_disable(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   Check if interrupt flag is set.
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
sl_status_t sl_coulomb_counter_int_is_set(uint8_t flag, bool *is_set);

/***************************************************************************//**
 * @brief
 *   Clear interrupt flag.
 *
 * @param[in] flag
 *   Interrupt flag to be checked.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_int_clear(uint8_t flag);

/***************************************************************************//**
 * @brief
 *   Initialize Coulomb counter peripheral.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_init(void);

/***************************************************************************//**
 * @brief
 *   Start counting coulombs.
 *
 * @details
 *   This starts the coulomb counter. Outputs selected for measurements must be
 *   calibrated when this function is called, or it will fail. When counting,
 *   calibration is not permitted.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_start(void);

/***************************************************************************//**
 * @brief
 *   Stop counting coulombs.
 *
 * @details
 *   This stops the coulomb counter. Note that the counter must be stopped
 *   before calling @ref sl_coulomb_counter_calibrate().
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_stop(void);

/***************************************************************************//**
 * @brief
 *   Read coulomb counters.
 *
 * @details
 *   Read coulomb counters for all outputs selected via configuration header.
 *   Note that all outputs must be read at once, as counters are automatically
 *   cleared after reading.  Values read are accumulated into internal counters.
 *   Each count in the counters represents 2^(16 - 2 * prescaler) PFM pulses.
 *   This function only reads the counters registers and accumulate these values
 *   with counters stored in the driver, to get the charge, one must call
 *   @ref sl_coulomb_counter_get_charge() or
 *   @ref sl_coulomb_counter_get_total_charge().
 *
 * @warning
 *   Some counters increment slowly, especially when counting in EM2 mode, so if
 *   this function is called in a tight loop, it is possible that it keeps
 *   reading 0 because the number of PFM pulses is never high enough to reach
 *   2^(16 - 2 * prescaler) and because counters are cleared after being read.
 *   That can lead to some PFM cycles not being counted, thus inaccurate
 *   results.
 *
 * @note
 *   Outputs' modes of operation can change between two reads. If that happens,
 *   counters will be accumulated with current calibration settings, and
 *   outputs will be set in the out parameter. Subsequent reads of these
 *   outputs will be skipped until they are recalibrated.
 *
 * @param[out] outputs_need_calibration_mask
 *   Will contain mask of outputs that need to be recalibrated.
 *
 * @return
 *   0 if successful, error code otherwise.
 ******************************************************************************/
sl_status_t sl_coulomb_counter_read(sl_coulomb_counter_output_mask_t *outputs_need_calibration_mask);

/***************************************************************************//**
 * @brief
 *   Return total charge only for selected outputs.
 *
 * @param[in] outputs_mask
 *   Mask of outputs.
 *
 * @details
 *   Return total charge that has flown through selected outputs. This is
 *   computed using values accumulated via @ref sl_coulomb_counter_read() and
 *   calibration settings.
 *
 * @return
 *   Returns total charge for selected outputs.
 ******************************************************************************/
float sl_coulomb_counter_get_charge(sl_coulomb_counter_output_mask_t outputs_mask);

/***************************************************************************//**
 * @brief
 *   Return total charge.
 *
 * @details
 *   Return total charge for all outputs that were selected during initialization.
 *
 * @return
 *   Returns total charge for selected outputs.
 ******************************************************************************/
float sl_coulomb_counter_get_total_charge(void);

/***************************************************************************//**
 * @brief
 *   Return available outputs.
 *
 * @details
 *   Depending on the configuration option, this driver use different
 *   peripherals for Coulomb counting. This function returns outputs that are
 *   available on the selected peripheral.
 *
 * @return
 *   Returns a mask of available outputs for the selected peripheral.
 ******************************************************************************/
sl_coulomb_counter_output_mask_t sl_coulomb_counter_outputs_available(void);

/***************************************************************************//**
 * @brief
 *   Return outputs that must be recalibrated.
 *
 * @details
 *   Some events might require an output to be recalibrated to correctly
 *   measure the charge, for instance, if a DCDC changes its operating mode.
 *
 * @return
 *   Returns a mask of outputs that needs to be recalibrated.
 ******************************************************************************/
sl_coulomb_counter_output_mask_t sl_coulomb_counter_outputs_need_calibration(void);

/***************************************************************************//**
 * @brief
 *   Setup calibration for selected outputs.
 *
 * @details
 *   Calibrate, or recalibrate, selected outputs. The counter must be off for
 *   calibration to be possible.
 *
 * @warning
 *   During calibration, power consumption of the system must remain stable.
 *
 * @param[in] outputs_mask
 *   A mask of outputs to be calibrated.
 *
 * @return
 *   SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS if successful,
 *   SL_COULOMB_COUNTER_CALIBRATION_ERROR otherwise.
 ******************************************************************************/
sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate_init(sl_coulomb_counter_output_mask_t outputs_mask);

/***************************************************************************//**
 * @brief
 *   Wait for current step of the calibration routine to complete.
 *
 * @details
 *   This routine waits for the current calibration step to complete before
 *   returning. When calibrating in polling mode, this function must be called
 *   when @ref sl_coulomb_counter_calibrate() returns
 *   SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS. When calibrating in interrupt
 *   mode, this function is not used.
 *
 * @return
 *   SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS if successful,
 *   SL_COULOMB_COUNTER_CALIBRATION_ERROR otherwise.
 ******************************************************************************/
sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate_wait(void);

/***************************************************************************//**
 * @brief
 *   Step function for calibration.
 *
 * @details
 *   This function is to be called repeatedly either from a loop or from
 *   interrupt handler (when STATUS_G.CCC_ISDONE is set) to iterate over
 *   the calibration process.
 *
 * @warning
 *   During calibration, power consumption of the system must remain stable.
 *
 * @return
 *   SL_COULOMB_COUNTER_CALIBRATION_DONE when calibration is complete; or
 *   SL_COULOMB_COUNTER_CALIBRATION_ERROR in case of error; or
 *   SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0 or
 *   SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2 if the peak current must
 *   be changed; or
 *   SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS. In last three cases,
 *   @ref sl_coulomb_counter_calibrate() must be called again to
 *   continue the calibration process.
 ******************************************************************************/
sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate(void);

/** @} (end addtogroup coulomb_counter) */

/* *INDENT-OFF* */
/* THE REST OF THE FILE IS DOCUMENTATION ONLY! */
/// @addtogroup coulomb_counter Coulomb Counter API
/// @{
///
///   @details
///
///   @li @ref coulomb_counter_intro
///   @li @ref coulomb_counter_calibration
///   @li @ref coulomb_counter_em2_output
///   @li @ref coulomb_counter_recalibration
///   @li @ref coulomb_counter_configuration
///   @li @ref coulomb_counter_usage
///
///   @n @section coulomb_counter_intro Introduction
///
///   DCDC converters use pulse-frequency modulation (PFM) to drive outputs.
///   The Coulomb counter counts the number of PFM pulses delivered to each
///   output. In order to get the total charge from an output, the
///   charge-per-pulse (CPP) must be determined, then the total charge is
///   computed like so: total_charge = CPP * Num_pulses.
///
///   Currently, only EFP's DCDC are supported by this driver. The Coulomb
///   counter driver provides an abstraction on top of the EFP registers to make
///   the calibration and reading the counters easier.
///
///   @n @section coulomb_counter_calibration Calibration
///
///   The goal of the calibration is to compute the charge-per-pulse value for
///   each output. In order to achieve that, known loads are applied to outputs
///   and the number of pulses are counted. More details can be found in
///   application note AN1188: EFP01 Coulomb Counting.
///
///   One of the main assumption that is made to compute the CPP is that the
///   system load current remains relatively constant during calibration, that
///   means the application should not switch energy mode, start radio
///   transmissions, etc. This is critical for calibration accuracy. If this is
///   not followed, the driver will return inaccurate results.
///
///   Hardware is capable of calibrating only one output at a time. For each
///   output, there are at least two measurements, one with low current and one
///   with high current. In the worst case scenario, a measurement can take up
///   to 6.5ms (that is 2^16, the size of the counter, divided by 10MHz, the
///   oscillator frequency). So if several outputs are to be calibrated, it will
///   happen sequentially, that means the calibration time is proportional to
///   the number of outputs.
///
///   @n @section coulomb_counter_em2_output EM2 Output
///
///   EFP's VOB output has different voltage and peak current configuration for
///   EM0 and EM2 energy modes. This output can be used to power the DECOUPLE
///   line of the MCU. It also has different Coulomb counters for these two
///   energy modes. As peak current and output voltage are some of the
///   parameters that determine the CPP, the output must be calibrated for each
///   energy mode.
///
///   Typically, EM2 peak current and voltage are lower than those of EM0 for
///   improved efficiency. In order to accurately compute the CPP, the Coulomb
///   counter driver will request at some point during the calibration that the
///   EM2 peak current configuration is applied. Application should take care of
///   limiting actions that draw too much current during that time.
///
///   @n @section coulomb_counter_recalibration Recalibration
///
///   A number of conditions can affect the CPP. Due to the following changes, a
///   recalibraiton might be necessary for accurate results:
///
///   @li Significant change in input voltage
///   @li Change of output voltage
///   @li Change of output peak current
///   @li Change in energy mode
///   @li Change of DCDC operating mode
///
///   For the latter, the driver will report when such event is detected, to let
///   the application know a recalibration must occur.
///
///   @n @section coulomb_counter_configuration Configuration
///
///   The Coulomb counter can be configured by using the configuration tool
///   within Simplicity Studio. From there one can select the number of the EFP
///   instance to use, which output to measure, prescaler and threshold values.
///
///   @n @section coulomb_counter_usage Usage
///
///   The expected flow of execution is:
///
///   @li initialize the driver
///   @li calibrate
///   @li start the counters
///   @li periodically or interrupt-based event to read the counters
///
///   Initialization is done by calling @ref sl_coulomb_counter_init(). It will
///   prepare internal structure and setup communication with Coulomb counter
///   device.
///
///   Calibration is the next step and is done in two folds, first the
///   calibration is initialized by calling @ref
///   sl_coulomb_counter_calibrate_init() and then
///   @ref sl_coulomb_counter_calibrate() is called repeatedly until it reports
///   an error or completion.
///
///   When calibration is done successfully, counters can be started by calling
///   @ref sl_coulomb_counter_start().
///
///   Then, periodically counters be read by calling
///   @ref sl_coulomb_counter_read(). This function has an output parameter to
///   notify if some outputs must be recalibrated. That will happen in the
///   output operates in a different DCDC mode than when it was calibrated. Note
///   that when counters are read, they are cleared afterwards, so if the
///   function is called too often, it will keep reading zeroes. This might be
///   an issue for output that have a slowly increasing count of pulses, like
///   VOB in EM2. To prevent that, one can wait for the interrupt threshold to
///   be raised before reading registers.
///
///   Finally, to get the value of all outputs in Coulomb, call
///   @ref sl_coulomb_counter_get_total_charge(). If interested in only one or
///   more outputs, use @ref sl_coulomb_counter_get_charge() instead.
///
///   Here is a complete example:
///
///   @code{.c}
///sl_coulomb_counter_calibration_status_t status;
///sl_coulomb_counter_output_mask_t outputs;
///sl_status_t err;
///float charge;
///
///err = sl_coulomb_counter_init();
///if (err != SL_STATUS_OK) {
///  /* handle error */
///}
///
////* get outputs that need to be calibrated */
///outputs = sl_coulomb_counter_outputs_need_calibration();
///status = sl_coulomb_counter_calibrate_init(outputs);
///
///while (status == SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS
///       || status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0
///       || status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2) {
///
///  status = sl_coulomb_counter_calibrate();
///
///  if (status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0) {
///    /* Apply EM0 peak current configuration */
///  } else if (status == SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2) {
///    /* Apply EM2 peak current configuration */
///  } else if (status == SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS) {
///    /*
///     * wait for hardware to notify that operation is complete. When using
///     * interrupts, one can wait on the interrupt flag instead.
///     */
///    status = sl_coulomb_counter_calibrate_wait();
///  }
///}
///
///if (status != SL_COULOMB_COUNTER_CALIBRATION_DONE) {
///  /* handle error */
///}
///
///err = sl_coulomb_counter_start();
///if (err != SL_STATUS_OK) {
///  /* handle error */
///}
///
////*
/// * outputs is a bitmask of outputs that need recalibration because DCDC
/// * operation mode changed. If some bits are set, Coulomb counter must be stop
/// * and corresponding outputs recalibrated.
/// */
///err = sl_coulomb_counter_read(&outputs);
///if (err != SL_STATUS_OK) {
///  /* handle error */
///}
///
///float = sl_coulomb_counter_get_total_charge();
///   @endcode
///
/// @} (end addtogroup coulomb_counter)

#ifdef __cplusplus
}
#endif

#endif /* SL_COULOMB_COUNTER_DRIVER_H */
