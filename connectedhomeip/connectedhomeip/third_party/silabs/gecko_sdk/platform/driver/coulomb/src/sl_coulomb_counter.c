/***************************************************************************//**
 * @file
 * @brief Coulomb Counter Driver Implementation.
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

#include <math.h>

#include "em_emu.h"
#include "sl_coulomb_counter.h"
#include "sl_coulomb_counter_config.h"
#include "sli_coulomb_counter.h"

// constants for Calibration Current Load
#define CCL_LEVEL_LOW     (0x3)
#define CCL_LEVEL_HIGH    (0x7)

// global variable to store state of the coulomb counter driver
static sli_coulomb_counter_handle_t handle;

sl_status_t sl_coulomb_counter_int_enable(uint8_t flag)
{
  return sli_coulomb_counter_hal_int_enable(flag);
}

sl_status_t sl_coulomb_counter_int_disable(uint8_t flag)
{
  return sli_coulomb_counter_hal_int_disable(flag);
}

sl_status_t sl_coulomb_counter_int_is_set(uint8_t flag, bool *is_set)
{
  return sli_coulomb_counter_hal_int_is_set(flag, is_set);
}

sl_status_t sl_coulomb_counter_int_clear(uint8_t flag)
{
  return sli_coulomb_counter_hal_int_clear(flag);
}

sl_status_t sl_coulomb_counter_init(void)
{
  sl_status_t status;

  handle.prescaler = SL_COULOMB_COUNTER_PRESCALER;
  handle.threshold = SL_COULOMB_COUNTER_THRESHOLD;
  handle.running = false;
  handle.selected_outputs = 0x0
#if SL_COULOMB_COUNTER_OUTPUT_USE_EFP_VOA
                            | SL_COULOMB_COUNTER_OUTPUT_EFP_VOA
#endif
#if SL_COULOMB_COUNTER_OUTPUT_USE_EFP_VOB_EM0
                            | SL_COULOMB_COUNTER_OUTPUT_EFP_VOB_EM0
#endif
#if SL_COULOMB_COUNTER_OUTPUT_USE_EFP_VOB_EM2
                            | SL_COULOMB_COUNTER_OUTPUT_EFP_VOB_EM2
#endif
#if SL_COULOMB_COUNTER_OUTPUT_USE_EFP_VOC
                            | SL_COULOMB_COUNTER_OUTPUT_EFP_VOC
#endif
  ;

  sl_slist_init(&handle.output_head);

  status = sli_coulomb_counter_hal_init(&handle);
  if (status != SL_STATUS_OK) {
    return status;
  }

  for (unsigned int mask = 0x1; mask <= SL_COULOMB_COUNTER_OUTPUT_ALL; mask <<= 1) {
    if (handle.selected_outputs & mask) {
      sli_coulomb_counter_output_t *output = sli_coulomb_counter_hal_get_output(mask);
      if (output == NULL) {
        return SL_STATUS_FAIL;
      }

      sl_slist_push(&handle.output_head, &output->node);
    }
  }

  return SL_STATUS_OK;
}

sl_status_t sl_coulomb_counter_start(void)
{
  sli_coulomb_counter_output_t *output;
  sl_status_t status;

  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    if (output->calibration_state != SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE) {
      return SL_STATUS_FAIL;
    }
  }

  status = sli_coulomb_counter_hal_start();
  if (status == SL_STATUS_OK) {
    handle.running = true;
  }

  return status;
}

sl_status_t sl_coulomb_counter_stop(void)
{
  handle.running = false;
  return sli_coulomb_counter_hal_stop();
}

sl_status_t sl_coulomb_counter_read(sl_coulomb_counter_output_mask_t *outputs_need_calibration_mask)
{
  sli_coulomb_counter_output_t *output;
  sl_status_t status;

  *outputs_need_calibration_mask = 0;

  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    bool output_need_recalibration;

    /*
     * Skip reading if output was not calibrated
     */
    if (output->calibration_state != SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE) {
      *outputs_need_calibration_mask |= output->mask;
      continue;
    }

    status = sli_coulomb_counter_hal_read_output(output, &output_need_recalibration);
    if (status != SL_STATUS_OK) {
      return status;
    }

    if (output_need_recalibration) {
      *outputs_need_calibration_mask |= output->mask;
    }
  }

  return sli_coulomb_counter_hal_clear_counters();
}

float sl_coulomb_counter_get_charge(sl_coulomb_counter_output_mask_t outputs_mask)
{
  sli_coulomb_counter_output_t *output;
  float total = 0;
  int power;

  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    if (output->mask & outputs_mask) {
      total = total + output->total_charge;
    }
  }

  /*
   * Results are scaled according to the prescaler:
   *
   *                             (16 - 2 * CC_PRESCL)
   *  Number of Pulses = reg * 2^
   */
  power = 16 - 2 * handle.prescaler;
  total = total * pow(2, power);

  return total;
}

