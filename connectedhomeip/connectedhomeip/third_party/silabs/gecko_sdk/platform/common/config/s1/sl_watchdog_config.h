/***************************************************************************//**
 * @file
 * @brief Software Watchdog configuration file.
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

// <<< Use Configuration Wizard in Context Menu >>>

#ifndef SL_WATCHDOG_CONFIG_H
#define SL_WATCHDOG_CONFIG_H

// <h>Software Watchdog Configuration

// <q SL_WATCHDOG_RUN_DURING_DEBUG_HALT> Watchdog timer continues to run during debug halt
// <d> 0
#define SL_WATCHDOG_RUN_DURING_DEBUG_HALT 0

// <q SL_WATCHDOG_DISABLE_RESET> Disable Watchdog reset
// <i> If enabled, a watchdog timeout will not reset the device
// <d> 0
#define SL_WATCHDOG_DISABLE_RESET 0

// </h>

// <h>Software Watchdog Interrupt Settings

// <o SL_WATCHDOG_WARNING_SELECTION> Watchdog warning timeout period
// <wdogWarnDisable=> Watchdog warning period is disabled
// <wdogWarnTime25pct=> Watchdog warning period is 25% of the timeout
// <wdogWarnTime50pct=> Watchdog warning period is 50% of the timeout
// <wdogWarnTime75pct=> Watchdog warning period is 75% of the timeout
// <d> wdogWarnDisable
#define SL_WATCHDOG_WARNING_SELECTION wdogWarnDisable

// <o SL_WATCHDOG_WINDOW_SELECTION> Watchdog illegal window limit
// <i> This interrupt occurs when the watchdog is cleared below a certain threshold
// <wdogIllegalWindowDisable=> Watchdog illegal window disabled
// <wdogIllegalWindowTime12_5pct=> Window timeout is 12.5% of the timeout
// <wdogIllegalWindowTime25_0pct=> Window timeout is 25% of the timeout
// <wdogIllegalWindowTime37_5pct=> Window timeout is 37.5% of the timeout
// <wdogIllegalWindowTime50_0pct=> Window timeout is 50% of the timeout
// <wdogIllegalWindowTime62_5pct=> Window timeout is 62.5% of the timeout
// <wdogIllegalWindowTime75_0pct=> Window timeout is 75% of the timeout
// <wdogIllegalWindowTime87_5pct=> Window timeout is 87.5% of the timeout
// <d> wdogIllegalWindowDisable
#define SL_WATCHDOG_WINDOW_SELECTION wdogIllegalWindowDisable

// </h>
#endif /* SL_WATCHDOG_CONFIG_H */

// <<< end of configuration section >>>
