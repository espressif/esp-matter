/***************************************************************************//**
 * @file
 * @brief NVM3 data access lock API definition
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

#ifndef NVM3_LOCK_H
#define NVM3_LOCK_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************//**
 * @addtogroup nvm3
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup nvm3lock NVM3 Lock
 * @brief NVM3 lock functions
 * @{
 * @details
 * This module provides data protection tools for NVM3.
 *
 * The functions in this module are the default protection tools for NVM3.
 * The application can substitute the nvm3_lockBegin and nvm3_lockEnd
 * functions as long as the NVM3 functions are protected from
 * being called re-entrant.
 *
 * @note These functions are used by the NVM3 and should not be used by
 * any applications.
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *  Begin a lock section.
 ******************************************************************************/
void nvm3_lockBegin(void);

/***************************************************************************//**
 * @brief
 *  End a lock section.
 ******************************************************************************/
void nvm3_lockEnd(void);

/***************************************************************************//**
 * @brief
 *  Disable execution from data area.
 ******************************************************************************/
void nvm3_lockDisableExecute(void* address, size_t size);

/** @} (end addtogroup nvm3lock) */
/** @} (end addtogroup nvm3) */

#ifdef __cplusplus
}
#endif

#endif //NVM3_LOCK_H