float sl_coulomb_counter_get_total_charge(void)
{
  return sl_coulomb_counter_get_charge(handle.selected_outputs);
}

sl_coulomb_counter_output_mask_t sl_coulomb_counter_outputs_available(void)
{
  sl_coulomb_counter_output_mask_t available_outputs = 0x0;

  for (unsigned int mask = 0x1; mask <= SL_COULOMB_COUNTER_OUTPUT_ALL; mask <<= 1) {
    sli_coulomb_counter_output_t *output = sli_coulomb_counter_hal_get_output((sl_coulomb_counter_output_mask_t)mask);
    if (output) {
      available_outputs |= mask;
    }
  }

  return available_outputs;
}

sl_coulomb_counter_output_mask_t sl_coulomb_counter_outputs_need_calibration(void)
{
  sl_coulomb_counter_output_mask_t mask = 0;
  sli_coulomb_counter_output_t *output;

  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    if (output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED) {
      mask |= output->mask;
    }
  }

  return mask;
}

/***************************************************************************//**
 * @brief
 *   Compute nreq. Details are given in AN1188.
 *
 * @param[in] ccc
 *   Value read in CCC_MSBY and CCC_LSBY
 *
 * @return
 *   Max nreq that is guaranteed to not generate an overflow
 ******************************************************************************/
static int sli_coulomb_counter_calibrate_compute_nreq(int16_t ccc)
{
  int nreq;

  /*
   * Formula is given in application note AN1188:
   *
   *                               2**17
   *  Max NReq = rounddown( log2 (-------) - 1 )
   *                                CCC
   */
  nreq = (int)floor(log((double)pow(2, 17) / (double)ccc) / log(2) - 1);
  if (nreq > 7) {
    nreq = 7;
  }

  EFM_ASSERT(nreq >= 0);

  return nreq;
}

/***************************************************************************//**
 * Read calibration result
 *
 * @param[out] result
 *   Pointer to where to store the result.
 *
 * @return SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
static sl_status_t sli_coulomb_counter_read_result(uint16_t *result)
{
  sl_status_t status;

  status = sli_coulomb_counter_hal_cal_stop();
  if (status != SL_STATUS_OK) {
    return status;
  }

  return sli_coulomb_counter_hal_cal_read_result(result);
}

/***************************************************************************//**
 * @brief
 *   Compute charge-per-pulse. Details are given in the AN1188.
 *
 * @return
 *   Computed CPP, in nanocoulombs.
 ******************************************************************************/
static float sli_coulomb_counter_compute_cpp(void)
{
  float cpp;

  /*
   * AN188 gives following formula:
   *
   *                       I_large_load - I_small_load
   * CPP = ---------------------------------------------------
   *            7    nreq_large + 1        7    nreq_small + 1
   *         10^ * 2^                 - 10^ * 2^
   *         ----------------------     ----------------------
   *                 count_large               count_low
   *
   * Rearranged:
   *
   *
   *                (count_low * count_large) * (I_large_load - I_small_load)
   * CPP = -----------------------------------------------------------------------------
   *            7                   nreq_large + 1                      nreq_small + 1
   *        10^   * [(count_low * 2^               ) - (count_large * 2^               )]
   */

  float i_large_load_ua = sli_coulomb_counter_hal_cal_get_load_current(CCL_LEVEL_HIGH);
  float i_small_load_ua = sli_coulomb_counter_hal_cal_get_load_current(CCL_LEVEL_LOW);

  float pulses_large = (float)(pow(2, handle.cal_nreq_high + 1)) * (float)handle.cal_count_low;
  float pulses_small = (float)(pow(2, handle.cal_nreq_low + 1)) * (float)handle.cal_count_high;

  /* numerator */
  cpp = handle.cal_count_low * handle.cal_count_high;
  cpp = cpp * (i_large_load_ua - i_small_load_ua);

  /* denominator */
  cpp = cpp / (pulses_large - pulses_small);

  /*
   * Note that current values are in uA, so it should be divided by 10^6 to get
   * results in Amp. But CPP values are in the order of nano coulombs, so that
   * means multiplying by 10^9 to convert it to this unit.
   * So instead, multiply by 10^(9-6), 10^3.
   *  cpp = cpp / 1000000;    // convert from uA to A
   *  cpp = cpp * 1000000000; // convert from C to nC
   *  => cpp = cpp * 1000;
   */
  cpp = cpp * 1000;

  /*
   * Finally, divide that cpp by 10^7, for the 10MHz oscillator frequency. It's
   * possible that the HAL provides a more precise frequency for that
   * oscillator, so use that instead for more accurate cpp.
   */
  cpp = cpp / sli_coulomb_counter_hal_get_osc_frequency();

  return cpp;
}

