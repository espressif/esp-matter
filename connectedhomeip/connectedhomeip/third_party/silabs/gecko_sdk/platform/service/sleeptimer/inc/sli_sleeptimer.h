/***************************************************************************//**
 * @file
 * @brief SLEEPTIMER SDK internal APIs.
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SLI_SLEEPTIMER_H
#define SLI_SLEEPTIMER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "em_device.h"

#define SLEEPTIMER_EVENT_OF (0x01)
#define SLEEPTIMER_EVENT_COMP (0x02)

#define SLI_SLEEPTIMER_POWER_MANAGER_EARLY_WAKEUP_TIMER_FLAG 0x02

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Hardware Abstraction Layer to get interrupt status.
 *
 * @param local_flag Internal interrupt flag.
 *
 * @return Boolean indicating if specified interrupt is set.
 ******************************************************************************/
bool sli_sleeptimer_hal_is_int_status_set(uint8_t local_flag);

/**************************************************************************//**
 * Determines if next timer to expire has the option flag
 * "SLI_SLEEPTIMER_POWER_MANAGER_EARLY_WAKEUP_TIMER_FLAG".
 *
 * @return true if power manager will expire at next compare match,
 *         false otherwise.
 *****************************************************************************/
bool sli_sleeptimer_is_power_manager_timer_next_to_expire(void);

#ifdef __cplusplus
}
#endif

#endif /* SLI_SLEEPTIMER_H */
