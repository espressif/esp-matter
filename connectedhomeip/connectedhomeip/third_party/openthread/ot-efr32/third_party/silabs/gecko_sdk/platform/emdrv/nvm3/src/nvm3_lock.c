/***************************************************************************//**
 * @file
 * @brief NVM3 data access lock API implementation
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

#include "nvm3_lock.h"
#include "nvm3.h"

#ifdef NVM3_HOST_BUILD
#include "nvm3_config.h"
#include "nvm3_trace.h"
#include "nvm3_hal.h"
#else
#include "em_core.h"
#endif

#if defined(SL_COMPONENT_CATALOG_PRESENT)
#include "sl_component_catalog.h"
#endif

#if defined(SL_CATALOG_MPU_PRESENT)
#include "sl_mpu.h"
#endif

//****************************************************************************

#ifdef NVM3_HOST_BUILD
#define SL_WEAK
#endif

#ifdef NVM3_HOST_BUILD
static int lockCount = 0;
#else
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
CORE_DECLARE_IRQ_STATE;
/// @endcond
#endif

/***************************************************************************//**
 * @addtogroup nvm3
 * @{
 ******************************************************************************/

/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
nvm3_Obj_t nvm3_internalObjectHandleA;
nvm3_Obj_t nvm3_internalObjectHandleB;
nvm3_Obj_t nvm3_internalObjectHandleC;
nvm3_Obj_t nvm3_internalObjectHandleD;
const uint8_t nvm3_maxFragmentCount = NVM3_FRAGMENT_COUNT;
const size_t  nvm3_objHandleSize = sizeof(nvm3_Obj_t);

/// @endcond

/***************************************************************************//**
 * @addtogroup nvm3lock
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @details
 * The default lock-begin implementation.
 ******************************************************************************/
SL_WEAK void nvm3_lockBegin(void)
{
#ifdef NVM3_HOST_BUILD
  lockCount++;
#else
/// @cond DO_NOT_INCLUDE_WITH_DOXYGEN
  CORE_ENTER_CRITICAL();
/// @endcond
#endif
}

/***************************************************************************//**
 * @details
 * The default lock-end implementation.
 ******************************************************************************/
SL_WEAK void nvm3_lockEnd(void)
{
#ifdef NVM3_HOST_BUILD
  if (lockCount == 0) {
    nvm3_tracePrint(NVM3_TRACE_LEVEL_ERROR, "NVM3 ERROR - lockEnd: invalid lock count.\n");
  }
  lockCount--;
#else
  CORE_EXIT_CRITICAL();
#endif
}

/***************************************************************************//**
 * @details
 *  Disable execution from data area.
 *
 * @param[in]  address Start of memory range
 *
 * @param[in]  size Size of memory range.
 ******************************************************************************/
void nvm3_lockDisableExecute(void *address, size_t size)
{
#if defined(__MPU_PRESENT) && (__MPU_PRESENT == 1U) && defined(SL_CATALOG_MPU_PRESENT)
  // The memory range used by nvm3 may not be compatible with the mpu.
  // Just ignore errors.
  sl_mpu_disable_execute((uint32_t)address, (uint32_t)address + size - 1, size);
#else
  (void)address;
  (void)size;
#endif
}

/** @} (end addtogroup nvm3lock) */
/** @} (end addtogroup nvm3) */
