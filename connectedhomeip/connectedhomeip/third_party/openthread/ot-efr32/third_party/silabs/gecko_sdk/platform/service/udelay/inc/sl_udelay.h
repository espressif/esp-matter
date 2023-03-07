/***************************************************************************//**
 * @file
 * @brief Microsecond delay.
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

#ifndef UDELAY_H
#define UDELAY_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup udelay Microsecond Delay
 * @brief Microsecond delay function
 * @{
 ******************************************************************************/

/**
 * @brief
 *   Delay a number of microseconds
 *
 * @details
 *   This function will use a busy loop to delay code execution by a certain
 *   number of microseconds before returning to the caller. This function will
 *   not return to the caller earlier than the time given as the input parameter.
 *   This function will not use any hardware timing peripherals, it is using
 *   the core clock frequency to calculate the delay.
 *
 *   Note that there will always be some overhead associated with calling this
 *   function in addition to the internal delay loop. This overhead is relatively
 *   small when the delay is large (>= 100us).
 *
 *   The accuracy of this delay loop will be affected by interrupts and context
 *   switching. If accuracy is needed, a hardware timer should be used
 *   to handle delays.
 *
 * @param[in] us
 *   This is the number of microseconds to delay execution. This function will
 *   return after this amount of time has elapsed. Minimum value is 0 us and
 *   maximum value is 100 000 us (100 ms). It is however recommended to use
 *   the sleeptimer api for delays of more than 1 ms as it is using a hardware
 *   counter and will result in better accuracy.
 */
void sl_udelay_wait(unsigned us);

#ifdef __cplusplus
}
#endif

/** @} (end addtogroup udelay) */

#endif
