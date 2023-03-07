/***************************************************************************//**
 * @file
 * @brief NVM3 driver HAL for memory mapped FLASH
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

#ifndef NVM3_HAL_FLASH_H
#define NVM3_HAL_FLASH_H

#include "nvm3_hal.h"

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup nvm3
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup nvm3hal
 * @{
 * @details
 * This module provides the NVM3 interface to the EFM and EFR Flash NVM.
 *
 * @note The features available through the handle are used by the NVM3 and
 * should not be used directly by any applications.
 ******************************************************************************/

/*******************************************************************************
 ***************************   GLOBAL VARIABLES   ******************************
 ******************************************************************************/

extern const nvm3_HalHandle_t nvm3_halFlashHandle;      ///< The HAL flash handle.

/** @} (end addtogroup nvm3hal) */
/** @} (end addtogroup nvm3) */

#ifdef __cplusplus
}
#endif

#endif /* NVM3_HAL_FLASH_H */
