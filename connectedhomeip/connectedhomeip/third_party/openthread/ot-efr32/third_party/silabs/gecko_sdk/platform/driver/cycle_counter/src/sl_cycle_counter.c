/*******************************************************************************
 * @file
 * @brief Cycle counter interface.
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

#include <stddef.h>
#include "sl_cycle_counter.h"

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Enable cycle counter.
 ******************************************************************************/
void sl_cycle_counter_enable(void)
{
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  ITM->LAR          = 0xc5acce55;
  DWT->CTRL        |= DWT_CTRL_CYCCNTENA_Msk;
}

/***************************************************************************//**
 * Initialize cycle counter instance.
 ******************************************************************************/
sl_status_t sl_cycle_counter_init(sl_cycle_counter_handle_t *handle)
{
  if (handle == NULL) {
    return SL_STATUS_NULL_POINTER;
  }

  handle->start   = 0;
  handle->cycles  = 0;
  handle->max     = 0;
  return SL_STATUS_OK;
}

/***************************************************************************//**
 * Start a recording.
 ******************************************************************************/
void sl_cycle_counter_start(sl_cycle_counter_handle_t *handle)
{
  handle->start = DWT->CYCCNT;
}

/***************************************************************************//**
 * Stop a recording.
 ******************************************************************************/
void sl_cycle_counter_stop(sl_cycle_counter_handle_t *handle)
{
  handle->cycles = DWT->CYCCNT - handle->start;

  if (handle->cycles > handle->max) {
    handle->max = handle->cycles;
  }
}

/***************************************************************************//**
 * Reset cycle counter instance.
 ******************************************************************************/
sl_status_t sl_cycle_counter_reset(sl_cycle_counter_handle_t *handle)
{
  return sl_cycle_counter_init(handle);
}

/***************************************************************************//**
 * Get cycles elapsed in last recording.
 ******************************************************************************/
uint32_t sl_cycle_counter_get_cycles(sl_cycle_counter_handle_t *handle)
{
  if (handle == NULL) {
    return 0u;
  }

  return handle->cycles;
}

/***************************************************************************//**
 * Get max cycles elapsed for cycle counter instance.
 ******************************************************************************/
uint32_t sl_cycle_counter_get_max(sl_cycle_counter_handle_t *handle)
{
  if (handle == NULL) {
    return 0u;
  }

  return handle->max;
}

/***************************************************************************//**
 * Read the current value of the cycle counter.
 *****************************************************************************/
extern __INLINE uint32_t sl_cycle_counter_get_counter(void);

/***************************************************************************//**
 * Pause cycle counter.
 ******************************************************************************/
extern __INLINE void sl_cycle_counter_pause(void);

/***************************************************************************//**
 * Resume cycle counter.
 ******************************************************************************/
extern __INLINE void sl_cycle_counter_resume(void);