/***************************************************************************//**
 * @brief
 *   Step function to calibrate a single output. It handles outputs' internal
 *   state machines.
 *
 * @param[in] output
 *   Output under calibration
 *
 * @return
 *   SL_STATUS_OK if successful, an error code otherwise.
 ******************************************************************************/
static sl_status_t sl_coulomb_counter_calibrate_output(sli_coulomb_counter_output_t *output)
{
  sl_status_t status;
  uint16_t ccc;

  switch (output->calibration_state) {
    case SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED:
      if (sli_coulomb_counter_hal_output_supports_em2(output)) {
        output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_EM2_SETTINGS_REQUESTED;
      } else {
        output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_EM0_SETTINGS_REQUESTED;
      }

      return SL_STATUS_OK;
      break;

    case SLI_COULOMB_COUNTER_CALIBRATION_EM0_SETTINGS_REQUESTED:
    case SLI_COULOMB_COUNTER_CALIBRATION_EM2_SETTINGS_REQUESTED:
      status = sli_coulomb_counter_hal_cal_start(output, 0, CCL_LEVEL_LOW);
      if (status == SL_STATUS_OK) {
        output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_NREQ_LOW_STARTED;
      }

      return status;
      break;

    case SLI_COULOMB_COUNTER_CALIBRATION_NREQ_LOW_STARTED:
      status = sli_coulomb_counter_read_result(&ccc);
      if (status != SL_STATUS_OK) {
        return status;
      }

      handle.cal_nreq_low = sli_coulomb_counter_calibrate_compute_nreq(ccc);

      status = sli_coulomb_counter_hal_cal_start(output, handle.cal_nreq_low, CCL_LEVEL_LOW);
      if (status == SL_STATUS_OK) {
        output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_CAL_LOW_STARTED;
      }
      return status;

      break;

    case SLI_COULOMB_COUNTER_CALIBRATION_CAL_LOW_STARTED:
      status = sli_coulomb_counter_read_result(&ccc);
      if (status != SL_STATUS_OK) {
        return status;
      }

      handle.cal_count_low = ccc;

      handle.cal_nreq_high = 0x7;
      status = sli_coulomb_counter_hal_cal_start(output, handle.cal_nreq_high, CCL_LEVEL_HIGH);
      if (status == SL_STATUS_OK) {
        output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_CAL_HIGH_STARTED;
      }

      return status;
      break;

    case SLI_COULOMB_COUNTER_CALIBRATION_CAL_HIGH_STARTED:
      status = sli_coulomb_counter_read_result(&ccc);
      if (status != SL_STATUS_OK) {
        return status;
      }

      handle.cal_count_high = ccc;

      output->cpp = sli_coulomb_counter_compute_cpp();
      if (sli_coulomb_counter_hal_output_supports_em2(output)
          && handle.cal_em0_vscale != handle.cal_em2_vscale) {
        sli_coulomb_counter_hal_cal_adjust_em2_cpp(output, handle.cal_em0_vscale, handle.cal_em2_vscale);
      }

      output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE;

      return status;
      break;

    case SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE:
      return SL_STATUS_OK;
      break;

    default:
      return SL_STATUS_FAIL;
      break;
  }
}

/***************************************************************************//**
 * @brief
 *   Return the next output that is not completely calibrated.
 *
 * @return
 *   A pointer to a sli_coulomb_counter_output_t, or NULL if all outputs are
 *   calibrated.
 ******************************************************************************/
static sli_coulomb_counter_output_t* sli_coulomb_counter_get_next_output_being_calibrated(void)
{
  sli_coulomb_counter_output_t *output;

  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    if (output->calibration_state < SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE) {
      return output;
    }
  }

  return NULL;
}

sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate_init(sl_coulomb_counter_output_mask_t outputs_mask)
{
  sli_coulomb_counter_output_t *output;

  /* cannot calibrate when running */
  if (handle.running) {
    return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
  }

  /*
   * check that user didn't request outputs that were not selected
   * during initialization
   */
  if (outputs_mask & ~handle.selected_outputs) {
    return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
  }

  /*
   * Mark outputs that were passed as parameter as not calibrated
   */
  SL_SLIST_FOR_EACH_ENTRY(handle.output_head, output, sli_coulomb_counter_output_t, node) {
    if (output->mask & outputs_mask) {
      output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED;
    }
  }

#if defined(EMU_VSCALE_PRESENT)
  /* Initialize EM23 voltage scaling. */
  EMU_VScaleEM23_TypeDef em23VScale = (EMU_VScaleEM23_TypeDef)((EMU->CTRL & _EMU_CTRL_EM23VSCALE_MASK)
                                                               >> _EMU_CTRL_EM23VSCALE_SHIFT);

  if (em23VScale == emuVScaleEM23_LowPower) {
    handle.cal_em2_vscale = EMU_VSCALE0;
  } else if (em23VScale == emuVScaleEM23_FastWakeup) {
    handle.cal_em2_vscale = EMU_VSCALE2;
  }

  /* Initialize EM01 voltage scaling. */
#if defined(EMU_VSCALE_EM01_PRESENT)
  EMU_VScaleEM01_TypeDef em01VScale = EMU_VScaleGet();

  if (em01VScale == emuVScaleEM01_HighPerformance) {
    handle.cal_em0_vscale = EMU_VSCALE2;
  } else if (em01VScale == emuVScaleEM01_LowPower) {
#if defined(_SILICON_LABS_32B_SERIES_2)
    handle.cal_em0_vscale = EMU_VSCALE1;
#else
    handle.cal_em0_vscale = EMU_VSCALE0;
#endif
  }
#else
  /* If EM01 voltage scaling not present, use default value. */
  handle.cal_em0_vscale = EMU_VSCALE2;
#endif

#else
  /* If EMx voltage scaling not present, use default values. */
  handle.cal_em2_vscale = EMU_VSCALE2;
  handle.cal_em0_vscale = EMU_VSCALE2;
#endif

  /* assume we run by default with EM0 settings */
  handle.em2_requested = false;

  /* The minimum between EM0 and EM2 VSCALEs will be applied when going to EM2 */
  if (handle.cal_em2_vscale > handle.cal_em0_vscale) {
    handle.cal_em2_vscale = handle.cal_em0_vscale;
  }

  return SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS;
}

sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate_wait(void)
{
  sli_coulomb_counter_output_t *output;

  output = sli_coulomb_counter_get_next_output_being_calibrated();
  if (output == NULL) {
    /*
     * means that the function was called when calibration was already complete
     * or not initialized
     */
    return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
  }

  if (output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_NREQ_LOW_STARTED
      || output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_CAL_LOW_STARTED
      || output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_CAL_HIGH_STARTED) {
    sl_status_t status;
    bool set;

    do {
      status = sli_coulomb_counter_hal_int_is_set(SL_COULOMB_COUNTER_INT_CALIBRATION_DONE, &set);
    } while ((status == SL_STATUS_OK) && !set);

    if (status != SL_STATUS_OK) {
      return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
    }

    status = sli_coulomb_counter_hal_int_clear(SL_COULOMB_COUNTER_INT_CALIBRATION_DONE);
    if (status != SL_STATUS_OK) {
      return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
    }

    return SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS;
  }

  return SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS;
}

sl_coulomb_counter_calibration_status_t sl_coulomb_counter_calibrate(void)
{
  sli_coulomb_counter_output_t *output;
  sl_status_t status = SL_STATUS_OK;

  /* cannot calibrate when running */
  if (handle.running) {
    return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
  }

  output = sli_coulomb_counter_get_next_output_being_calibrated();
  if (output == NULL) {
    /*
     * means that the function was called when calibration was already complete
     * or not initialized
     */
    return SL_COULOMB_COUNTER_CALIBRATION_DONE;
  }

  status = sl_coulomb_counter_calibrate_output(output);
  if (status != SL_STATUS_OK) {
    output->calibration_state = SLI_COULOMB_COUNTER_CALIBRATION_NOT_CALIBRATED;
    return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
  }

  if (output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_COMPLETE) {
    /*
     * if calibration for current output is done:
     *  - either there is a next output to be calibrated, in that case start
     *    the process
     *  - output list to be calibrated is exhausted, process is done
     */
    output = sli_coulomb_counter_get_next_output_being_calibrated();
    if (output == NULL) {
      return SL_COULOMB_COUNTER_CALIBRATION_DONE;
    } else {
      status = sl_coulomb_counter_calibrate_output(output);
      if (status != SL_STATUS_OK) {
        return SL_COULOMB_COUNTER_CALIBRATION_ERROR;
      }
    }
  }

  if (output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_EM0_SETTINGS_REQUESTED) {
    if (handle.em2_requested) {
      handle.em2_requested = false;
      return SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM0;
    } else {
      return sl_coulomb_counter_calibrate();
    }
  } else if (output->calibration_state == SLI_COULOMB_COUNTER_CALIBRATION_EM2_SETTINGS_REQUESTED) {
    if (!handle.em2_requested) {
      handle.em2_requested = true;
      return SL_COULOMB_COUNTER_CALIBRATION_PEAK_CURRENT_EM2;
    } else {
      return sl_coulomb_counter_calibrate();
    }
  }

  return SL_COULOMB_COUNTER_CALIBRATION_IN_PROGRESS;
}
