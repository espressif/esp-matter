/***************************************************************************//**
 * @file
 * @brief Silicon Labs TrustZone secure NVM3 service (secure side).
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef __TFM_NVM3_INCLUDE_H__
#define __TFM_NVM3_INCLUDE_H__

/* Define NVM3_SECURE to signal that we are compiling for the SPE */
#define NVM3_SECURE 1

#define SUPPRESS_UNUSED_IOVEC_PARAM_WARNING() \
  (void)in_vec;                               \
  (void)in_len;                               \
  (void)out_vec;                              \
  (void)out_len;

/* Include the nvm3_spe.h header before including the NVM3 header from
 * emdrv
 */
#if !defined(TFM_CONFIG_SL_SECURE_LIBRARY)
#include "nvm3_spe.h"
#endif

#include "nvm3.h"

#endif /* __TFM_NVM3_INCLUDE_H__ */
