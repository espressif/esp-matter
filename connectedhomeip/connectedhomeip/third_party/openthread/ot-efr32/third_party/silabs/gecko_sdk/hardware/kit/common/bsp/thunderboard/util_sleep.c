/***************************************************************************//**
 * @file
 * @brief Utility Functions for the Thunderboard Sense
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "thunderboard/util.h"
#include "sl_sleeptimer.h"
#include "em_emu.h"

/**************************************************************************//**
* @addtogroup TBSense_BSP
* @{
******************************************************************************/

/***************************************************************************//**
 * @defgroup Util Utility Functions
 * @{
 * @brief Utility functions
 ******************************************************************************/

/** @cond DO_NOT_INCLUDE_WITH_DOXYGEN */

/* Local variables */
static volatile bool                timer_event;
static sl_sleeptimer_timer_handle_t timer_handle;

void SysTick_Handler(void);

/** @endcond */

/***************************************************************************//**
 * @brief
 *    sleeptmer callback function, called when the timer expired.
 ******************************************************************************/
static void timer_callback(sl_sleeptimer_timer_handle_t *handle, void *data)
{
  (void)handle;
  (void)data;
  timer_event = true;
}

/***************************************************************************//**
 * @brief
 *    Sets up the sleeptimer used for sleep functions.
 *
 * @return
 *    Returns zero on OK, non-zero otherwise.
 ******************************************************************************/
uint32_t UTIL_sleepInit(void)
{
  sl_sleeptimer_init();

  return 0;
}

/***************************************************************************//**
 * @brief
 *    Delays number of milliseconds in sleep mode (EM2) using the sleeptimer.
 *
 * @param ms
 *    Number of milliseconds to sleep.
 *
 * @return
 *    None
 ******************************************************************************/
void UTIL_sleep(uint32_t ms)
{
  timer_event = false;
  sl_sleeptimer_start_timer_ms(&timer_handle, ms, timer_callback, NULL, 0, 0);

  while (!timer_event) {
    EMU_EnterEM2(true);
  }
}

/***************************************************************************//**
 * @brief
 *    Delays number of milliseconds in sleep mode (EM2) using the sleeptimer
 *    but returns if an event wakes up the MCU.
 *
 * @param ms
 *    The maximum number of milliseconds to sleep.
 *
 * @return
 *    The number of milliseconds remained from the wait.
 ******************************************************************************/
uint32_t UTIL_waitForEvent(uint32_t ms)
{
  uint32_t remaining_ticks;
  uint32_t remaining_ms;

  timer_event = false;
  sl_sleeptimer_start_timer_ms(&timer_handle, ms, timer_callback, NULL, 0, 0);

  EMU_EnterEM2(true);

  if (timer_event) {
    remaining_ms = 0;
  } else {
    sl_sleeptimer_get_timer_time_remaining(&timer_handle, &remaining_ticks);
    sl_sleeptimer_stop_timer(&timer_handle);
    remaining_ms = sl_sleeptimer_tick_to_ms(remaining_ticks);
  }

  return remaining_ms;
}

/** @} */
/** @} {end addtogroup TBSense_BSP} */
