/***************************************************************************//**
 * @file
 * @brief SWO Trace API (for eAProfiler)
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

#ifndef __BSP_TRACE_H
#define __BSP_TRACE_H

#include "em_device.h"
#if (defined(BSP_ETM_TRACE) && defined(ETM_PRESENT)) \
  || defined(GPIO_ROUTE_SWOPEN)                      \
  || defined(GPIO_ROUTEPEN_SWVPEN)                   \
  || defined(GPIO_TRACEROUTEPEN_SWVPEN)

#include <stdint.h>
#include <stdbool.h>
#if defined(HAL_CONFIG)
#include "tracehalconfig.h"
#else
#include "traceconfig.h"
#endif

/***************************************************************************//**
 * @addtogroup BSP
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup BSPCOMMON Common BSP for all kits
 * @{
 ******************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(BSP_ETM_TRACE) && defined(ETM_PRESENT)) \
  || defined(GPIO_TRACECLK_PORT)
void BSP_TraceEtmSetup(void);
#endif

#if defined(GPIO_ROUTE_SWOPEN) || defined(_GPIO_ROUTEPEN_SWVPEN_MASK) \
  || defined(GPIO_TRACEROUTEPEN_SWVPEN)
bool BSP_TraceProfilerSetup(void);
void BSP_TraceSwoSetup(void) SL_DEPRECATED_API_SDK_4_1;
#endif

#ifdef __cplusplus
}
#endif

/** @} (end group BSP) */
/** @} (end group BSP) */

#endif /* (defined(BSP_ETM_TRACE) && defined( ETM_PRESENT )) || defined( GPIO_ROUTE_SWOPEN ) */
#endif /* __BSP_TRACE_H */
