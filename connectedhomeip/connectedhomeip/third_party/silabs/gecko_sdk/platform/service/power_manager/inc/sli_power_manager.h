/***************************************************************************//**
 * @file
 * @brief Power Manager Private API definition.
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

#ifndef SLI_POWER_MANAGER_H
#define SLI_POWER_MANAGER_H

#include "sl_power_manager.h"
#include "em_device.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#if !defined(SLI_DEVICE_SUPPORTS_EM1P) && defined(_SILICON_LABS_32B_SERIES_2_CONFIG) && _SILICON_LABS_32B_SERIES_2_CONFIG >= 2
#define SLI_DEVICE_SUPPORTS_EM1P
#endif

/*******************************************************************************
 *****************************   PROTOTYPES   **********************************
 ******************************************************************************/

#if defined(SLI_DEVICE_SUPPORTS_EM1P)
void sli_power_manager_update_hf_clock_settings_preservation_requirement(bool add);
#endif

/***************************************************************************//**
 * Adds requirement on the preservation of the High Frequency Clocks settings.
 *
 * @note FOR INTERNAL USE ONLY.
 *
 * @note Must be used together with adding an EM2 requirement.
 ******************************************************************************/
__STATIC_INLINE void sli_power_manager_add_hf_clock_settings_preservation_requirement(void)
{
#if defined(SLI_DEVICE_SUPPORTS_EM1P)
  sli_power_manager_update_hf_clock_settings_preservation_requirement(true);
#else
  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
#endif
}

/***************************************************************************//**
 * Removes requirement on the preservation of the High Frequency Clocks settings.
 *
 * @note FOR INTERNAL USE ONLY.
 *
 * @note Must be used together with removing an EM2 requirement.
 ******************************************************************************/
__STATIC_INLINE void sli_power_manager_remove_hf_clock_settings_preservation_requirement(void)
{
#if defined(SLI_DEVICE_SUPPORTS_EM1P)
  sli_power_manager_update_hf_clock_settings_preservation_requirement(false);
#else
  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
#endif
}

/***************************************************************************//**
 * Informs the power manager module that the high accuracy/high frequency clock
 * is used.
 *
 * @note FOR INTERNAL USE ONLY.
 *
 * @note Must be called by RAIL initialization in case radio clock settings
 *       are not set before the Power Manager initialization.
 ******************************************************************************/
__WEAK void sli_power_manager_set_high_accuracy_hf_clock_as_used(void);

/***************************************************************************//**
 * Gets the wake-up restore process time.
 * If we are not in the context of a deepsleep and therefore don't need to
 * do a restore, the return value is 0.
 *
 *
 * @return   Wake-up restore process time.
 ******************************************************************************/
uint32_t sli_power_manager_get_restore_delay(void);

/***************************************************************************//**
 * Initiates the wake-up restore process.
 ******************************************************************************/
void sli_power_manager_initiate_restore(void);

#ifdef __cplusplus
}
#endif

#endif /* SLI_POWER_MANAGER_H */
