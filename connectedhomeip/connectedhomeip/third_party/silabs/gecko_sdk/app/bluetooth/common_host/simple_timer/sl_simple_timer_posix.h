/***************************************************************************//**
 * @file
 * @brief Simple timer Posix specific header
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

#ifndef SL_SIMPLE_TIMER_POSIX_H
#define SL_SIMPLE_TIMER_POSIX_H

#include <signal.h>
#include <time.h>
#include <stdlib.h>

/**************************************************************************//**
 * @addtogroup timer
 * @{
 *****************************************************************************/

/// POSIX specific timer structure
struct timer_data_s{
  timer_t                timer_id;       // Timer ID generated at timer start
  struct sigevent        sig_evt;        // Signal used for timers
  struct itimerspec      handle_timeout;  // Timeout in required format
  bool                   triggered;      // Indication of expired timer
};

typedef struct timer_data_s simple_timer_handle_t;

/** @} (end addtogroup timer) */
#endif // SL_SIMPLE_TIMER_POSIX_H
