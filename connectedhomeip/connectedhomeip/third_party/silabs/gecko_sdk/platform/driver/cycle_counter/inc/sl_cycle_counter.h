/*******************************************************************************
 * @file
 * @brief Cycle counter driver interface.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_CYCLE_COUNTER_H
#define SL_CYCLE_COUNTER_H

#include "em_device.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup cycle_counter Cycle Counter
 * @brief Provides an interface to the cycle counter of the DWT unit.
 *
 * @details
 * ## Initialization
 *
 *   The cycle counter must be enabled prior to any call to its API.
 *   If sl_system is used, only sl_system_init() must be called, otherwise
 *   sl_cycle_counter_enable() must be called manually.
 *
 * ## Limitations
 *
 *   The cycle counter is stored in an unsigned 32-bit integer. As the counter
 *   frequently rolls over, timing durations should be kept short.
 *
 * ## Usage Notes
 *
 *   When entering sleep in Energy Mode 1 (EM1), the DWT cycle counter continues
 *   to increment. The cycle counter is used by the emlib CORE component to
 *   measure the interrupt maximum disable time, when this feature is enabled.
 *   Since the power manager must enter sleep with PRIMASK set to 1, it will
 *   pause the cycle counter before entering EM1 and resume it on wakeup, in
 *   order to provide a realistic interrupt disable time. If this behaviour is
 *   not desired, the maximum interrupt disable feature must be disabled in the
 *   CORE component.
 *
 * ## Usage example
 * ```
 * #include "sl_cycle_counter.h"
 *
 * void main(void)
 * {
 *   sl_cycle_counter_handle_t counter;
 *
 *   // Enable the cycle counter; not needed if sl_system_init() is used.
 *   sl_cycle_counter_enable();
 *
 *   status = sl_cycle_counter_init(&counter);
 *   if (status != SL_STATUS_OK) {
 *      // handle error
 *   }
 *
 *   sl_cycle_counter_start(&counter);
 *   function_to_measure();
 *   sl_cycle_counter_stop(&counter);
 *   printf("Timing: %lu\r\n", sl_cycle_counter_get_cycles(&counter));
 *
 * }
 * ```
 * @{
 ******************************************************************************/

/*******************************************************************************
 ********************************   STRUCTS   **********************************
 ******************************************************************************/

/// A Cycle Counter Instance
typedef struct {
  uint32_t start;    ///< Cycle counter at start of recording
  uint32_t cycles;   ///< Cycles elapsed in last recording
  uint32_t max;      ///< Max recorded cycles since last reset or init
} sl_cycle_counter_handle_t;

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

/*****************************************************************************
* Enable the cycle counter.
*****************************************************************************/
void sl_cycle_counter_enable(void);

/***************************************************************************//**
 * Pause cycle counter.
 *
 * @note this function pauses the DWT cycle counter
 ******************************************************************************/
__INLINE void sl_cycle_counter_pause(void)
{
  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk;
}

/***************************************************************************//**
 * Resume cycle counter.
 *
 * @note this function resumes the DWT cycle counter
 ******************************************************************************/
__INLINE void sl_cycle_counter_resume(void)
{
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/***************************************************************************//**
 * Initialize the cycle counter handle.
 *
 * @param handle  Pointer to counter handle to initialize.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *****************************************************************************/
sl_status_t sl_cycle_counter_init(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Start recording.
 *
 * @param handle  Pointer to initialized counter handle.
 *
 *****************************************************************************/
void sl_cycle_counter_start(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Stop recording.
 *
 * @param handle  Pointer to initialized counter handle.
 *
 *****************************************************************************/
void sl_cycle_counter_stop(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Reset the cycle counter handle.
 *
 * @param handle  Pointer to initialized counter handle.
 *
 * @return SL_STATUS_OK if successful. Error code otherwise.
 *****************************************************************************/
sl_status_t sl_cycle_counter_reset(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Read the last recording duration.
 *
 * @param handle  Pointer to initialized counter handle.
 *
 * @return cycles elapsed in last recording.
 *****************************************************************************/
uint32_t sl_cycle_counter_get_cycles(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Read the max recording duration.
 *
 * @param handle  Pointer to initialized counter handle.
 *
 * @return max cycles elapsed in recordings.
 *****************************************************************************/
uint32_t sl_cycle_counter_get_max(sl_cycle_counter_handle_t *handle);

/***************************************************************************//**
 * Read the current value of the cycle counter.
 *
 * @return cycle counter value.
 *****************************************************************************/
__INLINE uint32_t sl_cycle_counter_get_counter(void)
{
  return DWT->CYCCNT;
}

/** @} (end group cycle_counter) */

#ifdef __cplusplus
}
#endif

#endif
