/*******************************************************************************
 * @file
 * @brief Implementation of software watchdog functions.
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

#ifndef SL_WATCHDOG_H
#define SL_WATCHDOG_H

#include "em_common.h"
#include "em_wdog.h"
#include "sl_status.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup sl_watchdog Software Watchdog
 *
 * @brief The Software Watchdog can be
 *        instantiated by applications and stacks. The hardware watchdog
 *        is only fed once all instances are fed. This allows each task
 *        to have its own watchdog.
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * Software Watchdog handle typedef
 ******************************************************************************/
typedef uint32_t sl_watchdog_handle_t;

/***************************************************************************//**
 * Initialize Software Watchdog.
 * @return Status code
 ******************************************************************************/
sl_status_t sl_watchdog_init(void);

/***************************************************************************//**
 * Register Software Watchdog.
 * @param watchdog  Pointer to watchdog handle
 * @return Status code
 ******************************************************************************/
sl_status_t sl_watchdog_register_watchdog(sl_watchdog_handle_t *watchdog);

/***************************************************************************//**
 * Unregister Software Watchdog.
 * @param watchdog  Watchdog handle
 * @return Status code
 ******************************************************************************/
sl_status_t sl_watchdog_unregister_watchdog(sl_watchdog_handle_t watchdog);

/***************************************************************************//**
 * Feed Software Watchdog instance.
 * @param watchdog  Watchdog handle
 * @return Status code
 ******************************************************************************/
sl_status_t sl_watchdog_feed_watchdog(sl_watchdog_handle_t watchdog);

#if (_SILICON_LABS_32B_SERIES >= 1)
/***************************************************************************//**
 * Function called when watchdog warning limit reached.
 * @param unfed_watchdogs Unfed watchdogs
 * @return Status code
 ******************************************************************************/
SL_WEAK void sl_watchdog_warning_callback(uint32_t unfed_watchdogs);

#endif

/** @} (end addtogroup sl_watchdog) */
#ifdef __cplusplus
}
#endif

#endif // SL_WATCHDOG_H
