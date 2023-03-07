/***************************************************************************//**
 * @file
 * @brief NVM3 definition of the default data structures.
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

#ifndef NVM3_DEFAULT_H
#define NVM3_DEFAULT_H
#include "nvm3.h"

/***************************************************************************//**
 * @addtogroup nvm3
 * @{
 ******************************************************************************/
/***************************************************************************//**
 * @addtogroup nvm3default NVM3 Default Instance
 * @brief NVM3 default instance functions and handles
 * @{
 ******************************************************************************/

extern nvm3_Handle_t *nvm3_defaultHandle;       ///< The default handle.
extern nvm3_Init_t   *nvm3_defaultInit;         ///< Default initialization data.

/***************************************************************************//**
 * @brief
 *  Initialize the default NVM3 instance.
 *  Once initialized the instance can be accessed through the NVM3 API using
 *  nvm3_defaultHandle as the nvm3_Handle_t handle.
 *
 * @return
 *   @ref ECODE_NVM3_OK on success and a NVM3 @ref Ecode_t on failure.
 ******************************************************************************/
Ecode_t nvm3_initDefault(void);

/***************************************************************************//**
 * @brief
 *  Deinit the default NVM3 instance.
 *
 * @return
 *   @ref ECODE_NVM3_OK on success and a NVM3 @ref Ecode_t on failure.
 ******************************************************************************/
Ecode_t nvm3_deinitDefault(void);

/** @} (end addtogroup nvm3default) */
/** @} (end addtogroup nvm3) */

#endif /* NVM3_DEFAULT_H */
